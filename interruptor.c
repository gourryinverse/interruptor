#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/semaphore.h>
#include <linux/ioctl.h>
#include <linux/version.h>
#pragma GCC diagnostic pop
MODULE_LICENSE("GPL");

#define INTERRUPT_TEST_MINOR MISC_DYNAMIC_MINOR

typedef spinlock_t SpinLock;
SpinLock global_mutex;

#define IA32_PMC0								0xc1
#define IA32_PERFEVTSEL0						0x186
#define PERFEVTSEL_BRANCH_INSTRUCTION_RETIRED	0x00c4
#define PERFEVTSEL_USER							(1 << 16)
#define PERFEVTSEL_OS 							(1 << 17)
#define PERFEVTSEL_INTERRUPT					(1 << 20)
#define PERFEVTSEL_ENABLE 						(1 << 22)


int InterruptTest(void);

static long MainDeviceIoctl(struct file* file, unsigned int ioctl, unsigned long arg)
{
	unsigned int result =0;
	result = InterruptTest();
	if (result != 0)
		printk("interruptor test: %d\n", result);
	return 0;
}

static int MainDeviceOpen(struct inode* inode, struct file* file)
{
	return 0;
}

static int MainDeviceRelease(struct inode* inode, struct file* file)
{
	return 0;
}

static int MainDeviceMemoryMap(struct file* file, struct vm_area_struct* vma)
{
	return 0;
}


struct file_operations mainDeviceOps =
{
	.owner = THIS_MODULE,
	.unlocked_ioctl = MainDeviceIoctl,
	.compat_ioctl = MainDeviceIoctl,
	.open = MainDeviceOpen,
	.release = MainDeviceRelease,
	.mmap = MainDeviceMemoryMap
};

struct miscdevice mainDevice =
{
	INTERRUPT_TEST_MINOR,
	"interrupt_test",
	&mainDeviceOps
};


static int InterruptTestInit(void)
{
	spin_lock_init(&global_mutex);
	return misc_register(&mainDevice);
}


static void InterruptTestExit(void)
{
	misc_deregister(&mainDevice);
}

module_init(InterruptTestInit);
module_exit(InterruptTestExit);


int InterruptTest(void)
{
	int branches = 0;
	unsigned long flags;

	// Disable interrupts, because they count as branches
	local_irq_save(flags);

	__asm__ __volatile__
	(
		// Disable IA32_PMC0
		"xorl %%eax, %%eax\n\t"
		"xorl %%edx, %%edx\n\t"
		"movl %[perfevtsel0], %%ecx\n\t"
		"wrmsr\n\t"
		
		// Clear IA32_PMC0
		"xorl %%eax, %%eax\n\t"
		"xorl %%edx, %%edx\n\t"
		"movl %[pmc0], %%ecx\n\t"
		"wrmsr\n\t"

		// Enable IA32_PMC0 with branches retired
		"movl %[perfevtsel0], %%ecx\n\t"
		"movl %[branchCounterEnable], %%eax\n\t"
		"wrmsr\n\t"

		// Spin for a little bit
		"nop\n\t"
		
		// Disable IA32_PMC0
		"xorl %%eax, %%eax\n\t"
		"wrmsr\n\t"
		
		// Read IA32_PMC0
		"xorl %%eax, %%eax\n\t"
		"xorl %%edx, %%edx\n\t"
		"movl %[pmc0], %%ecx\n\t"
		"rdmsr\n\t"

		// Write the branch count
		"movl %%eax, %[branches]\n\t"

		// Clear IA32_PMC0
		"xorl %%eax, %%eax\n\t"
		"xorl %%edx, %%edx\n\t"
		"wrmsr\n\t"

        : [branches] "=rm" (branches)
        : [perfevtsel0] "i" (IA32_PERFEVTSEL0)
        , [pmc0] "i" (IA32_PMC0)
        , [branchCounterEnable] "i" (PERFEVTSEL_BRANCH_INSTRUCTION_RETIRED | PERFEVTSEL_ENABLE | PERFEVTSEL_USER | PERFEVTSEL_INTERRUPT | PERFEVTSEL_OS)
        : "cc", "eax", "ecx", "edx"
    );   

    // Re-enable interrupts
    local_irq_restore(flags);

    return branches;
}
