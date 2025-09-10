#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hello");
MODULE_DESCRIPTION("World");

static struct proc_dir_entry *proc_folder;
static struct proc_dir_entry *proc_file;

static ssize_t my_read(struct file *File, char *buf, size_t cnt, loff_t *offs)
{
	char text[] = "Hello procfs file\n";
	int to_copy, not_copied, delta;

	to_copy = min(cnt, sizeof(text));
	not_copied = copy_to_user(buf, text, to_copy);
	delta = to_copy - not_copied;

	return delta;
}

static ssize_t my_write(struct file *File, const char *buf, size_t cnt,
			loff_t *offs)
{
	char text[255];

	int to_copy, not_copied, delta;

	memset(text, 0, sizeof(text));

	to_copy = min(cnt, sizeof(text));
	not_copied = copy_from_user(text, buf, to_copy);
	printk("procfs: Written %s\n", text);

	delta = to_copy - not_copied;

	return delta;
}

static struct proc_ops fops = { .proc_read = my_read, .proc_write = my_write };

static LIST_HEAD(hello_list);

struct TestStruct {
	int a;
	struct list_head list;
	int b;
	int val;
};

static int list_test(void)
{
	printk("Hello: Linked list init 2\n");

	struct TestStruct l1 = { .val = 7 };
	struct TestStruct l2 = { .val = 8 };
	struct TestStruct l3 = { .val = 9 };

	list_add_tail(&(l1.list), &hello_list);
	list_add_tail(&(l2.list), &hello_list);
	list_add_tail(&(l3.list), &hello_list);

	struct TestStruct *pos;
	list_for_each_entry(pos, &hello_list, list) {
		printk("Hello: list val %d\n", pos->val);
	}

	printk("Hello: Linked list exit\n");
	return 0;
}

static int __init hi(void)
{
	printk("Hello: hello world\n");

	proc_folder = proc_mkdir("hello", NULL);
	if (!proc_folder) {
		printk("Hello: procfs: Creating /proc/hello error\n");
		proc_remove(proc_folder);
		return -ENOMEM;
	}

	proc_file = proc_create("dummy", 0666, proc_folder, &fops);
	if (!proc_file) {
		printk("Hello: procfs: Creating /proc/hello/dummy error\n");
		proc_remove(proc_folder);
		return -ENOMEM;
	}

	printk("Hello: procfs: Created /proc/hello/dummy\n");

	list_test();

	return 0;
}

static void __exit bye(void)
{
	printk("Hello: Bye\n");
	proc_remove(proc_file);
	proc_remove(proc_folder);
}

module_init(hi);
module_exit(bye);
