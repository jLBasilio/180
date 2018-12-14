#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include<sys/time.h>
#include<pthread.h>


float *pearsonValues;
typedef struct node{
  int **ownMatrixX, *vectorY;
  int startIndex, size, colSize;
} arg;


void printMatrix(int size, int cols, int **matrixX) {
  int i, j;
  printf("\n");
  for (i = 0; i < size; i++) {
    for (j = 0; j < cols; j++) {
      printf("%d ", matrixX[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void printVector(int size, int *vector) {
  int j;
  for (j = 0; j < size; j++) {
    printf("%d ", vector[j]);
  }
  printf("\n");
}

void printResults(int size, float *vector) {
  int j;
  for (j = 0; j < size; j++) {
    printf("%f ", vector[j]);
  }
  printf("\n");
}

int sumOfVector(int *vector, int size) {
  int sum = 0, i;
  for (i = 0; i < size; i++) 
    sum += vector[i];
  return sum;
}

int sumOfSquaredVector(int *vector, int size) {
  int sum = 0, i;
  for (i = 0; i < size; i++)
    sum += (vector[i] * vector[i]);
  return sum;
}


// Gets the matrix vector multiplication, sum of columns, squared sum of columns
void multiOperation(int **ownMatrixX, int *vectorY, int size, int colSize, int *Xjy, int *xSummation, int *xSquaredInside) {

  int i, j, XjyElement, summationElement, squaredInsideElement;
  
  i = 0;
  while ( i < colSize ) {
    XjyElement = 0;
    summationElement = 0;
    squaredInsideElement = 0;

    for (j = 0; j < size; j++) {
      XjyElement += ownMatrixX[j][i] * vectorY[j];
      summationElement += ownMatrixX[j][i];
      squaredInsideElement += ownMatrixX[j][i] * ownMatrixX[j][i];
    }

    Xjy[i] = XjyElement;
    xSummation[i] = summationElement;
    xSquaredInside[i] = squaredInsideElement;
    i++;
  }
}
  
// void *pearson_cor(int size, int **matrixX, int *vectorY, float *pearsonValues, int startIndex) {
  void *pearson_cor(void *args) {

  arg *argLocal = (arg *) args;
  int size = argLocal->size;
  int colSize = argLocal->colSize;
  int startIndex = argLocal->startIndex;
  int **ownMatrixX = argLocal->ownMatrixX;
  int *vectorY = argLocal->vectorY;

  int *Xjy = (int *) malloc(size * sizeof(int));
  int *xSummation = (int *) malloc(size * sizeof(int));
  int *xSquaredInside = (int *) malloc(size * sizeof(int));
  int i, mxjy, xjy, y, ySquaredInside, ySquared, numerator; 
  float denominator1, denominator2;
  

  // Perform matrix to vector multiplication, also get the sum of columns, and the sum of squared elements of columns
  multiOperation(ownMatrixX, vectorY, size, colSize, Xjy, xSummation, xSquaredInside);
  y = sumOfVector(vectorY, size);
  ySquaredInside = sumOfSquaredVector(vectorY, size) * size;
  ySquared = y * y;

  // printVector(size, Xjy);
  // printVector(size, xSummation);
  // printVector(size, xSquaredInside);
  // printf("Xjy: ");
  // printf("Xsummation: ");
  // printf("xSquaredInside: ");
  // printf("Y: %d\n", y);
  // printf("ySquaredInside: %d\n", ySquaredInside);
  // printf("ySquared: %d\n", ySquared);

  i = 0;
  while( i < colSize ) {

    mxjy = size * Xjy[i];
    xjy = xSummation[i] * y;
    numerator = mxjy - xjy;
    denominator1 = size * xSquaredInside[i] - xSummation[i] * xSummation[i];
    denominator2 = ySquaredInside - ySquared;
    pearsonValues[startIndex] = numerator / sqrt(denominator1 * denominator2);

    // printf("Numerator: %d\n", numerator);
    // printf("denominator1: %f\n", denominator1);
    // printf("denominator2: %f\n", denominator2);
    // printf("finalDenom: %f\n", sqrt(denominator1 * denominator2));
    // printf("Result: %f\n", pearsonValues[startIndex]);

    startIndex++;
    i++;

  }

  free(Xjy);
  free(xSummation);
  free(xSquaredInside);
  pthread_exit(NULL);
}

int main() {

  struct timeval begin, end;
  double timeTaken;
  int size, colSize, threadSize, subMatrices, remainder, i, j, k, startIndex, endIndex;
  int *vectorY;
  pthread_t *threadArray;
  arg *argArray;

  // printf("Input n<space>t: ");
  // scanf("%d %d", &size, &threadSize);
  printf("Input t: ");
  scanf("%d", &threadSize);
  size = 25000;

  // Initiate Matrix
  // matrixX = (int **) malloc(size * sizeof(int*));
  vectorY = (int *) malloc(size * sizeof(int));
  pearsonValues = (float *) malloc(size * sizeof(float));

  // Initialize thread stuff
  threadArray = (pthread_t *) malloc(threadSize * sizeof(pthread_t));
  argArray = (arg *) malloc(size * sizeof(arg));

  // Divide matrix into chunks of submatrices
  subMatrices = size / threadSize;
  remainder = size % threadSize;

  // printf("Remainder: %d\n", remainder);

  // Seed and Randomize
  srand(time(NULL));
  for (i = 0; i < size; i++) 
    vectorY[i] = rand() % 10 + 1;

  // Create args for threads
  startIndex = 0;
  endIndex = subMatrices;
  colSize = subMatrices;

  for (i = 0; i < threadSize; i++) {

    // last thread gets the remainder
    if (i == threadSize - 1) {
      colSize += remainder;
      endIndex += remainder;
    }

    argArray[i].ownMatrixX = (int **) malloc(size * sizeof(int*));
    for (j = 0; j < size; j++) {
      argArray[i].ownMatrixX[j] = (int *) malloc(colSize * sizeof(int));
      for (k = 0; k < colSize; k++)
        argArray[i].ownMatrixX[j][k] = rand() % 10 + 1;
    }

    argArray[i].startIndex = startIndex;
    argArray[i].size = size;
    argArray[i].colSize = colSize;
    argArray[i].vectorY = vectorY;
    startIndex = endIndex;
    endIndex += subMatrices;
  }


  // printVector(size, vectorY);


  // int accessor = threadSize;
  // for (i = 0; i < threadSize; i++) {
  //   if( i == threadSize - 1)
  //     accessor += remainder;
  //   printMatrix(size, accessor, argArray[i].ownMatrixX);
  // }



  // matrixX[0][0] = 3;
  // matrixX[0][1] = 2;
  // matrixX[0][2] = 1;
  // matrixX[0][3] = 16;
  // matrixX[0][4] = 1;
  // matrixX[1][0] = 4;
  // matrixX[1][1] = 5;
  // matrixX[1][2] = 6;
  // matrixX[1][3] = 5;
  // matrixX[1][4] = 2;
  // matrixX[2][0] = 7;
  // matrixX[2][1] = 8;
  // matrixX[2][2] = 9;
  // matrixX[2][3] = 4;
  // matrixX[2][4] = 3;
  // matrixX[3][0] = 10;
  // matrixX[3][1] = 11;
  // matrixX[3][2] = 12;
  // matrixX[3][3] = 5;
  // matrixX[3][4] = 4;
  // matrixX[4][0] = 11;
  // matrixX[4][1] = 3;
  // matrixX[4][2] = 7;
  // matrixX[4][3] = 2;
  // matrixX[4][4] = 6;


  /* t = 2 */
  // vectorY[0] = 4;
  // vectorY[1] = 5;
  // vectorY[2] = 6;
  // vectorY[3] = 7;
  // vectorY[4] = 8;

  // argArray[0].ownMatrixX[0][0] = 3;
  // argArray[0].ownMatrixX[0][1] = 2;
  // argArray[0].ownMatrixX[1][0] = 4;
  // argArray[0].ownMatrixX[1][1] = 5;
  // argArray[0].ownMatrixX[2][0] = 7;

  // argArray[0].ownMatrixX[2][1] = 8;
  // argArray[0].ownMatrixX[3][0] = 10;
  // argArray[0].ownMatrixX[3][1] = 11;
  // argArray[0].ownMatrixX[4][0] = 11;
  // argArray[0].ownMatrixX[4][1] = 3;

  // argArray[1].ownMatrixX[0][0] = 1;
  // argArray[1].ownMatrixX[0][1] = 16;
  // argArray[1].ownMatrixX[0][2] = 1;

  // argArray[1].ownMatrixX[1][0] = 6;
  // argArray[1].ownMatrixX[1][1] = 5;
  // argArray[1].ownMatrixX[1][2] = 2;

  // argArray[1].ownMatrixX[2][0] = 9;
  // argArray[1].ownMatrixX[2][1] = 4;
  // argArray[1].ownMatrixX[2][2] = 3;

  // argArray[1].ownMatrixX[3][0] = 12;
  // argArray[1].ownMatrixX[3][1] = 5;
  // argArray[1].ownMatrixX[3][2] = 4;

  // argArray[1].ownMatrixX[4][0] = 7;
  // argArray[1].ownMatrixX[4][1] = 2;
  // argArray[1].ownMatrixX[4][2] = 6;


  /* t = 3 */
  // vectorY[0] = 4;
  // vectorY[1] = 5;
  // vectorY[2] = 6;
  // vectorY[3] = 7;
  // vectorY[4] = 8;

  // argArray[0].ownMatrixX[0][0] = 3;
  // argArray[0].ownMatrixX[1][0] = 4;
  // argArray[0].ownMatrixX[2][0] = 7;
  // argArray[0].ownMatrixX[3][0] = 10;
  // argArray[0].ownMatrixX[4][0] = 11;

  // argArray[1].ownMatrixX[0][0] = 2;
  // argArray[1].ownMatrixX[1][0] = 5;
  // argArray[1].ownMatrixX[2][0] = 8;
  // argArray[1].ownMatrixX[3][0] = 11;
  // argArray[1].ownMatrixX[4][0] = 3;

  // argArray[2].ownMatrixX[0][0] = 1;
  // argArray[2].ownMatrixX[0][1] = 16;
  // argArray[2].ownMatrixX[0][2] = 1;

  // argArray[2].ownMatrixX[1][0] = 6;
  // argArray[2].ownMatrixX[1][1] = 5;
  // argArray[2].ownMatrixX[1][2] = 2;

  // argArray[2].ownMatrixX[2][0] = 9;
  // argArray[2].ownMatrixX[2][1] = 4;
  // argArray[2].ownMatrixX[2][2] = 3;

  // argArray[2].ownMatrixX[3][0] = 12;
  // argArray[2].ownMatrixX[3][1] = 5;
  // argArray[2].ownMatrixX[3][2] = 4;

  // argArray[2].ownMatrixX[4][0] = 7;
  // argArray[2].ownMatrixX[4][1] = 2;
  // argArray[2].ownMatrixX[4][2] = 6;


  // vectorY[0] = 4;
  // vectorY[1] = 5;
  // vectorY[2] = 6;
  // vectorY[3] = 7;
  // vectorY[4] = 8;

  // for (i = 0; i < threadSize; i++)
  //   printMatrix(size, argArray[i].colSize, argArray[i].ownMatrixX);
  // printVector(size, vectorY);

  // printMatrix(size, matrixX);


  // Transpose array

  // Start the threads and join after
  gettimeofday(&begin, NULL);
  for (i = 0; i < threadSize; i++) 
    pthread_create(&threadArray[i], NULL, pearson_cor, (void *) &argArray[i]);
  for(i = 0; i < threadSize; i++)
    pthread_join(threadArray[i], NULL);
  gettimeofday(&end, NULL);
  // printResults(size, pearsonValues);
  timeTaken =  (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
  unsigned long long t = 1000 * (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec) / 1000;
  printf("Time taken: %f seconds\n", timeTaken);
  printf("Time taken: %llu milliseconds\n", t);

  // Free stuff
  for (i = 0; i < threadSize; i++) {
    for (j = 0; j < size; j++)
      free(argArray[i].ownMatrixX[j]);
    free(argArray[i].ownMatrixX);
  }
  free(vectorY);
  free(pearsonValues);

  return 0;
}
