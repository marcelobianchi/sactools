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

/**
 * @file   sacio.h
 * @author Marcelo Belentani de Bianchi
 * @date   Sat Sep 25 21:17:32 2010
 * 
 * @brief  
 *  Avaliable methods for input and output. 
 *  This methods handles the reading and writting of binary SAC files to and from Disk.
 */
#include <sachead.h>

/// Version of the SacIO group of methods
extern const char *io_version;

/// Write a full SAC file, header and data
int io_writeSac(char *filename, SACHEAD * h, float *y);

/// Write only the data part of the SAC file
int io_writeSacData(char *filename, SACHEAD * h, float *y);

/// Write only the header parat of the SAC file
int io_writeSacHead(char *filename, SACHEAD * h);

/// Read a full SAC file, header and data
float *io_readSac(char *filename, SACHEAD ** h);

/// Read the data part only of a SAC file
float *io_readSacData(char *filename, SACHEAD * h);

/// Read the header part of the SAC file
SACHEAD *io_readSacHead(char *filename);

/// Properly free the memory allocated under the pointer p
void *io_freeData(void *p);
