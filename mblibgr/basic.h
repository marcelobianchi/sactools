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
 * @file   basic.h
 * @author Marcelo Belentani de Bianchi
 * @date   Sat Sep 25 20:41:16 2010
 * 
 * @brief  
 *  Include file with basic definitions for the mblibgr. This defines the graphic control g_ctl struct and also the basic
 *  methods for open the graphics plot and rescale method to make the graphics plot fit any screen.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpgplot.h>
#include <ctype.h>
#include <math.h>

#ifndef _gr_basic_
#define _gr_basic_

/// Constant to convert from degrees to radians
#define TORADIANS M_PI/180.0
/// Constant to convert from radians to degrees
#define TODEGREES 180.0/M_PI

#define ON 1
#define OFF 0

#define WARNING 7
#define ERROR 2
#define ANOUNCE 3

/// Graphic Control Structure (g_ctl)
/**
 This structure is responssible for sub allocating the graphics panel.
 You should use this structure to reposition your plot on the graphics panel. 
 For example, if you want to plot 3 differents graphics on the same graphic panel, 
 at the same time, you should create three g_ctl structures each of them configured 
 to one certain location on the plotting panel. 

 Keep in mind that the graphics panel coordinates (from the graphics panel) range from 0.0 to 1.0 on X and Y.
 */
typedef struct {
  /// X position of the ploting area defined this g_ctl (reference is bottom/left).
  float xpos;

  /// Y position of the ploting area defined this g_ctl (reference is bottom/left).
  float ypos;

  /// Width of the ploting area.
  float xsize;

  /// height of the ploting area.
  float ysize;
 
  /// Minimun value for the X-range of the ploting area.
  float xmin;

  /// Maximun value for the X-range of the ploting area.
  float xmax;

  /// bottom value of the yaxis.
  float ymin;

  /// top value of the yaxis.
  float ymax;

  /// Width of the window (g_ctl::xmax - g_ctl::xmin).
  float w;

  /// Height of the window (g_ctl::ymax - g_ctl::ymin).
  float h;
  
  /// Graph title.
  char title[256];

  /// Label on the xaxis.
  char xlabel[256];

  /// Label on the yaxis.
  char ylabel[256];
  
  /// Anotation specification for the xaxis.
  char xaxis[15];

  /// Anotation specification for the yaxis.
  char yaxis[15];
  
  /// Enable or disable 5% expanssion of the w and h.
  int expand; 
} g_ctl;

/// Open the graphic panel.
int opengr();

/// Expand the opened graphic panel to the current desktop window
void resizemax(float scale);
#endif
