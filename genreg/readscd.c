#include <stdio.h>
#include <stdlib.h>

char n,k,t=3;

dekomp(file,code)
FILE *file;
char* code;
{
 char readbits,samebits;

 samebits=getc(file);
 if(feof(file))
    return(0);

 readbits=n*k/2-samebits;

 fread(code+samebits,sizeof(char),readbits,file);
 if(feof(file))
    return(-1);

 return(1);
}

void codetonlist(code,l)
char *code,**l;
{
 char i,v=1,w;

 for(i=1;i<=n;i++)
     l[i][0]=0;

 for(i=n*k/2;i>0;i--)
    {
     w=*code;
     while(l[v][0]==k)v++;
     l[v][++l[v][0]]=w;
     l[w][++l[w][0]]=v;
     code++;
    }
}

void nachblist(l)
char **l;
{
 char i,j;
 for(i=1;i<=n;i++)
    {
     printf("\n%d : ",i);
     for(j=1;j<=k;j++)
	 printf("%d ",l[i][j]);
    }
 printf("\n");
}

main(argc,argv)
int  argc;
char **argv;
{
 int i,j,erg;
 char *code,**nlist,scd[20];
 long anz=0;
 FILE* scdfile;

 if(argc<3)
   {
    fprintf(stderr,"zu wenige Parameter\n");
    return 0;
   }

 n=(char)atoi(*++argv);
 k=(char)atoi(*++argv);
 argc-=2;

 if(argc>1)
    t=atoi(*(argv+1));

 sprintf(scd,"%d%d_%d_%d.scd",n/10,n%10/k,t);
 if((scdfile=fopen(scd,"rb"))==NULL)
   {
    sprintf(scd,"%d%d_%d_%d.scd",n/10,n%10,k,t);
    if((scdfile=fopen(scd,"rb"))==NULL)
      {
       fprintf(stderr,"scd-file not found\n");
       return(0);
      }
   }

 code=(char*)calloc(n*k/2,sizeof(char));
 nlist=(char**)calloc(n+1,sizeof(char*));
 for(i=1;i<=n;i++)
     nlist[i]=(char*)calloc(k,sizeof(char));

 while((erg=dekomp(scdfile,code))!=0)
      {
       printf("\n%d. Graph gelesen\n",++anz);
       codetonlist(code,nlist);
       nachblist(nlist);
      }

 if(erg==-1)
   {
    fprintf(stderr,"scd-file fehlerhaft\n");
    return(-1);
   }

 return(1);
}

