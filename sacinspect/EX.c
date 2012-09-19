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

#include <inspect.h>
#include <collector.h>
#include <aux.h>

void EX_process(glob_t * glb, char *oldstationlist)
{
	defs *d;
	long pCount = 0;
	int evi;
	long outliars;

	stations *ss;
	events *evs;

	if (oldstationlist == NULL)
		ss = newStationList(glb);
	else
		ss = readStationList(oldstationlist);

	evs = newEventList(glb, ss);

	for (evi = 0; evi < evs->n; evi++)
		pCount += evs->elist[evi]->n;

	if (collectorVerbose)
		fprintf(stderr, "\n");
	
	fprintf(stderr, "Total of %d Stations collected.\n", ss->n);
	fprintf(stderr, "Total of %d Events collected.\n", evs->n);
	fprintf(stderr, "Total of %ld Picks collected.\n", pCount);

	if (oldstationlist == NULL)
		writeoutstation(ss);
	else
		fprintf(stderr,
				"Station list not written. Used the supplied one: %s\n",
				oldstationlist);

	float sigma = 3.0;
	outliars = writeoutevent(evs, ss, 0.0, sigma);

	fprintf(stderr, "Total of %ld picks considered.\n", pCount - outliars);
	fprintf(stderr, "Total of %ld outliars ( 0.0 to %f sigmas )\n",
			outliars, sigma);

	killStations(ss);
	killEvents(evs);
}
