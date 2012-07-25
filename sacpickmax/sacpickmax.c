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
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <sac.h>

#define ABS 0
#define MAX 1
#define MIN 2

void showHelp()
{
	fprintf(stdout, "Usage:\n");
	fprintf(stdout,
		"\tsacpickmax\n\t\t[-h (Help)]\n\t\t[-nd (don't de-mean window)]\n\t\t[-min|-max (default is to mark the global (absolute) max amplitude value)]\n\t\t<-f sac filename>\n\t\t<-i reference field>\n\t\t<-o output mark field>\n\t\t<-w window size to consider from input field>\n");
}

int main(int argc, char **argv)
{
	float *data = NULL;
	SACHEAD *hdr = NULL;
	int err, i, j;
	float picktime, maxpicktime;
	int first, last;

	/* transform to boolean someday */
	int rmean = 1;
	int markmode = ABS;
	int picktype;

	int filter=0;
	int NH=0;
	int NL=0;
	float FH, FL;
	
	char *input = NULL, *output = NULL, *filename = NULL;
	float size = -1.0;

	if (argc == 1) {
		showHelp();
		return 0;
	}

	/* check of parameters */
	for (i = 1; i < argc; i++) {
		/*
		   if ( (j = strncmp(argv[i],"-v",2)) == 0){
		   fprintf(stderr,
		   "Version: %s\n         %s\n         %s)\n",
		   version,
		   io_version,
		   hd_version
		   );
		   return 0;
		   }
		 */

		if ((j = strncmp(argv[i], "-hp", 3)) == 0 && i < argc - 1) {
		        NH=2;
			FH = atof(argv[i + 1]);
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-lp", 3)) == 0 && i < argc - 1) {
		        NL=2;
			FL = atof(argv[i + 1]);
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-min", 4)) == 0) {
			markmode = MIN;
			continue;
		}

		if ((j = strncmp(argv[i], "-max", 4)) == 0) {
			markmode = MAX;
			continue;
		}

		if ((j = strncmp(argv[i], "-h", 2)) == 0) {
			showHelp();
			return 0;
		}

		if ((j = strncmp(argv[i], "-nd", 3)) == 0) {
			rmean = 0;
			continue;
		}

		if ((j = strncmp(argv[i], "-i", 2)) == 0 && i < argc - 1) {
			input = argv[i + 1];
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-o", 2)) == 0 && i < argc - 1) {
			output = argv[i + 1];
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-w", 2)) == 0 && i < argc - 1) {
			size = atof(argv[i + 1]);
			i++;
			continue;
		}

		if ((j = strncmp(argv[i], "-f", 2)) == 0 && i < argc - 1) {
			filename = argv[i + 1];
			i++;
			continue;
		}

		fprintf(stderr,
			"Error on option %s (Unrecognized or not enough arguments.\n",
			argv[i]);
		continue;
	}

	err = 0;

	if (filename == NULL)
		err = fprintf(stderr, "E:> Invalid filename.\n");
	if (input == NULL)
		err = fprintf(stderr, "E:> Invalid input field.\n");
	if (output == NULL)
		err = fprintf(stderr, "E:> Invalid output field.\n");
	if (size == 0.0)
		err = fprintf(stderr, "E:> Size should be larger than 0.0.\n");

	if (err != 0) {
		return -1;
	}

	filter = ((NH != 0) || (NL!=0));
	
	/* data input */
	data = io_readSac(filename, &hdr);
	if (data == NULL) {
		fprintf(stderr, "Error reading file: %s\n", filename);
		return -1;
	}
        
        /* Filter data if necessary */
        if (filter) {
         float *temp = iir(data, hdr->npts, hdr->delta, NH, FH, NL, FL); 
         if (temp != NULL) {
           io_freeData(data);
           data = temp;         
         }
       }
       
	/* Check pick variables */
	err = 0;

	if (!isMarkField(output)) {
		fprintf(stderr,
			"Variable indicated for output field is not valid for markers.\n");
		err = 1;
	}

	if (!isMarkField(input)) {
		fprintf(stderr,
			"Variable indicated for input field is not valid.\n");
		err = 1;
	}

	if (err)
		return -1;

	/* find desired pick time */
	err = hdu_getValueFromChar(input, hdr, &picktime, NULL, NULL);
	if ((err != TYPE_FLOAT) || (picktime == SAC_HEADER_FLOAT_UNDEFINED)) {
		fprintf(stderr,
			"Variable %s is undefined (%f) or other error.\n",
			argv[2], picktime);
		return -1;
	}

	/* Get samples bounding the window */
	first = hdu_getNptsFromSeconds(hdr, picktime);
	last = hdu_getNptsFromSeconds(hdr, picktime + size);

	/* check bounds */
	err = 0;

	if (hdu_checkNPTS(hdr, first)) {
		fprintf(stderr, "First sample out of data bounds.\n");
		err = 1;
	}

	if (hdu_checkNPTS(hdr, last)) {
		fprintf(stderr, "Last sample out of data bounds.\n");
		err = 1;
	}

	if (last <= first) {
		fprintf(stderr, "Size of window should be larger than 0.0s\n");
		err = 1;
	}

	if (err)
		return -1;

	/* find min max */
	{
		double mean = 0.0;
		float min, max;
		int maxj, minj, count;

		j = first;
		maxj = minj = j;
		mean = max = min = data[j];
		count = 1;

		for (j = first + 1; j <= last; j++) {
			mean += data[j];
			count++;

			if (data[j] > max) {
				max = data[j];
				maxj = j;
			}
			if (data[j] < min) {
				min = data[j];
				minj = j;
			}
		}

		if (rmean == 1) {
			mean = mean / count;
			max -= mean;
			min -= mean;
		}

		switch (markmode) {
		case (ABS):
			if (rmean == 0)
				fprintf(stderr,
					"W:> ABS mode without mean removal.\n");

			if (fabs(min) > max) {
				maxpicktime = hdu_getSecondsFromNPTS(hdr, minj);
				picktype = MIN;
			} else {
				maxpicktime = hdu_getSecondsFromNPTS(hdr, maxj);
				picktype = MAX;
			}
			break;

		case (MAX):
			maxpicktime = hdu_getSecondsFromNPTS(hdr, maxj);
			picktype = MAX;
			break;

		case (MIN):
			maxpicktime = hdu_getSecondsFromNPTS(hdr, minj);
			picktype = MIN;
			break;
		}
	}

	/* report */
	fprintf(stdout,
		"%s %s ( %d / %f ) [ %.1f ( %d ) -> %.1f ( %d ) ] = %f type %s\n",
		filename, (filter==1)?"(F)":"(NF)", hdr->npts, hdr->delta, picktime, first,
		picktime + size, last, maxpicktime,
		(picktype == MAX) ? "max" : "min");

	/* write back variable */
	if ((err =
	     hdu_changeValueFromChar(hdr, output, &maxpicktime, NULL, NULL)) != 0) {
		fprintf(stderr, "Could not save max amp pick to %s.", output);
		return -1;
	} else {
		char ctemp[16];
		sprintf(ctemp, "K%s", output);
		hdu_changeValueFromChar(hdr, ctemp, NULL, NULL, "AUTO PICK");
	}

	/* write sac hdr */
	io_writeSacHead(filename, hdr);

        /* Write the filtered trace ? */
        /* io_writeSac("lala.sac", hdr, data); */

	/* quit */
	io_freeData(data);
	io_freeData(hdr);
	return 0;
}
