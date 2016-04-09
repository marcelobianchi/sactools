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
#include <time.h>

#ifndef timeu
#define timeu

#define TIME_CHGMT 0
#define TIME_ISO 1
#define TIME_PLAIN 2
#define TIME_KZDATE 3
#define TIME_KZTIME 4

typedef struct {
	struct tm fs;				// Time of the trace
	int ms;						// MS part of the time
	float b;					// SAC begin time
} SACTIME;

SACTIME *getTimeStructFromSAC(SACHEAD * h);
SACTIME *getTimeStructFromJDNumbers(int y, int jd, int h, int m, int s,
									int ms);
SACTIME *getTimeStructFromNumbers(int y, int mo, int d, int h, int mi,
								  int s, int ms);

int julday(int year, int mon, int day);
int calday(int year, int yearday, int *mon, int *day);
char *print_time(SACTIME * t, int format);
double getValue(SACTIME * tr, SACTIME * t);
char *marktime(SACHEAD * h, char *mark, int format);
void sumation(SACTIME * tr, float n);
#endif
