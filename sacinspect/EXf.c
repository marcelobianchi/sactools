/*
/*
  Module for exporting picks to Koulakov/Jackovlev code filtering picks and events 
  by correlation coef.
  It write waves travel-times.
  It only supports P-wave now.
  It has fixed weights.
*/

#include <inspect.h>
#include <collector.h>
#include <aux.h>


// Write out an eventlist 
void writeCatalog(events * evs, stations * ss, float mme, float mms) {
	int e, p;
	FILE *out;
	int count = 0;

	out = fopen("eventpick_filtered.lst", "w");

	fprintf(stderr, "Enter write catalog\n");

	for (e = 0; e < evs->n; e++) {
		event *ev = evs->elist[e];
		count = 0;

		/* Quick hack to print the correct number of "good" picks for each event */
		for (p = 0; p < ev->n; p++) {
			pick *pk = ev->picks[p];
			if (pk->tcorr >= mms){
				count++;
			}
		}

		/*if there is more than 4 good traces and ev correlation is greater than mme print out */
		if (count > 4 && ev->ecorr >= mme){
			fprintf(out, "%f\t%f\t%f\t%d\t%s \t evcorr: %.3f\n", ev->lon, ev->lat, ev->depth,
					count, ev->Id, ev->ecorr);

			for (p = 0; p < ev->n; p++) {
				pick *pk = ev->picks[p];
				float nStd_signal = (pk->difference - ev->resMean) / ev->resStd;

				if ( pk->tcorr >= mms) {
					fprintf(out, "%d\t%d\t%f\t%f\t%s \t Res/Std: %5.2f \t stcorr %.3f\n",
							pk->phase, pk->station + 1, pk->tobs, pk->tref,
							ss->slist[pk->station]->name, nStd_signal, pk->tcorr);
				}
			}
		}
	}

	fclose(out);
	return;
}

void EXf_process(glob_t * glb, float mme, float mms) {
	defs *d;
	long pCount = 0;
	int evi;

	stations *ss;
	events *evs;

	ss = newStationList(glb);
	evs = newEventList(glb, ss, mms);

	for (evi = 0; evi < evs->n; evi++)
		pCount += evs->elist[evi]->n;

	if (collectorVerbose)
		fprintf(stderr, "\n");

	writeCatalog(evs, ss, mme, mms);

	fprintf(stderr, "Total of %d Stations collected.\n", ss->n);
	fprintf(stderr, "Total of %d Events collected.\n", evs->n);
	fprintf(stderr, "Total of %ld Picks collected.\n", pCount);

	killStations(ss);
	killEvents(evs);

}
