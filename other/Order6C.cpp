/*
 *  File:    Order6C.cpp
 *  Author:  S Harry White
 *  Created: 2009-12-02
 *  Updated: 2012-01-29
 */

// #include "stdafx.h"
#include <conio.h>
#include <direct.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

const int M  = 4,
          MM = M * M,                // 16

          N     = 6,
          NN    = N * N,             // 36
          Half  = NN / 2,            // 18
          Sum2  = NN + 1,            // 37
          Sum4  = Sum2 + Sum2,       // 74
          MSum = N * (NN + 1) / 2,   // 111

//Partitions of 1 .. NN as MM/2 pairs and other (NN-MM)/2 pairs.
          numPartitions = 43758;     // 18! / (8!10!)
//---------------------------- start center data ----------------------------

int Centers[numPartitions][MM], numCenters;

const int maxCenterCombos = 86, maxCenterPermus = 2064;    // 86 x 4!
int CenterCombos[maxCenterCombos][M], numCenterCombos,
    CenterPermus[maxCenterPermus][M], numCenterPermus;

// Only CenterPermus in which third > second.
const int maxCenterGPermus = 1032;
int *CenterGPermus[maxCenterGPermus], numCenterGPermus;

// CenterGPermus indexed by the second and third numbers.
const int maxCenterGPermus23 = 14;
int *CenterGPermus23[NN+1][NN+1][maxCenterGPermus23],
    numCenterGPermus23[NN+1][NN+1];

// Third number of CenterPermus indexed by the first, second, and fourth.
int CenterPermus124[NN+1][NN+1][NN+1];

int xSquare[N][N];

//const int numSquares = 22145;
// Output is 1 square for each of the partitions that make a center square.
int numSquares = 0;
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
int *BorderGGroups16[NN+1][NN+1][maxBorderGGroups16],
    numBorderGGroups16[NN+1][NN+1];

const int maxNumCenterSquares = 1000;  // 880 ?
int centerSquares[maxNumCenterSquares][MM];
//int numCenterSquares;

const int maxNumBorderGroups = 1000;  // 453 ?
int borderGroups[maxNumBorderGroups][NN-MM];
//int numBorderGroups;
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

  for (int i1 = 1; i1 <= (Half-7); i1++)
    for (int i2 = i1+1; i2 <= (Half-6); i2++)
      for (int i3 = i2+1; i3 <= (Half-5); i3++)
	    for (int i4 = i3+1; i4 <= (Half-4); i4++)
	      for (int i5 = i4+1; i5 <= (Half-3); i5++)
	        for (int i6 = i5+1; i6 <= (Half-2); i6++)
	          for (int i7 = i6+1; i7 <= (Half-1); i7++)
		        for (int i8 = i7+1; i8 <= Half; i8++)
		          putCenter(i1, i2, i3, i4, i5, i6, i7, i8);
 // assert(numCenters == numPartitions);
} // getCenters
//---------------------------------------------------------------------------

struct bools { bool used[NN+1]; }; struct bools allfree;

void getBorders() {
//   ----------
  for (int i = 0; i < numPartitions; i++) {
    struct bools v = allfree; int *p = Centers[i];

    for (int j = 0; j < MM; j++) v.used[p[j]] = true;

    p = Borders[i];  int k = 0;
    for (int j = 1; j <= NN; j++) if (!v.used[j]) p[k++] = j;
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
  numCenterCombos = 0;

  int *p = Centers[i];

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
  for (int i = 0; i <= NN; i++)
    for (int j = 0; j <= NN; j++)
      numCenterGPermus23[i][j] = 0;

  for (int i = 0; i < numCenterGPermus; i++) {
    int *p = CenterGPermus[i];

    CenterGPermus23[p[1]][p[2]][(numCenterGPermus23[p[1]][p[2]])++] = p;
  }
} // getCenterGPermus23 
//----------------------------------------------------------------------------

void getCenterPermus124() {
//   ------------------
  for (int i = 0; i <= NN; i++)
    for (int j = 0; j <= NN; j++)
      for (int k = 0; k <= NN; k++)
        CenterPermus124[i][j][k] = 0;

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
  int *p = Borders[i]; struct bools v = allfree;

  numBorderCombos = 0;
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
              if ( ((p[i] + p[j] + p[k] + p[l] + p[m]+ p[n]) == MSum) )
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
  for (int i = 0; i <= NN; i++)
    for (int j = 0; j <= NN; j++)
      numBorderGGroups16[i][j] = 0;

  for (int i = 0; i < numBorderGGroups; i++) {
    int *p = BorderGGroups[i];

    BorderGGroups16[p[0]][p[5]][(numBorderGGroups16[p[0]][p[5]])++] = p;
  }
} // getBorderGGroups16 
//--------------------------------------------------------------------------

int makeCenterSquares() {
//   -----------------
  int upb = -1; struct bools v = allfree;

  for (int i0 = 0; i0 < numCenterGPermus; i0++) {
    int *yx = CenterGPermus[i0];                    // back diag
    int yx0 = yx[0];  int yx1 = yx[1];  int yx2 = yx[2];  int yx3 = yx[3]; 
    v.used[yx0] = true; v.used[yx1] = true;
    v.used[yx2] = true; v.used[yx3] = true;

    for (int i1 = 0; i1 < numCenterGPermus; i1++) {
      int *xy = CenterGPermus[i1];                  // forward diag
      int xy0 = xy[0];  int xy1 = xy[1];  int xy2 = xy[2];  int xy3 = xy[3];  

      if (!(v.used[xy0] || v.used[xy1] || v.used[xy2] || v.used[xy3])) {
        v.used[xy0] = true; v.used[xy1] = true;
        v.used[xy2] = true; v.used[xy3] = true; 
	    int i2Max = numCenterGPermus23[yx1][xy1];
	    int **p2 = CenterGPermus23[yx1][xy1];

	    for (int i2 = 0; i2 < i2Max; i2++) {
	      int *r2 = *(p2+i2);                 // second row

          if (!(v.used[r2[0]] || v.used[r2[3]])) {
            v.used[r2[0]] = true; v.used[r2[3]] = true;
            int c13 = CenterPermus124[yx0][r2[0]][xy3];

	        if ( (c13 != 0) && !v.used[c13]) {
              v.used[c13] = true;
	          int c43 = CenterPermus124[xy0][r2[3]][yx3];

              if ( (c43 != 0) && !v.used[c43] &&
		           (c13 + xy2 + yx2 + c43 == Sum4) ) {
                v.used[c43] = true;
		            int i3Max = numCenterGPermus23[yx1][xy2];
                int **p3 = CenterGPermus23[yx1][xy2];

                for (int i3 = 0; i3 < i3Max; i3++) {
	              int *c2 = *(p3+i3);         // second column

                  if (!(v.used[c2[0]] || v.used[c2[3]])) {
                    v.used[c2[0]] = true; v.used[c2[3]] = true;
		                int c31 = CenterPermus124[yx0][c2[0]][xy0];

		                 if ( (c31 != 0) && !v.used[c31]) {
		                   int c34 = CenterPermus124[xy3][c2[3]][yx3];

		                   if ( (c34 != 0) && !v.used[c34] &&
						             (c31 + xy1 + yx2 + c34 == Sum4) ) {
//------------------------------------------------------
int *p = &centerSquares[++upb][0];

*p++ = yx0;   *p++ = c2[0]; *p++ = c31; *p++ = xy0;

*p++ = r2[0]; *p++ = yx1;   *p++ = xy1; *p++ = r2[3];

*p++ = c13;   *p++ = xy2;   *p++ = yx2; *p++ = c43;

*p++ = xy3;   *p++ = c2[3]; *p++ = c34; *p++ = yx3;
//------------------------------------------------------
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
  return upb+1;
} // makeCenterSquares
//---------------------------------------------------------------------------

int makeBorderGroups() {
//   ----------------
  int upb = -1; struct bools v = allfree;

  for (int i0 = 0; i0 < numBorderGGroups; i0++) {
    int *r1 = BorderGGroups[i0];                        // first row
	int r11 = r1[0];

    if ( (r11 <= Half) && (r1[5] <= Half) ) {
      v.used[r1[1]] = true; v.used[r1[2]] = true;
      v.used[r1[3]] = true; v.used[r1[4]] = true;
      v.used[Sum2-r1[1]] = true; v.used[Sum2-r1[2]] = true;
      v.used[Sum2-r1[3]] = true; v.used[Sum2-r1[4]] = true;

      int c16 = Sum2 - r1[5];
      int i1Max = numBorderGGroups16[r11][c16];
      int **p1  = BorderGGroups16[r11][c16];

      for (int i1 = 0; i1 < i1Max; i1++) {
        int *c1 = *(p1+i1);                          // first column
        if (!(v.used[c1[1]] || v.used[c1[2]] || v.used[c1[3]] || v.used[c1[4]])) {
 //-------------------------------------------------------
int *p = &borderGroups[++upb][0];

*p++ = r11;        *p++ = r1[1];      *p++ = r1[2];
*p++ = r1[3];      *p++ = r1[4];      *p++ = r1[5];
*p++ = c1[1];      *p++ = Sum2-c1[1];
*p++ = c1[2];      *p++ = Sum2-c1[2];
*p++ = c1[3];      *p++ = Sum2-c1[3];
*p++ = c1[4];      *p++ = Sum2-c1[4];
*p++ = c16;        *p++ = Sum2-r1[1]; *p++ = Sum2-r1[2];
*p++ = Sum2-r1[3]; *p++ = Sum2-r1[4]; *p++ = Sum2-r11;
//---------------------------------------------------------		  
		}
      }
      v.used[r1[1]] = false; v.used[r1[2]] = false;
      v.used[r1[3]] = false; v.used[r1[4]] = false;
      v.used[Sum2-r1[1]] = false; v.used[Sum2-r1[2]] = false;
      v.used[Sum2-r1[3]] = false; v.used[Sum2-r1[4]] = false;
    }
  }
  return upb+1;
} // makeBorderGroups 
//-----------------------------  end border code -----------------------------

const int bufSize = 128;
bool openDir(bool *first) {
//   -------
  char buf[bufSize], msg[bufSize];
  if (*first)
    *first = false;
  else
    if (_chdir("..") == -1) {
      strcpy_s(msg, bufSize, "\aCan't open folder ..");
      perror(msg); return false;
    }
  const char *baseName = "Order6C"; strcpy_s(buf, bufSize, baseName);
  int sub = 0;

  do {
    if (_mkdir(buf) != -1) break;
    if (errno != EEXIST) {
      sprintf_s(msg, bufSize, "\aCan't make folder %s", buf);
      perror(msg); return false;
    }
    sprintf_s(buf, bufSize, "%s_%d", baseName, ++sub);
  } while (true);

  if (_chdir(buf) == -1) {
    sprintf_s(msg, bufSize, "\aCan't open folder %s", buf);
    perror(msg); return false;
  }
  printf("\nOutput files are in folder %s\n", buf);
  return true;
} // openDir
//--------------------------------------------------------------------------

FILE *openFile(int partition, int which) {
//    --------
  char buf[bufSize];

  if (which == 0) {
    strcpy_s(buf, bufSize, "Order6C_Summary.txt");
  } else {
    sprintf_s(buf, bufSize, "Partition%d%s.txt", partition+1,
                     (which == 4) ? "_Center4x4s" : "_BorderGroups");
  }
  FILE *wfp = NULL;
  if (fopen_s(&wfp, buf, "w") != 0) {
    char msg[bufSize];
    sprintf_s(msg, bufSize, "\a\nCan't open for write %s", buf);
    perror(msg);
  }
  return wfp;
} // openFile
//--------------------------------------------------------------------------

const int oneThousand = 1000;
const int oneMillion  = 1000000;
const int oneBillion  = 1000000000;
int summaryGroupLines = 0;
int totalSquares = 0, totalBillion = 0;

bool makeSquares(int partition, FILE *summary, bool *writeError) {
//   -----------
  const int rotPerm = 8 * 24 * 24; bool ok = true; *writeError = true;
  int numCenterSquares = 0, numBorderGroups = 0;

  getPartitions();
  getCenterCombos(partition);  getCenterPermus();  getCenterGPermus();
  getCenterGPermus23();  getCenterPermus124();
  numCenterSquares = makeCenterSquares();
  getBorderCombos(partition);  getBorderGGroups();  getBorderGGroups16();
  numBorderGroups = makeBorderGroups();

  numSquares = numCenterSquares * numBorderGroups * rotPerm;
  if ((totalSquares += numSquares) > oneBillion) {
	++totalBillion; totalSquares  -= oneBillion;
  }

  if (summaryGroupLines == 10) {
	if (fputc('\n', summary) == EOF) return false; summaryGroupLines = 1;
  } else
	++summaryGroupLines;

  if (fprintf(summary, "  %5d            %3d              %3d          ",
	    partition+1, numCenterSquares, numBorderGroups) < 0) return false;
  if ( numSquares == 0) {
    if (fputs("            0\n", summary) == EOF) return false;
  } else {
    if (numSquares > oneBillion) {
      if (fprintf(summary, "%d,", numSquares / oneBillion)
        < 0) return false;
      numSquares %= oneBillion;
      if (fprintf(summary, "%03d,", numSquares / oneMillion)
        < 0) return false;
    } else
      if (fprintf(summary, "  %3d,", numSquares / oneMillion)
        < 0) return false;
    numSquares %= oneMillion;
    if (fprintf(summary, "%03d,%03d\n", numSquares / oneThousand,
                                    numSquares % oneThousand)
     < 0) return false;
  }

  if (numCenterSquares != 0) {
    FILE *wfp4 = NULL, *wfp6 = NULL;
    if ( ((wfp4 = openFile(partition, 4)) != NULL) &&
	     ((wfp6 = openFile(partition, 6)) != NULL) ) {
	  for (int i = 0; i < numCenterSquares; i++) {
	    int *p = centerSquares[i];
	    int row = 0;

	    while (row++ < M) {
		  int a = *p++, b = *p++, c = *p++, d = *p++;
          if (fprintf(wfp4, "%2d %2d %2d %2d\n", a, b, c, d) < 0) {
            ok = false; break;
          };
	    }
        if (ok) ok = fputc('\n', wfp4) != EOF; if (!ok) break;
	  }

	  if (ok) for (int i = 0; i < numBorderGroups; i++) {
	    int *p = borderGroups[i];
	    int a = *p++, b = *p++, c = *p++, d = *p++, e = *p++, f = *p++;
        if (ok = fprintf(wfp6, "%2d %2d %2d %2d %2d %2d\n",
                                a, b, c, d, e, f) > 0) {
	      int row = 1;
	      while (row++ < (N-1)) {
		    a = *p++; b = *p++;
	  	    if (!(ok = (fprintf(wfp6, "%2d             %2d\n",
                                        a, b) > 0))) break;
	      }
          if (ok) {
            a = *p++; b = *p++; c = *p++; d = *p++; e = *p++; f = *p++;
	        ok = fprintf(wfp6, "%2d %2d %2d %2d %2d %2d\n\n",
                                a, b, c, d, e, f) > 0;
          }
	    }
      }
    } else {
      ok = false; *writeError = false;
    } // ((wfp4 = openFile
    if (wfp4 != NULL) fclose(wfp4); if (wfp6 != NULL) fclose(wfp6);
  } // (numCenterSquares != 0)
  if (ok) *writeError = false;
  return ok;
} // makeSquares
//--------------------------------------------------------------------------

void get_rest_of_line() {
//   ----------------
  int c;  do { c = getchar(); } while (c != '\n');
}

bool getY() {
//   ----
  int c;  do { c = getchar(); } while ((c == ' ') || (c == '\t') || (c == '\n'));
  get_rest_of_line();  return (c == 'Y') | (c == 'y');
}

bool getPartitionRange(int *startPartition, int *endPartition) {
//   -----------------
  printf("\nInput the start and end partitions: ");
  int n = scanf_s("%d %d", startPartition, endPartition);  get_rest_of_line();
  if (n != 2) {
	printf("Incorrect partitions input. Should be two numbers, for example: 1 99\n");
	return false;
  }

  bool badStart = (*startPartition < 1) || (*startPartition > numPartitions);
  bool badEnd   = (*endPartition < 1)   || (*endPartition > numPartitions);
  if ( badStart || badEnd) {
	printf("\n%d is not a valid partition. Range is %d to %d\n",
	badStart ? *startPartition : *endPartition, 1, numPartitions);
	return false;
  }
  if (*startPartition > *endPartition) {
	int tmp = *startPartition; *startPartition = *endPartition; *endPartition = tmp;
  }
  return true;
} // getPartitionRange
//--------------------------------------------------------------------------

bool makeSample(
//   ----------
                int startPartition, int endPartition,
                FILE *summary, bool *writeError)
{
  *writeError = true;
  if (fputs(
    "\nPartition     Center 4x4s     Border Groups         Squares\n",
    summary) == EOF) return false;
  if (fputs(
    "---------     -----------     -------------     ---------------\n\n",
    summary) == EOF) return false;

  summaryGroupLines = 0;
  totalSquares = 0; totalBillion = 0;
  for (int partition = startPartition; partition <= endPartition; partition++)
    if (!makeSquares(partition-1, summary, writeError)) return false;     
  if (fputs(
    "                                              -----------------\n",
    summary) == EOF) return false;
  if (fputs("                                               ",
    summary) == EOF) return false;
  if ( (totalBillion == 0) && (totalSquares == 0) ) {
    if (fputs("              0\n", summary) == EOF) return false;
  } else {
    if (totalBillion != 0) {
      if (fprintf(summary, "%3d,", totalBillion) < 0) return false;
      if (fprintf(summary, "%03d,", totalSquares / oneMillion)
        < 0) return false;
    } else
      if (fprintf(summary, "    %3d,", totalSquares / oneMillion)
        < 0) return false;
    totalSquares %= oneMillion;
    if (fprintf(summary, "%03d,%03d\n", totalSquares / oneThousand,
                                       totalSquares % oneThousand)
      < 0) return false;
  }
  *writeError = false; return true;
} // makeSample
//--------------------------------------------------------------------------

void outputLocalTime() {
//   --------------
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
  bool another = true, first = true;
  int startPartition = 0, endPartition = 0;

  do {
    if (getPartitionRange(&startPartition, &endPartition)) {
      bool noOutputError = true, writeError = false;
	    time_t startTime = time(NULL); 
      if (openDir(&first)) {
        FILE *summary = NULL;
        if ( (summary = openFile(0, 0)) != NULL ) {
          noOutputError = makeSample(startPartition, endPartition,
                                     summary, &writeError);
          fclose(summary);

          int et = (int)difftime(time(NULL), startTime);
          if (et != 0)
            printf("elapsed time %d:%02d:%02d\n", et/3600, (et%3600)/60, et%60);
        }
      }
      if (noOutputError) {
	    printf("\nContinue? input y (yes) or n (no) ");
        if (!getY()) another = false;
      } else {
        if (writeError) perror("\a\nError writing file");
        another = false;
      }
    }
  } while (another);

  printf("\nPress a key to close the console");
  while (!_kbhit()) Sleep(250);
  return 0;
} // _tmain