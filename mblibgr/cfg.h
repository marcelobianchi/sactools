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
#include <string.h>

#ifndef _cfg_h
#define _cfg_h

typedef struct kp {
    char *key;
    char *value;
} KP;

typedef struct cfg {
    char *filename;
    int nkps;
    KP *kps;
} CFG;

// Read, Write, Kill
CFG  * readConfig (char *filename);
int    writeConfig (CFG *config);
void * killConfig (CFG * config);

// Gets
int    getConfigValueSize(CFG *config, char *key);
char * getConfigAsString(CFG *config, char *key, char *defaultValue);
float  getConfigAsNumber(CFG *config, char *key, float defaultValue);
int    getConfigAsBoolean(CFG *config, char *key, int defaultValue);

// Sets
int setConfigString(CFG *config, char *key, char *value);
int setConfigNumber(CFG *config, char *key, float value);
int setConfigBoolean(CFG *config, char *key, int value);

#endif
