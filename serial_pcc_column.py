import random
import time
import numpy

def printSquareMatrix(n):
  for i in range(len(n)):
    for j in range(len(n)):
      print(n[j][j], end=" ")
    print()

def initializeOneVector(n):
  listReturn = []
  for i in range(n):
    listReturn.append(random.randrange(10))
  return listReturn

def initializeNxN(n):
  nxnMatrix = []
  for i in range(n):
    oneVector = []
    for j in range(n):
      oneVector.append(random.randrange(10))
    nxnMatrix.append(oneVector)
  return nxnMatrix

#Returns  a vector v
def pearson_cor(matrixX, vectorY, mRow, nColumn):
  vectorV = []
  matrixLen = len(matrixX)
  Xjy = sum([i*j for i,j in zip(matrixX,vectorY)])      #Get the all the dot product of the vectors of matriX against the vectorY, performed column-wise
  XSummation = numpy.sum(matrixX, axis=0)               #Compute the summation of matriX in columns, given by axis=0
  xSquaredInside = numpy.sum(matrixX ** 2, axis=0)      #Get the squared summation of columns in matrix
  y = sum(vectorY)                                      #Get the sum of vectorY
  ySquaredInside = matrixLen * sum(map(lambda y: y * y, vectorY)) #Get the squared summation of elements of vectorY multiplied by the size of matrix
  ySquared = y ** 2                                     #Get the square of summation of vectorY
  # print("Xjy", Xjy)
  # print("Xsummation", XSummation)
  # print("Y", y)
  # print("xSquaredInside", xSquaredInside)
  # print("ySquaredInside", ySquaredInside)
  # print("ySquared", ySquared)

  for j in range(mRow):
    mxjy = matrixLen * Xjy[j]                           #Select the Xjy from the list of computed dot products. multiplied by the size of matrix
    xjy = XSummation[j] * y                             #Select the x from the summations of rows of Matrix, multiplied by the summation of vectorY
    numerator = mxjy - xjy
    denominator1 = (matrixLen * xSquaredInside[j]) - (XSummation[j] * XSummation[j])
    denominator2 = ySquaredInside - ySquared
    finaldenom = (denominator1 * denominator2) ** 0.5

    print("Numerator ", numerator)
    print("Denominator1", denominator1)
    print("denominator2", denominator2)
    print("Finaldenom", finaldenom);

    print("Result: ", numerator / finaldenom)

    vectorV.append((mxjy - xjy) / (((matrixLen * xSquaredInside[j] - XSummation[j] ** 2) * (ySquaredInside - ySquared)) ** 0.5))       #Perform Pearson operation
  return vectorV

''' Main program '''
nInput = int(input("Input n: "))
matrixX = initializeNxN(nInput)                         #Initializes the nxn matrix
vectorY = initializeOneVector(nInput)                   #Initializes the n-sized vector
matrixX = [[3, 2, 1, 16, 1], [4, 5, 6, 5, 2], [7, 8, 9, 4, 3], [10, 11, 12, 5, 4], [11, 3, 7, 2 ,6]]
vectorY = [4, 5, 6, 7, 8]
time_before = time.time()
returnVal = pearson_cor(numpy.array(matrixX), numpy.array(vectorY), nInput, nInput)
time_after = time.time()
print("Time elapsed: ", time_after - time_before)
print("returnVal: ", returnVal)