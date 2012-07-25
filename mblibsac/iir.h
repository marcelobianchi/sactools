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
 * @file   iir.h
 * @author Marcelo Belentani de Bianchi
 * @date   Sun Sep 26 00:26:57 2010
 * 
 * @brief Filter methods.
 * 
 * This set of methods can be used to filter (frequency) equal 
 * sampled timeseries. They were not written originally by me
 * but adapted as needed.
 *
 */
#include <stdio.h>
#include <stdlib.h>

#ifndef PI
#define PI  M_PI
#endif

#ifndef IIRC

#define IIRC
/// Real and Imaginary c99 like struct of data for complex numbers.
typedef struct {
	/// The real part of the complex number
	double real;
	/// The imaginary part of the comples number
	double imag;
} iircomplex;
#endif

/// Filter the input data by a low pass and a high pass filter
float *iir(float *as, int number, float dt, int NH, float FH, int NL,
		   float FL);
