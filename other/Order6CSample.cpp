/*
 *  File:    Order6CSample.cpp 
 *  Author:  S Harry White
 *  Created: 2009-08-06
 *  Updated: 2012-01-29
 */

// #include "stdafx.h"
#include <conio.h>
#include <errno.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

const int M  = 4,
          MM = M * M,                // 16

          N    = 6,
          NN   = N * N,             //  36
          Half = NN / 2,            //  18

// numbers 0 .. NN-1
          Sum2 = NN - 1,            //  35
          Sum4 = Sum2 + Sum2,       //  70
          Sum6 = Sum4 + Sum2,       // 105

//Partitions of 0 .. NN-1 as MM/2 pairs and other (NN-MM)/2 pairs.
          numPartitions = 43758;     // 18! / (8!10!)
//---------------------------- start center data ----------------------------

int Centers[numPartitions][MM], numCenters;

const int maxCenterCombos = 86, maxCenterPermus = 2064;    // 86 x 4!
int CenterCombos[maxCenterCombos][M], numCenterCombos;
int CenterPermus[maxCenterPermus][M], numCenterPermus;

// Only CenterPermus in which third > second.
const int maxCenterGPermus = 1032;
int *CenterGPermus[maxCenterGPermus], numCenterGPermus;

// CenterGPermus indexed by the second and third numbers.
const int maxCenterGPermus23 = 14;
int *CenterGPermus23[NN][NN][maxCenterGPermus23],
    numCenterGPermus23[NN][NN],
// Third number of CenterPermus indexed by the first, second, and fourth.
    CenterPermus124[NN][NN][NN];

int xSquare[N][N];

const int numSquares = 22145;
// Output is 1 square for each of the partitions that make a center square.
//---------------------------- start border data ----------------------------

int Borders[numPartitions][NN-MM];

// Combinations that do not include complementary pairs.
const int maxBorderCombos = 330;
int BorderCombos[maxBorderCombos][N], numBorderCombos;

// Groups of BorderCombos in which the sixth is greater than the first.
const int maxBorderGGroups = 4950;
int BorderGGroups[maxBorderGGroups][N], numBorderGGroups;

// BorderGGroups indexed by the first and sixth numbers.
const int maxBorderGGroups16 = 43;
int *BorderGGroups16[NN][NN][maxBorderGGroups16],
    numBorderGGroups16[NN][NN];
//------------------------------- start code --------------------------------

void putCenter(int a, int b, int c, int d, int e, int f, int g, int h) {
//   ---------
  int *p = Centers[numCenters++];

  p[0] = a;  p[1] = b; p[2] = c;  p[3] = d;
  p[4] = e;  p[5] = f; p[6] = g;  p[7] = h;
  p[8]  = Sum2 - h;  p[9]  = Sum2 - g;  p[10] = Sum2 - f;  p[11] = Sum2 - e;
  p[12] = Sum2 - d;  p[13] = Sum2 - c;  p[14] = Sum2 - b;  p[15] = Sum2 - a;
} // putCenter
//---------------------------------------------------------------------------

void getCenters() {
//   ----------
  numCenters = 0;

  for (int i1 = 0; i1 < (Half-7); i1++)
    for (int i2 = i1+1; i2 < (Half-6); i2++)
      for (int i3 = i2+1; i3 < (Half-5); i3++)
	    for (int i4 = i3+1; i4 < (Half-4); i4++)
	      for (int i5 = i4+1; i5 < (Half-3); i5++)
	        for (int i6 = i5+1; i6 < (Half-2); i6++)
	          for (int i7 = i6+1; i7 < (Half-1); i7++)
		        for (int i8 = i7+1; i8 < Half; i8++)
		          putCenter(i1, i2, i3, i4, i5, i6, i7, i8);
 // assert(numCenters == numPartitions);
} // getCenters
//---------------------------------------------------------------------------

struct bools { bool used[NN]; };
struct bools allfree;

void getBorders() {
//   ----------
  for (int i = 0; i < numPartitions; i++) {
    struct bools v = allfree; int *p = Centers[i];

    for (int j = 0; j < MM; j++) v.used[p[j]] = true;

    p = Borders[i];  int k = 0;
    for (int j = 0; j < NN; j++) if (!v.used[j]) p[k++] = j;
  }
} // getBorders
//---------------------------------------------------------------------------

void getPartitions() {
//   -------------
  getCenters();
  getBorders();
} // getPartitions
//---------------------------- start center code ----------------------------

void putCenterCombo(int a, int b, int c, int d) {
//   --------------
  int *p = CenterCombos[numCenterCombos++];
  p[0] = a;  p[1] = b;  p[2] = c;  p[3] = d;
} // putCenterCombo
//---------------------------------------------------------------------------

void getCenterCombos(int i) {
//   ---------------
  numCenterCombos = 0; int *p = Centers[i];

  for (int i = 0; i < (MM-3); i++)
    for (int j = i+1; j < (MM-2); j++)
      for (int k = j+1; k < (MM-1); k++)
	    for (int l = k+1; l < MM; l++)
	      if (p[i] + p[j] + p[k] + p[l] == Sum4)
	        putCenterCombo(p[i], p[j], p[k], p[l]);
} // getCenterCombos
//----------------------------------------------------------------------------

void permute4_3(int a, int b, int c, int d) {
//   ----------
  int *p;

  p = CenterPermus[numCenterPermus++];
  p[0] = a;  p[1] = b;  p[2] = c;  p[3] = d;
  p = CenterPermus[numCenterPermus++];
  p[0] = a;  p[1] = b;  p[2] = d;  p[3] = c;
  p = CenterPermus[numCenterPermus++];
  p[0] = a;  p[1] = c;  p[2] = b;  p[3] = d;
  p = CenterPermus[numCenterPermus++];
  p[0] = a;  p[1] = c;  p[2] = d;  p[3] = b;
  p = CenterPermus[numCenterPermus++];
  p[0] = a;  p[1] = d;  p[2] = b;  p[3] = c;
  p = CenterPermus[numCenterPermus++];
  p[0] = a;  p[1] = d;  p[2] = c;  p[3] = b;
} // permute4_3
//---------------------------------------------------------------------------

void getCenterPermus() {
//   ---------------
  numCenterPermus = 0;

  for (int i = 0; i < numCenterCombos; i++) {
    int *p = CenterCombos[i];

    permute4_3(p[0], p[1], p[2], p[3]);	 
    permute4_3(p[1], p[0], p[2], p[3]);
    permute4_3(p[2], p[1], p[0], p[3]);
    permute4_3(p[3], p[1], p[2], p[0]);
  }
} // getCenterPermus
//---------------------------------------------------------------------------

void getCenterGPermus() {
//   ----------------
  numCenterGPermus = 0;

  for (int i = 0; i < numCenterPermus; i++) {
    int *p = CenterPermus[i];

    if (p[2] > p[1]) CenterGPermus[numCenterGPermus++] = p;
  }
} // getCenterGPermus
//---------------------------------------------------------------------------

void getCenterGPermus23() {
//   ------------------
  for (int i = 0; i < NN; i++)
    for (int j = 0; j < NN; j++)
      numCenterGPermus23[i][j] = 0;

  for (int i = 0; i < numCenterGPermus; i++) {
    int *p = CenterGPermus[i];

    CenterGPermus23[p[1]][p[2]][(numCenterGPermus23[p[1]][p[2]])++] = p;
  }
} // getCenterGPermus23 
//----------------------------------------------------------------------------

void getCenterPermus124() {
//   ------------------
  for (int i = 0; i < NN; i++)
    for (int j = 0; j < NN; j++)
      for (int k = 0; k < NN; k++)
        CenterPermus124[i][j][k] = -1;

  for (int i = 0; i < numCenterPermus; i++) {
    int *p = CenterPermus[i];

    CenterPermus124[p[0]][p[1]][p[3]] = p[2];
  }
} // getCenterPermus124
//----------------------------- start border code ----------------------------

void putBorderCombo(int a, int b, int c, int d, int e, int f) {
//   --------------
  int *p = BorderCombos[numBorderCombos++];

  p[0] = a;  p[1] = b;  p[2] = c;  p[3] = d;  p[4] = e;  p[5] = f;
} // putBorderCombo
//--------------------------------------------------------------------------

void getBorderCombos(int i) {
//   ---------------
  int *p = Borders[i]; struct bools v = allfree; numBorderCombos = 0;

  for (int i = 0; i < (NN-MM-5); i++) {
    v.used[Sum2-p[i]] = true;
    for (int j = i+1; j < (NN-MM-4); j++) if (!v.used[p[j]]) {
      v.used[Sum2-p[j]] = true;
      for (int k = j+1; k < (NN-MM-3); k++) if (!v.used[p[k]]) {
        v.used[Sum2-p[k]] = true;
	      for (int l = k+1; l < (NN-MM-2); l++) if (!v.used[p[l]]) {
          v.used[Sum2-p[l]] = true;
	        for (int m = l+1; m < (NN-MM-1); m++) if (!v.used[p[m]]) {
            v.used[Sum2-p[m]] = true;
            for (int n = m+1; n < (NN-MM); n++) if (!v.used[p[n]])
              if ( ((p[i] + p[j] + p[k] + p[l] + p[m]+ p[n]) == Sum6) )
		            putBorderCombo(p[i], p[j], p[k], p[l], p[m], p[n]);
            v.used[Sum2-p[m]] = false;
          }
          v.used[Sum2-p[l]] = false;
        }
        v.used[Sum2-p[k]] = false;
      }
      v.used[Sum2-p[j]] = false;
    }
    v.used[Sum2-p[i]] = false;
  }
} // getBorderCombos
//--------------------------------------------------------------------------

void putBorderGGroup(int a, int b, int c, int d, int e, int f) {
//   ---------------
  int *p;

  if (a < b) {
    p = BorderGGroups[numBorderGGroups++];
    p[0] = a;  p[1] = c;  p[2] = d;  p[3] = e;  p[4] = f;  p[5] = b;
  }
} // putBorderGGroup
//---------------------------------------------------------------------------

void permute6_5(int a, int b, int c, int d, int e, int f) {
//   ----------
  putBorderGGroup(a, b, c, d, e, f);
  putBorderGGroup(a, c, b, d, e, f);
  putBorderGGroup(a, d, c, b, e, f);
  putBorderGGroup(a, e, c, d, b, f);
  putBorderGGroup(a, f, c, d, e, b);
} // permute6_5
//----------------------------------------------------------------------------

void getBorderGGroups() {
//   ----------------
  numBorderGGroups = 0;

  for (int i = 0; i < numBorderCombos; i++) {
    int *p = BorderCombos[i],
        p0 = p[0], p1 = p[1], p2 = p[2], 
        p3 = p[3], p4 = p[4], p5 = p[5];

    permute6_5(p0, p1, p2, p3, p4, p5);
    permute6_5(p1, p0, p2, p3, p4, p5);
    permute6_5(p2, p1, p0, p3, p4, p5);
    permute6_5(p3, p1, p2, p0, p4, p5);
    permute6_5(p4, p1, p2, p3, p0, p5);
    permute6_5(p5, p1, p2, p3, p4, p0);
  }
} // getBorderGGroups 
//--------------------------------------------------------------------------

void getBorderGGroups16() {
//   ------------------
  for (int i = 0; i < NN; i++)
    for (int j = 0; j < NN; j++)
      numBorderGGroups16[i][j] = 0;

  for (int i = 0; i < numBorderGGroups; i++) {
    int *p = BorderGGroups[i];

    BorderGGroups16[p[0]][p[5]][(numBorderGGroups16[p[0]][p[5]])++] = p;
  }
} // getBorderGGroups16 
//--------------------------------------------------------------------------

//bool isCorrect() {
////   ---------
//  const int chkSum = Sum6;
//  int sumX, sumY, sumXY, sumYX;
//
//  sumXY = 0;  sumYX = 0;
//  for (int i = 0; i < N; i++) {
//    sumX = 0; sumY = 0;
//    for (int j = 0; j < N; j++) {
//      sumX += xSquare[i][j];
//      sumY += xSquare[j][i];
//    }
//    if ((sumX != chkSum) || (sumY != chkSum)) return false;
//    sumXY += xSquare[i][N - 1 - i];
//    sumYX += xSquare[i][i];
//  }
//  return ((sumXY == chkSum) && (sumYX == chkSum));
//} // isCorrect
//---------------------------------------------------------------------------

bool outputSquare(FILE *wfp) {
//   ------------
  for (int row = 0; row < N; row++) {
    if (fprintf(wfp, "%2d", xSquare[row][0]) < 0) return false;
    for (int col = 1; col < N; col++)
      if (fprintf(wfp, " %2d", xSquare[row][col]) < 0) return false;
    if (fputc('\n', wfp) == EOF) return false; 
  }
  //return isCorrect();
  return fputc('\n', wfp) != EOF;
 } // outputSquare
//----------------------------------------------------------------------------

bool makeCenter4Square() {
//   -----------------
  struct bools v = allfree;

  for (int i0 = 0; i0 < numCenterGPermus; i0++) {
    int *yx = CenterGPermus[i0],                    // back diag
        yx0 = yx[0], yx1 = yx[1], yx2 = yx[2], yx3 = yx[3]; 
    v.used[yx0] = true; v.used[yx1] = true;
    v.used[yx2] = true; v.used[yx3] = true;

    for (int i1 = 0; i1 < numCenterGPermus; i1++) {
      int *xy = CenterGPermus[i1],                  // forward diag
          xy0 = xy[0], xy1 = xy[1], xy2 = xy[2], xy3 = xy[3];
      if (!(v.used[xy0] || v.used[xy1] || v.used[xy2] || v.used[xy3])) {
        v.used[xy0] = true; v.used[xy1] = true;
        v.used[xy2] = true; v.used[xy3] = true; 
	      int i2Max = numCenterGPermus23[yx1][xy1],
	          **p2 = CenterGPermus23[yx1][xy1];

	    for (int i2 = 0; i2 < i2Max; i2++) {
	      int *r2 = *(p2+i2);                 // second row
        if (!(v.used[r2[0]] || v.used[r2[3]])) {
            v.used[r2[0]] = true; v.used[r2[3]] = true;
	          int c13 = CenterPermus124[yx0][r2[0]][xy3];

            if ( (c13 >= 0) && !v.used[c13]) {
              v.used[c13] = true;
	            int c43 = CenterPermus124[xy0][r2[3]][yx3];

              if ( (c43 >= 0) && !v.used[c43] &&
		               (c13 + xy2 + yx2 + c43 == Sum4) ) {
                v.used[c43] = true;
		            int i3Max = numCenterGPermus23[yx1][xy2],
                    **p3 = CenterGPermus23[yx1][xy2];

                for (int i3 = 0; i3 < i3Max; i3++) {
	                int *c2 = *(p3+i3);         // second column

                  if (!(v.used[c2[0]] || v.used[c2[3]])) {
                    v.used[c2[0]] = true; v.used[c2[3]] = true;
		                int c31 = CenterPermus124[yx0][c2[0]][xy0];

		                if ( (c31 >= 0) && !v.used[c31]) {
		                  int c34 = CenterPermus124[xy3][c2[3]][yx3];

		                  if ( (c34 >= 0) && !v.used[c34] &&
                           (c31 + xy1 + yx2 + c34 == Sum4) ) {
//------------------------------------------------------
int *p = xSquare[1];

p[1] = yx0+1;    p[2] = c2[0]+1;  p[3] = c31+1;  p[4] = xy0+1;
p = xSquare[2];
p[1] = r2[0]+1;  p[2] = yx1+1;    p[3] = xy1+1;  p[4] = r2[3]+1;
p = xSquare[3];
p[1] = c13+1;    p[2] = xy2+1;    p[3] = yx2+1;  p[4] = c43+1;
p = xSquare[4];
p[1] = xy3+1;    p[2] = c2[3]+1;  p[3] = c34+1;  p[4] = yx3+1;
//------------------------------------------------------
                        return true;
					            }
                      v.used[c31] = false;
		                }
                    v.used[c2[0]] = false; v.used[c2[3]] = false;
		              }
		            }
                v.used[c43] = false;
              }
              v.used[c13] = false;
	          }
            v.used[r2[0]] = false; v.used[r2[3]] = false;
	        }
	      }
        v.used[xy0] = false; v.used[xy1] = false;
        v.used[xy2] = false; v.used[xy3] = false;
      }
    }
    v.used[yx0] = false; v.used[yx1] = false;
    v.used[yx2] = false; v.used[yx3] = false;
  }
  return false;
} // makeCenter4Square
//---------------------------------------------------------------------------

bool makeOrder6Squares(FILE *wfp) {
//   -----------------
  struct bools v = allfree;

  for (int i0 = 0; i0 < numBorderGGroups; i0++) {
    int *r1 = BorderGGroups[i0];                        // first row

    if ( (r1[0] < Half) && (r1[5] < Half) ) {
      v.used[r1[1]] = true; v.used[r1[2]] = true;
      v.used[r1[3]] = true; v.used[r1[4]] = true;
      v.used[Sum2-r1[1]] = true; v.used[Sum2-r1[2]] = true;
      v.used[Sum2-r1[3]] = true; v.used[Sum2-r1[4]] = true;

      int c16 = Sum2 - r1[5];
      int i1Max = numBorderGGroups16[r1[0]][c16];
      int **p1  = BorderGGroups16[r1[0]][c16];

      for (int i1 = 0; i1 < i1Max; i1++) {
        int *c1 = *(p1+i1);                          // first column
        if (!(v.used[c1[1]] || v.used[c1[2]] || v.used[c1[3]] || v.used[c1[4]])) {
 //-------------------------------------------------------
int *p = xSquare[0];

p[0] = r1[0]+1;       p[1] = r1[1]+1;       p[2] = r1[2]+1;
p[3] = r1[3]+1;       p[4] = r1[4]+1;       p[5] = r1[5]+1;
p = xSquare[1];       p[0] = c1[1]+1;       p[5] = Sum2-c1[1]+1;
p = xSquare[2];       p[0] = c1[2]+1;       p[5] = Sum2-c1[2]+1;
p = xSquare[3];       p[0] = c1[3]+1;       p[5] = Sum2-c1[3]+1;
p = xSquare[4];       p[0] = c1[4]+1;       p[5] = Sum2-c1[4]+1;
p = xSquare[5];
p[0] = c16+1;         p[1] = Sum2-r1[1]+1;  p[2] = Sum2-r1[2]+1;
p[3] = Sum2-r1[3]+1;  p[4] = Sum2-r1[4]+1;  p[5] = Sum2-r1[0]+1;
//---------------------------------------------------------		  
          if ( outputSquare(wfp) ) {
		       	return true;
		      } else {
	          perror("\a\nError writing file"); return false;
		      }
        }
      }
      v.used[r1[1]] = false; v.used[r1[2]] = false;
      v.used[r1[3]] = false; v.used[r1[4]] = false;
      v.used[Sum2-r1[1]] = false; v.used[Sum2-r1[2]] = false;
      v.used[Sum2-r1[3]] = false; v.used[Sum2-r1[4]] = false;
    }
  }
  printf("\a\nNot working. Please report by email.\n");
  return false;
} // makeOrder6Squares 
//-----------------------------  end border code -----------------------------

void makeSquares(FILE *wfp) {
//   -----------
  time_t startTime = time(NULL);
  getPartitions();

  for (int i = 0; i < numPartitions; i++) {
    getCenterCombos(i);    getCenterPermus();    getCenterGPermus();
    getCenterGPermus23();  getCenterPermus124();
    if ( makeCenter4Square() ) {
      getBorderCombos(i);  getBorderGGroups();  getBorderGGroups16();
	  if ( !makeOrder6Squares(wfp) ) return;
    }
  }
  int et = (int)difftime(time(NULL), startTime);
  printf("\nsample number of squares: %d, "
    "elapsed time: %d:%02d:%02d\n", numSquares, et/3600, et%3600/60, et%60);
} // makeSquares
//--------------------------------------------------------------------------

FILE *openFile() {
//    --------
  const int bufSize = 128; char buf[bufSize];
  const char *defaultName = "Order6ConcentricMagicSquares";
  sprintf_s(buf, bufSize, "%s.txt", defaultName);
  int sub = 0;

  do {
    if (_access_s(buf, 00) == ENOENT)
      break;
    else
      sprintf_s(buf, bufSize, "%s_%d.txt", defaultName, ++sub);
  } while (true);

  FILE *wfp = NULL;
  if (fopen_s(&wfp, buf, "w") == 0)
    printf("Output file is %s\n", buf);
  else {
    char msg[bufSize];
    sprintf_s(msg, bufSize, "\a\nCan't open for write %s", buf);
    perror(msg);
  }
  
  return wfp;
} // openFile
//--------------------------------------------------------------------------

void outputLocalTime() {
//   ---------------
  time_t startTime = time(NULL); struct tm local;
  if (localtime_s(&local, &startTime) == 0) {
    char dateTime[100];
    size_t slen = strftime(dateTime, 100, "%A %Y-%m-%d %X %Z\n\n\0", &local);
    printf("\n%s", dateTime);
  }
} // outputLocalTime
//--------------------------------------------------------------------------

int _tmain() {
//  ------

  outputLocalTime();
  FILE *wfp = openFile();

  if (wfp != NULL) {
    makeSquares(wfp); fclose(wfp);
  }
  printf("\nPress a key to close the console");
  while (!_kbhit()) Sleep(250);
  return 0;
} // _tmain