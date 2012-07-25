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
 * @file   ctl.h
 * @author Marcelo Belentani de Bianchi
 * @date   Sat Sep 25 23:36:38 2010
 * 
 * @brief  Methods to handle g_ctl data structs.
 * 
 * The g_ctl data struct is a higher level abstraction of the PGPLOT 
 * graphics library. It alows you to subdivide the graphics pane while 
 * keeping control of the area without too much efforts.
 * 
 * For each plot area inside the whole graphics pane you should keep one g_ctl,
 * and using the methods supplied you can plot data to this areas. The g_ctl
 * data structure holds information to where in the graphic pane the graph 
 * should be made. Wich is the axis interval for these specific plot area, and 
 * also which labels, and how to label the plot area.
 */
#include <basic.h>

/// Reset the ymin, ymax for a g_ctl based on input data and xmin, xmax limits from g_ctl
void ctl_yupdate_ndb(g_ctl * ctl, float *y, int npts, float delta,
					 float b);

/// Reset the xmin, xmax from g_ctl based on number of points, sampling and begin time for a time series
void ctl_xreset_ndb(g_ctl * ctl, int npts, float delta, float b);

/// Reset the xmin, xmax from g_ctl based on supplied xmin and xmax values
void ctl_xreset_mm(g_ctl * ctl, float xmin, float xmax);

/// Get a new ready to use g_ctl data structure
g_ctl *ctl_newctl(float xpos, float ypos, float wsize, float ysize);

/// Draw the axis based on the information of the g_ctl supplied
void ctl_drawaxis(g_ctl * ctl);

/// Reposition and resizing the graphics panel
void ctl_resizeview(g_ctl * ctl);

/// Erase the full graphics pane
void ctl_clean(g_ctl * ctl);

/// Set normal (full) border and anotations
void ctl_axisfull(g_ctl * ctl);

/// Disable the axis and anotations
void ctl_axisnone(g_ctl * ctl);

/// Set only anotation on bottom of the g_ctl
void ctl_axisbottom(g_ctl * ctl);

/// Check if global (from 0.0 to 1.0) ax and ay is inside the g_ctl supplied
int ctl_checkhit(g_ctl * ctl, float ax, float ay);
