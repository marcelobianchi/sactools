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
#include <sac.h>
#include <gr.h>
#include <aux.h>

float minmaxrefine(tf *f, defs *d, float ax, float halfmaxdist) {
  float min,max;
  float *data;
  float start, end;
  float step;
  float w;

  int Nstart, Nend;
  int current, status;
  float pos;

  if (d->filter == 1){
    if (f->zf == NULL) return ax;
    data = f->zf;
  } else {
    data = f->z;
  }

  status = 0;

  step =  (halfmaxdist / 50.0);
  while (step < f->hz->delta) step += (halfmaxdist / 50.0);

  w = step;
  while (status == 0 && w < halfmaxdist){
    start = ax - w;
    end = ax + w;
    
    current = hdu_getNptsFromSeconds (f->hz, ax);
    Nstart = hdu_getNptsFromSeconds (f->hz, start);
    Nend = hdu_getNptsFromSeconds (f->hz, end);
    
    if (hdu_checkNPTS (f->hz, current)) return ax;
    if (hdu_checkNPTS (f->hz, Nstart)) return ax;    
    if (hdu_checkNPTS (f->hz, Nend)) return ax;
    if (Nend <= Nstart ) return ax;
  
    if ((data[Nstart] < data[current]) && (data[Nend] < data[current]))
      status = 1; /* MAX */
    else if  ((data[Nstart] > data[current]) && (data[Nend] > data[current]))
      status = 2; /* MIN */
    else {
      w += step;
    }    
  }
    
  getminmax (data, f->hz, start, end, &min, &max);

  if (status == 1) 
    pos = max;
  else if (status == 2) 
    pos = min;
  else 
    pos = ax;

  // fprintf(stderr,"%f %f\n",ax,pos);
  return pos;
}

void deletemark(tf *f, float ax) {
  f->hz->f = SAC_HEADER_FLOAT_UNDEFINED;
}

void mark(g_ctl * ctl, float x, char *c, int color)
{
  int oldcolor;
  cpgqci(&oldcolor);

  cpgsci(color);
  cpgmove(x, ctl->ymin + ctl->h*0.05);
  cpgdraw(x, ctl->ymax - ctl->h*0.05);
  cpgptxt(x, ctl->ymin + ctl->h*0.1, 0.0, -0.55, c);
  cpgsci(oldcolor);
}

void timedraw(g_ctl *ctl, tf *f, defs *d, int gravity)
{
  char txt[500];
  float *x = NULL;
  int i;

  x = malloc(sizeof(float) * f->hz->npts);
  for (i = 0; i < f->hz->npts; i++)
    x[i] = i * f->hz->delta + f->hz->b;
  
  
  float *y = (d->filter && f->zf!=NULL)?f->zf:f->z;
  ctl_yupdate_ndb(ctl, y, f->hz->npts, f->hz->delta, f->hz->b);
  ctl_clean(NULL);
  ctl_resizeview(ctl);
  ctl_axisfull(ctl);
  ctl->expand=0;
  ctl_drawaxis(ctl);
  cpgline(f->hz->npts, x, y);

  free(x);
  x = NULL;
  
  if (f->hz->a !=  SAC_HEADER_FLOAT_UNDEFINED) mark(ctl, f->hz->a, "a",2);
  if (f->hz->f !=  SAC_HEADER_FLOAT_UNDEFINED) mark(ctl, f->hz->f, "f",2);
  
  if (d->filter) {
    sprintf(txt,"%s [Filtering, bandpass from %.2f to %.2f]", (gravity) ? "Gravity On":"Gravity Off",d->hp,d->lp);
  } else {
    sprintf(txt,"%s [No Filtering]",(gravity) ? "Gravity On" : "Gravity Off");
  }  
  cpgmtxt("B",-1.,0.01,0.0,txt);

  cpgmtxt("T",3.5,0.0,0.0,"Click-Click to Zoom. Double Click to UnZoom. Q to go back (Quit Edit Mode)");
  cpgmtxt("T",2.5,0.0,0.0,"D to delete f Mark. Y/Z for marking f phase. G to enable/disable gravity.");
  cpgmtxt("T",1.5,0.0,0.0,"F to switch filter On/Off. L for Low pass frequency (Hz). H for High pass frequency (Hz).");
}

void edit_tf(g_ctl *ctl, tf *f, defs* d)
{
  /* We add a quick hack in making the filter option on/off with key 'F' */
  float ax, ay, aux;
  char op;
  int  filterneed=0;
  int gravity = 1;  

  strcpy(ctl->title,"** timeplot type Q to return to main menu **");
  strcpy(ctl->xlabel,"Time (s)");
  strcpy(ctl->ylabel,"Amp.");

  switch(d->alig){
  case (ALIGO):
  case (ALIGA):
    ctl->xmin = f->hz->a - d->prephase;
    ctl->xmax = f->hz->a + d->postphase;
    break;
  case(ALIGF):
    ctl->xmin = f->hz->f - d->prephase;
    ctl->xmax = f->hz->f + d->postphase;
    break;
  }

  while (op!='Q')
    {
      if (filterneed == 1){
	if (d->filter) filtertf(f, d);
	filterneed=0;
      }

      timedraw(ctl, f, d, gravity);
      cpgband(7,0,0.0,0.0,&ax,&ay,&op);
      op=toupper(op);
      
      switch(op)
	{
	case 'G':
	  gravity = !gravity;
	  break;

	case 'Z':	  
	case 'Y':
	  f->hz->f = (gravity)?minmaxrefine(f, d, ax, 2.):ax;
	break;
	
	case 'A':
	  aux=ax;
	  cpgband(7,0,0.0,0.0,&ax,&ay,&op);
	  if (aux<ax)
	    {
	      ctl->xmin=aux;
	      ctl->xmax=ax;
	    } else {
	    ctl->xmin=hdu_getSecondsFromNPTS(f->hz,0);
	    ctl->xmax=hdu_getSecondsFromNPTS(f->hz,f->hz->npts);
	  }
	  break;
	  
	case 'F':
	  d->filter = ! d->filter;	  
	  filterneed=1;
	  break;

	case 'D':
	  deletemark(f,ax);
	  break;

	case 'L':
	  d->lp=lerfloat("Enter Low-Pass filter frequency (Hz):");
	  filterneed=1;
	  break;
	  
	case 'H':
	  d->hp=lerfloat("Enter High-Pass filter frequency (Hz):");
	  filterneed=1;
	  break;
	}
    }
}

