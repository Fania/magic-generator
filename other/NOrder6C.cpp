/*
 *  File:    NOrder6C.cpp
 *  Author:  S Harry White
 *  Created: 2009-05-30
 *  Updated: 2012-01-29
 */

#include "stdafx.h"
#include <conio.h>    // not CLR
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>  // not CLR

const int M  = 4,
          MM = M * M,               //  16

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
int CenterCombos[maxCenterCombos][M], numCenterCombos,
    CenterPermus[maxCenterPermus][M], numCenterPermus;

// Only CenterPermus in which third > second.
const int maxCenterGPermus = 1032;
int *CenterGPermus[maxCenterGPermus], numCenterGPermus;

//CenterGPermus indexed by the second and third numbers.
const int maxCenterGPermus23 = 14;
int *CenterGPermus23[NN][NN][maxCenterGPermus23],
    numCenterGPermus23[NN][NN],

// Third number of CenterPermus indexed by the first, second, and fourth.
    CenterPermus124[NN][NN][NN];
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
} // getCenters
//---------------------------------------------------------------------------

struct bools { bool used[NN]; }; struct bools allfree;

void getBorders() {
//   ----------
  for (int i = 0; i < numPartitions; i++) {
    struct bools v = allfree;  int *p = Centers[i];

    for (int j = 0; j < MM; j++) v.used[p[j]] = true;

    p = Borders[i]; int k = 0;
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

void getCenterCombos(int partition) {
//   ---------------
  numCenterCombos = 0;

  int *p = Centers[partition-1];

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

void getBorderCombos(int partition) {
//   ---------------
  numBorderCombos = 0; struct bools v = allfree;

  int *p = Borders[partition-1];
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
            for (int n = m+1; n < (NN-MM); n++) if (!v.used[p[n]]) {
	          if ( ((p[i] + p[j] + p[k] + p[l] + p[m]+ p[n]) == Sum6) )
		        putBorderCombo(p[i], p[j], p[k], p[l], p[m], p[n]);
            }
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
    int *p = BorderCombos[i];
    int p0 = p[0];  int p1 = p[1];  int p2 = p[2]; 
    int p3 = p[3];  int p4 = p[4];  int p5 = p[5];

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
//-----------------------------  end border code -----------------------------

int countCenters() {
//   -----------
  int count = 0; struct bools v = allfree;

  for (int i0 = 0; i0 < numCenterGPermus; i0++) {
    int *yx = CenterGPermus[i0];                    // back diag   
    int yx0 = yx[0], yx1 = yx[1], yx2 = yx[2], yx3 = yx[3];
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
		            int i3Max = numCenterGPermus23[yx1][xy2];
                int **p3 = CenterGPermus23[yx1][xy2];

                for (int i3 = 0; i3 < i3Max; i3++) {
	                int *c2 = *(p3+i3);         // second column

                  if (!(v.used[c2[0]] || v.used[c2[3]])) {
                    v.used[c2[0]] = true; v.used[c2[3]] = true;
		                int c31 = CenterPermus124[yx0][c2[0]][xy0];

                    if ( (c31 >= 0) && !v.used[c31]) {
                      v.used[c31] = true;
		                  int c34 = CenterPermus124[xy3][c2[3]][yx3];                     

                      if ( (c34 >= 0) && !v.used[c34] &&
                           (c31 + xy1 + yx2 + c34 == Sum4) ) {
                        ++count;
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
  return count;
} // countCenters
//--------------------------------------------------------------------------

int countBorders() {
//  ------------
  int count = 0; struct bools v = allfree;

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

        if (!(v.used[c1[1]] || v.used[c1[2]] ||
              v.used[c1[3]] || v.used[c1[4]]))
          ++count;
      }
      v.used[r1[1]] = false; v.used[r1[2]] = false;
      v.used[r1[3]] = false; v.used[r1[4]] = false;
      v.used[Sum2-r1[1]] = false; v.used[Sum2-r1[2]] = false;
      v.used[Sum2-r1[3]] = false; v.used[Sum2-r1[4]] = false;
    }
  }
  return count;
} // countBorders 
//----------------------------------------------------------------------------

/*
int MaxCenterCombos, MaxCenterPermus, MaxCenterGPermus, MaxCenterGPermus23;
int MaxBorderCombos, MaxBorderGGroups, MaxBorderGGroups16;
int printCount = 0;
int zeroCenterCombo = 0, zeroCenterCount = 0,
    zeroBorderCombo = 0, zeroBorderCount = 0;

void getCenterMaxs() {
//   -------------
  if (numCenterCombos > MaxCenterCombos) MaxCenterCombos = numCenterCombos;
  if (numCenterPermus > MaxCenterPermus) MaxCenterPermus = numCenterPermus;
  if (numCenterGPermus > MaxCenterGPermus) MaxCenterGPermus = numCenterGPermus;
  for (int j = 0; j <= NN; j++)
    for (int k = 0; k <= NN; k++) {
      if (numCenterGPermus23[j][k] > MaxCenterGPermus23)
        MaxCenterGPermus23 = numCenterGPermus23[j][k];
    }
} // getCenterMaxs

void getBorderMaxs() {
//   -------------
  if (numBorderCombos > MaxBorderCombos) MaxBorderCombos = numBorderCombos;
  if (numBorderGGroups > MaxBorderGGroups) MaxBorderGGroups = numBorderGGroups;
  for (int j = 1; j <= NN; j++)
    for (int k = 1; k <= NN; k++)
      if (numBorderGGroups16[j][k] > MaxBorderGGroups16)
        MaxBorderGGroups16 = numBorderGGroups16[j][k];
} // getBorderMaxs

void printMaxs(FILE *wfp) {
//   ---------
  printf(" CenterCombos %d CenterPermus %d CenterGPermus %d CenterGPermus23 %d\n",
           MaxCenterCombos, MaxCenterPermus, MaxCenterGPermus, MaxCenterGPermus23);
  printf(" BorderCombos %d BorderGGroups %d BorderGGroups16 %d\n",
           MaxBorderCombos, MaxBorderGGroups, MaxBorderGGroups16);
} // printMaxs

void printPartition(int partition, FILE *wfp) {
//   --------------
  int *p = Centers[partition-1];
  fprintf(wfp, "Center:");
  for (int a = 0; a < MM; ++a) fprintf(wfp, " %2d", p[a]);
  fprintf(wfp, "\n");

  p = Borders[partition-1];
  fprintf(wfp, "Border:");
  for (int a = 0; a < (NN-MM); ++a) fprintf(wfp, " %2d", p[a]);
  fprintf(wfp, "\n\n");
} // printPartition

void printCenterCombos(FILE *wfp) {
//   -----------------
  for (int a = 0; a < numCenterCombos; ++a) {
    int *p = CenterCombos[a];
    fprintf(wfp, "%2d %2d %2d %d\n", p[0], p[1], p[2], p[3]);
  }
  fprintf(wfp, "\n");
} // printCenterCombos

void printBorderCombos(FILE *wfp) {
//   -----------------
  for (int a = 0; a < numBorderCombos; ++a) {
    int *p = BorderCombos[a];
    fprintf(wfp, "%2d %2d %2d %2d %2d %d\n", p[0], p[1], p[2], p[3], p[4], p[5]);
  }
  fprintf(wfp, "\n");
} // printBorderCombos
*/

//void countSquares(bool consecutive, FILE *wfp) {
void countSquares(bool consecutive) {
//   ------------
  const unsigned int oneBillion = 1000000000,
          multiplier = 8 * 24 * 24; // Center rotations * border permutations.
  unsigned int Total = 0, TotalB = 0; //, total = 0;

  getPartitions();
  int start = consecutive ? numPartitions : 1;
  for (int partition = start; partition <= numPartitions; ++partition) {
    //printPartition(partition, wfp);
    getCenterCombos(partition);
      //if (numCenterCombos == 0) continue;
      //{ ++zeroCenterCombo; continue; } //printCenterCombos(wfp);
    getCenterPermus(); getCenterGPermus();
    getCenterGPermus23(); getCenterPermus124(); //getCenterMaxs();
    int centerCount = countCenters();
    if (centerCount == 0) continue; //{ ++zeroCenterCount; continue; }
    getBorderCombos(partition);
      //if (numBorderCombos == 0) continue;
      //{ ++zeroBorderCombo; continue; } //printBorderCombos(wfp);
    getBorderGGroups(); getBorderGGroups16(); //getBorderMaxs();
    int borderCount = countBorders();
      //if (borderCount == 0) continue;
      //{ ++zeroBorderCount; continue; };
    unsigned int count = centerCount * borderCount;
    //total += count;
    unsigned int Count = count * multiplier;
    Total += Count;
    while (Total >= oneBillion) { ++TotalB; Total -= oneBillion; }
    // printf(wfp, "P %5d Count %10u Total %3u%09u\n", partition, Count, TotalB, Total);
  }
  /*
  printMaxs(wfp);
  printf("zeroCenterCombo %d zeroCenterCount %d zeroBorderCombo %d zeroBorderCount %d\n",
    zeroCenterCombo, zeroCenterCount, zeroBorderCombo, zeroBorderCount);
  fprintf(wfp, "zeroCenterCombo %d zeroCenterCount %d zeroBorderCombo %d zeroBorderCount %d\n",
    zeroCenterCombo, zeroCenterCount, zeroBorderCombo, zeroBorderCount);
  */
  const int oneMillion = 1000000, oneThousand = 1000;
  int TotalM = Total / oneMillion, TotalT = Total % oneMillion / oneThousand;

  //printf("\ntotal: %u\n", total);
  printf("\nNumber of squares: "); if (TotalB != 0) printf("%d,", TotalB);
  printf("%03d,%03d,%03d\n", TotalM, TotalT, Total % oneThousand);
} // countSquares
//--------------------------------------------------------------------------

void get_rest_of_line() {
//   ----------------
  int c;  do { c = getchar(); } while (c != '\n');
}

bool getY() {
//   ----
  int c;  do { c = getchar(); } while ((c == ' ') | (c == '\t') || (c == '\n'));
  get_rest_of_line();  return (c == 'Y') | (c == 'y');
}
//---------------------------------------------------------------------------

void outputLocalTime(time_t *startTime) {
//   --------------
  struct tm local;
  if (localtime_s(&local, startTime) == 0) {
    char dateTime[100];
    size_t slen = strftime(dateTime, 100, "%A %Y-%m-%d %X %Z\n\n\0", &local);
    printf("\n%s", dateTime);
  }
} // outputLocalTime
//--------------------------------------------------------------------------

//using namespace System;
//using namespace System::Threading;
//int main() {
// CLR

int _tmain() {
//  ------
  printf("Count only consecutively concentric squares? y (yes) or n (no) ");
  bool consecutive = getY();

  time_t startTime = time(NULL);
  outputLocalTime(&startTime);

  countSquares(consecutive);

  int et = (int)difftime(time(NULL), startTime);
  printf("\nelapsed time: %d:%02d:%02d\n", et/3600, et%3600/60, et%60);
 
  // while (!Console::KeyAvailable) Thread::Sleep(250); // CLR
  printf("\nPress a key to close the console");
  while (!_kbhit()) Sleep(250);
  return 0;
} // _tmain