#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(void)
{
	int fd;
	struct rtc_time rtc_tm;
	int ret;
	int val;
	fd = open("/dev/rtc", O_RDONLY, 0);

	val = 8192;
	ioctl(fd, RTC_IRQP_SET, &val);
	val = 1;	
	ioctl(fd, RTC_PIE_ON, &val);
	while(1)
	{
		ret = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	}
	//close(fd);
	return ret;
}
