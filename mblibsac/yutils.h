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
 * @file   yutils.h
 * @author Marcelo Belentani de Bianchi
 * @date   Sat Sep 25 23:17:18 2010
 * 
 * @brief  
 *  Set of utilities methods to process time series. 
 *  The time series should be equal sampled with npts samples and delta sample interval in seconds.
 */
#include <stdio.h>

/// Remove the mean value of the data points supplied
void yu_rmean(float *y, int npts);

/// Remove the 1-D trend of the data points supplied
void yu_rtrend(float *y, int npts);

/// LSQ methods for fitting a line to an array
float yu_linefit(float *y, int npts, float delta, float *a, float *b);

/// Normalize the data sequence to 1, also removes the trend before normalization
int yu_normalize(float *y, int npts);
