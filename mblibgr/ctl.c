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

#include <ctl.h>

/** 
 * Erase the full graphic pane (not only the g_ctl area).
 * In the future we must find a way to erase only the g_ctl area.
 * 
 * @param ctl The g_ctl area to erase (See description, it doesn't work).
 */
void ctl_clean(g_ctl * ctl)
{
	//if (ctl == NULL) 
	cpgeras();
}

/** 
 * Draw the box arround the plotting area. It folows the specification stored inside the g_ctl structure.
 * This method should be called after the ctl_resizeview() method.
 *
 * @param ctl A g_ctl structure to label.
 */
void ctl_drawaxis(g_ctl * ctl)
{
	cpgbox(ctl->xaxis, 0.0, 0, ctl->yaxis, 0.0, 0);

	if (strlen(ctl->xlabel) > 0)
		cpgmtxt("B", 2.4, 1.0, 1.0, ctl->xlabel);
	if (strlen(ctl->ylabel) > 0)
		cpgmtxt("L", 1.2, 0.5, 0.5, ctl->ylabel);
	if (strlen(ctl->title) > 0)
		cpgmtxt("R", 1.2, 0.5, 0.5, ctl->title);
}

/** 
 * Reposition the plotting area as defined by the g_ctl structure and also rescale the axis. 
 * You should call this method before any operation on this plotting area.
 * 
 * @param ctl The g_ctl structure that define the desired plotting area to switch to
 */
void ctl_resizeview(g_ctl * ctl)
{
	cpgsvp(ctl->xpos, ctl->xpos + ctl->xsize, ctl->ypos,
		   ctl->ypos + ctl->ysize);
	if (ctl->expand)
		cpgswin(ctl->xmin - ctl->w * 0.05, ctl->xmax + ctl->w * 0.05,
				ctl->ymin - ctl->h * 0.05, ctl->ymax + ctl->h * 0.05);
	else
		cpgswin(ctl->xmin, ctl->xmax, ctl->ymin, ctl->ymax);
}

/** 
 * Creates a new g_ctl structure ready for use. It will be scaled from 0.0 to 1.0 in X and Y.
 *
 * You should allow some space for the borders. It's a good advice to start x at 0.05 and end at 0.95. Same 
 * for Y-range.
 *
 * Those coordinates that you enter here are only for dividing the graphic pane, and not for the axis of the 
 * plotting window. For re-scaling the axis of a g_ctl use one of the methods indicated below.
 * 
 * @param xpos X position of the new g_ctl on the graphics pane, should be between 0.0 and 1.0 (bottom left corner).
 * @param ypos Y position of the new g_ctl on the graphics pane, should be between 0.0 and 1.0  (bottom left corner).
 * @param xsize X size (width) of the new _ctl on the graphics pane.
 * @param ysize Y position (height) of the new g_ctl on the graphics pane.
 * 
 * @return The newly created g_ctl.
 *
 * @see ctl_xreset_mm(), ctl_xreset_ndb(), ctl_yupdate_ndb()
 */
g_ctl *ctl_newctl(float xpos, float ypos, float xsize, float ysize)
{
	g_ctl *ctl;
	ctl = malloc(sizeof(g_ctl));

	ctl->xpos = xpos;
	ctl->ypos = ypos;
	ctl->xsize = xsize;
	ctl->ysize = ysize;

	ctl->xmin = 0.0;
	ctl->xmax = 1.0;
	ctl->ymin = 0.0;
	ctl->ymax = 1.0;
	ctl->w = 1.0;
	ctl->h = 2.0;

	ctl->expand = 1;

	strncpy(ctl->xlabel, "", 255);
	strncpy(ctl->ylabel, "", 255);
	strncpy(ctl->title, "", 255);

	return ctl;
}

/** 
 * Change the axis anotation style for the supplied g_ctl.
 * It makes:
 *
 * - Full border arround the plot.
 * - Anotation all arround.
 * - Tick marks all arround.
 * 
 * @param ctl The inputs g_ctl to change axis anotation style
 */
void ctl_axismap(g_ctl * ctl) {
	strncpy(ctl->xaxis, "BCNST", 14);
	strncpy(ctl->yaxis, "BCNST", 14);
}

/** 
 * Change the axis anotation style for the supplied g_ctl.
 * It makes:
 *
 * - Full border arround the plot.
 * - Anotation on the bottom side.
 * - Tick marks all arround.
 * 
 * @param ctl The inputs g_ctl to change axis anotation style
 */
void ctl_axisfull(g_ctl * ctl)
{
	strncpy(ctl->xaxis, "BCNST", 14);
	strncpy(ctl->yaxis, "BCST", 14);
}

/** 
 * Remove the axis anotation style for the supplied g_ctl. The plot will have no anotations or border.
 * 
 * @param ctl The inputs g_ctl to change axis anotation style
 */
void ctl_axisnone(g_ctl * ctl)
{
	strncpy(ctl->xaxis, "", 14);
	strncpy(ctl->yaxis, "", 14);
}

/** 
 * Change the anotattion style for the supplied g_ctl. It will make:
 * - Border only on bottom of the plot
 * - Anotattion only on the bottom of the plot
 *
 * @param ctl The inputs g_ctl to change axis anotation style
 */
void ctl_axisbottom(g_ctl * ctl)
{
	strncpy(ctl->xaxis, "BNST", 14);
	strncpy(ctl->yaxis, "", 14);
}

/** 
 * X reset giving min/max values. Note that if g_ctl::expand is different from 0 the minimun 
 * and maximun values supplied here will be expanded by 5%.
 * 
 * @param ctl The g_ctl structure to reset the x-range.
 * @param xmin The minimun value for the x-range.
 * @param xmax The maximun value for the x-range.
 * 
 * @see ctl_xreset_ndb, ctl_yupdate_ndb
 */
void ctl_xreset_mm(g_ctl * ctl, float xmin, float xmax)
{
	ctl->xmin = xmin;
	ctl->xmax = xmax;
	ctl->w = ctl->xmax - ctl->xmin;
}

/** 
 * X reset giving npts, delta, b.
 * It will calculate the ranges as;
 * 
 * - xmin = b
 * - xmax = npts * delta + b
 * 
 * @param ctl The g_ctl structure to reset the x-range.
 * @param npts The number of points that should be in the data array.
 * @param delta The sampling rate for the data array.
 * @param b The begin time (in seconds) for the first sample of the data array.
 *
 * @see ctl_xreset_mm, ctl_yupdate_ndb
 */
void ctl_xreset_ndb(g_ctl * ctl, int npts, float delta, float b)
{
	ctl_xreset_mm(ctl, b, (float) npts * delta + b);
}

/** 
 * Y reset giving min/max values. Note that if g_ctl::expand is different from 0 the minimun 
 * and maximun values supplied here will be expanded by 5%.
 * 
 * @param ctl The g_ctl structure to reset the x-range.
 * @param ymin The minimun value for the y-range.
 * @param ymax The maximun value for the y-range.
 * 
 * @see ctl_xreset_ndb, ctl_yupdate_ndb
 */
void ctl_yreset_mm(g_ctl * ctl, float ymin, float ymax){
	ctl->ymin = ymin;
	ctl->ymax = ymax;
	ctl->h = ctl->ymax - ctl->ymin;
}

/** 
 * Rescale the y-range of the supplied g_ctl based on the supplied y-data and, the x-range pre-defined in the g_ctl.
 * This method will re-scale the data contained in the x-range, window, of the g_ctl.
 *
 * For scaling to the full range of the data, you should first set the g_ctl like in:
 * 
 * @code
 * SACHEAD *h;
 * float *y;
 * y = io_readSac("somefile.sac", &h);
 * ctl_xreset_ndb(g_ctl, h->npts, h->delta, h->b);
 * ctl_yupdate_ndb(g_ctl, y, h->npts, h->delta, h->b);
 * @endcode
 *
 * @param ctl The g_ctl structure to re-scale.
 * @param y The y-array (value amplitudes) of values that should this g_ctl scale to.
 * @param npts The number of samples in the y-array.
 * @param delta The sampling rate of the y-array.
 * @param b The begin time of the y-array (time in seconds that corresponds to the first sample of the array).
 *
 * @see ctl_xreset_ndb, ctl_xreset_mm
 */
void ctl_yupdate_ndb(g_ctl * ctl, float *y, int npts, float delta, float b)
{
	float x;
	int i;

	ctl->ymin = ctl->ymax = y[0];
	for (i = 1; i < npts; i++) {
		x = i * delta + b;
		if (x > ctl->xmin && x < ctl->xmax) {
			if (y[i] > ctl->ymax)
				ctl->ymax = y[i];
			if (y[i] < ctl->ymin)
				ctl->ymin = y[i];
		}
	}

	if (ctl->ymin == ctl->ymax) {
		ctl->ymin -= fabs(ctl->ymin) * 0.10;
		ctl->ymax += fabs(ctl->ymax) * 0.10;
	}

	ctl->h = ctl->ymax - ctl->ymin;
}

/** 
 * Check if the supplied g_ctl contains the ax, ay point (was clicked) givin in world coordinates.
 *
 * @param ctl The g_ctl to check against.
 * @param ax The x position inside the graphic pane (ranging from 0.0 to 1.0)
 * @param ay The y position inside the graphic pane (ranging from 0.0 to 1.0)
 * 
 * @return 1 or 0 for true or false (respectively).
 */
int ctl_checkhit(g_ctl * ctl, float ax, float ay)
{
	if (ax >= ctl->xpos && ax <= (ctl->xpos + ctl->xsize))
		if (ay >= ctl->ypos && ay <= (ctl->ypos + ctl->ysize))
			return 1;
	return 0;
}

/**
 * Convert an ax, ay coordinates from 0 -> 1.0 into a certain CTL
 * x,y values.
 * 
 * @param ctl The g_ctl to convert to
 * @param ax  Input ax from 0.0 to 1.0
 * @param ay  Input ay from 0.0 to 1.0
 * @param *x  Converted x value
 * @param *y  Converted y value
 */
void ctl_convertxy(g_ctl *ctl, float ax, float ay, float *x, float *y) {
    *x = *y = 0.0;

    float x1, x2, y1, y2;

    x1 = ctl->xmin;
    x2 = ctl->xmax;
    y1 = ctl->ymin;
    y2 = ctl->ymax;

    if (ctl->expand) {
        x1 = ctl->xmin - ctl->w * 0.05;
        x2 = ctl->xmax + ctl->w * 0.05;
        y1 = ctl->ymin - ctl->h * 0.05;
        y2 = ctl->ymax + ctl->h * 0.05;
    }

    float xdiv = (x2 - x1) / ctl->xsize;
    float ydiv = (y2 - y1) / ctl->ysize;

    *x = x1 + (ax - ctl->xpos) * xdiv;
    *y = y1 + (ay - ctl->ypos) * ydiv;

    return;
}
