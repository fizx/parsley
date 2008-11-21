#ifndef KSTRING_INCLUDED
#define KSTRING_INCLUDED

extern char* aastrcat(int, char**);
extern void* memdup(void*, int);
extern char* astrcat(char*, char*);
extern char* astrcat3(char*, char*, char*);
extern char* astrcat4(char*, char*, char*, char*);
extern char* astrcat5(char*, char*, char*, char*, char*);
extern char* astrcat9(char*, char*, char*, char*, char*, char*, char*, char*, char*);
extern char* esc(char*);

#endif