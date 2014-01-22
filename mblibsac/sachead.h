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
 * @file   sachead.h
 * @author Marcelo Belentani de Bianchi
 * @date   Sun Sep 26 01:43:37 2010
 * 
 * @brief  Sac Header definitions.
 * 
 * This file define the Sac header. The header is contained in the 
 * begining of every sac file writen on disk. And should be readed
 * in the correct sequence as it is just a bunch of numbers.
 *
 */
#ifndef _sachead_h
#define _sachead_h

#define SAC_TRUE 1
#define SAC_FALSE 0

/// Float value for undefined
#define SAC_HEADER_FLOAT_UNDEFINED (-12345.0)
/// Int value for undefined
#define SAC_HEADER_INT_UNDEFINED   (-12345)
/// Char (size = 8) value for undefined
#define SAC_HEADER_CHAR_UNDEFINED  ("-12345  ")
/// Text for undefined
#define SAC_HEADER_UNDEFINED       ("UNDEFINED")

/// Number of fields on the header
#define SAC_HEADER_FIELDS          133
#define SAC_HEADER_FIELDS_EXTENDED          133+2
/// Size (in Bytes) of the number part of the header
#define SAC_HEADER_SIZE_NUMBERS    440
/// Size (in Bytes) of the header
#define SAC_HEADER_SIZE            632
#define SAC_HEADER_TMARK_POSITION  10
#define SAC_HEADER_USERN_POSITION  40

#define SAC_HEADER_FLOAT_MIN       0
#define SAC_HEADER_FLOAT_MAX       69
#define SAC_HEADER_INT_MIN         70
#define SAC_HEADER_INT_MAX         84
#define SAC_HEADER_ENUM_MIN        85
#define SAC_HEADER_ENUM_MAX        104
#define SAC_HEADER_LOGICAL_MIN     105
#define SAC_HEADER_LOGICAL_MAX     109
#define SAC_HEADER_CHAR_MIN        110
#define SAC_HEADER_CHAR_MAX        133
#define SAC_HEADER_CHAR_DOUBLE     111

/// Float type for SACHEADDEF
#define TYPE_FLOAT 1
/// Int type for SACHEADDEF
#define TYPE_INT   2
/// Char type for SACHEADDEF
#define TYPE_CHAR  3
/// Char type for SACHEADDEF
#define TYPE_OTHER 4

/** 
 * @brief Sac Header Definition Information
 *
 * This structure gives a set of information about a certain SAC header variable.
 * The information contained in this STRUCT item can help to read, format, and print 
 * correctly the variable.
 *
 * @see getSacHeadDefFromChar, getSacOffsetFromChar, getSacTypeFromChar, getSacCharsizeFromChar, 2
 */
typedef struct sac_head_def {
	/// Name of the header field
	char *name;			/**< Name of the SAC header variable */
	/// Offset of the header field
  /** 
   * The offset for the variable inside the header structure. 
   * The header value can be acessed by: 
   *
   * @code
   * void *p;
   * SACHEAD h;
   * float number;
   *
   * p  = &h->delta; // get the first var from h 
   * p += offset;
   * number = *(float *)p;
   * @endcode
   */
	long offset;
	/// The Type of the header field
	int type;		/**< The type of variable can be one of: TYPE_FLOAT, TYPE_INT, TYPE_CHAR */
	/// The lenght of the char header  field
	int charsize;		/**< The number of characters that can be stored inside the variable. */
	/// Indicates that this header can contain picks
	int isMark;			/**< True if this variable can be used to store a pick mark. */
} SACHEADDEF;

/**
 * @brief Sac Header Definition.
 *
 * This struct maps to the header of sac files as they are written on disk.
 * 
 * @see SACHEADDEF
 */
typedef struct sac_head {
	///  time increment, sec
	float delta;
	/// minimum amplitude
	float depmin;
	/// maximum amplitude
	float depmax;
	/// amplitude scale factor 
	float scale;
	/// observed time inc
	float odelta;
	///  initial time - wrt nz* 
	float b;
	///  end time 
	float e;
	/// event start
	float o;
	/// 1st arrival time 
	float a;
	/// internal use 
	float fmt;
	/// user-defined time pick 
	float t0;
	/// user-defined time pick 
	float t1;
	/// user-defined time pick 
	float t2;
	/// user-defined time pick 
	float t3;
	/// user-defined time pick 
	float t4;
	/// user-defined time pick 
	float t5;
	/// user-defined time pick 
	float t6;
	/// user-defined time pick 
	float t7;
	/// user-defined time pick 
	float t8;
	/// user-defined time pick 
	float t9;
	/// event end, sec > 0 
	float f;
	/// instrument respnse parm
	float resp0;
	/// instrument respnse parm
	float resp1;
	/// instrument respnse parm
	float resp2;
	/// instrument respnse parm
	float resp3;
	/// instrument respnse parm
	float resp4;
	/// instrument respnse parm
	float resp5;
	/// instrument respnse parm
	float resp6;
	/// instrument respnse parm
	float resp7;
	/// instrument respnse parm
	float resp8;
	/// instrument respnse parm
	float resp9;
	/// station latititude 
	float stla;
	/// station longitude
	float stlo;
	/// station elevation, m 
	float stel;
	/// station depth, m 
	float stdp;
	/// event latitude 
	float evla;
	/// event longitude
	float evlo;
	/// event elevation
	float evel;
	/// event depth
	float evdp;
	/// magnitude value
	float mag;
	/// available to user
	float user0;
	/// available to user
	float user1;
	/// available to user
	float user2;
	/// available to user
	float user3;
	/// available to user
	float user4;
	/// available to user
	float user5;
	/// available to user
	float user6;
	/// available to user
	float user7;
	/// available to user
	float user8;
	/// available to user
	float user9;
	/// stn-event distance, km 
	float dist;
	/// event-stn azimuth
	float az;
	/// stn-event azimuth
	float baz;
	/// stn-event dist, degrees
	float gcarc;
	/// saved b value
	float sb;
	/// saved delta value
	float sdelta;
	/// mean value, amplitude
	float depmen;
	/// T component azimuth
	float cmpaz;
	/// T component inclination
	float cmpinc;
	/// XYZ X minimum value
	float xminimum;
	/// XYZ X maximum value
	float xmaximum;
	/// XYZ Y minimum value
	float yminimum;
	/// XYZ Y maximum value
	float ymaximum;
	/// reserved for future use
	float unused6;
	/// reserved for future use
	float unused7;
	/// reserved for future use
	float unused8;
	/// reserved for future use
	float unused9;
	/// reserved for future use
	float unused10;
	/// reserved for future use
	float unused11;
	/// reserved for future use
	float unused12;
	/// zero time of file, yr
	int nzyear;
	/// zero time of file, day 
	int nzjday;
	/// zero time of file, hr
	int nzhour;
	/// zero time of file, min 
	int nzmin;
	/// zero time of file, sec 
	int nzsec;
	/// zero time of file, msec
	int nzmsec;
	/// header version number
	int nvhdr;
	/// Origin ID
	int norid;
	/// Event ID 
	int nevid;
	/// number of samples
	int npts;
	/// saved npts 
	int nsnpts;
	/// Waveform ID
	int nwfid;
	/// XYZ X size 
	int nxsize;
	/// XYZ Y size 
	int nysize;
	/// reserved for future use
	int unused15;
	/// type of file
	int iftype;
	/// type of amplitude
	int idep;
	/// zero time equivalence
	int iztype;
	/// reserved for future use
	int unused16;
	/// recording instrument 
	int iinst;
	/// stn geographic region
	int istreg;
	/// event geographic region
	int ievreg;
	/// event type 
	int ievtyp;
	/// quality of data
	int iqual;
	/// synthetic data flag
	int isynth;
	/// magnitude type 
	int imagtyp;
	/// magnitude source 
	int imagsrc;
	/// reserved for future use
	int unused19;
	/// reserved for future use
	int unused20;
	/// reserved for future use
	int unused21;
	/// reserved for future use
	int unused22;
	/// reserved for future use
	int unused23;
	/// reserved for future use
	int unused24;
	/// reserved for future use
	int unused25;
	/// reserved for future use
	int unused26;
	///  RA data-evenly-spaced flag
	int leven;
	/// station polarity flag
	int lpspol;
	/// overwrite permission 
	int lovrok;
	/// calc distance, azimuth 
	int lcalda;
	/// reserved for future use
	int unused27;
	/// F station name 
	char kstnm[8];
	/// event name 
	char kevnm[16];
	/// man-made event name
	char khole[8];
	/// event origin time id 
	char ko[8];
	/// 1st arrival time ident 
	char ka[8];
	/// time pick 0 ident
	char kt0[8];
	/// time pick 1 ident
	char kt1[8];
	/// time pick 2 ident
	char kt2[8];
	/// time pick 3 ident
	char kt3[8];
	/// time pick 4 ident
	char kt4[8];
	/// time pick 5 ident
	char kt5[8];
	/// time pick 6 ident
	char kt6[8];
	/// time pick 7 ident
	char kt7[8];
	/// time pick 8 ident
	char kt8[8];
	/// time pick 9 ident
	char kt9[8];
	/// end of event ident 
	char kf[8];
	/// available to user
	char kuser0[8];
	/// available to user
	char kuser1[8];
	/// available to user
	char kuser2[8];
	/// F component name 
	char kcmpnm[8];
	/// network name 
	char knetwk[8];
	/// date data read 
	char kdatrd[8];
	/// instrument name
	char kinst[8];
} SACHEAD;

#endif
