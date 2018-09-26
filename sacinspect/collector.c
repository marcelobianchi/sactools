#include <collector.h>
#include <inspect.h>

#include <sac.h>
#include <globers.h>
#include <libgen.h>

int collectorVerbose = 0;

void calculateResiduals(events * evs);

void calculateEventMean(events * evs);

void calculateMeanEventCorrelation(events * evs, float mms);

/*****************************************************************************************/
/* Station information                                                                   */
/*****************************************************************************************/
//Compare two station objects by name
static int cmpstation(const void *p1, const void *p2)
{
	station *t1 = *(station **) p1;
	station *t2 = *(station **) p2;
	//fprintf(stderr, "%s AND %s\n", t1->name, t2->name);
	return strcasecmp(t1->name, t2->name);
}

// Find a station Id by the name and net
int getStationId(stations * ss, char *name, char *net)
{
	int Id = -1;
	int j, aux;

	for (j = 0; j < ss->n; j++) {
		if (strlen(name) == strlen(ss->slist[j]->name))
			if (net == NULL || strlen(net) == strlen(ss->slist[j]->net))
				if ((aux =
					 strncasecmp(name, ss->slist[j]->name,
								 strlen(name))) == 0)
					if (net == NULL
						|| (aux =
							strncasecmp(net, ss->slist[j]->net,
										strlen(net))) == 0)
						return j;
	}

	return Id;
}

// New station list from a glob structure
stations *newStationList(glob_t *glb)
{
	stations *ss;
	int j, file;

	ss = malloc(sizeof(stations));
	ss->n = 0;
	ss->slist = NULL;

	/* Prepare stations table */
	for (j = 0; j < glb->gl_pathc; j++) {
		glob_t *glbs = filelist(glb->gl_pathv[j], getConfigAsString(config, NAME_Z, DEFAULT_Z));

		for (file = 0; file < glbs->gl_pathc; file++) {
			station *s = loadStation(glbs->gl_pathv[file]);
			int Id = getStationId(ss, s->name, s->net);
			if (Id == -1) {
				addss(ss, s);
			} else {
				killStation(s);
			}
		}
		glbs = killGlob(glbs);
	}

	qsort(&ss->slist[0], ss->n, sizeof(ss->slist[0]), cmpstation);
	return ss;
}

// Load a station file as used by the tomography code
stations *readStationList(char *filename)
{
	stations *ss;
	int j;
	FILE *in;

	float lon, lat, alt;
	char name[200], net[200];

	ss = malloc(sizeof(stations));
	ss->n = 0;
	ss->slist = NULL;

	in = fopen(filename, "r");
	if (in == NULL)
		return ss;

	j = fscanf(in, "%f\t%f\t%f\t%s\t%s", &lon, &lat, &alt, name, net);
	while (j == 5) {
		station *s = newStation(lon, lat, alt, name, net);
		if (s == NULL)
			return NULL;
		addss(ss, s);

		j = fscanf(in, "%f\t%f\t%f\t%s\t%s", &lon, &lat, &alt, name, net);
	}

	fclose(in);
	return ss;
}

//Free up the station list
stations *killStations(stations * ss)
{
	if (ss == NULL)
		return NULL;

	if (ss->slist != NULL) {
		int s;
		for (s = 0; s < ss->n; s++)
			ss->slist[s] = killStation(ss->slist[s]);
		free(ss->slist);
		ss->slist = NULL;
	}

	free(ss);
	return NULL;
}

// Add a station to a station list
void addss(stations * ss, station * s)
{
	if (ss == NULL)
		return;

	ss->n++;
	ss->slist = realloc(ss->slist, sizeof(station *) * ss->n);
	ss->slist[ss->n - 1] = s;

	return;
}

// Create a station
station *newStation(float lon, float lat, float el, char *name, char *net)
{
	station *s = NULL;

	s = malloc(sizeof(station));

	s->lat = lat;
	s->lon = lon;
	// Elevation should be in KM and + is down !
	s->el = el;

	s->name = NULL;
	s->net = NULL;

	if (name != NULL) {
		s->name = malloc(sizeof(char) * strlen(name));
		strcpy(s->name, name);
	}

	if (net != NULL) {
		s->net = malloc(sizeof(char) * strlen(net));
		strcpy(s->net, net);
	}

	return s;
}

// Create a station from a sacfile
station *loadStation(char *filename)
{
	station *s = NULL;

	SACHEAD *h = io_readSacHead(filename);
	if (h == NULL) {
		fprintf(stderr, "On Station file %s not found.\n", filename);
		return s;
	}


	s = malloc(sizeof(station));

	s->lat = h->stla;
	s->lon = h->stlo;
	// Elevation should be in KM and + is down !
	s->el = -h->stel / 1000.0;
	s->name = NULL;
	s->net = NULL;

	//  fprintf(stderr, "File: %s ", filename);
	hdu_getValueFromChar("kstnm", h, NULL, NULL, &s->name);
	hdu_getValueFromChar("knetwk", h, NULL, NULL, &s->net);

	h = io_freeData(h);

	return s;
}

// Free up memory of the station
station *killStation(station * s)
{
	if (s == NULL)
		return NULL;
	if (s->net != NULL)
		free(s->net);
	if (s->name != NULL)
		free(s->name);
	s->net = NULL;
	s->name = NULL;
	free(s);

	return NULL;
}

// Dump the station list
void printss(stations * ss)
{
	int j;
	for (j = 0; j < ss->n; j++) {
		fprintf(stderr, "%d %s.%s\n", j, ss->slist[j]->net,
				ss->slist[j]->name);
	}
}

// Create a station file
void writeoutstation(stations * ss)
{
	int i;
	FILE *out;

	out = fopen("station.lst", "w");
	for (i = 0; i < ss->n; i++) {
		fprintf(out, "%f\t%f\t%f\t%s\t%s\n", ss->slist[i]->lon,
				ss->slist[i]->lat, ss->slist[i]->el, ss->slist[i]->name,
				ss->slist[i]->net);
	}
	fclose(out);
}

/*****************************************************************************************/


/*****************************************************************************************/
/* Event information                                                                     */
/*****************************************************************************************/

// New event list from a glob
events *newEventList(glob_t *glb, stations * ss, float mms)
{
	events *evs;
	int j, file;

	evs = malloc(sizeof(events));
	evs->n = 0;
	evs->elist = NULL;

	/* Make event list */
	for (j = 0; j < glb->gl_pathc; j++) {
		glob_t *glbs = filelist(glb->gl_pathv[j], getConfigAsString(config, NAME_Z, DEFAULT_Z));

		if (collectorVerbose)
			fprintf(stdout, "%s: [ ] [   ] ", glb->gl_pathv[j]);
		
		event *ev = NULL;
		for (file = 0; file < glbs->gl_pathc; file++) {
			if (file == 0) {
				ev = loadEvent(glbs->gl_pathv[file]);
				if (ev == NULL) {
					fprintf(stderr, "Could not get event.\n");
					exit(-1);
				}
			}

			pick *p = loadPick(glbs->gl_pathv[file], ss);
			if (p != NULL) {
				addpick(ev, p);
				if (collectorVerbose) fprintf(stdout, ".");
			} else {
				if (collectorVerbose) fprintf(stdout, "x");
			}

		}


		if (ev != NULL && ev->n != 0) {
			addev(evs, ev);
			if (collectorVerbose) fprintf(stdout, "\r%s: [A] [%03d] \n", glb->gl_pathv[j], ev->n);
		} else {
			killEvent(ev);
			if (collectorVerbose) fprintf(stdout, "\r%s: [D] [%03d] \n", glb->gl_pathv[j], 0);
		}
		
		glbs = killGlob(glbs);
	}

	calculateEventMean(evs);
	calculateResiduals(evs);
	calculateMeanEventCorrelation(evs, mms);

	return evs;
}

// Create one event from a sac filename
event *loadEvent(char *sacFilename)
{
	event *e = NULL;
	int i;
	
	SACHEAD *h = io_readSacHead(sacFilename);
	if (h == NULL) {
		fprintf(stderr, "On Event file %s not found.\n", sacFilename);
		return e;
	}

	e = malloc(sizeof(event));

	e->Id = NULL;
	hdu_getValueFromChar("kevnm", h, NULL, NULL, &e->Id);
	// if no id use the folder name
	if ((i = strncmp(e->Id, SAC_HEADER_CHAR_UNDEFINED, strlen(SAC_HEADER_CHAR_UNDEFINED)) == 0)) {
		char *filenamecopy = malloc(sizeof(char) * (strlen(sacFilename)+1));
		strcpy(filenamecopy, sacFilename);

		char *basefolder;
		basefolder = dirname(filenamecopy);
		
		free(e->Id);
		e->Id = malloc(sizeof(char)*(strlen(basefolder)+1));
		strcpy(e->Id, basefolder);

		free(filenamecopy);
		filenamecopy = NULL;
		fprintf(stderr, "Warning, no kevnm defined using the folder name.\n");
	}

	e->lat = h->evla;
	e->lon = h->evlo;
	e->depth = h->evdp;
	e->magnitude = h->mag;
	e->n = 0;
	e->resMean = 0.0;
	e->resStd = 0.0;
	e->hasMean = 0;
	e->picks = NULL;

	h = io_freeData(h);

	return e;
}

// Free up memory from one event
event *killEvent(event * ev)
{
	int p;

	if (ev == NULL)
		return NULL;

	if (ev->picks != NULL) {
		for (p = 0; p < ev->n; p++)
			ev->picks[p] = killPick(ev->picks[p]);

		free(ev->picks);
		ev->picks = NULL;
	}

	if (ev->Id != NULL)
		free(ev->Id);
	ev->Id = NULL;

	free(ev);

	return NULL;
}

// Free up memory from events
events *killEvents(events * evs)
{
	if (evs == NULL)
		return NULL;

	if (evs->elist != NULL) {
		int e;
		for (e = 0; e < evs->n; e++) {
			evs->elist[e] = killEvent(evs->elist[e]);
		}
		free(evs->elist);
		evs->elist = NULL;
	}

	free(evs);
	return NULL;
}

// Add an event to an event list
void addev(events * evs, event * ev)
{
	if (evs == NULL)
		return;

	evs->n++;
	evs->elist = realloc(evs->elist, sizeof(event *) * evs->n);
	evs->elist[evs->n - 1] = ev;

	return;
}

// Write out an eventlist 
long writeoutevent(events * evs, stations * ss, float minStd, float maxStd)
{
	int e, p;
	FILE *out;
	long outliar = 0;
	int count = 0;

	out = fopen("eventpick.lst", "w");

	for (e = 0; e < evs->n; e++) {
		event *ev = evs->elist[e];
		count = 0;

		/* Quick hack to print the correct number of picks for each event */
		for (p = 0; p < ev->n; p++) {
			pick *pk = ev->picks[p];
			float nStd = fabs(pk->difference - ev->resMean) / ev->resStd;
			if (((nStd >= minStd) && (nStd <= maxStd))
				|| (minStd == maxStd)) {
				count++;
			}
		}

		fprintf(out, "%f\t%f\t%f\t%d %s\n", ev->lon, ev->lat, ev->depth,
				count, ev->Id);

		for (p = 0; p < ev->n; p++) {
			pick *pk = ev->picks[p];
			float nStd_signal = (pk->difference - ev->resMean) / ev->resStd;
			float nStd = fabs(pk->difference - ev->resMean) / ev->resStd;
			if (((nStd >= minStd) && (nStd <= maxStd))
				|| (minStd == maxStd)) {
				fprintf(out, "%d\t%d\t%f\t%f\t%s Res/Std: %.2f\n",
						pk->phase, pk->station + 1, pk->tobs, pk->tref,
						ss->slist[pk->station]->name, nStd_signal);
			} else {
				if (collectorVerbose)
					fprintf(stderr, "%s %s %.2f\n", ev->Id,
							ss->slist[pk->station]->name, nStd);
				outliar++;
			}
		}
	}

	fclose(out);
	return outliar;
}

/*****************************************************************************************/

/*****************************************************************************************/
/* Picks                                                                                 */
/*****************************************************************************************/
// Free up the memory used by a pick
pick *killPick(pick * p)
{
	if (p == NULL)
		return NULL;
	
	p->ev = NULL;
	free(p);
	
	return NULL;
}

// Add a pick to one event
void addpick(event * ev, pick * p)
{
	if (ev == NULL)
		return;

	ev->n++;
	ev->picks = realloc(ev->picks, sizeof(pick *) * ev->n);
	ev->picks[ev->n - 1] = p;
	p->ev = (void *) ev;

	return;
}

// Create a pick from a sacfile
pick *loadPick(char *filename, stations * ss)
{
	char *name = NULL;
	char *net = NULL;
	int Id;

	pick *p = NULL;

	SACHEAD *h = io_readSacHead(filename);
	if (h == NULL) {
		fprintf(stderr, "On pick file %s not found.\n", filename);
		return p;
	}

	/* Check if the file is picked */
	if (h->f == -12345. || h->a == -12345.) {
		h = io_freeData(h);
		return p;
	}

	hdu_getValueFromChar("KSTNM", h, NULL, NULL, &name);
	hdu_getValueFromChar("KNETWK", h, NULL, NULL, &net);

	Id = getStationId(ss, name, net);

	if (Id == -1) {
		fprintf(stderr, "Station for pick %s not found.\n", filename);
		h = io_freeData(h);
		name = io_freeData(name);
		net = io_freeData(net);
		return p;
	}

	p = malloc(sizeof(pick));
	p->station = Id;
	p->phase = 1;
	p->tobs = h->f - h->o;
	p->tref = h->a - h->o;
	p->difference = p->tobs - p->tref;
	p->residual = -999.9;
	p->tcorr = h->unused8;
	
	p->gcarc = h->gcarc;
	p->baz = h->baz;

	h = io_freeData(h);
	name = io_freeData(name);
	net = io_freeData(net);

	return p;
}

/*****************************************************************************************/

/*****************************************************************************************/
/* Computation methods                                                                   */
/*****************************************************************************************/
void calculateResiduals(events * evs)
{
	int evn;
	int pkn;

	for (evn = 0; evn < evs->n; evn++) {
		event *ev = evs->elist[evn];
		for (pkn = 0; pkn < ev->n; pkn++) {
			if (ev->hasMean == 1) {
				ev->picks[pkn]->residual =
					ev->picks[pkn]->difference - ev->resMean;
			} else {
				ev->picks[pkn]->residual = -999.0;
			}
		}
	}

	return;
}

void calculateEventMean(events * evs)
{
	int evn;
	int pkn;

	for (evn = 0; evn < evs->n; evn++) {
		event *ev = evs->elist[evn];
		ev->resMean = 0.0;
		ev->resStd = 0.0;

		// Average
		for (pkn = 0; pkn < ev->n; pkn++)
			ev->resMean += ev->picks[pkn]->difference;
		ev->resMean = ev->resMean / ev->n;


		// Standard Deviation (sigma = sqrt(Var))
		for (pkn = 0; pkn < ev->n; pkn++) {
			float x = (ev->picks[pkn]->difference - ev->resMean);
			ev->resStd += x * x;
		}

		ev->resStd = sqrt(ev->resStd / (ev->n - 1));
		ev->hasMean = 1;

		// fprintf(stderr,"%f %f\n", ev->resMean, ev->resStd);
	}

	return;
}

void calculateMeanEventCorrelation(events * evs, float mms) {
	int i, j;
	for (i = 0; i < evs->n; i++) {
		double m = 0.0;
		int cnt = 0;
		for (j = 0 ; j < evs->elist[i]->n; j++) {
			pick *p = evs->elist[i]->picks[j];
			if (p->tcorr == SAC_HEADER_FLOAT_UNDEFINED) {
				m = SAC_HEADER_FLOAT_UNDEFINED;
				break;

			}else if (p->tcorr >= mms){
				m = m + p->tcorr;
				cnt++;
			}
		}
		if (m != SAC_HEADER_FLOAT_UNDEFINED) m = m / cnt;
		evs->elist[i]->ecorr = m;
	}
	return;
}

pick **collectpicksbystation(events * evs, stations * ss, int Id,
							 int *count, float minStd, float maxStd)
{
	pick **plist = NULL;
	(*count) = 0;
	int e;
	int p;

	for (e = 0; e < evs->n; e++) {
		event *ev = evs->elist[e];
		for (p = 0; p < ev->n; p++) {
			pick *pk = ev->picks[p];
			if (pk->station == Id || Id == -1) {
				float nStd =
					fabs(pk->difference - ev->resMean) / ev->resStd;

				if ((nStd >= minStd) && (nStd <= maxStd)) {
					(*count)++;
					plist = realloc(plist, sizeof(pick *) * (*count));
					plist[(*count) - 1] = pk;
					// fprintf(stderr,"a: %s %s %f \n", ev->Id, ss->slist[Id]->name, nStd);
				} else {
					// fprintf(stderr,"o: %s %s %f \n", ev->Id, ss->slist[Id]->name, nStd);
				}
			}
		}
	}
	return plist;
}
/*****************************************************************************************/
