#include <inspect.h>
#include <collector.h>
#include <aux.h>

/*
 * Module for exporting picks to Rawlison code.
 * It write waves travel-times.
 * It only supports P-wave now.
 * It has fixed weights.
 */

void writeSources(events *evs) {
	int i;
	FILE *sai;
	
	sai = fopen("sources.dat", "w");
	
	fprintf(sai, "1\n");
	fprintf(sai, "%d\n",evs->n);
	for(i = 0; i < evs->n; i++) {
		fprintf(sai, "%10.6f %11.6f %8.4f %s\n", 
				evs->elist[i]->lat, 
				evs->elist[i]->lon, 
				evs->elist[i]->depth, 
				evs->elist[i]->Id);
		fprintf(sai, "  P \n");
	}
	
	fclose(sai);
	return;
}

void writeReceivers(stations *ss) {
	int i;
	FILE *sai;
	
	sai = fopen("receivers.dat", "w");
	if (sai == NULL) return;
	
	fprintf(sai, "1\n");
	fprintf(sai, "%d\n", ss->n);
	for(i = 0; i < ss->n; i++) {
		fprintf(sai, "%10.6f %11.6f %8.4f %s\n", 
				ss->slist[i]->lat, 
				ss->slist[i]->lon, 
				-1 * ss->slist[i]->el, 
				ss->slist[i]->name);
	}
	fclose(sai);
	
	return;
}

pick *getPick(event *ev, int station) {
	int i;

	for(i = 0; i < ev->n; i++) {
		if (ev->picks[i]->station == station) return ev->picks[i];
	}

	return NULL;
}

void writePicks(events *evs, stations *ss) {
	int ie, is; 
	FILE *sai;
	
	sai = fopen("picks.dat", "w");
	if (sai == NULL) return;
	fprintf(sai, "1\n");
	fprintf(sai, "%d\n", evs->n*ss->n);
	for(ie = 0; ie < evs->n; ie++) {
		for(is = 0; is < ss->n; is++) {
			pick *pp = getPick(evs->elist[ie], is);
			if (pp == NULL) {
				fprintf(sai, "%d %10.6f %6.3f %s %s\n",0,-9.99,1.0,evs->elist[ie]->Id,ss->slist[is]->name);
			} else {
				fprintf(sai, "%d %10.6f %6.3f %s %s\n",1,pp->tobs,1.0,evs->elist[ie]->Id,ss->slist[is]->name);
			}
		}
	}
	fclose(sai);

	return;
}

void EXr_process(glob_t * glb) {
	defs *d;
	long pCount = 0;
	int evi;
	long outliars;

	stations *ss;
	events *evs;

	ss = newStationList(glb);
	evs = newEventList(glb, ss);

	for (evi = 0; evi < evs->n; evi++)
		pCount += evs->elist[evi]->n;

	if (collectorVerbose)
		fprintf(stderr, "\n");
	
	writeSources(evs);
	writeReceivers(ss);
	writePicks(evs, ss);
	
	fprintf(stderr, "Total of %d Stations collected.\n", ss->n);
	fprintf(stderr, "Total of %d Events collected.\n", evs->n);
	fprintf(stderr, "Total of %ld Picks collected.\n", pCount);

	killStations(ss);
	killEvents(evs);
}
