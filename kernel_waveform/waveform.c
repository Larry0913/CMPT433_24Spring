/*
 * waveform.c
 * CMPT433_kernel exercise
 *      Author: Lingjie Li
 * 		Reference: demo_ledtrig.c from Brian Fraser
 */

/*
 * demo_ledtrig.c
 * - Demonstrate how to flash an LED using a custom trigger.
 *      Author: Brian Fraser
 */
#include <linux/module.h>
#include <linux/miscdevice.h>		// for misc-driver calls.
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
//#error Are we building this?

#define MY_DEVICE_FILE  "waveform"

// Global variables
static unsigned long times_high = 0;
static unsigned long times_low = 0;
static unsigned long longest_high = 0;
static unsigned long longest_low = 0;

#define BUFFER_SIZE  256

/******************************************************
 * LED
 ******************************************************/
#include <linux/leds.h>

DEFINE_LED_TRIGGER(ledtrig_demo);

static void led_register(void)
{
	// Setup the trigger's name:
	led_trigger_register_simple("waveform", &ledtrig_demo);
}

static void led_unregister(void)
{
	// Cleanup
	led_trigger_unregister_simple(ledtrig_demo);
}


/******************************************************
 * Callbacks
 ******************************************************/
static ssize_t my_read(struct file *file,
		char *buff, size_t count, loff_t *ppos)
{
    char temp_buf[BUFFER_SIZE]; // Temporary buffer for the statistics string
    int len; // Length of the formatted string

    printk(KERN_INFO "demo_miscdrv::my_read(), buff size %d, f_pos %d\n", (int) count, (int) *ppos);

    // Check if the position is beyond the beginning of the file
    if (*ppos > 0) {
        // No more data to read
        return 0;
    }
    
    // Format the statistics into the temporary buffer
    len = snprintf(temp_buf, sizeof(temp_buf),
                   "# times high: %lu\n"
                   "# times low:  %lu\n"
                   "Longest high: %lu\n"
                   "Longest low:  %lu\n",
                   times_high, times_low, longest_high, longest_low);
                   
    // Check if the user buffer is large enough to hold our message
    if (count < len) {
        return -EINVAL; // Return error if not enough space
    }
    
    // Copy the formatted string to user space
    if (copy_to_user(buff, temp_buf, len)) {
		printk(KERN_ERR "Unable to write to buffer.");
        return -EFAULT; // Return error if copy failed
    }
    
    // Update the position
    *ppos += len;
    
    // Return the number of bytes read
    return len;
}

static ssize_t my_write(struct file* file, const char *buff,
		size_t count, loff_t* ppos)
{
	size_t i;
	char stack_buf[BUFFER_SIZE]; // Stack-allocated buffer, size based on expected workload
	ssize_t processed = 0, ret = 0;
	size_t not_copied, to_copy; 
	unsigned long off_duration = 0, on_duration = 0; // Durations for the current 'low' and 'high'
	
	printk(KERN_INFO "demo_miscdrv: In my_write()\n");

	printk(KERN_INFO "demo_ledtrig: Flashing %d times for string.\n", count);
	while (count > 0) {
        to_copy = min(count, sizeof(stack_buf));
        
        // Copy from user space to the stack buffer
        not_copied = copy_from_user(stack_buf, buff + processed, to_copy);
		printk(KERN_INFO "no copy is %d\n", not_copied);
        if (not_copied) {
            ret = -EFAULT;
            break;
        }

        // Process each character in the buffer
        for (i = 0; i < to_copy - not_copied; ++i) {
            switch (stack_buf[i]) {
                case '-':
                    if (on_duration == 0) { // New 'high' sequence
                        times_high++;
                    }
                    on_duration++;
                    if (off_duration > longest_low) {
                        longest_low = off_duration;
                    }
                    off_duration = 0;
                    led_trigger_event(ledtrig_demo, LED_FULL);
                    usleep_range(250000, 250001);
                    break;
                case '_':
                    if (off_duration == 0) { // New 'low' sequence
                        times_low++;
                    }
                    off_duration++;
                    if (on_duration > longest_high) {
                        longest_high = on_duration;
                    }
                    on_duration = 0;
                    led_trigger_event(ledtrig_demo, LED_OFF);
                    usleep_range(250000, 250001);
                    break;
                default:
                    // Ignore any other characters
                    break;
            }
        }

        processed += to_copy - not_copied;
        count -= to_copy;
    }

	// Update for the last sequence if it was a 'high'
    if (on_duration > longest_high) {
        longest_high = on_duration;
    }
    // Update for the last sequence if it was a 'low'
    if (off_duration > longest_low) {
        longest_low = off_duration;
    }

	led_trigger_event(ledtrig_demo, LED_OFF);

	// If an error occurred, return that error
    if (ret)
        return ret;

    // Otherwise, return the number of processed characters
    return processed;
}

/******************************************************
 * Misc support
 ******************************************************/
// Callbacks:  (structure defined in <kernel>/include/linux/fs.h)
struct file_operations my_fops = {
	.owner    =  THIS_MODULE,
	.write    =  my_write,
	.read     =  my_read,
};

// Character Device info for the Kernel:
static struct miscdevice my_miscdevice = {
		.minor    = MISC_DYNAMIC_MINOR,         // Let the system assign one.
		.name     = MY_DEVICE_FILE,             // /dev/.... file.
		.fops     = &my_fops                    // Callback functions.
};


/******************************************************
 * Driver initialization and exit:
 ******************************************************/
static int __init my_init(void)
{
	int ret;
	printk(KERN_INFO "----> demo_misc driver init(): file /dev/%s.\n", MY_DEVICE_FILE);

	// Register as a misc driver:
	ret = misc_register(&my_miscdevice);

	// LED:
	led_register();

	return ret;
}

static void __exit my_exit(void)
{
	printk(KERN_INFO "<---- demo_misc driver exit().\n");

	// Unregister misc driver
	misc_deregister(&my_miscdevice);

	// LED:
	led_unregister();
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Lingjie Li");
MODULE_DESCRIPTION("CMPT433 Kernel Exercise");
MODULE_LICENSE("GPL");