#include <stdio.h>
#include <stdlib.h>
#include <glob.h>

#ifndef H_GLOBERS
#define H_GLOBERS
void * killGlob(glob_t *glb);
glob_t *dirlist(char *pattern);
glob_t *filelist(char *folder, char *fpattern);
#endif
