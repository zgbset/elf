#include <stdio.h>

#define LOG_INFO(fmt, arg...) printf("[%s:%s] " fmt "\n", __FILE__, __func__, ##arg)


static int foobar;

static int foobar_static(void)
{
	static int fb_s;

	LOG_INFO("foobar = %d", foobar);
	return foobar;
}

int bar_global(void)
{
	static int fb_g;

	int ret = foobar_static();
	LOG_INFO("call foobar_static, ret = %d", ret);
	return ret;
}
