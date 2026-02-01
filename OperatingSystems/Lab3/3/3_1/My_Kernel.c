#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>
#include <linux/sched.h>
// #include <string.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE];

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /* Do nothing */
	return 0;
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    struct task_struct* cureent_task = current;
    struct task_struct *thread;
    
    // refresh buffer
    memset(buf, '\0', BUFSIZE);
    // load new content
    int length = 0;
    for_each_thread(cureent_task, thread) {
        length += snprintf(buf + length, BUFSIZE - length, 
                        "PID: %d, TID: %d, Priority: %d, State: %d\n", 
                        cureent_task->pid, thread->pid, thread->prio, thread->__state);
    }

    // copy
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