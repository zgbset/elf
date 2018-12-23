#ifndef _ELF_SYMTAB_H_
#define _ELF_SYMTAB_H_

#ifdef __cplusplus
extern "C" {
#endif


int load_symtab(const char *elf);
void free_symtab(void);

void * get_addr_by_symbol(const char *name, const char *file);



#ifdef __cplusplus
}
#endif

#endif // _ELF_SYMTAB_H_
