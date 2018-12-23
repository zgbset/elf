#include <iostream>
#include <fstream>
#include "stub.h"
#include "elf_symtab.h"

using namespace std;

extern "C"
{
	int foo_global(void);
	int bar_global(void);
}

typedef int (*func_ptr)(void);

int main()
{
	foo_global();

	stubInfo si;
	setStub((void *)foo_global, (void *)bar_global, &si);
	foo_global();
	cleanStub(&si);

	foo_global();

    load_symtab(NULL);
    func_ptr sf1 = (func_ptr)get_addr_by_symbol("foobar_static", "foo.c");
    func_ptr sf2 = (func_ptr)get_addr_by_symbol("foobar_static", "bar.c");
    sf1();
    sf2();

    free_symtab();


    try
    {
        std::ofstream fout;
        fout.open("test.txt");

		fout << "Hello world" << endl;

        fout.close();
    }
    catch (...)
    {
        printf("unexpected exception encountered\n");
        return 1;
    }

    return 0;
}
