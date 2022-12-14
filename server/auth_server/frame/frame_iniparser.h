#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frame_dictionary.h"

int iniparser_getnsec(dictionary * d);
char * iniparser_getsecname(dictionary * d, int n);
void iniparser_dump_ini(dictionary * d, FILE * f);
void iniparser_dump(dictionary * d, FILE * f);
char * iniparser_getstring(dictionary * d, char * key, char * def);
int iniparser_getint(dictionary * d, char * key, int notfound);
double iniparser_getdouble(dictionary * d, char * key, double notfound);
int iniparser_getboolean(dictionary * d, char * key, int notfound);
int iniparser_set(dictionary * ini, char * entry, char * val);
void iniparser_unset(dictionary * ini, char * entry);
int iniparser_find_entry(dictionary * ini, char * entry) ;
dictionary * iniparser_load(char * ininame);
void iniparser_freedict(dictionary * d);
