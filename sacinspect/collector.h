#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <math.h>

// How it is
// Stations -> Station
// Events -> Event -> Picks -> Pick:: Station

extern int collectorVerbose;

typedef struct {
	char *name;
	char *net;
	float lat;
	float lon;
	float el;
} station;

typedef struct {
	int n;
	station **slist;
} stations;

typedef struct {
	int phase;
	int station;
	float baz, gcarc;
	float tobs;
	float tref;
	float difference;
	float residual;				/* ( tobs - tref ) / (mean event difference) */
	void *ev;
} pick;

typedef struct {
	char *Id;
	float lat;
	float lon;
	float depth;
	float resMean;
	float resStd;
	int hasMean;
	int n;
	pick **picks;
} event;

typedef struct {
	int n;
	event **elist;
} events;

// Pick
pick *killPick(pick * p);
void addpick(event * ev, pick * p);
pick *loadPick(char *filename, stations * ss);

// Station
static int cmpstation(const void *p1, const void *p2);
int getStationId(stations * ss, char *name, char *net);
void addss(stations * ss, station * s);
void writeoutstation(stations * ss);
void printss(stations * ss);

stations *newStationList(glob_t *glb);
stations *readStationList(char *filename);
station *newStation(float lon, float lat, float el, char *name, char *net);
station *loadStation(char *filename);

stations *killStations(stations * ss);
station *killStation(station * s);

// Event
void addev(events * evs, event * ev);
long writeoutevent(events * evs, stations * ss, float minStd, float maxStd);
events *newEventList(glob_t *glb, stations * ss);
event *loadEvent(char *sacFilename);
event *killEvent(event * ev);
events *killEvents(events * evs);

// Other methods
pick **collectpicksbystation(events * evs, stations * ss, int Id,
							 int *count, float minStd, float maxStd);
