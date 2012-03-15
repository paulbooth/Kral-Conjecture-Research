#include "gendef.h"
#define a3 6
#define a4 7
#define a5 3
#define a6 4
#define a7 3

static int k_min=3,k_max=7;

static int t_min[5]={5,4,4,4,4};
static int t_max[5]={8,6,6,4,5};

static int grad3[4][a3]=
	   {
	    {18,1,2,2,3,3},
	    {18,1,1,1,2,2},
	    {26,2,2,3,3,3},
	    {32,3,3,3,2,2}
	   };

static int grad4[3][a4]=
	   {
	    {14,0,1,0,0,0,0},
	    {19,1,0,1,0,1,1},
	    {27,1,1,1,0,0,0}
	   };

static int grad5[3][a5]=
	   {
	    {14,-1,-1},
	    {28,-2,-2},
	    {42,-5,-5}
	   };

static int grad6[1][a6]=
	   {
	    {14,-1,0,-1}
	   };

static int grad7[2][a7]=
	   {
	    {16,-1,-1},
	    {50,-10,-10}
	   };

SCHAR default_mid_max(n,k,t)
SCHAR n,k,t;
{
 int nn;

 if(k<k_min||k>k_max)
    return(0);

 if(t<t_min[k-k_min])
    return(0);
 if(t>t_max[k-k_min])
    return(n/2);
 t-=t_min[k-k_min];

 switch(k)
       {
	case 3: nn=(n-grad3[t][0])/2+1;
		if(nn<1)
		   return(0);
		if(nn>=a3)
		   return(n/2+grad3[t][a3-1]);
		return(n/2+grad3[t][nn]);

	case 4: nn=n-grad4[t][0]+1;
		if(nn<1)
		   return(0);
		if(nn>=a4)
		   return(n/2+grad4[t][a4-1]);
		return(n/2+grad4[t][nn]);

	case 5: nn=(n-grad5[t][0])/2+1;
		if(nn<1)
		   return(0);
		if(nn>=a5)
		   return(n/2+grad5[t][a5-1]);
		return(n/2+grad5[t][nn]);

	case 6: nn=n-grad6[t][0]+1;
		if(nn<1)
		   return(0);
		if(nn>=a6)
		   return(n/2+grad6[t][a6-1]);
		return(n/2+grad6[t][nn]);

	case 7: nn=(n-grad7[t][0])/2+1;
		if(nn<1)
		   return(0);
		if(nn>=a7)
		   return(n/2+grad7[t][a7-1]);
		return(n/2+grad7[t][nn]);

	default:fprintf(stderr,"kann gar nicht sein\n");
		return(0);
       }
}

SCHAR get_splitlevel(k,t)
SCHAR k,t;
{
 if(t==3)
   {
    switch(k)
	  {
	   case 3: return(11);
	   case 4: return(8);
	   case 5: return(7);
	   case 6: return(6);
	   case 7: return(6);
	  }
    }
/* folgende Rueckgabewerte wurden noch nicht
   ausreichend getestet; evtl. um 1 erhoehen,
   falls die Aufteilung zu ungleichmaessig ist
*/
 else
 if(t==4)
   {
    switch(k)
	  {
	   case 3: return(13);
	   case 4: return(9);
	   case 5: return(7);
	   case 6: return(7);
	   case 7: return(7);
	  }
   }
 else
 if(t==5)
   {
    switch(k)
	  {
	   case 3: return(14);
	   case 4: return(12);
	   case 5: return(13);  /*nicht zu empfehlen*/
	  }
   }
 else
 if(t==6)
   {
    switch(k)
	  {
	   case 3: return(16);
	   case 4: return(15);
	  }
   }
 else
 if(t==7&&k==3)
    return(18);  /*nicht zu empfehlen*/
 else
 if(t==8&&k==3)
    return(21);   /*nicht zu empfehlen*/

 fprintf(stderr,"splitlevel muss mit Option -split angegben werden\n");
 exit(0);
}

SCHAR default_splitlevel(n,k,t)
SCHAR n,k,t;
{
 SCHAR ret;

 ret=get_splitlevel(k,t);
 if(ret+3<n)
    return(ret);

 fprintf(stderr,"zu wenig Knoten fuer -m Option\n");
 exit(0);
}
