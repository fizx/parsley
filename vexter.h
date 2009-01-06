#ifndef VEXTER_H_INCLUDED
#define VEXTER_H_INCLUDED

#include "dexter.h"

dexPtr vex_compile(char*, char*);
xmlDocPtr vex_parse_doc(dexPtr, xmlDocPtr);

#endif