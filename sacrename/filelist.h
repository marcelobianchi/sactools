/*
    This file is part of sactools package.
    
    sactools is a package to managing and do simple processing with SAC
    seismological files.
    Copyright (C) 2012  Marcelo B. de Bianchi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>

typedef struct files {
  char *filename;
  char *newfilename;
  int ready;
} FILES;

extern FILES *filelist;
extern int nfilelist;
extern char *fileend;

/* filelist.c */
int loadfilelist(const char *fpath, const struct stat *sb, int typeflag);
void addFilename(const char *fpath);
void freefilelist(void);
void showfilelist(void);
int checkfilelist(void);
int preparefilelistdirs(void);
int createname(FILES *f);
