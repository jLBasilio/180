import random
import time
import numpy

def printSquareMatrix(n):
  for i in range(len(n)):
    for j in range(len(n)):
      print(n[i][j], end=" ")
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
  # Xjy = sum([i*j for i,j in zip(vectorY, matrixX)])
  Xjy = numpy.dot(matrixX, vectorY)           #Get the dot product of two vectors
  XSummation = numpy.sum(matrixX, axis=1)               
  y = sum(vectorY)
  xSquaredInside = numpy.sum(matrixX ** 2, axis=1)
  ySquaredInside = matrixLen * sum(map(lambda y: y * y, vectorY))
  ySquared = y ** 2

  for j in range(mRow):
    mxjy = matrixLen * Xjy[j]
    xjy = XSummation[j] * y
    vectorV.append((mxjy - xjy) / (((matrixLen * xSquaredInside[j] - XSummation[j] ** 2) * (ySquaredInside - ySquared)) ** 0.5))
  return vectorV

''' Main program '''
nInput = int(input("Input n: "))
matrixX = initializeNxN(nInput)
vectorY = initializeOneVector(nInput)
# matrixX = [[3, 2, 1, 16], [4, 5, 6, 5], [7, 8, 9, 4], [10, 11, 12, 5]]
# vectorY = [4, 5, 6, 7]

matrixX = numpy.transpose(numpy.array(matrixX))
vectorY = numpy.array(vectorY)

time_before = time.time()
unnecessaryReturn = pearson_cor(matrixX, vectorY, nInput, nInput)
time_after = time.time()
# print("Pearson: ", unnecessaryReturn)
print("Time elapsed: ", time_after - time_before)