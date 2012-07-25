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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>
#include <sacio.h>

int isdir(char *path)
{
  struct stat st;
  int i;

  i=stat(path,&st);
  if (i!=0) return 0;
  if (!S_ISDIR(st.st_mode)) return 0;

  return 1;
}

int isfile(char *path)
{
  struct stat st;
  int i;

  i=stat(path,&st);
  if (i!=0) return 0;

  return 1;
}

char *copystring(char *string)
{
  char *copy;
  copy=malloc(sizeof(char)*(strlen(string)+1));
  strcpy(copy,string);
  return copy;
}

int createdir(char *path,int depth)
{
  DIR *d;
  char *temp=NULL;
  char *dir=NULL;
  int i;
  
  if (isdir(path)) return 0;

  temp = copystring(path);
  dir = dirname(temp);

  // fprintf(stderr,"(%d) Abrindo %s\n",depth, dir);
  d=opendir(dir);
  if (d==NULL)
    {
      // Diretorio nao existe
      i=createdir(dir,depth+1);
      // fprintf(stderr,"(%d) Criando %s retornou %d.\n",depth, dir,i);
      if (i!=0) return i;
    } else { 
      closedir(d);
    }

  i=mkdir(path,00777);
  if (i!=0) 
    fprintf(stderr," Error creating directory: %s %d)\n",path,depth);
  else
    fprintf(stderr," Created directory: %s\n",path);
    

  if (temp!=NULL) free(temp);
  return i;
}


int copyfile(char *source, char *dest)
{
  FILE *ent,*sai;
  unsigned char *mem;
  int k;
  int blkbypass = 1024;

  fprintf(stderr,".");

  ent=fopen(source,"r");
  sai=fopen(dest,"w");

  if (ent==NULL) return -1;
  if (sai==NULL) return -1;

  mem=malloc(sizeof(unsigned char)*blkbypass);
  if (mem==NULL) return -1;

  while (!feof(ent))
    {
      k=fread(mem,sizeof(unsigned char),blkbypass,ent);
      fwrite(mem,sizeof(unsigned char),k,sai);
    }

  fclose(ent);
  fclose(sai);
  io_freeData(mem);
  return 0;
}
