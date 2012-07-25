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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <head.h>
#include <sacio.h>

#include <ftw.h>
 
char version[]="sacchhdr version 0.5";



/* Global vars */
char *var[500],*value[500];
int n=0;
char *fileend;

int process(char *filename);
void showHelp();
int preprocess(const char *fpath, const struct stat *sb, int typeflag);
int blacklisted(char *var);
void showVars();

int main(int argc, char **argv)
{
  int i;
  int j;
  int k;
  char *filename=NULL, *dir=NULL;
  int stop;
  extern char *var[],*value[];
  extern int n;

  /* Azimuth definition */
  char *orivars[]={ "kcmpnm", "cmpinc", "cmpaz"};
  char *oz[]     ={ "z"     , "0.0"   , "0.0"  };
  char *on[]     ={ "n"     , "90.0"  , "0.0"  };
  char *oe[]     ={ "e"     , "90.0"  , "90.0" };

  /* Files extension used in the search */
  char *defaultfileend=".sac";
  extern char *fileend;

  if (argc==1) {
    showHelp();
    return -1;
  }

  n=0;
  stop=0;
  fileend=defaultfileend;
  for(i=1;i<argc;i++)
    {
      if ( (j = strncmp(argv[i],"-v",2)) == 0){
	fprintf(stderr,
		"Version: %s\n         %s\n         %s\n	 %s\n",
		version,
		io_version,
		hd_version,
		hdu_version
		);
	return 0;
      }
	
      if ( (j = strncmp(argv[i],"-h",2)) == 0){
	showHelp();
	return 0;
      }
	
      if ( (j = strncmp(argv[i],"--vars",6)) == 0){
	showVars();
	return 0;
      }

      if ( (j = strncmp(argv[i],"-oz",3)) == 0){
	for(k=0;k<3;k++)
	  {
	    var[n]=orivars[k];
	    value[n]=oz[k];
	    n++;
	  }
	continue;
      }

      if ( (j = strncmp(argv[i],"-on",3)) == 0){
	for(k=0;k<3;k++)
	  {
	    var[n]=orivars[k];
	    value[n]=on[k];
	    n++;
	  }
	continue;
      }

      if ( (j = strncmp(argv[i],"-oe",3)) == 0){
	for(k=0;k<3;k++)
	  {
	    var[n]=orivars[k];
	    value[n]=oe[k];
	    n++;
	  }
	continue;
      }

      if ( (j = strncmp(argv[i],"-ogmt",5)) == 0){
	
	continue;
      }

      if ( (j = strncmp(argv[i],"-f",2)) == 0 && i < argc-1){
	filename=argv[i+1];
	i++;
	continue;
      }

      if ( (j = strncmp(argv[i],"-d",2)) == 0 && i < argc-1){
	dir=argv[i+1];
	i++;
	continue;
      }

      if ( (j = strncmp(argv[i],"-e",2)) == 0 && i < argc-1){
	fileend=argv[i+1];
	i++;
	continue;
      }

      if ( (j =  strncmp(argv[i],"--",2)) == 0 ) {
	i++; // I will jump "--"

	while (i<argc) {
	  var[n]=argv[i];
	  i++; /* Go to value */
	  if ( (j=getSacOffsetFromChar(var[n])) == -1) /* Check if var exist */
	    {
	      fprintf(stderr,"Variable '%s' doesn't exist, please try again.\n",var[n]);
	      stop=1;
	    }
	  if (blacklisted(var[n])) /* Check if we can change it */
	    {
	      fprintf(stderr,"Variable '%s' should not be changed.\n",var[n]);
	      stop=1;
	    }
	  if (i<argc) {
	    value[n]=argv[i];
	    i++;
	  } else {
	    fprintf(stderr,"Oops, variable '%s' is unpaired.\n",var[n]);
	    stop=1;
	    i=argc;
	    break;
	  }
	  
	  n++;
	}
	continue;
      }
      
      showHelp();
      return -1;
    }
  
  if (n == 0) {
    fprintf(stderr,"Nothing to do, leaving.\n");
    stop=1;
  }

  if (stop==1) return -1;

  if ((filename!=NULL) && (dir==NULL))
    process(filename);
  else if ((filename==NULL) && (dir!=NULL))
    ftw(dir,preprocess,20);
  else {
    fprintf(stderr,"Error, no file nor directory to process.\n");
    return -1;
  }

  return 0;
}

int preprocess(const char *fpath, const struct stat *sb, int typeflag)
{
  int i=0;
  char *path=NULL;
  extern char *fileend;

  if (
      (typeflag == FTW_F) &&
      ((i=strncasecmp(&fpath[strlen(fpath)-strlen(fileend)],fileend,strlen(fileend))) == 0)
      ) {
    path=malloc((strlen(fpath)+1)*sizeof(char));
    if (path!=NULL){
      strcpy(path,fpath);
      i = process(path);
      if (path!=NULL) free(path);
      path=NULL;
    } else {
      i=1;
    }
  }
  
  return 0;
}


int process(char *filename)
{
  int i;
  SACHEAD *h=NULL;

  extern char *var[],*value[];
  extern int n;

  fprintf(stderr,"%s [",filename);
  h = io_readSacHead(filename);
  if (h!=NULL)
    fprintf(stderr,"o -> ");
  else {
    fprintf(stderr,"Erro lendo arquivo (%s)]\n",filename);
    return 1;
  }
  
  for(i=0;i<n;i++){
    fprintf(stderr,".");
    changeCharValueFromChar(h,var[i],value[i]);
  }
  
  i = io_writeSacHead(filename,h);
  io_freeData(h);
  
  if (i==0) {
    fprintf(stderr," -> s].\n");
    return 0;
  } else {
    fprintf(stderr," -> Erro salvando arquivo].\n");
    return -2;
  }
}

void showVars()
{
  fprintf(stderr,"\n\n  List of sac Header variables:\n\n");
  showHeadElements(" ");
}

void showHelp()
{
  fprintf(stderr,"Usage:\n");
  fprintf(stderr,"  sacchhdr [-h] [-v] [--vars] [-e <file ending string>] <-f <file>|-d <dir>> <<-- <var> <value> [<var> <value> .... ]> | <-oz> | <-on> | <-oe> | <-ogmt> >\n");

  fprintf(stderr,"\n\t-h) Show this help.\n");
  fprintf(stderr,"\t-v) Show program version.\n");
  fprintf(stderr,"\t--vars) Show list of sac header variables.\n");
  fprintf(stderr,"\t-e) Supply file ending when searching for files.\n");
  fprintf(stderr,"\t-d) Supply directory for searching for files.\n");
  fprintf(stderr,"\t-f) Supply filenames for process.\n");
  fprintf(stderr,"\t-oz) Set orientation to vertical (cmpinc=0, cmpaz=0 and kcmpnm=z).\n");
  fprintf(stderr,"\t-on) Set orientation to NS (cmpinc=90, cmpaz=0 and kcmpnm=n).\n");
  fprintf(stderr,"\t-oe) Set orientation to EW (cmpinc=90, cmpaz=90 and kcmpnm=e).\n");
}

int blacklisted(char *var)
{
  char *list[]={ "delta", "npts", "leven","nvhrd" };
  int nlist=3,i;

  for (i=0;i<nlist;i++)
    if (!strcasecmp(var,list[i]))
	return 1;
  
  return 0;
}
