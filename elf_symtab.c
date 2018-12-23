
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include "elf_symtab.h"


#ifndef LOG_ERR
#define LOG_ERR(fmt, arg...) printf("[%s:%d] " fmt "\n", __func__, __LINE__, ##arg)
#endif

static Elf32_Sym * g_symtab = NULL;
static int g_symnum = 0;
static char * g_strtab = NULL;
static int g_strtab_size = 0;


int load_symtab(const char *elf)
{
    Elf32_Shdr *shdrs = NULL;
    int sh_len = 0;
    int ret = 0;

    elf = elf ? elf : "bin";
    FILE *fp = fopen(elf, "rb");
    if (NULL == fp)
    {
        LOG_ERR("open %s fail", elf);
        goto ERR_EXIT;
    }

    Elf32_Ehdr ehdr;
    ret = fread(&ehdr, sizeof(ehdr), 1, fp);
    if (ret != 1)
    {
        goto ERR_EXIT;
    }

    sh_len = ehdr.e_shnum * sizeof(Elf32_Shdr);
    shdrs = (Elf32_Shdr *)malloc(sh_len);
    if (NULL == shdrs)
    {
        goto ERR_EXIT;
    }

    fseek(fp, ehdr.e_shoff, SEEK_SET);
    ret = fread(shdrs, sh_len, 1, fp);
    if (ret != 1)
    {
        goto ERR_EXIT;
    }

    int i;
    for (i = 0 ; i < ehdr.e_shnum ; i++)
    {
        if (shdrs[i].sh_type != SHT_SYMTAB)
        {
            if (g_symtab || g_strtab)
            {
                LOG_ERR("duplicated symtab or strtab");
                goto ERR_EXIT;
            }

            int offset = 0;
            g_symnum = shdrs[i].sh_size / shdrs[i].sh_entsize - shdrs[i].sh_info;
            g_symtab = (Elf32_Sym *)malloc(g_symnum * sizeof(Elf32_Sym));
            if (NULL == g_symtab)
            {
                goto ERR_EXIT;
            }
            offset = shdrs[i].sh_offset + shdrs[i].sh_info * shdrs[i].sh_entsize;
            fseek(fp, offset, SEEK_SET);
            ret = fread(g_symtab, g_symnum * sizeof(Elf32_Sym), 1, fp);
            if (ret != 1)
            {
                goto ERR_EXIT;
            }

            g_strtab_size = shdrs[shdrs[i].sh_link].sh_size;
            g_strtab = (char *)malloc(g_strtab_size);
            if (NULL == g_strtab)
            {
                goto ERR_EXIT;
            }
            offset = shdrs[shdrs[i].sh_link].sh_offset;
            fseek(fp, offset, SEEK_SET);
            ret = fread(g_strtab, g_strtab_size, 1, fp);
            if (ret != 1)
            {
                goto ERR_EXIT;
            }
        }
	}

    fclose(fp);
    return 0;

ERR_EXIT:
    if (shdrs) free(shdrs);
    if (fp) fclose(fp);

    free_symtab();
    return -1;
}


void free_symtab(void)
{
    if (g_symtab)
    {
        free(g_symtab);
        g_symtab = NULL;
    }

    if (g_strtab)
    {
        free(g_strtab);
        g_strtab = NULL;
    }
}

void * get_addr_by_symbol(const char *name, const char *file)
{
    const char *cur_file = NULL;

    if (NULL == name)
    {
        return NULL;
    }

    int i;
    for (i = 0; i < g_symnum; i++)
    {
        Elf32_Sym *st = &g_symtab[i];
        if (ELF32_ST_TYPE(st->st_info) == STT_FILE)
        {
            cur_file = &g_strtab[st->st_name];
        }
        else if (ELF32_ST_TYPE(st->st_info) == STT_FUNC
            || ELF32_ST_TYPE(st->st_info) == STT_OBJECT)
        {
            if (!strcmp(&g_strtab[st->st_name], name))
            {
                if (file && strcmp(cur_file, file))
                {
                    continue;
                }

                return (void *)st->st_value;
            }
        }
    }
}
