/* final emulator for 2  neuron networks*/
#include <stdio.h>
#include <math.h>
#define N 2
#define INCREMENT 100
#define SPIKES 1000
#define F2OFF 1
#define ELAPSED 0
#define INTERVAL 1
#define INPUT 1
#define DEBUG 1
#define Q 10 /* length of input queues */
#define DELAY 5.4

int main()
{
FILE *fp1,*fp2,*fp3,*fp4,*fp5,*fp6,*fp7,*fp8;
/* for N>2 need to loop through files with string variable so that each neuron has its own f11 and f21*/
fp1 = fopen("00.Self2Single.data","r");
fp6 = fopen("54.Single2Self5P4.data","r");
fp2 = fopen("nperiod.data","r");
fp3 = fopen("nphi.data","r");
fp4 = fopen("sumf21.data","r");
/*fp5 = fopen("08.AnotherP8.data","r");*/
/*fp5 = fopen("08.Single2SelfP8.data","r");*/
/*fp7 = fopen("00.Self2Single.data","r");*/
fp8 = fopen("endphi.data","w");
int imin,init,stopflag,mark,i,k,j,n,min,count,first;
int checker1 = 0;
int checker2 = 0 ;
int update[N];
double delay[N];
double rem,dummy;
double f1[N][INCREMENT+1];
double f2[N][INCREMENT+1];
double f3[INCREMENT+1];
double period[N];
double time_to_next_spike[N];
double phi[N];
double freq1;
double phi_mirror[N];
double sumf21[N];
double min_input,interval,elapsed_time=0.0;
double spiking[N];
double time_to_next_input[N],inputq[N][Q];


for (i=0;i<N;i++) delay[i] = DELAY; /* Assigning delay array with the delay value for all neurons*/


for (i=0; i<N ; i++) /* Assigning input queues for each neuron to be -1, assuming no neuron is going to  get any input */
    {
       for (j=0; j<Q ; j++)
       {
         inputq[i][j] = -1.0;
       }
    }
for (n=0;n<INCREMENT;n++)
  {
  fscanf(fp1,"%lf %lf\n",&dummy,&f1[0][n]);
  /*fscanf(fp5," %lf\n",&f3[n]);*/
  fscanf(fp6,"%lf %lf\n",&dummy,&f1[1][n]);
  /*fscanf(fp7,"%lf %lf\n",&dummy,&f2[1][n]);*/
  /*if (F2OFF) f2[0][n]=0.0;*/
  /*if (F2OFF) f2[1][n]=0.0;*/
  }
f1[0][INCREMENT]=0.0;
f1[1][INCREMENT]=0.0;
/*f2[0][INCREMENT]=f1[0][0];*/
/*f2[1][INCREMENT]=f1[1][0];*/
  /*if (F2OFF) f2[0][INCREMENT]=0.0;printf("No Second Order Resetting");*/
  /*if (F2OFF) f2[1][INCREMENT]=0.0;*/
for (n=0;n<N;n++)
  {
  fscanf(fp2,"%lf\n",&period[n]);
  fscanf(fp3,"%lf\n",&phi[n]);
  fscanf(fp4,"%lf\n",&sumf21[n]);
  }
/*freq1 = 1.0/period[1];
period[0] = 1.0/(0.995*freq1);*/

/*---------------------------------------------------------------------------- MAIN ROUTINE LOOP ------------------------------------------------------------------------------------------------------------------ */

for(i=0;i<INCREMENT+1;i++)printf("%f\n",f3[i]);

for(k=0;k<SPIKES;k++) 
{
  

/* Loop across the number of spikes, when the number of spikes becomesSPIKES< this will stop*/  
  interval = 0.0;
  /*printf("1. \n k = %d \n",k);*/

do{/*-----------------------------------------------------Do BEGINS HERE HERE -----------------------------------------------------------------------------------------*/
	/*printf("2. \n k = %d \n",k);	*/
	
	time_to_next_spike[0] = period[0]*(1.0 - phi[0]);/* Based on the period of neuron 1 and its phase, time to next spike for that neuron is calculated here */
    min= 0;/* The underlyring assumption is that the neuron 1 spikes first, genarally so its time ti next spike is ideally minimum */
     

/* Figure out which neuron (s) will spike next without further inputs */
/* Basically, once you have set the time to next spike, you are looking at the neuron that has least time to spike*/

    for (n=1;n<N;n++)
     {
     time_to_next_spike[n] = period[n]*(1.0 - phi[n]); /* Assigning the time to ext spike based on initial phases in the first run. Otherwise, based on the updation, we calculate the time*/
     /* Its basically that for every neuron, we calculate and time to next spike and know which is gonna spike immediately.*/
     if(time_to_next_spike[n] < time_to_next_spike[min] ) min = n; /* 'min' contains the index of the neuron that is going to spike next. So based on the */
     }
        /*printf("%f %d\n",time_to_next_spike[min],min);*/
 
/* figure out which inputs will arrive before the next spike then figure out which is first */

stopflag = 0;

for(j=0;j<N;j++) 
   {/* STARTS HERE*/
          
          /* Inputq is supposed to be a 2D array that has inputs indicating the queue for each neuron */
          
          /* Initially, elapsed_time was assigned to zero*/
          if ( inputq[j][0] > 0 ) /*Inputq was basically assigned to -1, so if it's >0, then a neuron is gonna receive inputs*/
          {
			  time_to_next_input[j] = inputq[j][0] - elapsed_time ; /* This is basically the calculation based on the time at which previous neuron has spiked */
          }
          /* Basically, we're trying to calculate the time to next input based on queue value and time elapsed from last spike of whichever neuron */
          
          if ( inputq[j][0] > 0 && time_to_next_input[j] < time_to_next_spike[min]) 
          {	
			/* Imples that if a neuron has non-zero input queue and if it's gonna receive inputs from the neuron that is going to fire next, then assign it to imin and break */
			init = j;
			imin = j;    /* imin is the neuron which is gonna have */
			stopflag = 1;/* break condition is given after assigning 1 to stopflag*/
			break; 
          }
          
   }/*ENDS HERE*/

     
     
if(stopflag && (init < N-1)) for(j=init+1;j<N;j++) /* If stopflag is ON, it means theres a neuron thats gonna receive input from the neuron thats firing next */ 
     
     { /* init was assigned as the neuron which was receiving input from neuron spiking next. ONce we find the neuron thats receiving input from
		 the one spiking next, we want to find out who all are gonna receive inputs from that. This loop is for neurons after the init*/
          if ( inputq[j][0] > 0 ) time_to_next_input[j] = inputq[j][0] - elapsed_time ;
          if ( inputq[j][0] > 0 && time_to_next_input[j] < time_to_next_input[imin]) imin = j;/*If any neuron after init is going to get an input before the one assigned previously, 
          this one figures it and assigns imin to that neuron which would be first receiving the input*/
     }/* imin gets assiged to indicate the neuron between init and N, if it is going to get an input faster than the one assigned init. Then imin gets assigned to it*/


/* count how many inputs will arrive at that time and mark them in the update array */

if(stopflag)/*-------------------IF STOPFLAG BEGINS HERE-------------------------*/

/* Basically, with STOPFLAG ON, we are looking at a scenario of input going to reach a neuron before the spike of next neuron in queue, 
in that case we want to count how many will spike at once and make all adjustments necessary I suppose */
{	
	
	
    interval = interval + time_to_next_input[imin];/* Interval was assigned 0 initially, now we are adding the time to next input for the neuron that is going to receive the 
    fastest input*/
    for (n=0;n<N;n++) 
         {
			update[n]=0;/* For all n neurons, we are assigning 0 to the update variable */
         }
	count = 0;
    first = 1; 
	
	min_input = inputq[imin][0]; /* Remember imin was assigned the neuron receiving the fastest input. We are looking at its input queue*/
    
    
    for (n=0;n<N;n++)
     {/* BEGINS HERE*/
     
     if(inputq[n][0] > 0  && fabs(inputq[n][0] - min_input) < 0.000000001) /* For each neuron check if its input queue is greater than zero, and also 
     if it is gonna receive input same as neuron imin*/
       {/*CONDITIONAL BEGINNING*/
		
		update[n] = 1;/* if n is going to receive input same as imin, update for it */
		  /* Actually, INPUT is a macro which has value 0 throughout. But why is it in the condition is not clear still */
		  /* However, if IPUT is 1 and first is 1, then we print k(spikes), n(n th neuron under consideration) and time to next input for the n th neuron */
        if(!INPUT&&first) printf("I %d %d %f \n",k, n,time_to_next_input[n]);
        if(!first&&INPUT) printf("I %d %f \n",n,0.000000);/* this is also slightly unclear; '!first' is 1 and INPUT is 1, then it is asking to print something*/
        
        for(i=0;i<Q-1;i++) /* This loop is for every element in the queue, basically the second index in inputq[][], first is along th neuron*/
        {
			inputq[n][i] = inputq[n][i+1]; /* So, for each neuron, kind of left shifting the queue elements - Getting input queues back to -1 */
		}
        inputq[n][Q-1] = -1.0;/* Here, we are assigning the last element of the queue to be -1 */
        first = 0;
        count = count + 1;
        
       }/* CONDITIONAL END */
     
     }/* ENDS HERE */
    
    elapsed_time =  elapsed_time + time_to_next_input[imin];
    if(!INPUT) printf("I elapsed time = %f \n",elapsed_time);


/* store the f2 contributions of the next firing to all neurons except the one emitting the input */

/* FOR NOW LET US IGNORE SECOND ORDER RESETTING */

/*    for (n=0;n<N;n++)
     {
     if(update[n]==1) 
         {if(ELAPSED) printf("%f %d\n",elapsed_time,n);
          fflush(stdout);
           for(j=0;j<N;j++)
              {
              if (j!=n) 
               {phi_mirror[j] = phi[j] + time_to_next_input[imin]/period[j];
                mark = (phi_mirror[j]*INCREMENT)/1;
                rem = (phi_mirror[j]*INCREMENT) - mark;
                if(mark<0) sumf21[j] = sumf21[j] + f2[j][0];
                if(mark>=0) sumf21[j] = sumf21[j] + f2[j][mark];
                if(mark<INCREMENT &&mark>=0) sumf21[j] = sumf21[j] +rem*(f2[j][mark+1] -f2[j][mark]);
               }
              }

         }
    }
    
 */

/* update  f1 for the neuron that is emitting an input if some other neurons are also emitting simultaneously */
 
/* For a two neuron case, this loop typically just updates the phase of the spiking neuron and does nothing else*/ 
    for (n=0;n<N;n++)
     {
     if(update[n]==1) 
        { phi[n] = phi[n] + time_to_next_input[imin]/period[n];
		  /*printf(" Before: Neuron %d, Phase %f \n",n,phi[n]);*/
          mark = (phi[n]*INCREMENT)/1;
          /*printf(" Neuron %d, Mark %d \n",n,mark);*/
          rem = (phi[n]*INCREMENT) - mark;
          /*printf(" Neuron %d, Reminder %f \n",n,rem);*/
          if(mark==INCREMENT) phi[n] = phi[n] - (count-1)*f1[n][mark];/*For all those neurons that spiked simultaneously (p), reset its phase by incorporating PRC for (p-1) neurons*/
          /* Major assumption is that there are no self-connected neurons and therefore when 6 neurons spike together, each of those will receive only 5 inputs and accordingly PRC
           gets updated */
          if(mark<INCREMENT && mark>=0) phi[n] = phi[n] - (count-1)*f1[n][mark] - (count-1)*rem*(f1[n][mark+1] -f1[n][mark]);
          if(mark<0) phi[n] = phi[n] - (count-1)*f1[n][0] ;
          /*printf("After:  Neuron %d, Phase %f \n",n,phi[n]);*/
        }
      }

/* update the phase and add the f1 contributions for the neurons that aren't emiting inputs */

    for (n=0;n<N;n++)
    {
		if(update[n]==0) 
		  { 
			phi[n] = phi[n] + time_to_next_input[imin]/period[n];
			/*printf(" Before resetting: Neuron %d, Phase %f \n",n,phi[n]);*/
			mark = (phi[n]*INCREMENT)/1; /* Since mark is assigned as an integer variable it automatically truncates the final value and removes its decimals */
			rem = (phi[n]*INCREMENT) - mark; /* Reminder is assigned as a double variable and thus it takes the decimal value */
			if(mark==INCREMENT) phi[n] = phi[n] - f1[n][mark]; /* This means both are spiking at once??? */
			if(mark<INCREMENT && mark>=0) phi[n] = phi[n] - f1[n][mark] - rem*(f1[n][mark+1] -f1[n][mark]);
			if(mark<0) phi[n] = phi[n] - f1[n][0];
			/*printf(" After resetting: Neuron %d, Phase %f \n",n,phi[n]);*/
		 }
    }



}/*-------------------IF STOPFLAG ENDS HERE-------------------------*/


}while(stopflag); 
/*----------------------------------------------------------------Do ENDS HERE -----------------------------------------------------------------------------------------------------------------------------*/



/* how many neurons are firing */
    for (n=0;n<N;n++) 
    {
		update[n]=0;
    }
    count = 0;
    first = 1; 
    elapsed_time = elapsed_time + time_to_next_spike[min];
    /*printf("%f",elapsed_time);*/
    for (n=0;n<N;n++)
    {
     if(fabs(time_to_next_spike[n] - time_to_next_spike[min]) < 0.000000001)/* Basically this condition is satisfied only by the neuron which will spike during this iteration*/
		{/* Recording the spike of the neuron with least sike time and moving to that time. */
			update[n] = 1;
			/* load input queue */
			for(i=0;i<Q;i++)
			{
				if(inputq[n][i] <0)
				{ 	
					inputq[n][i] = elapsed_time + delay[n];
					break; /* This is for the for loop just above across the queues and not the one across neurons */
				}
			}
			spiking[n] = time_to_next_spike[n] + interval;
			if(INTERVAL&&first) printf(" End of Iteration: \n Neuron %d:  , Time to Next Spike: %f \n",n , time_to_next_spike[n] + interval);/*This is the Line that is printing*/
       
			if(!first&&INTERVAL) printf("%d %f \n",n,0.000000);
			first = 0;
			count = count + 1;
		}
    }
    if(!INPUT) for (n=0;n<Q;n++) printf("%f",inputq[0][n]);
    if(!INPUT) printf("\n");
    if(!INPUT) for (n=0;n<Q;n++) printf("%f",inputq[1][n]);
    if(!INPUT) printf("\n");
	if(INPUT) printf("k = %d, Elapsed Time = %f \n",k,elapsed_time);

/* update time to next spike add f2*/
/* update the f2 contributions from the previous cycle for the neuron(s)
that is firing */
 
    for (n=0;n<N;n++)
    {
		if(update[n]==1) 
        {
			 phi[n]= 0.0 - sumf21[n] ;
			 sumf21[n]=0;
        }
    } 
/* update nonfiring neurons to spike time */
    for (n=0;n<N;n++)
     {
     if(update[n]==0) 
        { 
			phi[n] = phi[n] + time_to_next_spike[min]/period[n];
        }
     printf(" Phase of neuron %d is %f \n",n, phi[n]);
     }

/*
	if(k >= 50 && k <= 55) 
	{
		printf("k = %d, TTNS[0] = %f, TTNS[1] = %f",k,spiking[0],spiking[1]);
		if(spiking[0]<0.05)checker1 = checker1 +1;
		if(spiking[1]<0.05)checker2 = checker2 +1;
		if (checker1 == 3) 
		{ 
			printf("Checker 1 = %d",checker1);
			for(i = 0;i < INCREMENT+1; i++) f1[1][i] = f3[i];
			period[1] = 6.2058;
			
		}
		if (checker2 == 3) 
		{ 
			printf("Checker 2 = %d",checker2);
			for(i = 0;i < INCREMENT+1; i++) f1[1][i] = f3[i];
			period[1] = 6.2058;
		}
	}
*/
}/*----------------------------------------------------------------------------   MAIN ROUTINE LOOP ENDS  --------------------------------------------------------------------------------------------------------- */



for (n=0;n<N;n++) 
{
	fprintf(fp8,"%f ",phi[n]); 
}

fclose(fp1);
fclose(fp2);
fclose(fp3);
fclose(fp4);
/*fclose(fp5);*/
fclose(fp6);
/*fclose(fp7);*/
fclose(fp8);

}
