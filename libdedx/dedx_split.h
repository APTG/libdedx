#ifndef DEDX_SPLIT_H_INCLUDED
#define DEDX_SPLIT_H_INCLUDED
char** _dedx_split(char* string, char splitCaracter, unsigned int* items, unsigned int strLength);
void _dedx_free_split_temp(char ** temp);

#endif // DEDX_SPLIT_H_INCLUDED
