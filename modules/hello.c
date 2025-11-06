#define pr_fmt(fmt) "hello: " fmt

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("IO-32 Sophiia Kulichenko");
MODULE_DESCRIPTION("LKM with charp, uint param, ktime, and lists.");
MODULE_VERSION("1.1-mod"); 

static char *name = "world";
module_param(name, charp, 0444);
MODULE_PARM_DESC(name, "The name to display.");

static unsigned int howmany = 1;
module_param(howmany, uint, 0444);
MODULE_PARM_DESC(howmany, "Number of times to print 'Hello' (default: 1)");

struct hello_event {
	struct list_head list;
	ktime_t event_time;
};

static LIST_HEAD(event_list_head);

static int __init hello_init(void)
{
	int i;
	struct hello_event *new_event;
	ktime_t current_time;

	pr_info("Module loading sequence initiated...\n");

	if (howmany > 10) {
		pr_err("'howmany' (%u) exceeds the maximum limit of 10. Aborting load.\n", howmany);
		return -EINVAL;
	}

	if (howmany == 0 || (howmany >= 5 && howmany <= 10))
		pr_notice("'howmany' (%u) is set to a special range. Proceeding with execution.\n",
			howmany);

	for (i = 0; i < howmany; i++) {
		new_event = kmalloc(sizeof(*new_event), GFP_KERNEL);
		if (!new_event)
			goto cleanup;

		current_time = ktime_get();
		new_event->event_time = current_time;
		list_add_tail(&new_event->list, &event_list_head);

		pr_info("Hello, %s! Time recorded: %lld ns\n",
			name, ktime_to_ns(current_time));
	}

	return 0;

cleanup:
	{
		struct hello_event *cur, *tmp;

		pr_info("Memory allocation failure detected. Cleaning up partial events...\n");
		list_for_each_entry_safe(cur, tmp, &event_list_head, list) {
			list_del(&cur->list);
			kfree(cur);
		}
	}
	return -ENOMEM;
}

static void __exit hello_exit(void)
{
	struct hello_event *cur, *tmp;
	int count = 0;

	pr_info("Module unloading. Reviewing and purging saved ktime events...\n");

	list_for_each_entry_safe(cur, tmp, &event_list_head, list) {
		count++;
		pr_info("Event %d recorded at: %lld ns\n",
			count, ktime_to_ns(cur->event_time));

		list_del(&cur->list);
		kfree(cur);
	}

	pr_info("Successfully cleaned up %d stored events. Kernel module finished, %s!\n", count, name);
}

module_init(hello_init);
module_exit(hello_exit);
