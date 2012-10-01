#include <inspect.h>
#include <collector.h>
#include <math.h>
#include <gr.h>

// Color scale bad 
// it just defines the number arround zero that is white
#define NEEDFIX 0.2

pick * findPickByStation(event *ev, int sid) {
	int i;
	
	for(i=0; i< ev->n; i++) {
		if (ev->picks[i]->station == sid) return ev->picks[i];
	}
	
	return NULL;
}

int findNeareastStation(stations *ss, float ax, float ay) {
	float dist = 1000000.0;
	int index = -1;
	int i;
	
	for(i=0;i<ss->n;i++) {
		float thisdistance = sqrt( (ss->slist[i]->lon - ax) * (ss->slist[i]->lon - ax) + (ss->slist[i]->lat - ay) * (ss->slist[i]->lat - ay));
		if (thisdistance < dist) {
			dist = thisdistance;
			index = i;
		}
	}
	
	return index;
	
}

pick * addLabel(g_ctl *label, pick *ppick, stations *ss, int stationId) {
	char aux[2000];
	int pos = -1.0;
	
	if (ppick != NULL) {
		ctl_resizeview(label);
		cpgsch(0.75);
		sprintf(aux, "Distance: %.2f", ppick->gcarc);
		cpgmtxt("B", pos, 0.02, 0.0, aux);
		pos --;
		
		sprintf(aux, "Back-Az: %.2f", ppick->baz);
		cpgmtxt("B", pos, 0.02, 0.0, aux);
		pos --;
		
		sprintf(aux, "Residual: %.2f", ppick->residual);
		cpgmtxt("B", pos, 0.02, 0.0, aux);
		pos --;
		
		cpgsch(1.0);
		cpgmtxt("B", pos, 0.02, 0.0, ss->slist[stationId]->name);
		pos --;
		
		ppick = NULL;
	}

	return ppick;
}

void addLegend(g_ctl * legend) {
	float ax[4], ay[4]; 

	ctl_resizeview(legend);

	cpgsch(0.95);
	cpgqtxt(0.30, 0.80, 0.0, 0.0, "Positive", ax, ay);
	cpgsci(2);
	cpgpt1 (0.15, ( ay[0] + ay[1] ) / 2, -3);
	cpgptxt(0.30, 0.80, 0.0, 0.0, "Positive");

	cpgqtxt(0.30, 0.50, 0.0, 0.0, "Neutral", ax, ay);
	cpgsci(1);
	cpgpt1 (0.15, ( ay[0] + ay[1] ) / 2, -3);
	cpgptxt(0.30, 0.50, 0.0, 0.0, "Neutral");

	cpgqtxt(0.30, 0.20, 0.0, 0.0, "Negative", ax, ay);
	cpgsci(4);
	cpgpt1 (0.15, ( ay[0] + ay[1] ) / 2, -3);
	cpgptxt(0.30, 0.20, 0.0, 0.0, "Negative");

	cpgsci(1);
	cpgsch(1.0);
}

void drawMap(g_ctl *map, int eventid, stations * ss, events *evs) {
	int is;
	int i;
	event *pevent = NULL;

	// Which event
	if (eventid < evs->n)
		pevent = evs->elist[eventid];
	else
		return;

	// Axis Set to Map
	ctl_clean(map);
	ctl_resizeview(map);
	ctl_drawaxis(map);

	char aux[2000];
	cpgmtxt("T",2.0,0.5,0.5,pevent->Id);
	sprintf(aux, "Longitude: %.4f Latitude: %.4f Depth:%.2f", pevent->lon, pevent->lat, pevent->depth);
	if (pevent->magnitude != SAC_HEADER_FLOAT_UNDEFINED) {
		char aux2[200];
		sprintf(aux2, " Magnitude: %.1f", pevent->magnitude);
		strcat(aux, aux2);
	}
	cpgsch(0.75);
	cpgmtxt("T",1.0 , 0.5, 0.5, aux);
	cpgsch(1.0);

	// All stations
	for (is = 0; is < ss->n; is++) {
		cpgpt1(ss->slist[is]->lon, ss->slist[is]->lat, -2);
	}
	
	
	// Picked stations
	for(i=0; i < pevent->n; i++) {
		int stationId = pevent->picks[i]->station;
		
		// St symbol
		float x = ss->slist[stationId]->lon;
		float y = ss->slist[stationId]->lat;
		float residual = pevent->picks[i]->residual;
		
		int color = 1;
		
		if (residual < -NEEDFIX)
			color = 4;
		
		if (residual > NEEDFIX)
			color = 2;
		
		cpgsci(color);
		cpgpt1(x, y, -3);
		
		// St arrow
		float size = 0.1;
		float deg2rad = M_PI / 180.0;
		
		float xb = size * cos(pevent->picks[i]->baz * deg2rad);
		float yb = size * sin(pevent->picks[i]->baz * deg2rad);
		
		cpgmove(x,y);
		cpgdraw(x+xb,y+yb);

		

		cpgsci(1);
	}
}

void MAP_process(glob_t * glb)
{
	// Helpers
	int i;
	float lonmin = 999.0, lonmax = -999.0, latmin = 999.0, latmax = -999.0;
	char aux[2048];

	// Keyboard interaction
	float ax,ay;
	char ch = 'e';

	// Index
	int eventid = 0;
	int stationId = -1;

	// Graphical screen id
	int grId = -1; 

	// Graphicas sub windows
	g_ctl *mapArea = NULL;
	g_ctl *legendArea = NULL;
	g_ctl *labelArea = NULL;

	// Data pointers
	pick *ppick = NULL;
	stations *ss = NULL;
	events *evs = NULL;

	// Load Data
	ss = newStationList(glb);
	if (ss == NULL) {
		fprintf(stderr, "No stations found.\n");
		return;
	}

	evs = newEventList(glb, ss);
	if (evs == NULL) {
		fprintf(stderr,"No Events found.\n");
		return;
	}

	fprintf(stderr,"Found %d stations and %d events.\n", ss->n, evs->n);
	if (ss->n == 0 || evs->n == 0) {
		ss = killStations(ss);
		evs = killEvents(evs);
		return;
	}

	// Preparing map window
	mapArea = ctl_newctl(0.08, 0.08, 0.84, 0.84);

	for(i=0; i<ss->n; i++){
		if (ss->slist[i]->lat > latmax) latmax = ss->slist[i]->lat;
		if (ss->slist[i]->lat < latmin) latmin = ss->slist[i]->lat;
		if (ss->slist[i]->lon > lonmax) lonmax = ss->slist[i]->lon;
		if (ss->slist[i]->lon < lonmin) lonmin = ss->slist[i]->lon;
	}
	
	ctl_axismap(mapArea);
	ctl_xreset_mm(mapArea, lonmin, lonmax);
	ctl_yreset_mm(mapArea, latmin, latmax);
	// End of Mapping coordinates setup

	// Label ctl
	labelArea = ctl_newctl(mapArea->xpos + 0.02, mapArea->ypos + 0.02, 0.3, 0.3);
	ctl_axisnone(labelArea);
	ctl_xreset_mm(labelArea, 0.0, 1.0);
	ctl_yreset_mm(labelArea, 0.0, 1.0);
	// End of Label

	// Legend ctl
	legendArea = ctl_newctl(mapArea->xpos + mapArea->xsize - 0.12, mapArea->ypos, 0.12, 0.15);
	ctl_axisnone(legendArea);
	ctl_xreset_mm(legendArea, 0.0, 1.0);
	ctl_yreset_mm(legendArea, 0.0, 1.0);
	// End of Label

	// Open my graphics
	grId = opengr();

	// Loop
	while (ch != 'q')
	{
		// Main map, draws a new page
		drawMap(mapArea, eventid, ss, evs);
		// Add a legend
		addLegend(legendArea);
		// Add a label area
		ppick = addLabel(labelArea, ppick, ss, stationId);

		// Prepare the viewport to get correct coordinates
		ctl_resizeview(mapArea);

		// Wait for user
		ch = getonechar(&ax, &ay);
		switch(ch) {
		// User click on a station
		case('A'): {
			stationId = findNeareastStation(ss, ax, ay);
			if (stationId >= 0) {
				ppick = findPickByStation(evs->elist[eventid], stationId);
			}
			break;
		}

		// Next event
		case('N'): {
			eventid++;
			if (eventid >= evs->n) eventid = evs->n - 1;
			break;
		}

		// Make printout for all events
		case('O'): {
			int ans = yesno("Really generate the output plot for all events loaded ?");
			if (!ans) break;
			for(i = 0; i < evs->n; i++) {
				sprintf(aux, "%s.ps/CPS", evs->elist[i]->Id);
				cpgopen(aux);
				drawMap(mapArea, i, ss, evs);
				addLegend(legendArea);
				cpgclos();
			}
			cpgslct(grId);
		}

		// Make printout for one event
		case('o'): {
			lerchar("Choose a filename to save (enter to use evid)", aux, 2047);
			if (strncmp(&aux[strlen(aux) - 3],".ps",3) == 0) aux[strlen(aux)-3] = '\0';
			if (strlen(aux) > 0) {
				sprintf(aux, "%s.ps/CPS", aux);
			} else {
				sprintf(aux, "%s.ps/CPS", evs->elist[eventid]->Id);
			}
			cpgopen(aux);
			drawMap(mapArea, eventid, ss, evs);
			addLegend(legendArea);
			cpgclos();
			cpgslct(grId);
		}

		// Previous event
		case('P'): {
			eventid--;
			if (eventid < 0) eventid = 0;
			break;
		}

		// Quit
		case('q'): break;
		}
	}

	// Free areas
	if (labelArea) free(labelArea);
	if (legendArea) free(legendArea);
	if (mapArea) free(mapArea);

	// Free data
	ss = killStations(ss);
	evs = killEvents(evs);

	// Close graphics
	cpgclos();

	return;
}
