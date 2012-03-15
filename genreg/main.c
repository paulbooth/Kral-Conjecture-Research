#include "gendef.h"

static FILE *ergfile,*lstfile,*autfile;
static char erg[20],lst[20],aut[20];

static SCHAR n,k,t=3,mid_max=NOTSET,splitlevel=NOTSET;
static SCHAR storeall=0,printall=0,efile=0;
static SCHAR codestdout=0,asciistdout=0;
static ULONG anz=0,tostore=0,toprint=0,count=0;
static UINT  jobs=0,jobnr=0;



void generate()
{
#ifdef DOSTIME
 clock_t start,stop;
 start=clock();
#endif
#ifdef UNIXTIME
 UINT stop;
 struct tms TMS;
#endif

 if(mid_max==NOTSET)
    mid_max=default_mid_max(n,k,t);

 if(splitlevel==NOTSET&&jobs>0)
    splitlevel=default_splitlevel(n,k,t);

 fprintf(ergfile,"\n          GENREG - Generator fuer regulaere Graphen\n");
 fprintf(ergfile,"          %d Knoten, Grad %d, ",n,k);
 fprintf(ergfile,"Taillenweite mind. %d\n",t);
 if(jobs)
    fprintf(ergfile,"          Erzeugung Teil %d von %d gestartet...\n",jobnr,jobs);
 else
    fprintf(ergfile,"          Erzeugung gestartet...\n");
 fflush(ergfile);

 ordstart(n,k,t,mid_max,
	  splitlevel,jobs,jobnr-1,
	  lstfile,autfile,ergfile,
	  tostore,toprint,count,
	  storeall,printall,&anz);

 fprintf(ergfile,"          %ld Graphen erzeugt.\n",anz);

#ifdef DOSTIME
 stop=clock()-start;
#endif
#ifdef UNIXTIME
 if(times(&TMS)==-1);
 stop=(UINT)TMS.tms_utime; 
#endif
#ifdef time_factor
 fprintf(ergfile,"          Laufzeit:%.1fs  ",(float)stop/time_factor);
 if(stop>time_factor)
    fprintf(ergfile,"%.1f Repr./s",(float)anz/((float)stop/time_factor));
 fprintf(ergfile,"\n");
#endif
}

SCHAR inputerror()
{
 SCHAR error=1;

 if(n>MAXN)
    fprintf(stderr,"maximal %d Knoten\n",MAXN);
 else
 if(n<MINN)
    fprintf(stderr,"mindestens %d Knoten\n",MINN);
 else
 if(k>MAXK)
    fprintf(stderr,"Grad maximal %d\n",MAXK);
 else
 if(k<MINK)
    fprintf(stderr,"Grad mindestens %d\n",MINK);
 else
 if(t>MAXT)
    fprintf(stderr,"Taillenweite maximal %d\n",MAXT);
 else
 if(t<MINT)
    fprintf(stderr,"Taillenweite mindestens %d\n",MINT);
 else
    error=0;

 return error;
}

void openfiles()
{
    if(codestdout)
       lstfile=stdout;
    else
    if(storeall==1||tostore>0)
      {
       sprintf(lst,"%d%d_%d_%d-%d.%s",n/10,n%10,k,t,jobnr,APP);
       lstfile=fopen(lst,"wb");
      }

    if(asciistdout)
       autfile=stdout;
    else
    if(printall==1||toprint>0)
      {
       sprintf(aut,"%d%d_%d_%d-%d.asc",n/10,n%10,k,t,jobnr);
       autfile=fopen(aut,"w");
      }
}

void closefiles()
{
    if((storeall==1||tostore>anz)&&!codestdout)
      {
       fclose(lstfile);
       if(anz==0)remove(lst);
      }

    if((printall==1||toprint>anz)&&!asciistdout)
      {
       fclose(autfile);
       if(anz==0)remove(aut);
      }
}

void movefiles()
{
    char neu[20];

    if((storeall==1||tostore>=anz)&&anz>0&&!codestdout)
      {
       sprintf(neu,"%d%d_%d_%d#%d.%s",n/10,n%10,k,t,jobnr,APP);
       remove(neu);
       rename(lst,neu);
      }

    if((printall==1||toprint>=anz)&&anz>0&&!asciistdout)
      {
       sprintf(neu,"%d%d_%d_%d#%d.asc",n/10,n%10,k,t,jobnr);
       remove(neu);
       rename(aut,neu);
      }
}



main(argc,argv)
int argc;
char **argv;
{
 char  neu[20],rec[20];
 FILE  *recfile;
 ULONG newanz=0;

/*********** Parameter erkennen **********/

 if(argc<3)
   {
    fprintf(stderr,"zu wenige Parameter\n");
    return 0;
   }

 n=(SCHAR)atoi(*++argv);
 k=(SCHAR)atoi(*++argv);
 argc-=2;

 if(argc>1&&(t=atoi(*(argv+1)))>0)
   {
    argv++;argc--;
   }
 else
    t=3;

 if(inputerror())return 1;


/********** Optionen erkennen **********/

 while(--argc>0)
      {
       ++argv;
       if(!strcmp(*argv,"-e"))
	  efile=1;
       else
       if(!strcmp(*argv,"-c"))
	 {
          if(argc>1)
	     count=atoi(*(argv+1));
	  if(count>0)
	    {
	     argv++;
	     argc--;
	    }
	  else count=COUNTSTEP;
	 }
       else
       if(!strcmp(*argv,"-max"))
	 { 
          if(argc>1)
	     mid_max=atoi(*++argv);argc--;
	  if(mid_max==0)
	    {
	     fprintf(stderr,"%s : kein erlaubter Wert fuer Option -max\n",*argv);
	     return(1);
	    }
	 }
       else
       if(!strcmp(*argv,"-split"))
	 {
          if(argc>1)
	     splitlevel=atoi(*++argv);argc--;
	  if(splitlevel==0)
	    {
	     fprintf(stderr,"%s : kein erlaubter Wert fuer Option -split\n",*argv);
	     return(1);
	    }
	 }
       else
       if(!strcmp(*argv,"-s"))
	 {
	  if(argc>1)
            {
             if(!strcmp(*(argv+1),"stdout"))
	        codestdout=storeall=1;
	     else
	        tostore=atoi(*(argv+1));
            }
	  if(tostore>0||codestdout)
	    {
	     argv++;
	     argc--;
	    }
	  else storeall=1;
	 }
       else
       if(!strcmp(*argv,"-a"))
	 {
          if(argc>1)
            {  
             if(!strcmp(*(argv+1),"stdout"))
	        asciistdout=printall=1;
	     else
	        toprint=atoi(*(argv+1));
            }
	  if(toprint>0||asciistdout)
	    {
	     argv++;
	     argc--;
	    }
	  else printall=1;
	 }
       else
       if(!strcmp(*argv,"-m"))
	 {
          if(argc>1)
            {
	     jobnr=atoi(*++argv);
             argc--;
            }
          if(argc>1)
            {
	     jobs=atoi(*++argv);
	     argc--;
            }
	  if(jobs==0||jobnr==0||jobnr>jobs)
	    {
	     fprintf(stderr,"%d %d : keine erlaubten Werte fuer Option -m\n",jobnr,jobs);
	     return 1;
	    }
	 }
       else
	 {
	  fprintf(stderr,"%s : keine erlaubte Option\n",*argv);
	  return 1;
	 }
      }


/********** -m Modus **********/

 if(jobs>0)
   {
    openfiles();

    if(!efile)
       ergfile=stderr;
    else
      {
       sprintf(erg,"%d%d_%d_%d-%d.erg",n/10,n%10,k,t,jobnr);
       ergfile=fopen(erg,"w");
      }

    generate();

    closefiles(anz);

    movefiles(anz);

    if(efile)
      {
       fclose(ergfile);
       sprintf(neu,"%d%d_%d_%d#%d.erg",n/10,n%10,k,t,jobnr);
       remove(neu);
       rename(erg,neu);
      }
   }
 else


/********** Normalmodus **********/

   {
    if(codestdout)
       lstfile=stdout;
    else
    if(storeall==1||tostore>0)
      {
       sprintf(lst,"%d%d_%d_%d-U.%s",n/10,n%10,k,t,APP);
       lstfile=fopen(lst,"wb");
      }

    if(asciistdout)
       autfile=stdout;
    else
    if(printall==1||toprint>0)
      {
       sprintf(aut,"%d%d_%d_%d-U.asc",n/10,n%10,k,t);
       autfile=fopen(aut,"w");
      }

    if(!efile)
       ergfile=stderr;
    else
      {
       sprintf(erg,"%d%d_%d_%d-U.erg",n/10,n%10,k,t);
       ergfile=fopen(erg,"w");
      }

    generate();

    if((storeall==1||tostore>anz)&&!codestdout)
      {
       fclose(lstfile);
       if(anz==0)remove(lst);
      }

    if((printall==1||toprint>anz)&&!asciistdout)
      {
       fclose(autfile);
       if(anz==0)remove(aut);
      }

    if(efile)
      {
       fclose(ergfile);
       sprintf(neu,"%d%d_%d_%d.erg",n/10,n%10,k,t);
       remove(neu);
       rename(erg,neu);
      }

    if((storeall==1||tostore>=anz)&&anz>0&&!codestdout)
      {
       sprintf(neu,"%d%d_%d_%d.%s",n/10,n%10,k,t,APP);
       remove(neu);
       rename(lst,neu);
      }

    if((printall==1||toprint>=anz)&&anz>0&&!asciistdout)
      {
       sprintf(neu,"%d%d_%d_%d.asc",n/10,n%10,k,t);
       remove(neu);
       rename(aut,neu);
      }
   }

 return 0;
}


