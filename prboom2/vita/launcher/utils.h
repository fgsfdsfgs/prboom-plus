#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

int isdir(const char *path);
void *fload(const char *path, size_t *fsize);
int fexists(const char *path);
const char *fext(char *filename);
int fextcmp(char *filename, const char *cmpext);

char *strdup(const char *str);
char *strparse(char *data, char *token);

void I_Cleanup(void);
void I_Error(const char *error, ...);
