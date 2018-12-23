#include <iostream>
#include <fstream>
#include "stub.h"
#include "elf_symtab.h"


#define LOG_INFO(fmt, arg...) printf("[%s:%s] " fmt "\n", __FILE__, __func__, ##arg)

extern "C"
{
	int foo_global(void);
	int bar_global(void);
}

typedef int (*func_ptr)(void);

static int stub_foobar_static(void)
{
	static int fb_s;

	LOG_INFO("stub func");
	return 99;
}

int main(int argc, const char *argv[])
{
    load_symtab(NULL);

    int *p = (int *)get_addr_by_symbol("foobar", "foo.c");
    if (p) *p = 100;

    printf("call static func ...\n");
    func_ptr sf1 = (func_ptr)get_addr_by_symbol("foobar_static", "foo.c");
    func_ptr sf2 = (func_ptr)get_addr_by_symbol("foobar_static", "bar.c");
    if (NULL == sf1 || NULL == sf2)
    {
        LOG_INFO("get_addr_by_symbol fail");
        return 1;
    }

    sf1();
    sf2();

    printf("replace static func ...\n");
    foo_global();
    stubInfo si;
	setStub((void *)sf1, (void *)stub_foobar_static, &si);
	foo_global();
	cleanStub(&si);

    free_symtab();

    try
    {
        std::ofstream fout;
        fout.open("test.txt");
		fout << "Hello world" << std::endl;
        fout.close();
    }
    catch (...)
    {
        printf("unexpected exception encountered\n");
        return 1;
    }

    return 0;
}
