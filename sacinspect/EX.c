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
#include <aux.h>

int v = 0;

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
  float residual; /* ( tobs - tref ) / (mean event difference) */
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


static int cmpstation(const void *p1, const void *p2) {
  station *t1 = *(station **) p1;
  station *t2 = *(station **) p2;
  //fprintf(stderr, "%s AND %s\n", t1->name, t2->name);
  return strcasecmp(t1->name, t2->name);
}

void calculateResiduals(events *evs) {
  int evn;
  int pkn;
  
  for(evn = 0; evn < evs->n; evn++) {
    event *ev = evs->elist[evn];
    for(pkn = 0; pkn < ev->n; pkn++) {
      if (ev->hasMean == 1) {
	ev->picks[pkn]->residual = ev->picks[pkn]->difference - ev->resMean;
      } else {
	ev->picks[pkn]->residual = -999.0;
      }
    }
  }

  return;
}

void calculateEventMean(events *evs) {
  int evn;
  int pkn;

  for(evn = 0; evn < evs->n; evn++){
    event *ev = evs->elist[evn];
    ev->resMean = 0.0;
    ev->resStd = 0.0;
    
    // Average
    for(pkn = 0; pkn < ev->n; pkn++)
      ev->resMean += ev->picks[pkn]->difference;
    ev->resMean = ev->resMean / ev->n;

    
    // Standard Deviation (sigma = sqrt(Var))
    for(pkn = 0; pkn < ev->n; pkn++) {
      float x = (ev->picks[pkn]->difference - ev->resMean);
      ev->resStd += x*x;
    }

    ev->resStd = sqrt( ev->resStd / (ev->n - 1) );
    ev->hasMean = 1;

    // fprintf(stderr,"%f %f\n", ev->resMean, ev->resStd);
  }
  
  return;
}


long writeoutevent(events *evs, stations *ss, float minStd, float maxStd) {
  int e, p;
  FILE *out;
  long outliar = 0;
  int count = 0;

  out = fopen("eventpick.lst","w");  

  for(e=0; e < evs->n; e++){
    event *ev = evs->elist[e];
    count = 0;

    /* Quick hack to print the correct number of picks for each event */
    for(p=0; p < ev->n; p++){
      pick *pk = ev->picks[p];
      float nStd = fabs(pk->difference - ev->resMean) / ev->resStd;
      if ( ((nStd >= minStd) && (nStd<=maxStd)) || (minStd == maxStd) ){
	count++;
      }
    }    
    
    fprintf(out,"%f\t%f\t%f\t%d %s\n", ev->lon, ev->lat, ev->depth, count, ev->Id);

    for(p=0; p < ev->n; p++){
      pick *pk = ev->picks[p];
      float nStd = fabs(pk->difference - ev->resMean) / ev->resStd;
      if ( ((nStd >= minStd) && (nStd<=maxStd)) || (minStd == maxStd) ){
	fprintf(out,"%d\t%d\t%f\t%f\t%s Res/Std: %.2f\n", pk->phase, pk->station+1, pk->tobs, pk->tref, ss->slist[pk->station]->name, nStd);
      } else {
	if (v) fprintf(stderr,"%s %s %.2f\n",ev->Id,ss->slist[pk->station]->name, nStd);
	outliar++;
      }
    }
  }

  fclose(out);
  return outliar;
}

void writeoutstation(stations *ss){
  int i;
  FILE *out;

  out = fopen("station.lst","w");
  for(i=0; i < ss->n; i++) {
    fprintf(out,"%f\t%f\t%f\t%s\t%s\n", ss->slist[i]->lon, ss->slist[i]->lat, ss->slist[i]->el, ss->slist[i]->name, ss->slist[i]->net);
  }
  fclose(out);
}



void addpick(event *ev, pick* p){
  if (ev == NULL) return;

  ev->n++;
  ev->picks = realloc(ev->picks, sizeof(pick *) * ev->n);
  ev->picks[ ev->n - 1] = p;
  p->ev = (void*)ev;

  return;
}

void addev(events *evs, event *ev){
  if (evs == NULL) return;

  evs->n++;
  evs->elist = realloc(evs->elist, sizeof(event *)*evs->n);
  evs->elist[ evs->n - 1] = ev;

  return;
}

void addss(stations *ss, station *s){
  if (ss == NULL) return;

  ss->n++;
  ss->slist = realloc(ss->slist, sizeof(station *)*ss->n);
  ss->slist[ ss->n - 1] = s;

  return;
}

void printss(stations *ss){
  int j;
  for(j=0;j<ss->n;j++){
    fprintf(stderr,"%d %s.%s\n",j, ss->slist[j]->net,ss->slist[j]->name);
  }    
}


int getStationId(stations *ss, char *name, char *net){
  int Id = -1;
  int j,aux;

  for(j=0;j<ss->n;j++){
    if (strlen(name) == strlen(ss->slist[j]->name))
      if (net == NULL || strlen(net) == strlen(ss->slist[j]->net))
	if ((aux=strncasecmp(name,ss->slist[j]->name,strlen(name))) == 0)
	  if (net == NULL || (aux=strncasecmp(net,ss->slist[j]->net,strlen(net))) == 0)
	    return j;
  }

  return Id;
}

pick *loadPick(char *filename, stations *ss) {
  char *name = NULL;
  char *net = NULL;
  int Id;

  pick *p = NULL;

  SACHEAD *h =  io_readSacHead(filename);
  if (h == NULL) {
    fprintf(stderr,"On pick file %s not found.\n", filename);
    return p;
  }
  
  /* Check if the file is picked */
  if (h->f == -12345. || h->a == -12345.) return p;

  hdu_getValueFromChar ("KSTNM", h, NULL, NULL, &name);
  hdu_getValueFromChar ("KNETWK", h, NULL, NULL, &net);

  if (strncmp(name,"BB23",4) == 0) {
	fprintf(stderr,"Stations BB23 rejected. %s\n", filename);
	h = io_freeData(h);
	name = io_freeData(name);
	net = io_freeData(net);
	return p;
  }

  if (strncmp(name,"BB05",4) == 0) {
	fprintf(stderr,"Stations BB05 rejected. %s\n", filename);
	h = io_freeData(h);
	name = io_freeData(name);
	net = io_freeData(net);
	return p;
  }

  if (strncmp(name,"BB02",4) == 0) {
	fprintf(stderr,"Stations BB02 rejected.\n");
	h = io_freeData(h);
	name = io_freeData(name);
	net = io_freeData(net);
	return p;
  }

  Id = getStationId(ss, name, net);

  if (Id == -1) {
    fprintf(stderr,"Station for pick %s not found.\n", filename);
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

  p->gcarc = h->gcarc;
  p->baz = h->baz;

  h = io_freeData(h);
  name = io_freeData(name);
  net = io_freeData(net);

  return p;
}

event *loadEvent(char *filename) {
  event *e = NULL;

  SACHEAD *h =  io_readSacHead(filename);
  if (h == NULL) {
    fprintf(stderr,"On Event file %s not found.\n", filename);
    return e;
  }
  
  e = malloc(sizeof(event));

  e->Id = NULL;
  hdu_getValueFromChar ("kevnm", h, NULL, NULL, &e->Id);

  e->lat = h->evla;
  e->lon = h->evlo;
  e->depth = h->evdp;
  e->n = 0;
  e->resMean = 0.0;
  e->resStd = 0.0;
  e->hasMean = 0;
  e->picks = NULL;

  h = io_freeData(h);

  return e;
}

pick *killPick(pick *p) {
  p->ev = NULL;
  if (p != NULL) free(p);
  
  return NULL;
}

station *newStation(float lon, float lat, float el, char *name, char *net){
  station *s = NULL;

  s = malloc(sizeof(station));
  
  s->lat = lat;
  s->lon = lon;
  // Elevation should be in KM and + is down !
  s->el = el;

  s->name = NULL;
  s->net = NULL;

  if (name != NULL) {
    s->name = malloc(sizeof(char)*strlen(name));
    strcpy(s->name, name);
  }
  
  if (net != NULL) {
    s->net = malloc(sizeof(char)*strlen(net));
    strcpy(s->net, net);
  }
  
  return s;
}

station *loadStation(char *filename) {
  station *s = NULL;

  SACHEAD *h =  io_readSacHead(filename);
  if (h == NULL) {
    fprintf(stderr,"On Station file %s not found.\n", filename);
    return s;
  }
  
  
  s = malloc(sizeof(station));
  
  s->lat = h->stla;
  s->lon = h->stlo;
  // Elevation should be in KM and + is down !
  s->el = - h->stel / 1000.0;
  s->name = NULL;
  s->net  = NULL;
  
  //  fprintf(stderr, "File: %s ", filename);
  hdu_getValueFromChar ("kstnm", h, NULL, NULL, &s->name);
  hdu_getValueFromChar ("knetwk", h, NULL, NULL, &s->net);

  h = io_freeData(h);

  return s;
}

station * killStation(station *s){
  if (s == NULL) return NULL;
  if (s->net != NULL) free(s->net);
  if (s->name != NULL) free(s->name);
  s->net = NULL;
  s->name = NULL;    
  free(s);
  
  return NULL;
}

event *killEvent(event *ev) {
  int p;

  if (ev==NULL) return NULL;

  for(p=0;p<ev->n;p++)
    ev->picks[p] = killPick(ev->picks[p]);

  if (ev->picks != NULL) free(ev->picks);
  ev->picks = NULL;

  if (ev->Id != NULL) free(ev->Id);
  ev->Id = NULL;

  free(ev);

  return NULL;
}

stations *killStations(stations *ss) {
  int s;

  if (ss==NULL) return NULL;

  for(s=0;s<ss->n;s++)
    ss->slist[s] = killStation(ss->slist[s]);

  if (ss->slist != NULL) free(ss->slist);
  ss->slist=NULL;

  free(ss);
  return NULL;
}

events *killEvents(events *evs) {
  int e;

  if (evs == NULL) return NULL;

  for(e=0;e<evs->n;e++)
    evs->elist[e] = killEvent(evs->elist[e]);    

  if (evs->elist !=NULL) free(evs->elist);
  evs->elist = NULL;

  if (evs != NULL) free(evs);

  return NULL;
}

pick **collectpicksbystation(events *evs, stations *ss, int Id, int *count, float minStd, float maxStd) {
  pick **plist = NULL;
  (*count) = 0;
  int e;
  int p;

  for(e=0; e < evs->n; e++){
    event *ev = evs->elist[e];
    for(p=0; p < ev->n; p++){
      pick *pk = ev->picks[p];
      if (pk->station == Id || Id == -1){
	float nStd = fabs(pk->difference - ev->resMean)/ev->resStd;

	if ((nStd >= minStd) && (nStd <= maxStd)) {
	  (*count)++;
	  plist = realloc(plist, sizeof(pick)*(*count));
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

stations * readStationList(char *filename) {
  stations *ss;
  int j;
  FILE *in;

  float lon, lat, alt;
  char name[200], net[200];
  
  ss = malloc(sizeof(stations));
  ss->n = 0;
  ss->slist = NULL;
  
  in = fopen(filename,"r");
  if (in == NULL) return ss;
  
  j = fscanf(in,"%f\t%f\t%f\t%s\t%s",&lon,&lat,&alt,name,net);  
  while(j==5) {
    station *s = newStation(lon,lat,alt,name,net);
    if (s == NULL) return NULL;
    addss(ss, s);

    j = fscanf(in,"%f\t%f\t%f\t%s\t%s",&lon,&lat,&alt,name,net);
  }
    
  fclose(in);
  return ss;
}

stations * newStationList(defs *d) {
  stations *ss;
  int j, file;
  
  ss = malloc(sizeof(stations));
  ss->n = 0;
  ss->slist = NULL;
  
  /* Prepare stations table */
  for (j = 0; j < d->glb->gl_pathc; j++) {
    d->currentdir = j;
    glob_t *glbs = saclist(d);
        
    for(file = 0; file < glbs->gl_pathc; file++) {
      station *s = loadStation(glbs->gl_pathv[file]);
      int Id = getStationId(ss, s->name, s->net);
      if (Id == -1) {
	addss(ss, s);
      } else {
	killStation(s);
      }
    }
    globfree(glbs);
    glbs = NULL;
  }
  
  qsort(&ss->slist[0], ss->n, sizeof(ss->slist[0]), cmpstation);  
  return ss;
}

events *newEventList(defs *d, stations *ss){
  events *evs;
  int j, file;
 
  evs = malloc(sizeof(events));
  evs->n = 0;
  evs->elist = NULL;
  
  /* Make event list */
  for (j=0; j < d->glb->gl_pathc; j++) {
    d->currentdir = j;
    glob_t *glbs = saclist(d);
        
    if (v) fprintf(stdout,"%s: [ ] [   ] ", d->glb->gl_pathv[d->currentdir]);
    event *ev = NULL;
    for(file=0; file < glbs->gl_pathc; file++) {
      if (file == 0) {
	ev = loadEvent(glbs->gl_pathv[file]);
	if (ev == NULL) {
	  fprintf(stderr,"Could not get event.\n");
	  exit(-1);
	}
      }

      pick *p = loadPick(glbs->gl_pathv[file], ss);
      if (p != NULL){
	addpick(ev, p);
	if (v) fprintf(stdout,".");
      }	else {
	if (v) fprintf(stdout,"x");
      }

    }
    
    
    if (ev->n != 0) {
      addev(evs,ev);
      if (v) fprintf(stdout,"\r%s: [A] [%03d] \n",d->glb->gl_pathv[d->currentdir], ev->n);
    } else {
      if (v) fprintf(stdout,"\r%s: [D] [%03d] \n",d->glb->gl_pathv[d->currentdir], ev->n);
    }
    
    globfree(glbs);
    glbs = NULL;
  }
  
  calculateEventMean(evs);
  calculateResiduals(evs);

  return evs;
}

void realhistogram(float *x,  int n, float Smin, float Smax, int nbin){
  float min, max;
  char string[500];
  int i;

  if (n == 0)  return;
  if (nbin == 0) return;

  min = max = x[0];
  for(i=0; i < n; i++) {
    if (x[i]<min) min = x[i];
    if (x[i]>max) max = x[i];
  }  
  
  if (Smin == Smax)
    cpghist(n, x, min, max, nbin, 4);
  else
    cpghist(n, x, Smin, Smax, nbin, 4);
      
  sprintf(string,"%.1f/%.1f",min, max);
  cpgmtxt("R", 1.5, 0.5, 0.5,string);
}


void makehistorgram(pick **plist, int n, float Smin, float Smax, int nbin, int type) {
  float *x;
  int i;
  
  x = malloc(sizeof(float)*n);
  
  switch(type){
  case(0):
    for(i=0; i < n; i++)
      x[i] = plist[i]->residual;
    break;

  case(1):
    for(i=0; i < n; i++){
      float Std =  ((event*)(plist[i]->ev))->resStd;
      x[i] = fabs(plist[i]->residual) / Std;
    }
    break;
  }

  realhistogram(x,n,Smin,Smax,nbin);

  free(x);
  x = NULL;
}

void EX_process(glob_t *glb, char *oldstationlist) {
  defs *d;
  long pCount = 0;
  int evi;
  long outliars; 

  stations *ss;
  events *evs;

  d = newdefs(glb);

  if(oldstationlist == NULL)    
    ss = newStationList(d);
  else
    ss = readStationList(oldstationlist);

  evs = newEventList(d, ss);
  
  for(evi=0;evi<evs->n;evi++)
    pCount += evs->elist[evi]->n;
    
  if (v) fprintf(stderr,"\n");
  fprintf(stderr,"Total of %d Stations collected.\n", ss->n);
  fprintf(stderr,"Total of %d Events collected.\n", evs->n);
  fprintf(stderr,"Total of %ld Picks collected.\n", pCount);  
  
  if (oldstationlist == NULL) 
    writeoutstation(ss);
  else
    fprintf(stderr,"Station list not written. Used the supplied one: %s\n",oldstationlist);
  
  outliars = writeoutevent(evs,ss,0.0,3.0);

  fprintf(stderr,"Total of %ld picks considered.\n",pCount - outliars);
  fprintf(stderr,"Total of %ld outliars ( 0.0 to 3.0 sigmas )\n", outliars);

  killStations(ss);
  killEvents(evs);
}

void drawaz(float size, float interval, float anot) {
  int i;
  char string[500];

  cpgsfs(2);   
  cpgenv(-size, size, -size, size, 1, -2);
  
  for(i=1;(i*interval) < size;i++)
    cpgcirc(0.0, 0.0, (float)i*interval);
  
  for(i=1;(i*anot) < size;i++){
    sprintf(string, "%.2f", i*anot);
    cpgptxt(-i*anot*1.03*sin(45*M_PI/180), i*anot*1.03*cos(45*M_PI/180), 45.0, 0.5, string);
  }

  cpgcirc(0.0, 0.0, size);
  sprintf(string, "%.2f", size);
  cpgptxt(-size*1.04*sin(45*M_PI/180), size*1.04*cos(45*M_PI/180), 45.0, 0.5, string);
  
  cpgmove(0.0,-size);
  cpgdraw(0.0,size);

  cpgmove(-size,0.0);
  cpgdraw( size,0.0);

  cpgptxt(0.0 , size * 1.02, 0.0, 0.5, "N");
  cpgptxt(0.0 ,-(size * 1.06), 0.0, 0.5, "S");
  cpgptxt( size, 0.0, 0.0, -.5, "E");
  cpgptxt(-size, 0.0, 0.0, 1.5, "W");
}

void EXAZ_process(glob_t *glb, char *stname){
  defs *d;
  stations *ss;
  events *evs;
  char ch;
  float ay;
  int n;
  long pCount = 0;
  pick **plist;
  int i;

  d = newdefs(glb);
  ss = newStationList(d);
  evs = newEventList(d, ss);

  for(n=0;n<evs->n;n++)
    pCount += evs->elist[n]->n;
  
  if (v) fprintf(stderr,"\n");
  fprintf(stderr,"Total of %d Stations collected.\n", ss->n);
  fprintf(stderr,"Total of %d Events collected.\n", evs->n);
  fprintf(stderr,"Total of %ld Picks collected.\n", pCount);  

  opengr();
  if (stname == NULL) { return; }

  int ID=getStationId(ss, stname, NULL);
  plist = collectpicksbystation(evs, ss, ID , &n, 0, 3.0);

  cpgsubp(2,1);
  makehistorgram(plist,n,-4,4,31,0);

  cpgsch(0.675);
  drawaz(2.0, 0.5, 1.0);

  cpgsch(1.0);
  cpgmtxt("T", 2.0, 0.5, 0.5, ss->slist[ID]->name);
  cpgsch(0.675);

  for(i=0;i<n;i++){
    // float x = plist[i]->gcarc*sin(plist[i]->baz*M_PI/180.0);
    // float y = plist[i]->gcarc*cos(plist[i]->baz*M_PI/180.0);
    
    float x = fabs(plist[i]->residual)*sin(plist[i]->baz*M_PI/180.0);
    float y = fabs(plist[i]->residual)*cos(plist[i]->baz*M_PI/180.0);

    int color = 1;
    if (plist[i]->residual < -0.2)
      color = 4;
    if (plist[i]->residual > 0.2) 
      color = 2;

    cpgsci(color); // blue;
    cpgsch(1.2);
    cpgpt1(x, y ,-3);
    cpgsci(1);
  }  
  ch = getonechar(&ay, &ay);
  cpgclos();  
}

void EXHIST_process(glob_t *glb){
  defs *d;
  stations *ss;
  events *evs;
  int n;    
  long totaln = 0;
  long pCount = 0;
  int panel, sta;
  
  float ay;
  char ch;
  
  d = newdefs(glb);
  ss = newStationList(d);
  evs = newEventList(d, ss);

  for(n=0;n<evs->n;n++)
    pCount += evs->elist[n]->n;
      
  if (v) fprintf(stderr,"\n");
  fprintf(stderr,"Total of %d Stations collected.\n", ss->n);
  fprintf(stderr,"Total of %d Events collected.\n", evs->n);
  fprintf(stderr,"Total of %ld Picks collected.\n", pCount);  
  
  d->offset = 0;
  
  // opengr();
  cpgopen("/cps");

  cpgsch(2.5);
  cpgsubp(11,7);
  d->max = 77;
  for (panel=0; ((panel < d->max)) && ((panel + d->offset)) < ss->n; panel++) {
    sta = d->offset + panel;

    // Collect
    pick **plist = collectpicksbystation(evs, ss, sta , &n, 0, 3.0);
    totaln += n;
    if (plist == NULL) continue;
        
    // Plot
    if (v) fprintf(stderr,"Hist %d (%s %s)\n", sta, ss->slist[sta]->net, ss->slist[sta]->name);
    makehistorgram(plist,n,-4,4,31,0);

    cpgsch(3);
    if (panel % 10 == 0)
      cpgmtxt("L",2.5,0.5,0.5,"Count");
    
    if (panel >= 50 )
      cpgmtxt("B",3.0,0.5,0.5,"Res");
    
    cpgsch(4);
    cpgmtxt("T",0.65,0.5,0.5,ss->slist[sta]->name);
    cpgsch(2.5);
    

    // Clear
    if (plist != NULL) free(plist);
    plist = NULL;
  }
  fprintf(stderr,"Total of %ld picks considered.\n",totaln);
  fprintf(stderr,"Total of %ld outliars ( 0.0 to 3.0 sigmas )\n", pCount - totaln);

  ch = getonechar(&ay, &ay);
  cpgclos();

  /* All stations histogram
  opengr();
  pick **plist;
  cpgsch(2.5);
  cpgsubp(2,1);

  plist = collectpicksbystation(evs, ss, -1, &n, 0, 3.5);
  makehistorgram(plist, n, -4, 4, 31,0);

  plist = collectpicksbystation(evs, ss, -1, &n, 3.01, 100.0);
  makehistorgram(plist, n, 0, 10, 20,1);

  ch = getonechar(&ay, &ay);
  cpgclos();
  */

  killStations(ss); 
  killEvents(evs);  
}

