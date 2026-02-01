#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE]; //kernel buffer

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    struct task_struct* current_task = current;

    // refresh buffer
    memset(buf, '\0', BUFSIZE);

    // copy
    int length = BUFSIZE < buffer_len ? BUFSIZE : buffer_len;
    if (copy_from_user(buf, ubuf, length)) {
        return -EFAULT;
    }

    // load new content
    length += snprintf(buf + length, BUFSIZE - length, 
                    "\nPID: %d, TID: %d, time: %llu\n", 
                    current_task->tgid, current_task->pid, current_task->utime/100/1000);
    
    if (*offset >= length || *offset < 0) {
        return 0;
    }

    *offset += length;
    return length;
    /****************/
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/

    // copy
    int length = strlen(buf);
    length = buffer_len < length ? buffer_len : length;
    if (*offset >= length || *offset < 0) {
        return 0;
    }
    if (copy_to_user(ubuf, buf, length)) {
        return -EFAULT;
    }

    *offset += length;
    return length;
    /****************/
}

static struct proc_ops Myops = {
    .proc_read = Myread,
    .proc_write = Mywrite,
};

static int My_Kernel_Init(void){
    proc_create(procfs_name, 0644, NULL, &Myops);   
    pr_info("My kernel says Hi");
    return 0;
}

static void My_Kernel_Exit(void){
    pr_info("My kernel says GOODBYE");
}

module_init(My_Kernel_Init);
module_exit(My_Kernel_Exit);

MODULE_LICENSE("GPL");
