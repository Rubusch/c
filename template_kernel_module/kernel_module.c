// kernel_module.c
/*
  compile:
  gcc -I/lib/modules/'uname -r'/build/include -c module.c
*/

#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif
#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

MODULE_AUTHOR("linux-netzwerkarchitektur.de");
MODULE_DESCRIPTION("Book example");

/*
 Moduleparameters (example) 
*/
unsigned int variable1;
unsigned long variable2[3] = {0,1,2};

/*
 Function, to be exported as symbol (example)
*/
void method1(int test1, char *test2)
{
  // do anything
}

EXPORT_SYMBOL (variable1);
EXPORT_SYMBOL (variable2);
EXPORT_SYMBOL (method1);

MODULE_PARM (variable1, "i");
MODULE_PARM_DESC (variable1, "Description for the int val");

MODULE_PARM (variable2, "1-31");
MODULE_PARM_DESC (variable2, "Description for the long array");

/*
  Proc file support
//*/

#ifdef CONFIG_PROC_FS
struct proc_dir_entry* test_dir, *entry;


int test_proc_get_info(char* buf, char** start, off_t offset, int len)
{
  len = sprintf(buf, "\n This is a test module\n\n");
  len += sprintf(buf + len, " Integer: %u\n", variable1);
  len += sprintf(buf + len, " Long[0]: %lu\n", variable2[0]);
  len += sprintf(buf + len, " Long[1]: %lu\n", variable2[1]);
  len += sprintf(buf + len, " Long[2]: %lu\n", variable2[2]);
  return len;
}


int test_proc_read(char* buf, char** start, off_t off, int count, int *eof, void *data)
{
  unsigned int* ptr_var1 = data;
  return sprintf(buf, "%u\n", *ptr_var1);
}


int test_proc_write(struct file* file, const char* buffer, unsigned long count, void* data)
{
  unsigned int *ptr_var1 = data;
  printk(KERN_DEBUG "TEST: variable1 set to: %s", buffer);
  *ptr_var1 = simple_strtoul(buffer, NULL, 10);
  return count;
}


register_proc_files()
{
  test_dir = proc_mkdir("test_dir", &proc_root);
  if(!create_proc_info_entry("test", 0444, test_dir, test_proc_get_info))
    printk(KERN_DEBUG "TEST: Error creating /proc/test.");
  
  entry = create_proc_entry("test_rw", 0644, test_dir);
  
  entry->nlink = 1;
  entry->data = (void*) &varable1;
  entry->read_proc = test_proc_read;
  entry->write_proc = test_proc_write;
}

unregister_proc_files()
{
  remove_proc_entry("test", test_dir);
  remove_proc_entry("test_rw", test_dir);
  remove_proc_entry("test_dir", NULL);
}
#endif /*CONFIG_PROC_FS*/


/*
  initialization of the module
*/

int skull_init(void)
{
  // register the functionality of the module 
  // e.g. register_netdevice, inet_add_protocol, dev_add_pack, etc.

#ifdef CONFIG_PROC_FS
  register_proc_files();
#endif /* CONFIG_PROC_FS */

  return 0;
}


/*
  Cleaning up of the module context
*/

void skull_cleanup(void)
{
  // logout of the functional module
  // e.g. unregister_netdevice, inet_del_protocol, dev_remove_pack, etc

#ifdef CONFIG_PROC_FS
  unregister_proc_files();
#endif /* CONFIG_PROC_FS */
}


/*
 alternative names for init_module() and cleanup_module()
*/

module_init(skull_init);
module_exit(skull_cleanup);


