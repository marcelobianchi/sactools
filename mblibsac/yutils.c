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
#include <math.h>

/** 
 * Fits a line to the input data. If delta = 0.0, it uses the point index to fit the line.
 * 
 * @param y Data input array
 * @param npts Number of points in the array of data
 * @param delta Sample rate for the array of data
 * @param a Returned a parameter for the line equatio (y=a*x+b)
 * @param b Returned b parameter for the line equatio (y=a*x+b)
 * 
 * @return The RMS (Root Mean Squared) for the fitted line
 */
float yu_linefit(float *y, int npts, float delta, float *a, float *b)
{
	int i;
	double D, Sxx, Sy, Sx, Sxy, x;
	double rms;
	float realdelta = (delta <= 0.0) ? 1.0 : delta;

	Sxx = Sx = Sy = Sxy = D = 0.0;
	for (i = 0; i < npts; i++) {
		x = i * realdelta;
		Sxx += x * x;
		Sx += i * realdelta;
		Sy += y[i];
		Sxy += x * y[i];
	}

	D = npts * Sxx - Sx * Sx;
	*b = (1.0 / D) * (Sxx * Sy - Sx * Sxy);
	*a = (1.0 / D) * ((float) npts * Sxy - Sx * Sy);

	rms = 0.0;
	for (i = 0; i < npts; i++) {
		x = y[i] - ((*a) * (i * realdelta) + (*b));
		rms += (x * x);
	}

	rms = sqrt(rms / npts);
	return (float) rms;
}

/** 
 * Remove a 1D trend of the data input array. The data array is overwritten.
 * 
 * @param y Input data array.
 * @param npts Number of points in the input array.
 */
void yu_rtrend(float *y, int npts)
{
	float a, b;
	int i;

	yu_linefit(y, npts, 0.0, &a, &b);

	for (i = 0; i < npts; i++)
		y[i] -= (a * i + b);
}

/** 
 * Remove the mean value of the data input array. The data array is overwritten.
 * 
 * @param y Input data array
 * @param npts Number of points in the input array.
 */
void yu_rmean(float *y, int npts)
{
	double mean = 0.0;
	int i;

	for (i = 0; i < npts; i++)
		mean += y[i];
	mean = mean / npts;
	for (i = 0; i < npts; i++)
		y[i] -= mean;
}


/** 
 * Normalize the data making the max value equal to 1
 * 
 * @param y Input data array
 * @param npts Number of points in the input array.
 */
int yu_normalize(float *y, int npts)
{
	float max;
	int i, im;
	im = 0;

	yu_rtrend(y, npts);

	max = fabs(y[im]);
	for (i = 1; i < npts; i++) {
		if (fabs(y[i]) <= max)
			continue;
		im = i;
		max = fabs(y[i]);
	}

	for (i = 0; i < npts; i++)
		y[i] = y[i] / max;
	return im;
}
