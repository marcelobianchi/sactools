#include <inspect.h>
#include <collector.h>
#include <math.h>
#include <gr.h>

g_ctl* getMapCoordinates(stations *ss) {
	// Preparing coordinates
	g_ctl *mapctl = ctl_newctl(0.08, 0.08, 0.78, 0.84);
	int i;
	float lonmin, lonmax, latmin, latmax;
	
	latmin = 999.0;
	latmax = -999.0;
	lonmin = 999.0;
	lonmax = -999.0;
	
	for(i=0; i<ss->n; i++){
		if (ss->slist[i]->lat > latmax) latmax = ss->slist[i]->lat;
		if (ss->slist[i]->lat < latmin) latmin = ss->slist[i]->lat;
		if (ss->slist[i]->lon > lonmax) lonmax= ss->slist[i]->lon;
		if (ss->slist[i]->lon < lonmin) lonmin = ss->slist[i]->lon;
	}
	
	ctl_axismap(mapctl);
	ctl_xreset_mm(mapctl, lonmin, lonmax);
	ctl_yreset_mm(mapctl, latmin, latmax);
	
	return mapctl;
}

pick * findPickByStation(event *ev, int sid) {
	int i;
	
	for(i=0; i< ev->n; i++)
		if (ev->picks[i]->station == sid) return ev->picks[i];
	
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

pick * addLabel(pick *ppick, stations *ss, int stationId) {
	char aux[2000];
	int pos = -1.0;
	
	if (ppick != NULL) {
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

void addLegend(g_ctl * mapctl) {
	float ax[4], ay[4]; 
	
	// Bottom Right
	g_ctl *ctl = ctl_newctl(mapctl->xpos + mapctl->xsize - 0.11, mapctl->ypos + 0.01, 0.10, 0.1);
	
	// Bottom Left
	// g_ctl *ctl = ctl_newctl(mapctl->xpos + 0.01, mapctl->ypos + 0.01, 0.10, 0.1);

	ctl_yreset_mm(ctl, 0.0, 1.0);
	ctl_xreset_mm(ctl, 0.0, 1.0);
	ctl_axismap(ctl);
	ctl_resizeview(ctl);
	
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
	
	free(ctl);
	ctl_resizeview(mapctl);
}

void plotMap(int eventid, stations * ss, events *evs, g_ctl *mapctl) {
	int is;
	int i;
	
	// Which event
	event *pevent = evs->elist[eventid];

	// Axis Set to Map
	ctl_clean(mapctl);
	ctl_resizeview(mapctl);
	ctl_drawaxis(mapctl);
	
	char aux[2000];
	cpgmtxt("T",2.0,0.5,0.5,pevent->Id);
	sprintf(aux, "Longitude: %.4f Latitude: %.4f Depth:%.2f", pevent->lon, pevent->lat, pevent->depth);
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
		
		if (residual < -0.2)
			color = 4;
		
		if (residual > 0.2)
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
	int i;
	float ax,ay;
	char ch;
	
	stations *ss;
	events *evs;
	
	ss = newStationList(glb);
	evs = newEventList(glb, ss);

	g_ctl *mapctl = getMapCoordinates(ss);
	g_ctl *popup  = ctl_newctl(0.05, 0.05, 0.3, 0.3);
	
	ctl_axisnone(popup);
	ctl_xreset_mm(popup, 0.0, 1.0);
	ctl_yreset_mm(popup, 0.0, 1.0);
	
	opengr();
	
	int eventid = 1;
	pick *ppick = NULL;
	int stationId;
	while (ch != 'q')
	{
		plotMap(eventid, ss, evs, mapctl);
		addLegend(mapctl);
		ppick = addLabel(ppick, ss, stationId);
		
		// Stop to move
		ch = getonechar(&ax, &ay);
		switch(ch) {
		case('A'): {
			stationId = findNeareastStation(ss, ax, ay);
			if (stationId > 0)
				ppick = findPickByStation(evs->elist[eventid], stationId);
			break;
		}
			
		case('N'):
			eventid++;
			break;
			
		case('P'):
			eventid--;
			break;
		}
		if (eventid >= evs->n) eventid = evs->n - 1;
		if (eventid < 0) eventid = 0;
	}
	cpgclos();
}
