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
 * Program to test filters for PASSCAL Instrument 
 *
 * These filters are Butterworth highpass and lowpass filters 
 * both filters may be implemented or only one filter 
 * This program limits the number of poles to be either 0, 2 or 4 
 * The program to calculate the pole position or to filter does 
 * not have any limits 
 *
 * Input parameters 
 *
 * - NH = order of the high pass filter can be 0, or an even number up to 12 
 * - FH = high pass cutoff frequency 
 * - NL = order of the low pass filter can be 0, or an even number up to 12 
 * - FL = low pass cutoff frequency 
 * - dt = sample rate 
 *
 * The program calculates the filter poles then generates a linear 
 * sweep with beginning frequency = 0 to stop frequency = nyqiist 
 * This sweep is then filtered by the requested filters.  
 * The output is then written to disk in ascii format 
 * The file names for the output are sweep.asc and filter.asc 
 * written by jcf feb 1988 modified july 1993.  
 *
 * We APPLY THE FILTER TWO TIMES reverting the direction of the sequence 
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <iir.h>

/** 
 * Routine to add two iircomplex numbers
 *
 *  w = add_c(u,v) w = u + v 
 *
 * @param u Complex One
 * @param v Complex Two
 * 
 * @return The summed complex value
 */
iircomplex add_c(iircomplex u, iircomplex v)
{
    iircomplex      w;

    w.real = u.real + v.real;
    w.imag = u.imag + v.imag;

    return (w);
}

/**
 * Routine to multiply two iircomplex numbers
 * 
 * w = mul_c(u,v) w = u * v
 * 
 * @param u Complex One
 * @param v Complex Two
 * 
 * @return Multiplication result
 */
iircomplex mul_c(iircomplex u, iircomplex v)
{
    iircomplex      w;

    w.real = u.real * v.real - u.imag * v.imag;
    w.imag = u.real * v.imag + u.imag * v.real;

    return (w);
}

/**
 * Routine to multiply a real number times a iircomplex number
 * 
 * w = cmul_c (a,u)
 * 
 * a - real number u - iircomplex number 
 *
 * @param a Real Number
 * @param u Complex Number
 * 
 * @return Multiplication result
 */
iircomplex cmul_c(double a, iircomplex u)
{
    iircomplex      w;

    w.real = a * u.real;
    w.imag = a * u.imag;

    return (w);
}

/**
 * Routine to subtract two iircomplex numbers
 * 
 * w = sub_c(u,v) w = u - v 
 *
 * @param u Complex One
 * @param v Complex Two
 * 
 * @return Complex diference
 */
iircomplex sub_c(iircomplex u, iircomplex v)
{
    iircomplex      w;
    w.real = u.real - v.real;
    w.imag = u.imag - v.imag;

    return (w);
}

/**
 * Routine to divide two iircomplex numbers
 * 
 * w = div_c(u,v) w = u/v
 * 
 * @param u Complex One
 * @param v Complex Two
 * 
 * @return Complex Division 
 */
iircomplex div_c(iircomplex u, iircomplex v)
{
    iircomplex      w;

    /*
     * check for divide by 0 
     */
    if (v.real != 0 && v.imag != 0) {
	w.real = ((u.real * v.real) + (u.imag * v.imag)) /
	    ((v.real * v.real) + (v.imag * v.imag));
	w.imag = ((u.imag * v.real) - (u.real * v.imag)) /
	    ((v.real * v.real) + (v.imag * v.imag));

	return (w);
    } else {
	fprintf(stderr, "ERROR: iircomplex division by 0 in div_c\n");
	exit(1);
    }
}

/**
 * Routine to calculate the iircomplex conjugate
 * 
 * w = conjugate(u)
 * 
 * @param u Complex Number 
 * 
 * @return The Complex Conjugate of Number
 */
iircomplex conj_c(iircomplex u)
{
    iircomplex      w;

    w.real = u.real;
    w.imag = -u.imag;

    return (w);
}

/**
 * Routine to calculate linear frequency sweep.
 * 
 * @param f1 Stop Frequency
 * @param f0 Start Frequency
 * @param npts Number of points
 * @param dt Time between samples
 * @param as Array to store the calculated sweeps
 */
void sweep(double f1, double f0, int npts, double dt, double as[])
{
    double          tf,
                    df,
                    tt;
    int             i;

    tf = npts * dt; /* get final time */
    df = 2. * PI * (f1 - f0) / (tf * 2); /* frequency increment */
    for (i = 0; i < npts; i++) {
	tt = i * dt;
	as[i] = sin(f0 * tt + df * tt * tt);
    }
}

/**
 * Routine to apply a second order recursive filter to the data
 * denomonator polynomial is z**2 + a1*z + a2 numerator polynomial is z**2 
 * + b1*z + b2 fi = input array fo = output array npts = number of points
 * 
  * @param a1 Polinomial definition
  * @param a2 Polinomial definition
  * @param b1 Polinomial definition 
  * @param b2 Polinomial definition 
  * @param npts Number of points of the array 
  * @param fi Input array
  * @param fo Output array
 */
void filt(double a1, double a2, double b1, double b2, int npts, float *fi,
     float *fo)
{
    double          d1,
                    d2,
                    out;
    int             i;

    d1 = 0;
    d2 = 0;
    for (i = 0; i < npts; i++) {
	out = fi[i] + d1;
	d1 = b1 * fi[i] - a1 * out + d2;
	d2 = b2 * fi[i] - a2 * out;
	fo[i] = out;
    }

}

/**
 * Routine to compute lowpass filter poles for Butterworth filter fc =
 * desired cutoff frequency dt = sample rate in seconds n = number of
 * poles (MUST BE EVEN) p = pole locations (RETURNED) b = gain factor for
 * filter (RETURNED)
 * 
 * Program calculates a continuous Butterworth low pass IIRs with required 
 * cut off frequency. This program is limited to using an even number of poles 
 * Then a discrete filter is calculated utilizing the bilinear transform.
 * Methods used here follow those in Digital Filters and Signal Processing.
 *
 * by Leland B. Jackson 
 *
 * @param fc Cut off frequency
 * @param dt Sample rate
 * @param n Number of points
 * @param p Pole locations (returned)
 * @param b Gain of the filer (returned)
 */
void lowpass(double fc, double dt, int n, iircomplex p[], double *b)
{
    double          wcp,
                    wc,
                    b0;
    int             i,
                    i1;
    iircomplex      one,
                    x,
                    y;

    /*
     * Initialize variables 
     */
    wcp = 2 * fc * PI;
    wc = (2. / dt) * tan(wcp * dt / 2.);
    one.real = 1.;
    one.imag = 0.;
    for (i = 0; i < n; i += 2) {
	/*
	 * Calculate position of poles for continuous filter 
	 */

	i1 = i + 1;
	p[i].real = -wc * cos(i1 * PI / (2 * n));
	p[i].imag = wc * sin(i1 * PI / (2 * n));
	p[i + 1] = conj_c(p[i]);
    }
    for (i = 0; i < n; i += 2) {
	/*
	 * Calculate position of poles for discrete filter using 
	 */
	/*
	 * the bilinear transformation 
	 */

	p[i] = cmul_c(dt / 2, p[i]);
	x = add_c(one, p[i]);
	y = sub_c(one, p[i]);
	p[i] = div_c(x, y);
	p[i + 1] = conj_c(p[i]);
    }

    /*
     * calculate filter gain 
     */

    b0 = 1.;
    for (i = 0; i < n; i += 2) {
	x = sub_c(one, p[i]);
	y = sub_c(one, p[i + 1]);
	x = mul_c(x, y);
	b0 = b0 * 4. / x.real;
    }
    b0 = 1. / b0;
    *b = b0;
}

/*
 * Routine to compute lowpass filter poles for Butterworth filter fc =
 * desired cutoff frequency dt = sample rate in seconds n = number of
 * poles (MUST BE EVEN) p = pole locations (RETURNED) b = gain factor for
 * filter (RETURNED)
 * 
 * Program calculates a continuous Butterworth highpass IIRs 
 * First a low pass filter is calculated with required cut off frequency.
 * Then this filter is converted to a high pass filter 
 * This program is limited to using an even number of poles 
 * Then a discrete filter is calculated utilizing the bilinear transform 
 * Methods used here follow those in Digital Filters and Signal Processing 
 *
 * by Leland B. Jackson 
 *
 * @param fc Cut off frequency
 * @param dt Sample rate
 * @param n Number of points
 * @param p Poles computed (returned)
 * @param b Gain factor computed (returned)
 */
void highpass(double fc, double dt, int n, iircomplex p[], double *b)
{
    double          wcp,
                    wc,
                    alpha,
                    b0;
    int             i;
    iircomplex      one,
                    x,
                    y;

    /*
     * Initialize variables 
     */
    wcp = 2 * fc * PI;
    wc = (2. / dt) * tan(wcp * dt / 2.);
    alpha = cos(wc * dt);
    one.real = 1.;
    one.imag = 0.;

    /*
     * get poles for low pass filter 
     */

    lowpass(fc, dt, n, p, &b0);

    /*
     * now find poles for highpass filter 
     */

    for (i = 0; i < n; i += 2) {
	x = cmul_c(alpha, one);
	x = sub_c(x, p[i]);
	y = cmul_c(alpha, p[i]);
	y = sub_c(one, y);
	p[i] = div_c(x, y);
	p[i + 1] = conj_c(p[i]);
    }

    /*
     * Calculate gain for high pass filter 
     */

    b0 = 1.;
    for (i = 0; i < n; i += 2) {
	x = add_c(one, p[i]);
	y = add_c(one, p[i + 1]);
	x = mul_c(x, y);
	b0 = b0 * 4. / x.real;
    }
    b0 = 1. / b0;
    *b = b0;
}

/** 
 * Method to reverse a time series used to make a double pass on the filter.
 * 
 * @param af Input time serie.
 * @param npts The number of points contained in the time serie.
 */
void rev(float *af, int npts)
{
    int             j = 0;

    float           temp;

    for (j = 0; j < npts / 2; j++) {
	temp = af[j];
	af[j] = af[npts - j - 1];
	af[npts - j - 1] = temp;
    }
}

/** 
 * The filter routine.
 * 
 * This routine was extracted from the PQL (Passcall Quick Look) and adapted to be used inside this library. All the others
 * methods excluding the rev where also extracted from the PQL software.
 * 
 * This method, in comparison from the original one has the folow modification:
 *
 * - Use of two passes filtering for reducing the delay introduced by the filter. Each trace is filtered twice, 
 * one in each direction. The delay inserted on the first pass is removed on second pass.
 * 
 * @param as Input data array
 * @param number Number of points in the array
 * @param dt The sample rate of the time serie.
 * @param NH The order of the high pass filter. Must be a even number between 0 (disabled) and 12.
 * @param FH The high pass cut off frequency.
 * @param NL The order of the low pass filter. Must be a even number between 0 (disabled) and 12.
 * @param FL The low pass cut off frequency.
 * 
 * @return The filtered array or NULL on error.
 *
 * @see yu_rtrend, yu_rmean
 */
float *iir(float *as, int number, float dt, int NH, float FH, int NL, float FL)
{
    /*
     * Local variable definition 
     */
    int             i;
    iircomplex      pl[12],
                    ph[12];
    double          b0l,
                    b0h;
    double          f1,
                    f0;
    double          a1,
                    a2,
                    b1,
                    b2;

    float          *af;
    double          mean;
    /*
     * ptr to array of data to be filtered 
     */

    /*
     * get input parameters from keyboard 
     */
    /*
     * First get high pass filter parameters 
     */
    
    if (NH != 0) {
	if (FH <= 0.0) return NULL;
	if (NH % 2 != 0) return NULL;
	if (NH >= 13) return NULL;
      }

    if (NL != 0) {
      if (FL >= (1 / (2 * dt))) return NULL;
      if ((NL % 2) != 0) return NULL;
      if (NL >= 13) return NULL;
    }

    if (NH != 0 && NL != 0) 
      if (FL <= FH) return NULL;
    
    /*
     * Get highpass filter poles if necessary 
     */

    if (NH != 0) {
	highpass(FH, dt, NH, ph, &b0h);
    }

    /*
     * Get low pass filter poles if necessary 
     */

    if (NL != 0) {
	lowpass(FL, dt, NL, pl, &b0l);
    }

    /*
     * Through with calculation of poles 
     */
    /*
     * now calculate sweep for filtering 
     */

    f0 = 0.;			/* start frequency */
    f1 = 1. / (dt * 2.);	/* stop frequency = nyquist */

    if ((af = (float *) malloc(sizeof(float) * number)) == NULL) {
	return NULL;
    }

    for (mean = 0.0, i = 0; i < number; i++)
      mean += as[i];
    mean = mean / number;

    for (i = 0; i < number; i++) {
	af[i] = as[i] - mean;
    }

    if (NH != 0) {
	for (i = 0; i < NH; i += 2) {

	    /*
	     * get first set of second order filter coeficients 
	     */
	    /*
	     * from each pair of poles 
	     */
	    a1 = -2 * ph[i].real;
	    a2 = ph[i].real * ph[i].real + ph[i].imag * ph[i].imag;
	    b1 = -2;
	    b2 = 1;

	    filt(a1, a2, b1, b2, number, af, af);
	    /*
	     * apply gain section 
	     */
	    for (i = 0; i < number; i++) {
		af[i] = b0h * af[i];
	    }

	    rev(af, number);
	    filt(a1, a2, b1, b2, number, af, af);
	    /*
	     * apply gain section 
	     */
	    for (i = 0; i < number; i++) {
		af[i] = b0h * af[i];
	    }
	    rev(af, number);
	}

    }

    /*
     * apply low pass filter using poles pl 
     */
    /*
     * Numerator polynomial is z**2 + 2*z + 1 
     */
    if (NL != 0) {
	for (i = 0; i < NL; i += 2) {

	    a1 = -2 * pl[i].real;
	    a2 = pl[i].real * pl[i].real + pl[i].imag * pl[i].imag;
	    b1 = 2;
	    b2 = 1;

	    filt(a1, a2, b1, b2, number, af, af);
	    /*
	     * apply gain section 
	     */
	    for (i = 0; i < number; i++) {
		af[i] = b0l * af[i];
	    }

	    rev(af, number);
	    filt(a1, a2, b1, b2, number, af, af);
	    /*
	     * apply gain section 
	     */
	    for (i = 0; i < number; i++) {
		af[i] = b0l * af[i];
	    }
	    rev(af, number);

	}

    }

    return af;
}
