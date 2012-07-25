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

#include <interaction.h>

char message[2048];

void alert(int level)
{
 float ax,ay;
 char op;
 
 cpgsubp(1,1);
 cpgsvp(0.4,0.9,0.15,0.25);
 cpgswin(0.0,1.0,0.0,1.0); 

 cpgsci(level);
 cpgrect(0.0,1.0,0.0,1.0);
 cpgsci(0);
 cpgrect(0.03,.97,0.03,0.97);
 cpgsci(1);

 cpgmtxt("T",-1.0,0.02,.0,message);
 cpgmtxt("B",-1.0,0.02,.0,"Type 'q' or <ENTER> to quit");
 while (op!='Q' && op!=13)
 {
  cpgband (7, 0, 0, 0, &ax, &ay, &op);
  op=toupper(op);
 }
 
 if (level == ERROR) exit(-1);
}

int getmouse(int a, int b, int c, int d, char *message)
{
   char text[256];   
   float ax, ay;
   char ch[2];
   
   cpgeras();
   cpgsvp(0.1,0.9,0.1,0.90);
   cpgswin(-1.0,1.0,-1.0,1.0); 

   cpgrect(-1.00,1.0,-1.0,1.0);
   cpgsci(0);
   cpgrect(-0.98,0.98,-0.98,0.98);
   cpgsci(1);

   sprintf(text,"(%d)",a);
   cpgmtxt("T",1.0,0.5,.5,text);
   sprintf(text,"(%d)",b);
   cpgmtxt("B",1.0,0.5,.5,text);
   sprintf(text,"(%d)",c);
   cpgmtxt("L",1.0,0.5,.5,text);
   sprintf(text,"(%d)",d);
   cpgmtxt("R",1.0,0.5,.5,text);
   
   cpgptxt(0.0,0.0,0.0,0.5,message);

   cpgband (0, 0, 0, 0, &ax, &ay, &ch[0]);
   if ( fabs(ax) > fabs(ay) )
   {
     if (ax > 0.0)
       return d;
     else
       return c;
   } else
     if (ay > 0.0)
       return a;
     else
       return b;
}


/* read float number from PGPLOT */
float lerfloat(char *text)
{
 float ax,ay; 
 char ch[2]="0";
 float num;
 char retorno[128];

 int sgn=1;
 int integer=0;
 int frac=0;
 int fracd=1;
 int isfrac=0;
 
 ch[1]='\0';
 while (ch[0]!=13)
 {
   cpgsubp(1,1);
   cpgsvp(0.4,0.9,0.10,0.30);
   cpgswin(0.0,1.0,0.0,1.0); 

   cpgrect(0.0,1.0,0.0,1.0);
   cpgsci(0);
   cpgrect(0.01,.99,0.01,0.99);
   cpgsci(1);

   cpgmtxt("T",-1.0,0.02,.0,text);

   num=sgn * (double)integer+((double)(frac))/((double)fracd);
   sprintf(retorno,"%f",num);
   cpgmtxt("T",-2.0,0.02,.0,retorno);
   cpgband (7, 0, 0, 0, &ax, &ay, &ch[0]);
   ch[1]='\0'; 

   switch(ch[0])
   {
    case '.':
     isfrac=1;
    break;;
    
    case 13:
    break;
    
    case 8:
      if (!isfrac){
	integer=integer/10;
      }else{
	if (frac==0) {
	  isfrac=0;
	  fracd=1;
	} else {
	 frac=frac/10;
	 fracd=fracd/10;
	}
      }
    break;
    
    case '-':
      sgn=-sgn;
    break;
   
    default:
     if(!isfrac)
       integer=integer*10+atoi(ch);
     else {
       frac=frac*10+atoi(ch);
       fracd=fracd*10;
     }
   }
 }
 return num;
}

void lerchar(char *text, char *output, int max)
{
 char ch[2]="0";
 char retorno[128];
 int pos;
 float ax,ay;
   
 ch[1]='\0';
 pos=0;
 output[pos]='\0';
 while (ch[0]!=13)
 {
   cpgsubp(1,1);
   cpgsvp(0.4,0.9,0.10,0.30);
   cpgswin(0.0,1.0,0.0,1.0); 

   cpgrect(0.0,1.0,0.0,1.0);
   cpgsci(0);
   cpgrect(0.01,.99,0.01,0.99);
   cpgsci(1);

   cpgmtxt("T",-1.0,0.02,.0,text);

   sprintf(retorno,"%s",output);
   cpgmtxt("T",-2.0,0.02,.0,retorno);
   cpgband (7, 0, 0, 0, &ax, &ay, &ch[0]);
   ch[1]='\0'; 

   switch(ch[0])
   {
    case 8:
     pos--;
    break;
    
    case 13:
       output[pos]='\0';
    break;
    
    default:
     if (pos<(max-1))
     { 
      output[pos]=ch[0];
      pos++;
     }
    break;
   }
   output[pos]='\0';
 }
}

/* read intnumber from PGPLOT */
int lerint(char *text)
{
 float ax,ay; 
 char ch[2]="0";
 int num=0;
 char retorno[128];
 
 ch[1]='\0';
 while (ch[0]!=13)
 {
   cpgsubp(1,1);
   cpgsvp(0.4,0.9,0.10,0.30);
   cpgswin(0.0,1.0,0.0,1.0); 

   cpgrect(0.0,1.0,0.0,1.0);
   cpgsci(0);
   cpgrect(0.01,.99,0.01,0.99);
   cpgsci(1);

   cpgmtxt("T",-1.0,0.02,.0,text);

   sprintf(retorno,"%d",num);
   cpgmtxt("T",-2.0,0.02,.0,retorno);
   cpgband (7, 0, 0, 0, &ax, &ay, &ch[0]);
   ch[1]='\0'; 

   switch(ch[0])
   {
    case '-':
     num=-num;
    break;
    case 8:
     num=num/10;
    break;
    case 13:
    break;
    default:
     num=num*10+atoi(ch);
    break;
   }
 }

 return num;
}

char getonechar(float *axx, float *ayy) {
   char ch;
   float ax, ay;
   cpgband (7, 0, 0, 0, &ax, &ay, &ch);
   if(axx!=NULL) *axx=ax;
   if(ayy!=NULL) *ayy=ay;
   return ch;
 }
 
