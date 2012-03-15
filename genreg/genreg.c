/* genreg by Markus Meringer */
/* e-mail markus@btm2xg.mat.uni-bayreuth.de */


/* Diese Version: 2.November'95 */


#include "gendef.h"


static SCHAR n,k,t,mid_max,splitlevel;
static ULONG tostore,toprint,count;
static FILE  *ergfile,*lstfile,*autfile;
static UINT  jobs,jobnr;
static SCHAR storeall,printall;

static SCHAR springen,i1,j1,f0,f1,girth_exact;
static SCHAR **g,**l,**part,**einsen,**transp,**zbk;
static SCHAR *kmn,*grad,*lgrad,*lastcode;
static ULONG calls,dez,anz;

static long fpos;

/*
 mit Maxtest bei mid_max
 ACHTUNG: vm geaendert : vm=n+1 falls grad[n]>=1

 letzte Aenderung:
 statt if(vm<=n&&...)vm++ jetzt if(vm<=my)vm=my+1

 Aufteilung der Erzeugung in mehrere
 jetzt Schritte moeglich

 Shortcode implementiert
*/

#ifdef STAB
static SCHAR **aut;
static int  erz,merz;

static void stabprint()
{
 int i,j;
 long ord=1;
 SCHAR next,last=0,mult=1;

 for(i=1;i<=erz;i++)
    {
     next=aut[i][0];
     if(last==next)
	mult++;
     else
       {
	last=next;
	ord*=mult;
	mult=2;
       }
     fprintf(autfile,"\n%d :",next);
     for(j=1;j<=n;j++)
	fprintf(autfile," %d",aut[i][j]);
    }
 fprintf(autfile,"\nOrdnung: %ld\n\n",ord*mult);
}
#endif

static void err()
{
 fprintf(stderr,"Abbruchfehler: Speicher voll");
 exit(0);
}

static void nachblist()
{
 SCHAR i,j;
 for(i=1;i<=n;i++)
    {
     fprintf(autfile,"\n%d : ",i);
     for(j=1;j<=grad[i];j++)
	 fprintf(autfile,"%d ",l[i][j]);
    }
 fprintf(autfile,"\n");
}

/*
 (f0)f1 ist kleinster Knoten fuer
 den aufgrund der Taillenweite
 und des Grades (k)eine Kante
 fest vorgegeben ist

 Bsp.:  k=3,t=3 -> f1=2,f0=5
	k=3,t=5 -> f1=5,f0=11
*/

#ifndef SHORTCODE

static void komprtofile()
{
 SCHAR i,j,c;
 SCHAR *store;
 store=g[0];

 for(i=f1;i<f0;i++)
     store[i]=2;
 for(i=f0;i<=n;i++)
     store[i]=1;

 for(i=f1;i<=n;i++)
     for(j=store[i];j<=k;j++)
	{
	 c=l[i][j];
	 store[c]++;
	 putc(c,lstfile);
	}
}

#else

static void komprtofile()
{
 SCHAR i,j,c,equ=1;
 UINT  h=0;

 for(i=1;i<=n;i++)
     for(j=1;j<=k;j++)
	{
	 c=l[i][j];
	 if(c>i)
	   {
	    if(equ&&c!=lastcode[++h])
	      {
	       equ=0;
	       putc(h-1,lstfile);  /*Anzahl der uebereinstimmenden Kanten*/
	       lastcode[h]=c;
	       putc(c,lstfile);   /*nur fuer n<256*/
	      }
	    else
	    if(!equ)
	      {
	       lastcode[++h]=c;
	       putc(c,lstfile);
	      }
	   }
	}
}

#endif


/*
 girthstart ermittelt Taillenweite
 und gibt diese zurÅck bzw 0,
 falls Knoten 3 nicht auf dem
 (ersten Taillenkreis liegt
 wird nur aufgerufen, wenn
 1.Kreis soeben geschlossen wurde
*/

static SCHAR girthstart()
{
 SCHAR tw=2,last=1,now=2,next;

 while(now!=3)
      {
       next=l[now][1];
       if(next==last)
	  next=l[now][2];
       if(next==0||next==2)
	  return(0);
       last=now;now=next;tw++;
      }
 return(tw);
}

/*
 girthcheck2 testet,ob sich die
 Taillenweite nach EinfÅgen
 von Kante(mx,my) verkleinert,
 wenn ja wird weue tw zurÅckgegeben
 (d.h. G nicht max), sonst 127
*/

static SCHAR girthcheck2(mx,my,tw)
SCHAR mx,my,tw;
{
 SCHAR *status,*xnachb,*ynachb;
 SCHAR welle=1,a,i,j,x,y;
 xnachb=l[mx];ynachb=l[my];

 if(tw==4)
   {
    for(i=1;i<grad[mx];i++)
       {
	x=xnachb[i];
	for(j=1;j<grad[my];j++)
	    if(x==ynachb[j])
	       return(3);
       }
    return(127);
   }

 if(tw==5)
   {
    for(i=1;i<grad[mx];i++)
       {
	x=xnachb[i];
	for(j=1;j<grad[my];j++)
	   {
	    y=ynachb[j];
	    if(x==y)
	       return(3);
	    if(g[x][y])
	       return(4);
	   }
       }
    return(127);
   }

 status=zbk[0];
 for(i=1;i<=n;i++)status[i]=0;
 status[mx]=1;status[my]=2;
 zbk[2][1]=my;zbk[2][0]=1;
 for(i=1;i<grad[mx];i++)
    {
     x=xnachb[i];
     zbk[3][i]=x;
     status[x]=3;
    }
 zbk[3][0]=grad[mx]-1;
 while(++welle<tw-1)
      {
       a=0;
       for(i=1;i<=zbk[welle][0];i++)
	  {
	   x=zbk[welle][i];
	   for(j=1;j<=grad[x];j++)
	      {
	       y=l[x][j];
	       if(status[y]==0)
		 {
		  status[y]=welle+2;
		  zbk[welle+2][++a]=y;
		 }
	       else
	       if(status[y]==welle+1)
		  return(welle+1);
	      }
	  }
       zbk[welle+2][0]=a;
      }
 return(127);
}

/*
 ongirth0 prueft fuer gerade Taillenweite tw,
 ob Knoten v auf einem Taillenkreis liegt.
 Wenn ja, wird 1 zurueckgegben, sonst 0.
*/

static SCHAR ongirth0(v,tw)
SCHAR v,tw;
{
 SCHAR *status,last,a,h,i,j=0,x,y;
 status=zbk[0];
 for(i=1;i<=n;i++)status[i]=0;
 status[v]=(-1);
 for(h=1;h<=k;h++)
    {
     last=tw/2+h-2;
     x=l[v][h];status[x]=h;
     zbk[last][1]=x;zbk[last][0]=1;
     while(last>h)
	  {
	   a=0;
	   for(i=1;i<=zbk[last][0];i++)
	      {
	       x=zbk[last][i];
	       for(j=1;j<=k;j++)
		  {
		   y=l[x][j];
		   if(status[y]==0)
		     {
		      status[y]=h;
		      zbk[last-1][++a]=y;
		     }
		  }
	      }
	   zbk[--last][0]=a;
	  }
    }
 for(h=1;h<k;h++)
    {
     for(i=1;i<=zbk[h][0];i++)
	{
	 x=zbk[h][i];
	 for(j=1;j<=k;j++)
	    {
	     y=l[x][j];
	     if(status[y]>=0)
		status[y]=h;
	    }
	}
     for(last=h+1;last<=k;last++)
	{
	 for(i=1;i<=zbk[last][0];i++)
	    {
	     x=zbk[last][i];j=0;
	     for(j=1;j<=k;j++)
		{
		 y=l[x][j];
		 if(status[y]==h)
		    return(1);
		}
	    }
	}
    }
 return(0);
}

/*
 ongirth0 prueft fuer ungerade Taillenweite tw,
 ob Knoten v auf einem Taillenkreis liegt.
 Wenn ja, wird 1 zurueckgegben, sonst 0.
*/

static SCHAR ongirth1(v,tw)
SCHAR v,tw;
{
 SCHAR *status,last,a,h,i,j=0,x,y;
 status=zbk[0];
 for(i=1;i<=n;i++)status[i]=0;
 status[v]=(-1);
 for(h=1;h<=k;h++)
    {
     last=tw/2+h-1;
     x=l[v][h];status[x]=h;
     zbk[last][1]=x;zbk[last][0]=1;
     while(last>h)
	  {
	   a=0;
	   for(i=1;i<=zbk[last][0];i++)
	      {
	       x=zbk[last][i];
	       for(j=1;j<=k;j++)
		  {
		   y=l[x][j];
		   if(status[y]==0)
		     {
		      status[y]=h;
		      zbk[last-1][++a]=y;
		     }
		  }
	      }
	   zbk[--last][0]=a;
	  }
     if(h>1)
       {
	for(i=1;i<=zbk[h][0];i++)
	   {
	    x=zbk[h][i];
	    for(j=1;j<=k;j++)
	       {
		y=l[x][j];
		if(status[y]>0)
		   return(1);
	       }
	   }
       }
    }
 return(0);
}

static SCHAR knoten_als_eins(v,tw)
SCHAR v,tw;
{
 SCHAR i,j,x;
 if(tw==3)
   {
    for(i=1;i<=k;i++)
       {
	x=l[v][i];
	for(j=i+1;j<=k;j++)
	    if(g[x][l[v][j]])
	       return(1);
       }
    return(0);
   }
 else
 if(tw%2==0)
    return(ongirth0(v,tw));
 else
    return(ongirth1(v,tw));
}

static void sprungindex(i,j)
SCHAR i,j;
{
 SCHAR r,s,x,y,z,e;
 i1=1;j1=j;
 for(r=1;r<=i;r++)
    {
     e=lgrad[r];
     for(s=r+1;e<k&&(r<i||s<j);s++)
	{
	 z=g[r][s];e+=z;
	 if(z)
	   {
	    x=kmn[r];y=kmn[s];
	    if(x>y)
	      {z=x;x=y;y=z;}
	    if(x>i1||(x==i1&&y>j1))
	      {i1=x;j1=y;}
	   }
	}
    }
 x=kmn[i];y=kmn[j];
 if(x>y)
   {z=x;x=y;y=z;}
 if(x>i1||(x==i1&&y>j1))
   {i1=x;j1=y;}
}

static void transpinv(mperm)
SCHAR *mperm;
{
 SCHAR i,x,re,li;
 i=2*mperm[0];
 while(i>0)
      {
       re=mperm[i--];
       li=mperm[i--];
       x=kmn[re];
       kmn[re]=kmn[li];
       kmn[li]=x;
      }
}

static SCHAR maxinblock(zeile,mperm,e,li,re)
SCHAR *zeile,*mperm;
SCHAR e,li,re;
{
 SCHAR i,x;
 i=2*mperm[0];
 while(e>=0&&li<=re)
      {
       while(zeile[kmn[li]])
	    {
	     e--;
	     if(++li>re)
		return(e);
	    }
       if(li==re)
	  return(e);
       while(zeile[kmn[re]]==0)
	     if(--re==li)
		return(e);
       mperm[++i]=li;
       mperm[++i]=re;
       mperm[0]++;
       x=kmn[li];
       kmn[li]=kmn[re];
       kmn[re]=x;
       li++;re--;e--;
      }
 return(e);
}

static SCHAR maxinzeile(tz)
SCHAR tz;
{
 SCHAR b,*zeile,*block,*eintr,*mperm;
 SCHAR e,li,re,erg=0;
 zeile=g[kmn[tz]];
 block=part[tz];
 eintr=einsen[tz];
 mperm=transp[tz];
 mperm[0]=0;

 for(b=1;b<=block[0]&&erg==0;b++)
    {
     e=eintr[b];
     li=block[b];re=block[b+1]-1;
     erg=maxinblock(zeile,mperm,e,li,re);
    }
 if(erg==0)
    return(0);
 else
 if(erg>0)
    return(1);   /*urspr. Nummerierung groesser*/
 sprungindex(tz,li+e);
 return(-1);
}

static SCHAR maxrekneu(tz)
SCHAR tz;
{
 SCHAR i,x,e;
 SCHAR erg;
#ifdef STAB
 SCHAR *aut1;
 if(tz>=n-1)
   {
    aut1=aut[++erz];
    for(i=1;i<=n;i++)
       aut1[i]=kmn[i];
    return(0);
   }
#else
 if(tz>=n-1)
    return(0);
#endif
 /*x=kmn[tz];*/
 erg=maxinzeile(tz);
 if(erg==0)
    erg=maxrekneu(tz+1);
 transpinv(transp[tz]);
 e=part[0][tz];
 for(i=tz+1;i<=e&&erg==1;i++)
    {
     x=kmn[tz];
     kmn[tz]=kmn[i];
     kmn[i]=x;
     erg=maxinzeile(tz);
     if(erg==0)
	erg=maxrekneu(tz+1);
     transpinv(transp[tz]);
     x=kmn[tz];
     kmn[tz]=kmn[i];
     kmn[i]=x;
    }
 return(erg);
}

static SCHAR maxstartneu(tw)
SCHAR tw;
{
 SCHAR i,j,e;
 SCHAR erg;
#ifdef STAB
 erz=0;
#endif
 for(i=n-1;i>1;i--)             /*durchlauft die punktw. Stabilisatoren*/
    {                           /*Stab(n-2),...,Stab(0)=S(n)*/
     e=part[0][i];
     for(j=i+1;j<=e;j++)
	{
	 kmn[i]=j;
	 kmn[j]=i;              /*Transposition (i,j)*/
	 erg=maxinzeile(i);
	 if(erg==0)
	    erg=maxrekneu(i+1); /*durchlaeuft Nebenklasse (i,j)Stab{0,...,i}*/
#ifdef STAB
	 if(erg==0)
	    aut[erz][0]=i;
#endif
	 transpinv(transp[i]);
	 kmn[i]=i;
	 kmn[j]=j;
	 if(erg==-1)
	    return(0);
	}
    }

 for(j=2;j<=n;j++)              /*durchlauft die Nebenklassen von Stab(1)*/
   if(knoten_als_eins(j,tw))
	{
	 kmn[1]=j;
	 kmn[j]=1;              /*Transposition (i,j)*/
	 erg=maxinzeile(1);
	 if(erg==0)
	    erg=maxrekneu(2);   /*durchlaeuft Nebenklasse (i,j)Stab{0,...,i}*/
#ifdef STAB
	 if(erg==0)
	    aut[erz][0]=i;
#endif
	 transpinv(transp[1]);
	 kmn[1]=1;
	 kmn[j]=j;
	 if(erg==-1)
	    return(0);
	}

 return(1);
}

static SCHAR maxinzeile1(tz)
SCHAR tz;
{
 SCHAR e,li,re,erg=0;
 SCHAR b,*zeile,*block,*eintr,*mperm;
 zeile=g[kmn[tz]];
 block=part[tz];
 eintr=einsen[tz];
 mperm=transp[tz];
 mperm[0]=0;
 for(b=1;b<=block[0]&&erg==0;b++)
    {
     e=eintr[b];
     li=block[b];re=block[b+1]-1;
     erg=maxinblock(zeile,mperm,e,li,re);
    }
 if(erg==0)
    return(0);
 else
 if(erg>0)
    return(1);
 return(-1);
}

static SCHAR maxrekneu1(tz)
SCHAR tz;
{
 SCHAR i,x,e;
 SCHAR erg;
 if(tz==n-1)
    return(0);
 /*x=kmn[tz];*/
 erg=maxinzeile1(tz);
 if(erg==0)
    erg=maxrekneu1(tz+1);
 transpinv(transp[tz]);
 e=part[0][tz];
 for(i=tz+1;i<=e&&erg==1;i++)
    {
     x=kmn[tz];
     kmn[tz]=kmn[i];
     kmn[i]=x;
     erg=maxinzeile1(tz);
     if(erg==0)
	erg=maxrekneu1(tz+1);
     transpinv(transp[tz]);
     x=kmn[tz];
     kmn[tz]=kmn[i];
     kmn[i]=x;
    }
 return(erg);
}

static SCHAR maxstartneu1(vm)
SCHAR vm;
{
 SCHAR i,j,e;
 SCHAR erg;
 for(i=vm;i>1;i--)
    {
     e=part[0][i];
     for(j=i+1;j<=e;j++)
	{
	 kmn[i]=j;
	 kmn[j]=i;
	 erg=maxinzeile1(i);
	 if(erg==0&&i<n-1)
	    erg=maxrekneu1(i+1);
	 transpinv(transp[i]);
	 kmn[i]=i;
	 kmn[j]=j;
	 if(erg==-1)
	    return(0);
	}
    }

 for(j=2;j<=vm;j++)
     if(grad[j]==k)
       {
	kmn[1]=j;
	kmn[j]=1;
	erg=maxinzeile1(1);
	if(erg==0)
	   erg=maxrekneu1(2);
	transpinv(transp[1]);
	kmn[1]=1;
	kmn[j]=j;
	if(erg==-1)
	   return(0);
       }

 return(1);
}

/*
 semiverf erstellt part[x+1],
 die Verfeinerung von part[x]
 aufgrund der Eintraege in Zeile x.
 semiverf  wird erst aufgerufen,
 wenn Zeile x gefuellt ist.
*/

static void semiverf(x)
SCHAR x;
{
 SCHAR *nextpart,*block,blanz,blockgr,einsanz,i;
 block=part[x];
 blanz=block[0];
 nextpart=part[x+1];
 einsanz=einsen[x][1];
 blockgr=block[2]-block[1];

 if(blockgr==1)
   {
    nextpart[0]=0;
    part[0][x+1]=x+1;
   }
 else
 if(einsanz==1)
   {
    nextpart[1]=x+2;                                   /*blockgr-1;*/
    nextpart[0]=1;
    part[0][x+1]=x+1;
   }
 else
 if(einsanz==blockgr||einsanz==0)
   {
    nextpart[1]=x+2;                                   /*blockgr-1;*/
    nextpart[0]=1;
    part[0][x+1]=x+blockgr;
   }
 else
   {
    nextpart[1]=x+2;                                   /*einsanz-1;*/
    nextpart[2]=x+einsanz+1;                           /*blockgr-1;*/
    nextpart[0]=2;
    part[0][x+1]=x+einsanz;
   }

 for(i=2;i<=blanz;i++)
    {
     einsanz=einsen[x][i];
     blockgr=block[i+1]-block[i];
     if(einsanz==blockgr||einsanz==0)
	nextpart[++nextpart[0]]=block[i];              /*blockgr;*/
     else
       {
	nextpart[++nextpart[0]]=block[i];              /*einsanz;*/
	nextpart[++nextpart[0]]=block[i]+einsanz;      /*blockgr-einsanz;*/
       }
    }
 nextpart[nextpart[0]+1]=n+1;
}

/*
 ordrek erledigt das Einsetzen
 der Kanten, ruft ggf Girth-
 und Maxtest auf, steuert die
 Ausgabe. Uebergabeparameter:
 (mx,my) zuletzt einges. Kante
 vm min. Knoten mit grad=0
 vor Einsetzen von (mx,my)
 tw Taillenweite vor Einsetzen
 von (mx,my). tw=0, falls noch
 kein Kreis existiert
 lblock Block von part[mx], wo
 eingesetzt wurde
*/

static void ordrek(mx,my,vm,tw,lblock)
SCHAR mx,my,vm,tw,lblock;
{
 SCHAR i;

 if(my>n-k&&grad[mx]<k&&n-my<k-grad[mx])   /*noch genug Platz in Zeile mx,damit grad(mx)=k?*/
    return;				   /*nicht besonders effektiv*/

 if(mx>=n-k&&grad[mx]==k)
    for(i=my+1;i<=n;i++)                   /*testet,ob in Spalte i noch genug Platz*/
	if(n-mx-1<k-grad[i])               /*damit grad(i)=k werden kann (**)*/
	   return;

 if(my<vm)                                 /*my<vm notw. fuer neue Kreise*/
   {
    if(tw>3)                               /*falls Taillenweite >3, testen,*/
      {					   /*ob sie nach einfÅgen von (mx,my)*/
       if(girthcheck2(mx,my,tw)<tw)     /*gleich geblieben ist,*/
	  return;                          /*wenn nein abbrechen*/
      }
    else
    if(tw==0)                              /*dies ist der 1.Kreis*/
      {                                    /*mit girthstart seine Laenge*/
       tw=girthstart();                    /*(=Taillenweite) ermittelt*/
       if(tw==0)
	  return;
      }
   }

 while(mx<n&&grad[mx]==k)                  /*Zeile mx voll?*/
      {
       semiverf(mx++);                     /*verfeinerte Partition berechnen*/
       lblock=1;                           /*Einfuegen wieder moeglichst links*/
#ifdef JOBS
       if(mx==mid_max||mx==splitlevel)
	 {
	  if(maxstartneu1(vm-1)==0)
	     return;
	  if(mx==splitlevel)               /*hier wird das aufteilen auf*/
	     if(++calls%jobs!=jobnr)       /*mehrere jobs organisiert*/
		return;
	 }
#else
       if(mx==mid_max)
	  if(maxstartneu1(vm-1)==0)
	     return;
#endif
      }                                    /*mx ist nun min. Knoten mit grad<k*/

 if(vm<=my)vm=my+1;	                   /*vm ist nun min. Knoten mit grad=0*/

 if(mx==n&&grad[n]==k)                     /*Bedingung fuer Regularitaet*/
   {
    if(maxstartneu(tw))
      {
       anz++;
#ifdef STAB
       if(printall)
	 {
	  fprintf(autfile,"\nGraph %ld:\n",anz);
	  nachblist();
	  fprintf(autfile,"Taillenweite: %d\n",tw);
	  stabprint();
	 }
       else
       if(toprint>0)
	 {
	  fprintf(autfile,"\nGraph %ld:\n",anz);
	  nachblist();
	  fprintf(autfile,"Taillenweite: %d\n",tw);
	  stabprint();
	  if(--toprint==0)
	    {
	     fclose(autfile);
	     fprintf(ergfile,"          %ld Graphen erzeugt\r",anz);
	     fflush(ergfile);
	     fseek(ergfile,fpos,0);
	    }
	 }
#endif
#ifdef LIST                                /*Ausgabe*/
       if(storeall)
	  komprtofile();
       else
       if(tostore>0)
	 {
	  komprtofile();
	  if(--tostore==0)
	    {
	     fclose(lstfile);
	     fprintf(ergfile,"          %ld Graphen erzeugt\r",anz);
	     fflush(ergfile);
	     fseek(ergfile,fpos,0);
	    }
	 }
       if(count)
	 {
	  if(anz%dez==0)
	    {
	     fprintf(ergfile,"          %ld Graphen erzeugt\r",anz);
	     fflush(ergfile);
	     fseek(ergfile,fpos,0);
	     if(anz%(dez*count)==0)
		dez*=10;
	    }
	 }
#endif
       return;
      }
    springen=1;                            /*Lerneffekt aktivieren*/
    return;
   }

 for(i=lblock;i<=part[mx][0];i++)          /*Einsetzen der neuen Kante*/
     if((my=part[mx][i]+einsen[mx][i])<part[mx][i+1])
       {                                   /*moegliche Kante (mx,my)*/
	if(grad[my]<k&&my<=vm)             /*Bedingung my<=vm notw. fuer*/
	  {                                /*zusammenhaengende Graphen*/
	   g[mx][my]=g[my][mx]=1;
	   l[mx][++grad[mx]]=my;
	   l[my][++grad[my]]=mx;
	   einsen[mx][i]++;
	   lgrad[my]++;

	   ordrek(mx,my,vm,tw,i);

	   g[mx][my]=g[my][mx]=0;
	   l[mx][grad[mx]]=0;              /*diese beiden Zeilen koennten*/
	   l[my][grad[my]]=0;              /*noch weggelassen werden*/
	   grad[mx]--;grad[my]--;
	   lgrad[my]--;einsen[mx][i]--;

	   if(mx>=n-k&&n-mx-1<k-grad[my])
	      return;                      /*siehe (**)*/
	   if(springen)
	     {
	      if(g[i1][j1]==0)
		 springen=0;
	      else
		 return;
	     }
	  }
       }
 return;
}


/*
 ordstart initialisiert die benoetigten Datenstrukturen:

 g enthaelt die Adjazenzmatrix, also
 g[i][j]=1,falls i,j verbunden, =0 sonst

 l enthaelt die Nachbarschaftsliste, d.h.
 l[i][1],...,l[i][k] sind die Nachbarn von i

 part[i] enthaelt die Blockzerlegung aufgrund
 der Eintraege in  Zeile i-1 und part[i-1].
 Zeile i muss so gefuellt werden, dass innerhalb
 der Bloecke von part[i] die Einsen links und
 die Nullen rechts stehen.
 part[i][0] : Anzahl der Bloecke;
 part[i][j] : Beginn des j-ten Blocks
	      Ausnahme: j=part[i][0], dann n+1;
 part[0][i] : Ende des 1.Blocks von Zeile i vor
	      evtl. Abspalten eines 1er Blocks

 einsen[i] enthaelt Anzahl der Einsen in Zeile i
 bzgl. der Blockzerlegung von part[i].
 einsen[i][j] : Anzahl der Einsen in Block j

 lgrad[i] enthaelt Anzahl der Einsen
 in Zeile i links der Diagonalen
*/

void ordstart(_n,_k,_t,_mid_max,
	      _splitlevel,_jobs,_jobnr,
	      _lstfile,_autfile,_ergfile,
	      _tostore,_toprint,_count,
	      _storeall,_printall,_anz)
SCHAR _n,_k,_t,_mid_max,_splitlevel;
UINT _jobs,_jobnr;
FILE *_lstfile,*_autfile,*_ergfile;
ULONG _toprint,_tostore,_count;
SCHAR _storeall,_printall;
ULONG *_anz;
{
 SCHAR m,in=0,zu=1;
 int  h,i,j;

 n=_n;k=_k;t=_t;mid_max=_mid_max;
 splitlevel=_splitlevel;jobs=_jobs;jobnr=_jobnr;
 lstfile=_lstfile;autfile=_autfile;ergfile=_ergfile;
 toprint=_toprint;tostore=_tostore;count=_count;
 storeall=_storeall;printall=_printall;
 springen=girth_exact=calls=0;
 dez=1;
 anz=(*_anz);
 fpos=ftell(ergfile);

 if(k>=n)
    return;
 if(k==0||k==1)
    return;
 if(n%2==1&&k%2==1)
    return;

 if(!(g     =(SCHAR**)calloc(n+1,sizeof(SCHAR*))))err();
 if(!(l     =(SCHAR**)calloc(n+1,sizeof(SCHAR*))))err();
 if(!(zbk   =(SCHAR**)calloc(n+1,sizeof(SCHAR*))))err();
 if(!(transp=(SCHAR**)calloc(n+1,sizeof(SCHAR*))))err();
 if(!(einsen=(SCHAR**)calloc(n+1,sizeof(SCHAR*))))err();
 if(!(part  =(SCHAR**)calloc(n+1,sizeof(SCHAR*))))err();
 if(!(kmn   =(SCHAR*)calloc(n+1,sizeof(SCHAR))))err();
 if(!(grad  =(SCHAR*)calloc(n+1,sizeof(SCHAR))))err();
 if(!(lgrad =(SCHAR*)calloc(n+1,sizeof(SCHAR))))err();

 for(i=0;i<=n;i++)
    {
     if(!(g[i]     =(SCHAR*)calloc(n+1,sizeof(SCHAR))))err();
     if(!(l[i]     =(SCHAR*)calloc(k+1,sizeof(SCHAR))))err();
     if(!(transp[i]=(SCHAR*)calloc(n+1,sizeof(SCHAR))))err();
     if(!(einsen[i]=(SCHAR*)calloc(n+1,sizeof(SCHAR))))err();
     if(!(part[i]  =(SCHAR*)calloc(n+1,sizeof(SCHAR))))err();
     if(!(zbk[i]   =(SCHAR*)calloc(n+1+k*2,sizeof(SCHAR))))err();
    }

#ifdef STAB
 merz=n*(n-1)/2+1; /*n+(n-2)*k wuerde genuegen*/
 if(!(aut=(SCHAR**)calloc(merz+1,sizeof(SCHAR*))))err();
 for(i=1;i<=merz;i++)
     if(!(aut[i]=(SCHAR*)calloc(n+1,sizeof(SCHAR))))err();
#endif

#ifdef SHORTCODE
 if(!(lastcode=(SCHAR*)calloc(n*k/2+1,sizeof(SCHAR*))))err();
#endif

 for(i=1;i<=n;i++)
     kmn[i]=i;

 part[0][1]=n;
 part[1][0]=1;
 part[1][1]=2;
 part[1][2]=n+1;
 einsen[1][1]=k;

 for(i=3;i<=t;i++)
    {
     in+=zu;
     if(i%2==0)
	zu*=(k-1);
    }

 if(in*(k-1)+2>n)
    return;

 for(j=2;j<=k+1;j++)
    {
     l[1][++grad[1]]=j;
     l[j][++grad[j]]=1;
     g[1][j]=g[j][1]=1;
     lgrad[j]++;
    }

 for(i=2;i<=in;i++)
    {
     semiverf(i-1);
     for(h=1;h<k;h++)
	{
	 l[i][++grad[i]]=j;
	 l[j][++grad[j]]=i;
	 g[i][j]=g[j][i]=1;
	 lgrad[j++]++;
	}
     einsen[i][part[i][0]]=k-1;
    }

 f1=i;f0=j;
 m=(--j);--i;

 if(girth_exact)
   {
    semiverf(i);
    i=in+1;j=in+zu+1;h=1;
    l[i][++grad[i]]=j;
    l[j][++grad[j]]=i;
    g[i][j]=g[j][i]=1;
    lgrad[j]++;
    while(part[i][h]!=j)h++;
    einsen[i][h]++;
   }

 ordrek(i,j,m,0,1);

#ifdef STAB
 for(i=1;i<=merz;i++)
     free(aut[i]);
 free(aut);
#endif

#ifdef SHORTCODE
 free(lastcode);
#endif

 for(i=0;i<=n;i++)
    {
     free(g[i]);
     free(l[i]);
     free(zbk[i]);
     free(transp[i]);
     free(einsen[i]);
     free(part[i]);
    }

 free(g);
 free(l);
 free(kmn);
 free(zbk);
 free(part);
 free(grad);
 free(lgrad);
 free(einsen);
 free(transp);

 *_anz=anz;
 return;
}
