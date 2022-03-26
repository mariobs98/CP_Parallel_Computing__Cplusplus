# CP_Parallel_Computing__Cplusplus

Given a matrix A of (N+1)xN integers, the median of each row of the matrix must be calculated. 
To obtain the median of a vector (row of the matrix), you must sort the vector and select the element that is in the center point. 
Once the medians have been obtained, it must be calculated how many multiples of each of them there are in the matrix. In case the median is 0, the result 0. For example:
Matrix A: 6 -2 5 5 8 -6 -3 
          5 -4 1 0 2 -2 0 
          9 8 2 4 -7 2 -9 
          -5 -7 6 -6 -1 -7 -7 
          9 -5 0 6 2 -4 2 
          0 0 9 -4 5 0 5 
          -1 7 -4 -2 6 8 -8 
          8 0 -8 -6 3 7 -3 

Matrix A con las filas ordenadas(ordered rows): 
          6 -2 5 5 8 -6 -3 
          5 -4 1 0 2 -2 0 
          9 8 2 4 -7 2 -9 
          -5 -7 6 -6 -1 -7 -7 
          9 -5 0 6 2 -4 2 
          0 0 9 -4 5 0 5 
          -1 7 -4 -2 6 8 -8 
          8 0 -8 -6 3 7 -3 

Medianas y múltiplos(medians and multiples): 
          5 0 2 -6 2 0 -1 0 
          14 0 33 14 33 0 56 0 
          
To perform the ordering of the rows of the matrix, an own method must be implemented (the ordering functions available in c/c++ cannot be used).

INPUT FILE FORMAT:
The values of the different elements that make up the exercise must be indicated by means of an input file. The format of this file will be as follows: 

Number of experiments → Integer 
Size of the matrix of experiment 1 → Integer 
Seed of experiment 1 → Integer 
Lower limit of the values of the matrix of experiment 1 → Integer 
Upper limit of the values of the matrix of experiment 1 → Integer

The values from line 2 will be repeated as many times as experiments are desired to be carried out. Values can be separated by newlines or spaces (for ease of understanding, it is recommended to use one line per experiment). 
On the other hand, in order to correctly calculate the median value, the number of columns in the matrix must be odd (if it is an even value, there would be no data in the center). 

IMPORTANT: Letters or blank lines should not be used since the main program does not do any control over the file format, it simply reads numbers consecutively. In addition, as many groups of parameters as indicated in "Number of experiments" will be read, so if additional groups are added, they will not be taken into account, and if groups are missing, the behavior will be unpredictable. 

Input file example: 

2 
11 1 -1000 1000 
1021 4 -994 994


