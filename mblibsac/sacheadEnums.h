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

/* True/false definitions */
#ifndef TRUE
#define FALSE	0
#define TRUE	1
#endif

#define SAC_HEADER_FLOAT_UNDEFINED (-12345.0)
#define SAC_HEADER_INT_UNDEFINED   (-12345)
#define SAC_HEADER_CHAR_UNDEFINED  ("-12345  ")
#define SAC_HEADER_UNDEFINED       ("UNDEFINED")


enum SAC_HEADER_TYPES {
  SAC_HEADER_UNDEFINED_TYPE = 1, 
  SAC_HEADER_FLOAT_TYPE,   SAC_HEADER_INT_TYPE,   SAC_HEADER_ENUM_TYPE,
  SAC_HEADER_LOGICAL_TYPE, SAC_HEADER_CHAR8_TYPE, SAC_HEADER_CHAR16_TYPE
};
  
enum SAC_HEADER_ORDER {
  SAC_HEADER_DELTA = 0, 
  SAC_HEADER_DEPMIN,   SAC_HEADER_DEPMAX,    SAC_HEADER_SCALE,    SAC_HEADER_ODELTA,   SAC_HEADER_B,
  SAC_HEADER_E,        SAC_HEADER_O,         SAC_HEADER_A,        SAC_HEADER_FMT,      SAC_HEADER_T0,       
  SAC_HEADER_T1,       SAC_HEADER_T2,        SAC_HEADER_T3,       SAC_HEADER_T4,       SAC_HEADER_T5,       
  SAC_HEADER_T6,       SAC_HEADER_T7,        SAC_HEADER_T8,       SAC_HEADER_T9,       SAC_HEADER_F,        
  SAC_HEADER_RESP0,    SAC_HEADER_RESP1,     SAC_HEADER_RESP2,    SAC_HEADER_RESP3,    SAC_HEADER_RESP4,    
  SAC_HEADER_RESP5,    SAC_HEADER_RESP6,     SAC_HEADER_RESP7,    SAC_HEADER_RESP8,    SAC_HEADER_RESP9,    
  SAC_HEADER_STLA,     SAC_HEADER_STLO,      SAC_HEADER_STEL,     SAC_HEADER_STDP,     SAC_HEADER_EVLA,     
  SAC_HEADER_EVLO,     SAC_HEADER_EVEL,      SAC_HEADER_EVDP,     SAC_HEADER_MAG,      SAC_HEADER_USER0,    
  SAC_HEADER_USER1,    SAC_HEADER_USER2,     SAC_HEADER_USER3,    SAC_HEADER_USER4,    SAC_HEADER_USER5,    
  SAC_HEADER_USER6,    SAC_HEADER_USER7,     SAC_HEADER_USER8,    SAC_HEADER_USER9,    SAC_HEADER_DIST,     
  SAC_HEADER_AZ,       SAC_HEADER_BAZ,       SAC_HEADER_GCARC,    SAC_HEADER_SB,       SAC_HEADER_SDELTA,  
  SAC_HEADER_DEPMEN,   SAC_HEADER_CMPAZ,     SAC_HEADER_CMPINC,   SAC_HEADER_XMINIMUM, SAC_HEADER_XMAXIMUM, 
  SAC_HEADER_YMINIMUM, SAC_HEADER_YMAXIMUM,  SAC_HEADER_UNUSED6,  SAC_HEADER_UNUSED7,  SAC_HEADER_UNUSED8,  
  SAC_HEADER_UNUSED9,  SAC_HEADER_UNUSED10,  SAC_HEADER_UNUSED11, SAC_HEADER_UNUSED12, SAC_HEADER_NZYEAR,
  SAC_HEADER_NZJDAY,   SAC_HEADER_NZHOUR,    SAC_HEADER_NZMIN,    SAC_HEADER_NZSEC,    SAC_HEADER_NSMSEC,
  SAC_HEADER_NVHDR,    SAC_HEADER_NORID,     SAC_HEADER_NEVID,    SAC_HEADER_NPTS,     SAC_HEADER_NSNPTS,
  SAC_HEADER_NWFID,    SAC_HEADER_NXSIZE,    SAC_HEADER_NYSIZE,   SAC_HEADER_UNUSED15, SAC_HEADER_IFTYPE,
  SAC_HEADER_IDEP,     SAC_HEADER_IZTYPE,    SAC_HEADER_UNUSED16, SAC_HEADER_IINST,    SAC_HEADER_ISTREG, 
  SAC_HEADER_IEVREG,   SAC_HEADER_IEVTYP,    SAC_HEADER_IQUAL,    SAC_HEADER_ISYNTH,   SAC_HEADER_IMAGTYPE, 
  SAC_HEADER_IMAGSRC,  SAC_HEADER_UNUSED19,  SAC_HEADER_UNUSED20, SAC_HEADER_UNUSED21, SAC_HEADER_UNUSED22, 
  SAC_HEADER_UNUSED23, SAC_HEADER_UNUSED24,  SAC_HEADER_UNUSED25, SAC_HEADER_UNUSED26, SAC_HEADER_LEVEN,
  SAC_HEADER_LPSPOL,   SAC_HEADER_LOVROK,    SAC_HEADER_LCALDA,   SAC_HEADER_UNUSED27, SAC_HEADER_KSTNM,
  SAC_HEADER_KEVNM,    SAC_HEADER_KEVNM_END, SAC_HEADER_KHOLE,    SAC_HEADER_KO,       SAC_HEADER_KA,
  SAC_HEADER_KT0,      SAC_HEADER_KT1,       SAC_HEADER_KT2,      SAC_HEADER_KT3,      SAC_HEADER_KT4,
  SAC_HEADER_KT5,      SAC_HEADER_KT6,       SAC_HEADER_KT7,      SAC_HEADER_KT8,      SAC_HEADER_KT9,
  SAC_HEADER_KF,       SAC_HEADER_KUSER0,    SAC_HEADER_KUSER1,   SAC_HEADER_KUSER2,   SAC_HEADER_KCMPNM,
  SAC_HEADER_KNETWK,   SAC_HEADER_KDATRD,    SAC_HEADER_KINST
};

enum SAC_HEADER_ENUMS {
  /* enumerated header values */
  IREAL    = 0,  /* Undocumented                */
  ITIME    = 1,  /* Time series file            */
  IRLIM    = 2,  /* Spectral file-real/imag     */
  IAMPH    = 3,  /* Spectral file-ampl/phase    */
  IXY      = 4,  /* General x vs y file         */
  IUNKN    = 5,  /* Unknown                     */
  IDISP    = 6,  /* Displacement (NM)           */
  IVEL     = 7,  /* Velocity (NM/SEC)           */
  IACC     = 8,  /* Acceleration (CM/SEC/SEC)   */
  IB       = 9,  /* Begin time                  */
  IDAY     = 10,  /* GMT day                     */
  IO       = 11,  /* Event origin time           */
  IA       = 12,  /* First arrival time          */
  IT0      = 13,  /* User defined time pick 0    */
  IT1      = 14,  /* User defined time pick 1    */
  IT2      = 15,  /* User defined time pick 2    */
  IT3      = 16,  /* User defined time pick 3    */
  IT4      = 17,  /* User defined time pick 4    */
  IT5      = 18,  /* User defined time pick 5    */
  IT6      = 19,  /* User defined time pick 6    */
  IT7      = 20,  /* User defined time pick 7    */
  IT8      = 21,  /* User defined time pick 8    */
  IT9      = 22,  /* User defined time pick 9    */
  IRADNV   = 23,  /* Radial (NTS)                */
  ITANNV   = 24,  /* Tangential (NTS)            */
  IRADEV   = 25,  /* Radial (EVENT)              */
  ITANEV   = 26,  /* Tangential (EVENT)          */
  INORTH   = 27,  /* North positive              */
  IEAST    = 28,  /* East positive               */
  IHORZA   = 29,  /* Horizontal (ARB)            */
  IDOWN    = 30,  /* Down positive               */
  IUP      = 31,  /* Up positive                 */
  ILLLBB   = 32,  /* LLL broadband               */
  IWWSN1   = 33,  /* WWSN 15-100                 */
  IWWSN2   = 34,  /* WWSN 30-100                 */
  IHGLP    = 35,  /* High-gain long-period       */
  ISRO     = 36,  /* SRO                         */
  INUCL    = 37,  /* Nuclear event               */
  IPREN    = 38,  /* Nuclear pre-shot event      */
  IPOSTN   = 39,  /* Nuclear post-shot event     */
  IQUAKE   = 40,  /* Earthquake                  */
  IPREQ    = 41,  /* Foreshock                   */
  IPOSTQ   = 42,  /* Aftershock                  */
  ICHEM    = 43,  /* Chemical explosion          */
  IOTHER   = 44,  /* Other                       */
  IGOOD    = 45,  /* Good                        */
  IGLCH    = 46,  /* Gliches                     */
  IDROP    = 47,  /* Dropouts                    */
  ILOWSN   = 48,  /* Low signal to noise ratio   */
  IRLDTA   = 49,  /* Real data                   */
  IVOLTS   = 50,  /* Velocity (volts)            */
  IXYZ     = 51,  /* General XYZ (3-D) file      */
  /* These 18 added to describe magnitude type and source maf 970205 */
  IMB      = 52,  /* Bodywave Magnitude          */
  IMS      = 53,  /* Surface Magnitude           */
  IML      = 54,  /* Local Magnitude             */
  IMW      = 55,  /* Moment Magnitude            */
  IMD      = 56,  /* Duration Magnitude          */
  IMX      = 57,  /* User Defined Magnitude      */
  INEIC    = 58,  /* INEIC                       */
  IPDEQ    = 59,  /* IPDEQ                       */
  IPDEW    = 60,  /* IPDEW                       */
  IPDE     = 61,  /* IPDE                        */
  IISC     = 62,  /* IISC                        */
  IREB     = 63,  /* IREB                        */
  IUSGS    = 64,  /* IUSGS                       */
  IBRK     = 65,  /* IBRK                        */
  ICALTECH = 66,  /* ICALTECH                    */
  ILLNL    = 67,  /* ILLNL                       */
  IEVLOC   = 68,  /* IEVLOC                      */
  IJSOP    = 69,  /* IJSOP                       */
  IUSER    = 70,  /* IUSER                       */
  IUNKNOWN = 71,  /* IUNKNOWN                    */
  /* These  17 added for ievtyp. maf 970325 */
  IQB	   = 72,  /* Quarry or mine blast confirmed by quarry */
  IQB1	   = 73,  /* Quarry or mine blast with designed shot information-ripple fired */
  IQB2	   = 74,  /* Quarry or mine blast with observed shot information-ripple fired */
  IQBX     = 75,  /* Quarry or mine blast - single shot */
  IQMT     = 76,  /* Quarry or mining-induced events: tremors and rockbursts */
  IEQ      = 77,  /* Earthquake                  */
  IEQ1     = 78,  /* Earthquakes in a swarm or aftershock sequence */
  IEQ2     = 79,  /* Felt earthquake             */
  IME      = 80,  /* Marine explosion            */
  IEX	   = 81,  /* Other explosion             */
  INU	   = 82,  /* Nuclear explosion           */
  INC	   = 83,  /* Nuclear cavity collapse     */
  IO_	   = 84,  /* Other source of known origin */
  IL	   = 85,  /* Local event of unknown origin */
  IR	   = 86,  /* Regional event of unknown origin */
  IT	   = 87,  /* Teleseismic event of unknown origin */
  IU	   = 88,  /* Undetermined or conflicting information  */
  /* These 9 added for ievtype to keep up with database. maf 000530 */
  IEQ3     = 89,  /* Damaging Earthquake         */
  IEQ0     = 90,  /* Probable earthquake         */
  IEX0     = 91,  /* Probable explosion          */
  IQC      = 92,  /* Mine collapse               */
  IQB0     = 93,  /* Probable Mine Blast         */
  IGEY     = 94,  /* Geyser                      */
  ILIT     = 95,  /* Light                       */
  IMET     = 96,  /* Meteroic event              */
  IODOR    = 97   /* Odors                       */
};

char *SacHeaderEnums[] = {
  "IREAL",     /* 0    To be consistent with defines above */
  /* iftype */
  "ITIME",        /* 1    Time series file            */
  "IRLIM",        /* 2    Spectral file-real/imag     */
  "IAMPH",        /* 3    Spectral file-ampl/phase    */
  "IXY",          /* 4    General x vs y file         */
  "IUNKN",        /* 5    Unknown                     */

  /* idep */
  "IDISP",        /* 6    Displacement (NM)           */
  "IVEL",         /* 7    Velocity (NM/SEC)           */
  "IACC",         /* 8    Acceleration (CM/SEC/SEC)   */

  /* iztype */
  "IB",           /* 9    Begin time                  */
  "IDAY",         /* 10   GMT day                     */
  "IO",           /* 11   Event origin time           */
  "IA",           /* 12   First arrival time          */
  "IT0",          /* 13   User defined time pick 0    */
  "IT1",          /* 14   User defined time pick 1    */
  "IT2",          /* 15   User defined time pick 2    */
  "IT3",          /* 16   User defined time pick 3    */
  "IT4",          /* 17   User defined time pick 4    */
  "IT5",          /* 18   User defined time pick 5    */
  "IT6",          /* 19   User defined time pick 6    */
  "IT7",          /* 20   User defined time pick 7    */
  "IT8",          /* 21   User defined time pick 8    */
  "IT9",          /* 22   User defined time pick 9    */
  
  /* iinst */
  "IRADNV",	/* 23   Radial (NTS)                */
  "ITANNV",	/* 24   Tangential (NTS)            */
  "IRADEV",	/* 25   Radial (EVENT)              */
  "ITANEV",	/* 26   Tangential (EVENT)          */
  "INORTH",	/* 27   North positive              */
  "IEAST",	/* 28   East positive               */
  "IHORZA",	/* 29   Horizontal (ARB)            */
  "IDOWN",	/* 30   Down positive               */
  "IUP",	/* 31   Up positive                 */
  "ILLLBB",	/* 32   LLL broadband               */
  "IWWSN1",	/* 33   WWSN 15-100                 */
  "IWWSN2",	/* 34   WWSN 30-100                 */
  "IHGLP",	/* 35   High-gain long-period       */
  "ISRO",	/* 36   SRO                         */

  /* ievtyp */
  "INUCL",	/* 37   Nuclear event               */
  "IPREN",	/* 38   Nuclear pre-shot event      */
  "IPOSTN",	/* 39   Nuclear post-shot event     */
  "IQUAKE",	/* 40   Earthquake                  */
  "IPREQ",	/* 41   Foreshock                   */
  "IPOSTQ",	/* 42   Aftershock                  */
  "ICHEM",	/* 43   Chemical explosion          */
  "IOTHER",	/* 44   Other                       */

  /* iqual */
  "IGOOD",	/* 45   Good                        */
  "IGLCH",	/* 46   Gliches                     */
  "IDROP",	/* 47   Dropouts                    */
  "ILOWSN",	/* 48   Low signal to noise ratio   */

  /* isynth */
  "IRLDTA",	/* 49   Real data                   */
  "IVOLTS",	/* 50   Velocity (volts)            */
  "IXYZ",	/* 51   General XYZ (3-D) file      */

  /* These 18 added to describe magnitude type and source maf 970205 */
  "IMB",        /* 52   Bodywave Magnitude */  
  "IMS",        /* 53   Surface Magnitude */   
  "IML",        /* 54   Local Magnitude  */    
  "IMW",        /* 55   Moment Magnitude */
  "IMD",        /* 56   Duration Magnitude */
  "IMX",        /* 57   User Defined Magnitude */
  "INEIC",	/* 58   INEIC */
  "IPDEQ",	/* 59   IPDEQ */
  "IPDEW",	/* 60   IPDEW */
  "IPDE",       /* 61   IPDE */
  "IISC",       /* 62   IISC */
  "IREB",       /* 63   IREB */
  "IUSGS",	/* 64   IUSGS */
  "IBRK",       /* 65   IBRK */
  "ICALTECH",	/* 66   ICALTECH */
  "ILLNL",	/* 67   ILLNL */
  "IEVLOC",	/* 68   IEVLOC */
  "IJSOP",	/* 69   IJSOP */
  "IUSER",	/* 70   IUSER */
  "IUNKNOWN",	/* 71   IUNKNOWN */
  
  /*   These 17 added for ievtyp. maf 970325 */
  "IQB",        /* 72   Quarry or mine blast confirmed by quarry */
  "IQB1",       /* 73   Quarry or mine blast with designed shot information-ripple fired*/
  "IQB2",       /* 74   Quarry or mine blast with observed shot information-ripple fired*/
  "IQBX",       /* 75   Quarry or mine blast - single shot */
  "IQMT",       /* 76   Quarry or mining-induced events: tremors and rockbursts */
  "IEQ",        /* 77   Earthquake */
  "IEQ1",       /* 78   Earthquakes in a swarm or aftershock sequence */
  "IEQ2",       /* 79   Felt earthquake */
  "IME",       	/* 80   Marine explosion */
  "IEX",        /* 81   Other explosion */
  "INU",        /* 82   Nuclear explosion */
  "INC",        /* 83   Nuclear cavity collapse */
  "IO_",        /* 84   Other source of known origin */
  "IL",	        /* 85   Local event of unknown origin */
  "IR",	        /* 86   Regional event of unknown origin */
  "IT",	        /* 87   Teleseismic event of unknown origin */
  "IU",	        /* 88   Undetermined or conflicting information  */
  
  /*   These 9 added for ievtype to keep up with database. maf 000530 */
  "IEQ3",       /* 89   Damaging Earthquake */
  "IEQ0",       /* 90   Probable earthquake */
  "IEX0",       /* 91   Probable explosion */
  "IQC",        /* 92   Mine collapse */
  "IQB0",       /* 93   Probable Mine Blast */
  "IGEY",       /* 94   Geyser */
  "ILIT",       /* 95   Light */
  "IMET",       /* 96   Meteroic event */
  "IODOR"       /* 97   Odors */
};

const int SacHeaderEnumsLength = sizeof(SacHeaderEnums) / sizeof(char *);

char *SacHeaderEnumsDescription[] = {
  "Undocumented", 
  /* iftype */
  "Time Series File",   "Spectral File-Real/Imag",  "Spectral File-Ampl/Phase",
  "General X vs Y file",  "Unknown", 

  /* idep */
  "Displacement (nm)",  "Velocity (nm/sec)",  "Acceleration (cm/sec/sec)",

  /* iztype */
  "Begin Time",  "GMT Day",   "Event Origin Time",  "First Arrival Time",
  "User Defined Time Pick 0",  "User Defined Time Pick 1",
  "User Defined Time Pick 2",  "User Defined Time Pick 3",
  "User Defined Time Pick 4",  "User Defined Time Pick 5",
  "User Defined Time Pick 6",  "User Defined Time Pick 7",
  "User Defined Time Pick 8",  "User Defined Time Pick 9",
  
  /* iinst */
  "Radial (NTS)",  "Tangential (NTS)",  "Radial (Event)",  "Tangential (Event)",
  "North Positive",  "East Positive",  "Horizontal (ARB)",  "Down Positive",
  "Up Positive",  "LLL Broadband",  "WWSN 15-100",  "WWSN 30-100",
  "High Gain Long Period",  "SRO",

  /* ievtyp */
  "Nuclear Event",  "Nuclear Pre-Shot Event",  "Nuclear Post-Shot Event",
  "Earthquake",  "Foreshock",  "Aftershock",  "Chemical Explosion",
  "Other",

  /* iqual */
  "Good",  "Glitches",  "Dropouts",  "Low Signal to Noise Ratio",

  /* isynth */
  "Real Data",  "Velocity (Volts)",  "General XYZ (3-D) file",

  /* These 18 added to describe magnitude type and source maf 970205 */
  "Body Wave Magnitude (mb)",  "Surface Wave Magnitude (Ms)",
  "Local Magnitude (ML)",  "Moment Magnitude (Mw)",
  "Duration Magnitude (Md)",  "User Defined Magnitude",
  "NEIC",  "PDEQ",  "PDEW",  "PDE",  "ISC",  "REB",  "USGS",  "Berkeley",
  "Caltech",  "LLNL",  "EVLOC",  "JSOP",  "User",  "Unknown",
  
  /*   These 17 added for ievtyp. maf 970325 */
  "Quarry/Mine Blast, Confirmed by Quarry", 
  "Quarry/Mine Blast with Shot Information, Ripple Fired", 
  "Quarry/Mine Blast with Observed Shot Information, Ripple Fired",
  "Quarry/Mine Blast, Single Shot",
  "Quarry or Mining Induced Events, Tremors and Rockbursts",
  "Earthquake",  "Earthquake, Swarm or Aftershock Sequence",
  "Earthquake, Felt",  "Marine Explosion",
  "Other Explosion",  "Nuclear Explosion",
  "Nuclear Cavity Collapse",   "Other Source, Unknown Origin",
  "Local Event, Unknown Origin",  "Regional Event, Unknown Origin",
  "Teleseismic Event, Unknown Origin",	
  "Undetermined or Conflicting Information",
  
  /*   These 9 added for ievtype to keep up with database. maf 000530 */
  "Damaging Earthquake",  "Probable Earthquake",  "Probable Explosion",
  "Mine Collapse",      "Probable Mine Blast",   "Geyser",    
  "Light",       "Meteroic Event",   "Odors" 
};
