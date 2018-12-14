/***************************************************************
Author:         Jeriel G. Jaro
Description:    Library for Pearson-Correlation Coefficient
Date:           21/11/2018
Section:        GH-1L
**************************************************************/
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
#include  <time.h>
#include  <sys/time.h>
#include  <mpi.h>
#include  "lab05.h"

#define ROOT        0
#define DEFAULT_DIM 3

int main(int argc, char* argv[]){
  // Variable Declarations
  int *x = NULL,                            // master matrix; to be scattered into slave matrices
      *y = NULL,                            // master vector; to be broadcasted
      *z = NULL,                            // slave  matrix
      *zm   = NULL,                         // slave m   dimension
      *zmn  = NULL,                         // slave m*n size
      *zmo  = NULL,                         // master m   offsets; needed for scattering
      *zmno = NULL,                         // master m*n offsets; needed for gathering
       mm, mn, m, n,                        // dimensions: mm,mn are masters; m,n are slaves
       i, j,                                // counters
       tmpsize, tmpr, tmpo,                 // temp variables used in generating zm,zmn,zmno,zmo
       csize,                               // comm world size
       crank,                               // comm individual rank
       clen;                                // computer name length
  char cname[MPI_MAX_PROCESSOR_NAME];       // computer name
  float *p = NULL,                          // master pearson vector
        *q = NULL;                          // slave  pearson vector
  double tsec, tmill;                       // pearson's time-lapse in seconds and milliseconds
  struct timeval tstart,tend;               // pearson's start and end time

  MPI_Init(&argc, &argv);

  // Fetch MPI Details
  MPI_Comm_size(MPI_COMM_WORLD, &csize);
  MPI_Comm_rank(MPI_COMM_WORLD, &crank);
  MPI_Get_processor_name(cname, &clen);

  // Initial master jobs
  if(crank==0){
    // Fetch dimensions m,n of master matrix x
    if(argc==2) mm=n=strtol(argv[1],NULL,10);
    else if(argc==3){
      mm=strtol(argv[1],NULL,10);
      n=strtol(argv[2],NULL,10);
    }
    else mm=n=DEFAULT_DIM;

    // Generate matrix x and vector y
    x = malloc(mm*n*sizeof(*x));
    y = malloc(n*sizeof(*y));
    p = malloc(mm*sizeof(*p));
    srand(time(NULL));
    for(i=0;i<mm;i++){
      for(j=0;j<n;j++){
          x[i*n+j]= rand()%10+1;
          if(i==0) y[j] = rand()%10+1;
      }
    }

    // Generate slave m dimensions and offsets to be sent to slave nodes
    zm  = malloc(csize*sizeof(*zm));    //slave m dimension
    zmn = malloc(csize*sizeof(*zmn));   //slave m*n array size; individual buffer size
    zmo = malloc(csize*sizeof(*zmo));   //individual offsets on master matrix x
    zmno= malloc(csize*sizeof(*zmno));  //individual offsets on master matrix x
    tmpsize= mm/csize;
    tmpr   = mm%csize;
    tmpo   = 0;
    for (int i = 0; i < csize; i++) {
      zm[i] = tmpsize;
      zm[i] = (tmpr>0) ? zm[i]+1:zm[i];
      zmn[i]= zm[i]*n;
      zmo[i] = tmpo;
      zmno[i]= tmpo*n;
      tmpo  +=zm[i];
      tmpr  -=1;
    }
  }

  // Fetch dimensions of slave matrices. Master will send out the n-size,
  // and scatter the assigned elements for each slave. 
  MPI_Bcast(&n, 1, MPI_INT,ROOT, MPI_COMM_WORLD);
  MPI_Scatter(zm, 1, MPI_INT, &m, 1, MPI_INT,ROOT,MPI_COMM_WORLD);

  //Initialize slave matrices
  if(crank!=ROOT) y = malloc(n*sizeof(*y));
  z = malloc(m*n*sizeof(*z));
  q = malloc(m*n*sizeof(*q));

  // Solve pearson correlation-coefficient on slave matrices and gather to master matrix
  gettimeofday(&tstart, NULL);
  MPI_Scatterv(x,zmn,zmno,MPI_INT,z,m*n,MPI_INT,ROOT,MPI_COMM_WORLD);
  MPI_Bcast(y,n,MPI_INT,ROOT,MPI_COMM_WORLD);
  pearsonCC(q,z,y,m,n);
  MPI_Gatherv(q,m,MPI_INT,p,zm,zmo,MPI_INT,ROOT,MPI_COMM_WORLD);
  gettimeofday(&tend, NULL);

  // Fetch timelapse of execution
  tsec = (tend.tv_sec - tstart.tv_sec) + ((tend.tv_usec - tstart.tv_usec)/1000000.0);
  tmill= 1000*(tend.tv_sec - tstart.tv_sec) + ((tend.tv_usec - tstart.tv_usec)/1000.0);

  // Print timelapse
  printf("%s rnk:%d finished w/in %fs or %fms\n", cname, crank,tsec,tmill);

  // Free Matrices
  if(crank==ROOT){
      // Print Results
      printResults(p,x,y,mm,n);
      free(x);
      free(p);
      free(zm);
      free(zmn);
      free(zmo);
      free(zmno);
  }
  free(y);
  free(z);
  free(q);

  MPI_Finalize();
  return 0;
}
