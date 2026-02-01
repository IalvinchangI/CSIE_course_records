#include <linux/fs.h>
#include <linux/uaccess.h>
#include "osfs.h"

/**
 * Function: osfs_read
 * Description: Reads data from a file.
 * Inputs:
 *   - filp: The file pointer representing the file to read from.
 *   - buf: The user-space buffer to copy the data into.
 *   - len: The number of bytes to read.
 *   - ppos: The file position pointer.
 * Returns:
 *   - The number of bytes read on success.
 *   - 0 if the end of the file is reached.
 *   - -EFAULT if copying data to user space fails.
 */
static ssize_t osfs_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos)
{
    struct inode *inode = file_inode(filp);
    struct osfs_inode *osfs_inode = inode->i_private;
    struct osfs_sb_info *sb_info = inode->i_sb->s_fs_info;
    void *data_block;
    ssize_t bytes_read = len;

    // If the file has not been allocated a data block, it indicates the file is empty
    if (osfs_inode->i_blocks == 0)
        return 0;

    if (*ppos >= osfs_inode->i_size)
        return 0;

    if (*ppos + len > osfs_inode->i_size)
        bytes_read = osfs_inode->i_size - *ppos;

    // read
    uint32_t block_offset = *ppos % BLOCK_SIZE;
    uint32_t block_index = *ppos / BLOCK_SIZE;
    uint64_t current_position = 0;
    while (current_position < bytes_read) {
        // copy
        data_block = sb_info->data_blocks + osfs_inode->i_block[block_index] * BLOCK_SIZE + block_offset;
        size_t copy_len = bytes_read - current_position;
        if (current_position > BLOCK_SIZE) {
            copy_len = BLOCK_SIZE;
        }
        if (copy_to_user(buf + current_position, data_block, copy_len))
            return -EFAULT;
        
        // next
        current_position += copy_len;
        block_index++;
        block_offset = 0;
    }
    
    // update
    *ppos += bytes_read;

    return bytes_read;
}

/**
 * check is there any free space to write? should allocate new data block?
 */
static int __osfs_inode_has_free_space(struct osfs_inode *osfs_inode, size_t len, loff_t *start) {
    return (*start + len) < (osfs_inode->i_blocks * BLOCK_SIZE);
}

/**
 * allocate new data blocks and register them into inode
 */
static int __osfs_inode_allocate_register_data_blocks(struct inode *inode, size_t len, loff_t *start) {
    struct osfs_inode *osfs_inode = inode->i_private;

    // allocate block
    uint32_t new_size = *start + len;
    uint32_t new_block_count = new_size / BLOCK_SIZE + (new_size % BLOCK_SIZE != 0);
    for (int i = osfs_inode->i_blocks; i < new_block_count; i++) {
        if (osfs_alloc_data_block(inode->i_sb->s_fs_info, &(osfs_inode->i_block[i])) < 0) {
            // free data block for future use
            for (int j = i - 1; j >= osfs_inode->i_blocks; j--) {
                osfs_free_data_block(inode->i_sb->s_fs_info, osfs_inode->i_block[j]);
            }
            return -ENOSPC;
        }
    }

    // update metadata
    inode->i_blocks = new_block_count;
    osfs_inode->i_blocks = new_block_count;

    return 0;
}


/**
 * Function: osfs_write
 * Description: Writes data to a file.
 * Inputs:
 *   - filp: The file pointer representing the file to write to.
 *   - buf: The user-space buffer containing the data to write.
 *   - len: The number of bytes to write.
 *   - ppos: The file position pointer.
 * Returns:
 *   - The number of bytes written on success.
 *   - -EFAULT if copying data from user space fails.
 *   - Adjusted length if the write exceeds the block size.
 */
static ssize_t osfs_write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos)
{   
    //Step1: Retrieve the inode and filesystem information
    struct inode *inode = file_inode(filp);
    struct osfs_inode *osfs_inode = inode->i_private;
    struct osfs_sb_info *sb_info = inode->i_sb->s_fs_info;
    void *data_block;
    ssize_t bytes_written;
    int ret;

    // Step2: Check if a data block has been allocated; if not, allocate one
    if (__osfs_inode_has_free_space(osfs_inode, len, ppos) == 0) {
        ret = __osfs_inode_allocate_register_data_blocks(inode, len, ppos);
        if (ret < 0) {
            return ret;
        }
    }

    // Step3: Limit the write length to fit within one data block  ( we don't have to care about this anymore!! )
    // if (*ppos >= BLOCK_SIZE) {
    //     return -ENOSPC;
    // }
    bytes_written = len;
    // if (len + *ppos > BLOCK_SIZE) {
    //     bytes_written = BLOCK_SIZE - *ppos;
    // }

    // Step4: Write data from user space to the data block
    // data_block = sb_info->data_blocks + osfs_inode->i_block[0] * BLOCK_SIZE + *ppos;
    // if (copy_from_user(data_block, buf, bytes_written)) {
    //     return -EFAULT;
    // }
    uint32_t block_offset = *ppos % BLOCK_SIZE;
    uint32_t block_index = *ppos / BLOCK_SIZE;
    uint64_t current_position = 0;
    while (current_position < bytes_written) {
        // copy
        data_block = sb_info->data_blocks + osfs_inode->i_block[block_index] * BLOCK_SIZE + block_offset;
        size_t copy_len = bytes_written - current_position;
        if (current_position > BLOCK_SIZE) {
            copy_len = BLOCK_SIZE;
        }
        if (copy_from_user(data_block, buf + current_position, copy_len)) {
            return -EFAULT;
        }
        
        // next
        current_position += copy_len;
        block_index++;
        block_offset = 0;
    }
    // pr_err("osfs_write: Write Tag\n");
    
    // Step5: Update inode & osfs_inode attribute
    struct timespec64 now = current_time(inode);
    osfs_inode->__i_mtime = now;  // modify means write
    inode_set_mtime_to_ts(inode, now);  // modify means write
    inode_set_ctime_to_ts(inode, now);  // record the time we change i_mtime
    mark_inode_dirty(inode);  // require system to write inode back later because the inode on disk haven't change yet
    
    // Step6: Return the number of bytes written
    *ppos += bytes_written;
    if (*ppos > osfs_inode->i_size) {
        osfs_inode->i_size = *ppos;
        inode->i_size = *ppos;
    }
    
    return bytes_written;
}

/**
 * Struct: osfs_file_operations
 * Description: Defines the file operations for regular files in osfs.
 */
const struct file_operations osfs_file_operations = {
    .open = generic_file_open, // Use generic open or implement osfs_open if needed
    .read = osfs_read,
    .write = osfs_write,
    .llseek = default_llseek,
    // Add other operations as needed
};

/**
 * Struct: osfs_file_inode_operations
 * Description: Defines the inode operations for regular files in osfs.
 * Note: Add additional operations such as getattr as needed.
 */
const struct inode_operations osfs_file_inode_operations = {
    // Add inode operations here, e.g., .getattr = osfs_getattr,
};
