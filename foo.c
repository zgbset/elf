#include <stdio.h>

static int foobar;

static int foobar_static(void)
{
	static int fb_s;
	printf("[%s:%s] foobar_static\n", __FILE__, __func__);
}


int foo_global(void)
{
	static int fb_g;
	printf("[%s:%s] foo_global\n", __FILE__, __func__);
}
