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

#include <basic.h>
#include <X11/Xlib.h>

float BASIC_ASPECT = 0.0;

/** 
 * This method opens a graphic panel for plotting. All the subsequent plotting will be put in this panel.
 * 
 * @return The PGPLOT graphic Id just in case.
 */
int opengr()
{
	char aux[8];
	int i = 5;
	int GRid;

	cpgqinf("STATE", aux, &i);
	if (strncmp(aux, "OPEN", i) != 0) {
		GRid = cpgopen("/xwindow");
		cpgask(0);
		resizemax(0.85);
		return (GRid);
	} else {
		fprintf(stderr, "Device alredy open !!");
		return -1;
	}
}

/** 
 * This method will expand the graphics panel already openned to the maximun of the display area scaled by the scale value supplied.
 * 
 * @param scale Porcentage (0-100) of the display width to use.
 */
void resizemax(float scale)
{
	Display *disp;
	float ax, ay;
	int X, Y;

	/* Xlib code */
	disp = XOpenDisplay(NULL);
	if (disp == NULL) {
		fprintf(stderr, "No Display.\n");
		exit(-1);
	} else {
		Y = XDisplayHeightMM(disp, 0);
		X = XDisplayWidthMM(disp, 0) / 1.8;
	}
	XCloseDisplay(disp);
	/* End of Xlib code */

	ay = (double) Y / (double) X;
	ax = X / 25.4 * scale;
	cpgpap(ax, ay);
	cpgpage();

    BASIC_ASPECT = ay;
}

/**
 * Method to find the min and max of a vector of data
 *
 * @ param x Data array
 * @ param npts Is number of points in x
 * @ param xmin Is xmin
 * @ param xmax Is xmax
 * @ param imin Is i of xmin
 * @ param imax Is i of xmax
 */
void vecminmax(float *x, int npts, float *xmin, float *xmax, int *imin, int *imax) {
    *xmin = x[0];
    *xmax = x[0];

    int i;

    for(i = 1; i < npts; i++) {
        if (x[i] > *xmax) {
            *xmax  = x[i];
            if (imax != NULL) *imax = i;
        }

        if (x[i] <= *xmin) {
            *xmin = x[i];
            if (imin != NULL) *imin = i;
        }
    }

}
