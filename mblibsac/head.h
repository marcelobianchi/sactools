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
 * @file   head.h
 * @author Marcelo Belentani de Bianchi
 * @date   Sat Sep 25 21:19:50 2010
 * 
 * @brief  
 *  Include file that define the methods avaliable for handling the head of SAC files.
 *  The methods are devided in basic header methods (hd_) and header utils methods (hdu_).
 */
#include <stdio.h>
#include <stdlib.h>
#include <sachead.h>

/// Version of the header methods
extern const char *hd_version;
/// Version of the header utils methods
extern const char *hdu_version;

/// Get a SACHEADDEF (Sac Header Definition Structure) from its name
SACHEADDEF *getSacHeadDefFromChar(char *name);
/// Find the Offset of a header variable from its name
int getSacOffsetFromChar(char *name);
///  Find the type of a header variable from its name
int getSacTypeFromChar(char *name);
///  Find the string size of a header variable of type char[] from its name
int getSacCharsizeFromChar(char *name);
///  Change the value of a sac header from its name. The value should also be passed as a string
int changeCharValueFromChar(SACHEAD *h, char *var, char *value);

/// Check if the name for the supplied SAC FIELD is a mark field for some phase 
int isMarkField(char *name);

/// Show the list of sac header elements know
void showHeadElements(char *tab);
/// Prepare a header variable for printing by its name
char *hd_showValueFromChar(SACHEAD *h, char *var,char *fmtf, char *fmti,char *fmts);

/* headutils stuff */
/// Get a value for a certain header variable defined by a string name
int hdu_getValueFromChar(char *string, SACHEAD *hdr, float *Fvalue, int *Ivalue, char **str);
/// Convert seconds to point index giving a SACHEAD header.
int hdu_getNptsFromSeconds(SACHEAD *h, float picktime);
/// Convert a point index to seconds giving a SACHEAD header
float hdu_getSecondsFromNPTS(SACHEAD *h, int npts);
/// Check if the point index value is contained in the data part related to the SACHEAD supplied
int hdu_checkNPTS(SACHEAD *h, int value);
/// Change the value for a certain header variable defined by a string name
int hdu_changeValueFromChar(SACHEAD *h, char *string, float *Fvalue, int *Ivalue, char *Cvalue);
/// Round the supplied sample value to the nearest real sample contained in the data
int hdu_roundNPTS(SACHEAD *h, int value);
