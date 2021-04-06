/*
 *  File:    Order5Special.cpp
 *  Author:  S Harry White
 *  Created: 2009-12-28
 *  Updated: 2012-02-23
 */

// #include "stdafx.h"
#include <conio.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

const int M  = 3,
          MM   = M * M,           // 9
          MMd2 = MM /2,           // 4

          N     = 5,
          NN    = N * N,          // 25
          Mid   = NN / 2,         // 12
          NN_MM = NN - MM,        // 16

// zero base
          Sum2  = NN - 1,         // 24
          Sum3  = M * Mid,        // 36
          MSum  = N * Mid,        // 60

// one base
          Sum2_1  = NN + 1,       // 26
          Mid_1   = Mid + 1,      // 13
          MSum_1  = N * Mid_1,    // 65

// Partitions of 0 .. NN-1 as MM/2 pairs + Mid and other (NN-MM)/2 pairs.
// 12! / (4!8!) = 495
          numPartitions = 495;

int numCenters,
    Centers[numPartitions][MM-1],
    Borders[numPartitions][NN-MM];
//---------------------------------------------------------------------------

// Combinations that do not include complementary pairs.
const int max5Combos = 52;
int num5Combos,
    fiveCombos[max5Combos][N];

// Permutations of fiveCombos in which the fifth is greater than the first.
const int max5UPermus = 520;
int num5UPermus,
    fiveUPermus[max5UPermus][N];

struct ptrUsed { int *p; int u; };

// The fiveUPermus in which the fifth is less than Mid.
const int max5UPermusLH = 106;
int num5UPermusLH;
ptrUsed fiveUPermusLH[max5UPermusLH];


// The fiveUPermus indexed by the first and third.
const int max5UPermus15 = 10;
int num5UPermus15[NN][NN];
ptrUsed fiveUPermus15[NN][NN][max5UPermus15];
//---------------------------------------------------------------------------

enum t_kind { kAssoc, kBord, kConc, kCor, kSide, kPan, kUltra };
const t_kind firstKind = kAssoc, lastKind = kUltra;
const int squareBytes = 76, noFD  = -1, numKinds = lastKind-firstKind+1,
  nAssoc = 48544,  assocSize = nAssoc * squareBytes,
  nBord  = 2880,   bordSize  = nBord  * squareBytes,
  nConc  = 174240, concSize  = nConc  * squareBytes,
  nCor   = 12416,  corSize   = nCor   * squareBytes,
  nSide  = 444,    sideSize  = nSide  * squareBytes,
  nPan   = 3600,   panSize   = nPan   * squareBytes,
  nUltra = 16,     ultraSize = nUltra * squareBytes;
   
char associative[assocSize], bordered[bordSize], concentric[concSize],
     inlaidCorner[corSize],  inlaidSide[sideSize],
     pandiagonal[panSize],   ultramagic[ultraSize];
struct specialData { char *name, *buf, *s; int bufSize, number, count, wfd; };
specialData which[numKinds] =
{
  { "associative",  associative,  associative,  assocSize, nAssoc, 0, noFD},
  { "bordered",     bordered,     bordered,     bordSize,  nBord,  0, noFD},
  { "concentric",   concentric,   concentric,   concSize,  nConc,  0, noFD},
  { "inlaidCorner", inlaidCorner, inlaidCorner, corSize,   nCor,   0, noFD},
  { "inlaidSide",   inlaidSide,   inlaidSide,   sideSize,  nSide,  0, noFD},
  { "pandiagonal",  pandiagonal,  pandiagonal,  panSize,   nPan,   0, noFD},
  { "ultramagic",   ultramagic,   ultramagic,   ultraSize, nUltra, 0, noFD}
};
//---------------------------------------------------------------------------------

void putCenter(int a, int b, int c, int d) {
//   ---------
  int *p = Centers[numCenters++];

  p[0] = a;      p[1] = b;      p[2] = c;      p[3] = d;
  p[4] = Sum2-d; p[5] = Sum2-c; p[6] = Sum2-b; p[7] = Sum2-a;
} // putCenter

void getCenters() {
//   ----------
  numCenters = 0;

  for (int i1 = 0; i1 < (Mid-3); i1++)
    for (int i2 = i1+1; i2 < (Mid-2); i2++)
      for (int i3 = i2+1; i3 < (Mid-1); i3++)
        for (int i4 = i3+1; i4 < Mid; i4++)
          putCenter(i1, i2, i3, i4);
} // getCenters
//---------------------------------------------------------------------------

const int pairUsed[NN] =
  {
    0x1000001, 0x0800002, 0x0400004, 0x0200008, 0x0100010,
    0x0080020, 0x0040040, 0x0020080, 0x0010100, 0x0008200,
    0x0004400, 0x0002800, 0x0001000, 0x0002800, 0x0004400,
    0x0008200, 0x0010100, 0x0020080, 0x0040040, 0x0080020,
    0x0100010, 0x0200008, 0x0400004, 0x0800002, 0x1000001
  };
//--------------------------------------------------------------------------

void getBorders() {
//   ----------
  for (int i = 0; i < numPartitions; i++) {
    int *p = Centers[i], cUsed = 1<<Mid;
    for (int j = 0; j < MMd2; j++) cUsed |= pairUsed[p[j]];
    int k = 0; p = Borders[i];  
    for (int j = 0; j < NN; j++) if (((1<<j)&cUsed)==0) p[k++] = j;
  }
} // getBorders
//---------------------------------------------------------------------------

void getPartitions() {
//   -------------
  getCenters(); getBorders();
} // getPartitions
//--------------------------------------------------------------------------

int get5Combos(int partition) {
//  ----------
  num5Combos = 0;

  int *b = Borders[partition];
  for (int i = 0; i < (NN-MM-4); i++) {
    int iUsed = pairUsed[b[i]];
    for (int j = i+1; j < (NN-MM-3); j++) if (((1<<b[j])&iUsed)==0) {
      int jUsed = iUsed | pairUsed[b[j]];
      for (int k = j+1; k < (NN-MM-2); k++) if (((1<<b[k])&jUsed)==0) {
        int kUsed = jUsed | pairUsed[b[k]];
	       for (int l = k+1; l < (NN-MM-1); l++) if (((1<<b[l])&kUsed)==0) {
          int lUsed = kUsed | pairUsed[b[l]];
	         for (int m = l+1; m < (NN-MM); m++) if (((1<<b[m])&lUsed)==0)
	         if ( ((b[i] + b[j] + b[k] + b[l] + b[m]) == MSum) )  { 
            int *p = fiveCombos[num5Combos++];
            p[0] = b[i]; p[1] = b[j]; p[2] = b[k]; p[3] = b[l]; p[4] = b[m];
          }
        }
      }
    }
  }
  return num5Combos;
} // get5Combos
//-----------------------------------------------------------------------

void put5UPermu(int a, int b, int c, int d, int e) {
//   ----------
  int *p;

  p = fiveUPermus[num5UPermus++];
  p[0] = a; p[1] = b; p[2] = c; p[3] = d; p[4] = e;
  if (e < Mid) { // Note a < e
    ptrUsed *q = &(fiveUPermusLH[num5UPermusLH++]);
    q->p = p;
    q->u = (1<<b)|(1<<c)|(1<<d)|(1<<(Sum2-b))|(1<<(Sum2-c))|(1<<(Sum2-d));
  }
} // put5UPermu
//----------------------------------------------------------------------------

void get5UPermus() {
//   -----------
  num5UPermus = 0; num5UPermusLH = 0;

  for (int i = 0; i < num5Combos; i++) {
    int *p = fiveCombos[i];
    int p0 = p[0]; int p1 = p[1]; int p2 = p[2]; int p3 = p[3]; int p4 = p[4];

    put5UPermu(p0, p2, p3, p4, p1);
    put5UPermu(p0, p1, p3, p4, p2);
    put5UPermu(p0, p1, p2, p4, p3);
    put5UPermu(p0, p1, p2, p3, p4);

    put5UPermu(p1, p0, p3, p4, p2);
    put5UPermu(p1, p0, p2, p4, p3);
    put5UPermu(p1, p0, p2, p3, p4);

    put5UPermu(p2, p0, p1, p4, p3);
    put5UPermu(p2, p0, p1, p3, p4);

    put5UPermu(p3, p0, p1, p2, p4);
  }
} // get5UPermus 
//--------------------------------------------------------------------------

void get5UPermus15() {
//   -------------
  for (int i = 0; i < NN; i++) for (int j = 0; j < NN; j++)
    num5UPermus15[i][j] = 0;

  for (int i = 0; i < num5UPermus; i++) {
    int *p = fiveUPermus[i];  ptrUsed *q;

    q = &(fiveUPermus15[p[0]][p[4]][(num5UPermus15[p[0]][p[4]])++]);
    q->p = p; q->u = (1<<p[1])|(1<<p[2])|(1<<p[3]);
  }
} // get5UPermus15
//-------------------------------------------------------------------------------

//void printSquare(int x[N][N], FILE *wfp) {
////   -----------
//  for (int r = 0; r < N; ++r) {
//    fprintf(wfp, "%2d", x[r][0] & 0x1f);
//    for (int c = 1; c < N; ++c)
//      fprintf(wfp, " %2d", x[r][c] & 0x1f);
//    fprintf(wfp, "\n");
//  }
//  fprintf(wfp, "\n");
//} // printSquare
//---------------------------------------------------------------------------

void putSquare(int a[N][N], char **ss, bool isPan)
//   ---------
{
  char *s = *ss;
  if (isPan) {
    for (int i = 0; i < N; i++) { for (int j = 0; j < N; j++) {
	     for (int r = 0; r < N; r++) {
	       for (int c = 0; c < N; c++) {
          int ri = r+i, cj = c+j,
              riModN = ri < N ? ri : ri - N,
              cjModN = cj < N ? cj : cj - N,
              x = a[riModN][cjModN];

          if (x < 10)      { *s++ = ' '; *s++ = '0' + x; }
          else if (x < 20) { *s++ = '1'; *s++ = '0' - 10 + x; }
          else             { *s++ = '2'; *s++ = '0' - 20 + x; } *s++ = ' ';
        }
        *--s = '\n'; s++;
      }
      *s++ = '\n';
	   }}
  } else {
    for (int r = 0; r < N; ++r) {
      for (int c = 0; c < N; ++c) {
        int x = a[r][c];
        if (x < 10)      { *s++ = ' '; *s++ = '0' + x; }
        else if (x < 20) { *s++ = '1'; *s++ = '0' - 10 + x; }
        else             { *s++ = '2'; *s++ = '0' - 20 + x; } *s++ = ' ';
      }
      *--s = '\n'; s++;
    }
    *s++ = '\n';
  }
  *ss = s;
} // putSquare
//---------------------------------- Associative -------------------------------

// Combinations that do not have Sum3 or include complementary pairs.
const int maxAssoc3Combos = 32;
int numAssoc3Combos,
    assoc3Combos[maxAssoc3Combos][M];

// Permutations of assoc3Combos, in which the third is greater than the first.
// Gets 1/4 of squares. Make all squares by swapping rows and columns.
const int maxAssoc3UPermus = 96;
int numAssoc3UPermus,
    assoc3UPermus[maxAssoc3UPermus][M];

// The assoc3UPermus indexed by the first and third.
const int maxAssoc3UPermus13 = 4;
int numAssoc3UPermus13[NN][NN],
    assoc3UPermus13[NN][NN][maxAssoc3UPermus13];

const int maxAssoc3Squares = 96;
int numAssoc3Squares,
    xSquares[maxAssoc3Squares][N][N],
    assoc3Sums[maxAssoc3Squares][2];   // sums of column 1 and row 1
//---------------------------------------------------------------------------

void getAssoc3Combos(int partition) {
//   ---------------
  numAssoc3Combos = 0;

  int *p = Centers[partition];
  for (int i = 0; i < (MM-3); i++)
    for (int j = i+1; j < (MM-2); j++)
      for (int k = j+1; k < (MM-1); k++) {
	       int pi = p[i]; int pj = p[j]; int pk = p[k];

	       if ( (pi + pj + pk != Sum3) && ((pi + pj) != Sum2) &&
             ((pi + pk) != Sum2) && ((pj + pk) != Sum2) ) {
          int *p = assoc3Combos[numAssoc3Combos++];
          p[0] = pi; p[1] = pj; p[2] = pk;
        }
      }		
} // getAssoc3Combos
//---------------------------------------------------------------------------

void assocPermute3_2(int a, int b, int c) {
//   ---------------
  int *p;

  if (b > a) {
    p = assoc3UPermus[numAssoc3UPermus++];
    p[0] = a;  p[1] = c;  p[2] = b;
    assoc3UPermus13[a][b][(numAssoc3UPermus13[a][b])++] = c;
  }
  if (c > a) {
    p = assoc3UPermus[numAssoc3UPermus++];
    p[0] = a;  p[1] = b;  p[2] = c;
    assoc3UPermus13[a][c][(numAssoc3UPermus13[a][c])++] = b;
  }
} // assocPermute3_2
//---------------------------------------------------------------------------

void getAssoc3UPermus() {
//   ----------------
  numAssoc3UPermus = 0;
  for (int i = 0; i < NN; i++) for (int j = 0; j < NN; j++)
     numAssoc3UPermus13[i][j] = 0;

  for (int i = 0; i < numAssoc3Combos; i++) {
    int *p = assoc3Combos[i];

    assocPermute3_2(p[0], p[1], p[2]);
    assocPermute3_2(p[1], p[0], p[2]);
    assocPermute3_2(p[2], p[1], p[0]);
  }
} // getAssoc3UPermus
//---------------------------------------------------------------------------

int getAssoc3Squares() {
//  ----------------
  int count = 0, i0 = numAssoc3UPermus, *r1 = assoc3UPermus[0];
                  
  while (i0--) {  // first row 
    int r11 = r1[0], c13 = Sum2 - r1[2],
        i1 = numAssoc3UPermus13[r11][c13], *c1 = assoc3UPermus13[r11][c13];
    while (i1--) {
      int c12 = *c1;
      if ( (c12 != r1[1]) && (c12 != (Sum2-r1[1])) ) {
        int *p = assoc3Sums[count];
        p[0] = r11 + c12 + c13; p[1] = r11 + r1[1] + r1[2];

        p = xSquares[count][1];
        p[1] = r11; p[2] = r1[1]; p[3] = r1[2];
        p = xSquares[count][2];
        p[1] = c12; p[2] = Mid; p[3] = Sum2-c12;
        p = xSquares[count][3];
        p[1] = c13; p[2] = Sum2-r1[1]; p[3] = Sum2-r11;
        ++count;
      }
      ++c1;
    }
    r1 += M;
  }
  return numAssoc3Squares = count;
} // getAssoc3Squares
//--------------------------------------------------------------------------

void outSquaresAssoc(int x[N][N]) {
//   ---------------
  int y[N][N]; specialData *p = &which[kAssoc];
  for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c)
    y[r][c] = x[r][c]+1;
  putSquare(y, &p->s, false);
  for (int r = 0; r < N; ++r) {
    int tmp = y[r][1]; y[r][1] = y[r][3]; y[r][3] = tmp;
  }
  putSquare(y, &p->s, false);
  for (int c = 0; c < N; ++c) {
    int tmp = y[1][c]; y[1][c] = y[3][c]; y[3][c] = tmp;
  }
  putSquare(y, &p->s, false);
  for (int r = 0; r < N; ++r) {
    int tmp = y[r][1]; y[r][1] = y[r][3]; y[r][3] = tmp;
  }
  putSquare(y, &p->s, false);
  p->count += 4;
} // outSquaresAssoc
//---------------------------------------------------------------------------

void putAssocBorder(
//   --------------
                 int a, int *r, int *ro, bool cPlus,
                        int *c, int *co, bool rPlus, int count)
{
  while (--count >= 0) {
    int i, j, k, l, m, n;
         if (ro[count] == 0) { i = 1; j = 2; k = 3; }
    else if (ro[count] == 1) { i = 1; j = 3; k = 2; }
    else                     { i = 2; j = 3; k = 1; }
         if (co[count] == 0) { l = 1; m = 2; n = 3; }
    else if (co[count] == 1) { l = 1; m = 3; n = 2; }
    else                     { l = 2; m = 3; n = 1; }

    int w, x, y, z;
    if ((r[i] > r[j]) == cPlus)
      { w = r[i]; x = r[j]; } else { w = r[j]; x = r[i]; }
    if ((c[l] > c[m]) == rPlus)
      { y = c[l]; z = c[m]; } else { y = c[m]; z = c[l]; }
    int *p = xSquares[a][0];
    p[0] = r[0]; p[1] = w; p[2] = r[k]; p[3] = x; p[4] = r[4];
    p = xSquares[a][1]; p[0] = y;    p[4] = Sum2-z;
    p = xSquares[a][2]; p[0] = c[n]; p[4] = Sum2-c[n];
    p = xSquares[a][3]; p[0] = z;    p[4] = Sum2-y;
    p = xSquares[a][4];
    p[0] = c[4]; p[1] = Sum2-x; p[2] = Sum2-r[k];
                 p[3] = Sum2-w; p[4] = Sum2-r[0];
    outSquaresAssoc(xSquares[a]);
  }
} // putAssocBorder
//--------------------------------------------------------------------------

void getAssoc5Squares() {
//   ----------------
  ptrUsed *p0 = fiveUPermusLH; int i0 = num5UPermusLH;           
  while (i0--) {   // first row
    int *r1 = p0->p, r1Used = p0->u,
        r11 = r1[0], r12 = r1[1], r13 = r1[2], r14 = r1[3], c15 = Sum2-r1[4],
        r12_r13 = r12 > r13 ? r12 - r13 : r13 - r12,
        r12_r14 = r12 > r14 ? r12 - r14 : r14 - r12,
        r13_r14 = r13 > r14 ? r13 - r14 : r14 - r13,
        i1 = num5UPermus15[r11][c15]; 

    ptrUsed *p1 = fiveUPermus15[r11][c15];           
    while (i1--) {   // first column
      if ((r1Used & p1->u) == 0) {
        int *c1 = p1->p,
            c12 = c1[1], c13 = c1[2], c14 = c1[3],
            c12_c13 = c12 > c13 ? c12 - c13 : c13 - c12,
            c12_c14 = c12 > c14 ? c12 - c14 : c14 - c12,
            c13_c14 = c13 > c14 ? c13 - c14 : c14 - c13;

        for (int a = 0; a < numAssoc3Squares; ++a) {
          int cSum = assoc3Sums[a][0], rSum = assoc3Sums[a][1],
              colDiff = Sum3 > cSum ? Sum3 - cSum : cSum - Sum3,
              rowDiff = Sum3 > rSum ? Sum3 - rSum : rSum - Sum3;
          const int maxHits = 4; int ro[maxHits], co[maxHits], count = 0; 
          if (r12_r13 == colDiff) {
            if (c12_c13 == rowDiff) { ro[count] = 0; co[count++] = 0; }
            if (c12_c14 == rowDiff) { ro[count] = 0; co[count++] = 1; }
            if (c13_c14 == rowDiff) { ro[count] = 0; co[count++] = 2; }
          }
          if (r12_r14 == colDiff) {
            if (c12_c13 == rowDiff) { ro[count] = 1; co[count++] = 0; }
            if (c12_c14 == rowDiff) { ro[count] = 1; co[count++] = 1; }
            if (c13_c14 == rowDiff) { ro[count] = 1; co[count++] = 2; }
          }
          if (r13_r14 == colDiff) {
            if (c12_c13 == rowDiff) { ro[count] = 2; co[count++] = 0; }
            if (c12_c14 == rowDiff) { ro[count] = 2; co[count++] = 1; }
            if (c13_c14 == rowDiff) { ro[count] = 2; co[count++] = 2; }
          }
          if (count != 0)
            putAssocBorder(a, r1, ro, Sum3 > cSum, c1, co, Sum3 > rSum, count);
        }
      }
      ++p1;
    }
    ++p0;
  }
} // getAssoc5Squares 
//---------------------------------- Concentric --------------------------------

// Combinations that have Sum3 and do not include complementary pairs.
const int maxConc3Combos = 4;
int numConc3Combos,
    conc3Combos[maxConc3Combos][M];

// Permutations of conc3Combos, in which the third is greater than the first.
const int maxConc3UPermus = 12;
int numConc3UPermus,
    conc3UPermus[maxConc3UPermus][M];

// conc3UPermus having the third less than Half.
const int maxConc3UPermusLH = 2;
int numConc3UPermusLH,
    *conc3UPermusLH[maxConc3UPermusLH],

// The conc3UPermus indexed by the first and third, (maximum of 1 permutation).
   conc3UPermus13[NN][NN];
//-----------------------------------------------------------------------------

void getConc3Combos(int partition) {
//   --------------
  numConc3Combos = 0;

  int *p = Centers[partition];
  for (int i = 0; i < (MM-3); i++)
    for (int j = i+1; j < (MM-2); j++)
      for (int k = j+1; k < (MM-1); k++) {
        int pi = p[i]; int pj = p[j]; int pk = p[k];
        if ( (pi + pj + pk == Sum3) &&
             ((pi + pj) != Sum2) && ((pi + pk) != Sum2) &&
             ((pj + pk) != Sum2) ) {
          int *p = conc3Combos[numConc3Combos++];
          p[0] = pi; p[1] = pj; p[2] = pk;
        }
      }
} // getConc3Combos
//---------------------------------------------------------------------------

void putConc3UPermu(int a, int b, int c) {
//   --------------
  int *p = conc3UPermus[numConc3UPermus++];
  p[0] = a; p[1] = b; p[2] = c;
  
  conc3UPermus13[a][c] = b;
  if (c < Mid) conc3UPermusLH[numConc3UPermusLH++] = p;
} // putConc3UPermu
//----------------------------------------------------------------------------

void getConc3UPermus() {
//   ---------------
  numConc3UPermus = 0; numConc3UPermusLH = 0;
  for (int i = 0; i < NN; i++) for (int j = 0; j < NN; j++)
      conc3UPermus13[i][j] = -1;

  for (int i = 0; i < numConc3Combos; i++) {
    int *p = conc3Combos[i];

    putConc3UPermu(p[0], p[2], p[1]);
    putConc3UPermu(p[0], p[1], p[2]);
    putConc3UPermu(p[1], p[0], p[2]);
  }
} // getConc3UPermus 
//---------------------------------------------------------------------------

int getConc3Square(int x[N][N]) {
//  --------------
  const int T = Sum2; int i0 = numConc3UPermusLH, *r1 = conc3UPermusLH[0];                  
  while (i0--) {  // first row
    int c12 = conc3UPermus13[r1[0]][T-r1[2]];
    if ( (c12 >= 0) && (((1<<c12)&pairUsed[r1[1]])==0) ) {
      x[1][1] = r1[0];    x[1][2] = r1[1];    x[1][3] = r1[2];
      x[2][1] = c12;      x[2][2] = Mid;      x[2][3] = T-c12;
      x[3][1] = T-r1[2];  x[3][2] = T-r1[1];  x[3][3] = T-r1[0];
      return 1;
    }
    ++r1;
  }
  return 0;
} // getConc3Square
//--------------------------------------------------------------------------

bool Bordered;
void outSquaresConcRotateCenter(int x[N][N]) {
//   --------------------------
  int y[N][N]; specialData *p = &which[kConc], *q = &which[kBord];
  for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c) y[r][c] = x[r][c];
  putSquare(y, &p->s, false); if (Bordered) putSquare(y, &q->s, false);
  int loop = 3;
  while (loop--) { // rotate
    int tmp = y[1][1]; y[1][1] = y[3][1]; y[3][1] = y[3][3]; y[3][3] = y[1][3];
              y[1][3] = tmp;
        tmp = y[1][2]; y[1][2] = y[2][1]; y[2][1] = y[3][2]; y[3][2] = y[2][3];
              y[2][3] = tmp;
    putSquare(y, &p->s, false); if (Bordered) putSquare(y, &q->s, false);
   }
   // mirror
   int tmp = y[1][1]; y[1][1] = y[1][3]; y[1][3] = tmp;
       tmp = y[2][1]; y[2][1] = y[2][3]; y[2][3] = tmp;
       tmp = y[3][1]; y[3][1] = y[3][3]; y[3][3] = tmp;
   putSquare(y, &p->s, false); if (Bordered) putSquare(y, &q->s, false);
   loop = 3; // rotate
   while (loop--) {
    int tmp = y[1][1]; y[1][1] = y[3][1]; y[3][1] = y[3][3]; y[3][3] = y[1][3];
              y[1][3] = tmp;
        tmp = y[1][2]; y[1][2] = y[2][1]; y[2][1] = y[3][2]; y[3][2] = y[2][3];
              y[2][3] = tmp;
     putSquare(y, &p->s, false); if (Bordered) putSquare(y, &q->s, false);
   }
} // outSquaresConcRotateCenter

void outSquaresConcPermuteRows(int x[N][N]) {
//   -------------------------
  int y[N][N];
  for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c) y[r][c] = x[r][c];
  outSquaresConcRotateCenter(y);
  // Row permutations.
  int tmp = y[2][0]; y[2][0] = y[3][0]; y[3][0] = tmp;
      tmp = y[2][4]; y[2][4] = y[3][4]; y[3][4] = tmp;
  outSquaresConcRotateCenter(y);
  int loop = 2;
  while(loop--) {
    int tmp = y[1][0]; y[1][0] = y[2][0]; y[2][0] = tmp;
        tmp = y[1][4]; y[1][4] = y[2][4]; y[2][4] = tmp;
    outSquaresConcRotateCenter(y);
        tmp = y[2][0]; y[2][0] = y[3][0]; y[3][0] = tmp;
        tmp = y[2][4]; y[2][4] = y[3][4]; y[3][4] = tmp;
    outSquaresConcRotateCenter(y);
  }
} // outSquaresConcPermuteRows

void outSquaresConc(int x[N][N]) {
//   --------------
  int y[N][N];
  for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c)
    y[r][c] = x[r][c]+1;
  outSquaresConcPermuteRows(y);
  // Column permutations.
  int tmp = y[0][2]; y[0][2] = y[0][3]; y[0][3] = tmp;
      tmp = y[4][2]; y[4][2] = y[4][3]; y[4][3] = tmp;
  outSquaresConcPermuteRows(y);
  int loop = 2;
  while(loop--) {
    int tmp = y[0][1]; y[0][1] = y[0][2]; y[0][2] = tmp;
        tmp = y[4][1]; y[4][1] = y[4][2]; y[4][2] = tmp;
    outSquaresConcPermuteRows(y);
        tmp = y[0][2]; y[0][2] = y[0][3]; y[0][3] = tmp;
        tmp = y[4][2]; y[4][2] = y[4][3]; y[4][3] = tmp;
    outSquaresConcPermuteRows(y);
  }
  const int permutationsRotations = 6 * 6 * 8; // 288
  which[kConc].count += permutationsRotations;
  if (Bordered) which[kBord].count += permutationsRotations;
} // outSquaresConc
//---------------------------------------------------------------------------

void putConcBorder(int x[N][N], int *r, int *c)
//   -------------
{
  const int T = Sum2;
  x[0][0] = r[0];   x[0][1] = r[1];   x[0][2] = r[2];
  x[0][3] = r[3];   x[0][4] = r[4];
  x[1][0] = c[1];   x[1][4] = T-c[1];
  x[2][0] = c[2];   x[2][4] = T-c[2];
  x[3][0] = c[3];   x[3][4] = T-c[3];
  x[4][0] = c[4];   x[4][1] = T-r[1]; x[4][2] = T-r[2];
  x[4][3] = T-r[3]; x[4][4] = T-r[0];
  outSquaresConc(x);
} // putConcBorder
//--------------------------------------------------------------------------

void getConc5Squares(int x[N][N]) {
//   ---------------
  ptrUsed *p0 = fiveUPermusLH; int i0 = num5UPermusLH;           
  while (i0--) {   // first row
    int *r1 = p0->p, r1Used = p0->u,
        r11 = r1[0], r12 = r1[1], r13 = r1[2], r14 = r1[3], c15 = Sum2-r1[4],
        i1 = num5UPermus15[r11][c15]; 
    ptrUsed *p1 = fiveUPermus15[r11][c15];
                  
    while (i1--) {   // first column
      if ((r1Used & p1->u) == 0) putConcBorder(x, r1, p1->p); ++p1;
    }
    ++p0;
  }
} // getConc5Squares
//---------------------------------- Inlaid ------------------------------------

void outSquaresInlaidCorner(int x[N][N], int rotation) {
//   ----------------------
  int y[N][N]; specialData *p = &which[kCor];
  for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c)
    y[r][c] = x[r][c]+1;
  switch (rotation) {
    int tmp;
    case 1:
      tmp = y[0][0]; y[0][0] = y[2][0]; y[2][0] = y[2][2];
                     y[2][2] = y[0][2]; y[0][2] = tmp;
      tmp = y[0][1]; y[0][1] = y[1][0]; y[1][0] = y[2][1];
                     y[2][1] = y[1][2]; y[1][2] = tmp; break;
    case 2: 
      tmp = y[0][0]; y[0][0] = y[2][2]; y[2][2] = tmp;
      tmp = y[0][2]; y[0][2] = y[2][0]; y[2][0] = tmp;
      tmp = y[0][1]; y[0][1] = y[2][1]; y[2][1] = tmp;
      tmp = y[1][0]; y[1][0] = y[1][2]; y[1][2] = tmp; break;
    case 3:
      tmp = y[0][0]; y[0][0] = y[0][2]; y[0][2] = y[2][2];
                     y[2][2] = y[2][0]; y[2][0] = tmp;
      tmp = y[0][1]; y[0][1] = y[1][2]; y[1][2] = y[2][1];
                     y[2][1] = y[1][0]; y[1][0] = tmp; break;
    default: break;
  }
  putSquare(y, &p->s, false);
  for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c)
    y[r][c] = Sum2_1 - y[r][c];
  putSquare(y, &p->s, false);
  p->count += 2;
} // outSquaresInlaidCorner
//--------------------------------------------------------------------------

void getInlaidCorner5Squares(int partition, int y[N][N]) {
//   -----------------------
  int x[N][N], xUsed = 0, *pB = Borders[partition];
  x[0][0] = y[1][1]; x[0][1] = y[1][2]; x[0][2] = y[1][3];
  x[1][0] = y[2][1]; x[1][1] = y[2][2]; x[1][2] = y[2][3];
  x[2][0] = y[3][1]; x[2][1] = y[3][2]; x[2][2] = y[3][3];
  for (int r = 0; r < 3; ++r) for (int c = 0; c < 3; ++c)
    xUsed |= (1<<x[r][c]);

  const int T = Sum2; int *p0 = pB, r1 = NN_MM/2;
  while (r1--) {
    int r14 = *p0, r15 = T-r14,
        r1Used = pairUsed[r14] | xUsed, r2 = NN_MM, *p1 = pB;

    while (r2--) { if ((pairUsed[*p1] & r1Used) == 0) {
      int r24 = *p1, r25 = T-r24,
          r2Used = pairUsed[r24] | r1Used, r3 = NN_MM, *p2 = pB;

      while (r3--) { if ((pairUsed[*p2] & r2Used) == 0) {
       int r34 = *p2, r35 = T-r34,
           r3Used = pairUsed[r34] | r2Used, yx = NN_MM, *p3 = pB;

        while (yx--) { if ((pairUsed[*p3] & r3Used) == 0) {
          int r44 = *p3, r55 = T-r44, yxUsed = pairUsed[r44] | r3Used,
              r54 = MSum - r14 - r24 - r34 - r44, r45 = T-r54;

        if ((r54 >= 0) && (r54 < NN) && (((1<<r54) & yxUsed) == 0)) {
          yxUsed |= pairUsed[r54];
          int c1 = NN_MM, *p4 = pB;

          while (c1--) { if ((pairUsed[*p4] & yxUsed) == 0) {
            int r41 = *p4, r51 = T-r41,
                c1Used = pairUsed[r41] | yxUsed, c2 = NN_MM, *p5 = pB;

            while (c2--) { if ((pairUsed[*p5] & c1Used) == 0) {
              const int T = Sum2;
              int r42 = *p5, r52 = T-r42, c2Used = pairUsed[r42] | c1Used,
                z = MSum - r15 - r24 - r42 - r51, rotation = -1;
                     if (x[0][0] == z) rotation = 2;
                else if (x[0][2] == z) rotation = 1;
                else if (x[2][0] == z) rotation = 3;
                else if (x[2][2] == z) rotation = 0;

               if (rotation >= 0) {
                 int r43 = MSum - r41 - r42 - r44 - r45;
                 if ((r43 >= 0) && (r43 < NN) && (((1<<r43) & c2Used) == 0)) {
                   x[0][3] = r14; x[0][4] = r15;
                   x[1][3] = r24; x[1][4] = r25;
                   x[2][3] = r34; x[2][4] = r35;
                   x[3][0] = r41; x[3][1] = r42; x[3][2] = r43;
                   x[3][3] = r44; x[3][4] = r45;
                   x[4][0] = r51; x[4][1] = r52; x[4][2] = T-r43;
                   x[4][3] = r54; x[4][4] = r55;
                   outSquaresInlaidCorner(x, rotation);
                }
              }} ++p5; }}
	           ++p4; }
          }
	       } ++p3; }
      } ++p2; }
    } ++p1; }
    ++p0;
  }
} // getInlaidCorner5Squares 
//---------------------------------------------------------------------------

void outSquaresInlaidSide(int x[N][N], int rotation) {
//   --------------------
  int y[N][N]; specialData *p = &which[kSide];
  for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c)
    y[r][c] = x[r][c]+1;
  switch (rotation) {
    int tmp;
    case 1:
      tmp = y[0][1]; y[0][1] = y[2][1]; y[2][1] = y[2][3];
                     y[2][3] = y[0][3]; y[0][3] = tmp;
      tmp = y[0][2]; y[0][2] = y[1][1]; y[1][1] = y[2][2];
                     y[2][2] = y[1][3]; y[1][3] = tmp; break;
    case 2: 
      tmp = y[0][1]; y[0][1] = y[2][3]; y[2][3] = tmp;
      tmp = y[0][3]; y[0][3] = y[2][1]; y[2][1] = tmp;
      tmp = y[0][2]; y[0][2] = y[2][2]; y[2][2] = tmp;
      tmp = y[1][1]; y[1][1] = y[1][3]; y[1][3] = tmp; break;
    case 3:
      tmp = y[0][1]; y[0][1] = y[0][3]; y[0][3] = y[2][3];
                     y[2][3] = y[2][1]; y[2][1] = tmp;
      tmp = y[0][2]; y[0][2] = y[1][3]; y[1][3] = y[2][2];
                     y[2][2] = y[1][1]; y[1][1] = tmp; break;
    default: break;
  }
  putSquare(y, &p->s, false);
  for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c)
    y[r][c] = Sum2_1 - y[r][c];
  putSquare(y, &p->s, false); p->count += 2;
} // outSquaresInlaidSide
//--------------------------------------------------------------------------

void getInlaidSide5Squares(int partition, int y[N][N]) {
//   ---------------------
  int x[N][N], xUsed = 0, *pB = Borders[partition];
  x[0][1] = y[1][1]; x[0][2] = y[1][2]; x[0][3] = y[1][3];
  x[1][1] = y[2][1]; x[1][2] = y[2][2]; x[1][3] = y[2][3];
  x[2][1] = y[3][1]; x[2][2] = y[3][2]; x[2][3] = y[3][3];
  for (int r = 0; r < 3; ++r) for (int c = 1; c < 4; ++c)
    xUsed |= (1<<x[r][c]);
  const int T = Sum2; int *p0 = pB, r1 = NN_MM/2;

  while (r1--) {
    int r11 = *p0, r15 = T-r11,
        r1Used = pairUsed[r11] | xUsed, r2 = NN_MM, *p1 = pB;
    while (r2--) { if ((pairUsed[*p1] & r1Used) == 0) {
      int r21 = *p1, r25 = T-r21,
          r2Used = pairUsed[r21] | r1Used, r3 = NN_MM, *p2 = pB;
      while (r3--) { if ((pairUsed[*p2] & r2Used) == 0) {
        int r31 = *p2, r35 = T-r31,
        r3Used = pairUsed[r31] | r2Used, c2 = NN_MM, *p3 = pB;
        while (c2--) { if ((pairUsed[*p3] & r3Used) == 0) {
          int r42 = *p3, r52 = T-r42,
          c2Used = pairUsed[r42] | r3Used,
          z = MSum - r15 - r42, r51 = z-x[1][3]-x[2][2];
          for (int rotation = 0; rotation < 4; ++rotation) {
          if ((r51 >= 0) && (r51 < NN) && (((1<<r51)&c2Used)==0)) {
            int r51Used = c2Used | (1<<r51),
                r41 = MSum - r11 - r21 - r31 - r51;

            if ((r41 >= 0) && (r41 < NN) && (((1<<r41)&r51Used)==0)) {
              int c4 = NN_MM, *p4 = pB; r51Used |= (1<<r41);

              while (c4--) { if ((pairUsed[*p4] & r51Used) == 0) {
                int r44 = *p4, r54 = T-r44,
                    c4Used = pairUsed[r44] | r51Used,
                    z = MSum - r11 - r44,
                    r55 = rotation == 0 ? z-x[1][1]-x[2][2] :
                          rotation == 1 ? z-x[2][2]-x[1][3] :
                          rotation == 2 ? z-x[1][3]-x[0][2] :
                                          z-x[0][2]-x[1][1];
                if ((r55 >= 0) && (r55 < NN) && (((1<<r55)&c4Used)==0)) {
                  c4Used |= (1<<r55);
                  int r45 = MSum - r15 - r25 - r35 - r55;
                  if ((r45 >= 0) && (r45 < NN) && (((1<<r45)&c4Used)==0)) {
                    c4Used |= (1<<r45);
                    int r43 = MSum - r41 - r42 - r44 - r45;
                   
                  if ((r43 >= 0) && (r43 < NN) && (((1<<r43)&c4Used)==0) &&
                      (r51 + r52 - r43 + r54 + r55 == Sum3)) {
                    x[0][0] = r11; x[0][4] = r15;
                    x[1][0] = r21; x[1][4] = r25;
                    x[2][0] = r31; x[2][4] = r35;
                    x[3][0] = r41; x[3][1] = r42; x[3][2] = r43;
                    x[3][3] = r44; x[3][4] = r45;
                    x[4][0] = r51; x[4][1] = r52; x[4][2] = T-r43;
                    x[4][3] = r54; x[4][4] = r55;
                    outSquaresInlaidSide(x, rotation);
                  }}
               }
             } ++p4; }}}
                 if (rotation == 0) r51 = z-x[0][2]-x[1][3];
            else if (rotation == 1) r51 = z-x[1][1]-x[0][2];
            else if (rotation == 2) r51 = z-x[2][2]-x[1][1];
          }
	       } ++p3; }
      } ++p2; }
    } ++p1; }
    ++p0;
  }
} // getInlaidSide5Squares
//-------------------------------- Pandiagonal ---------------------------------

const int numGroup7 = 16;
int pan7[numGroup7] =
{
0x1f642605,0x1f694cc5,0x1f841e05,0x1f8944c5,0x1d542a24,0x1d5950e4,0x1d941a24,0x1d9940e4,
0x19542a62,0x19595122,0x19941a62,0x19994122,0x17642681,0x17694d41,0x17841e81,0x17894541
};
const int numGroup13022 = 32;
int pan13022[numGroup13022] =
{
0x32859193,0x2c8705f0,0x02ebaa47,0x08bb9e4a,0x332589c7,0x312605e6,0x02cbd113,0x04bbcd14,
0x02cbaa49,0x04bba64a,0x328589d1,0x308605f0,0x02ebd111,0x08bbc514,0x33259189,0x2d2705e6,
0x30861574,0x2a8789d1,0x04fba646,0x0acb9a49,0x04fbcd10,0x0acbc113,0x3126156a,0x2b2789c7,
0x2c871574,0x2a879193,0x08fb9e46,0x0aeb9a47,0x08fbc510,0x0aebc111,0x2d27156a,0x2b279189
};
const int numGroup13065 = 32;
int pan13065[numGroup13065] =
{
0x31278466,0x33270867,0x32e91967,0x28eb8562,0x2d278466,0x33261069,0x32c91969,0x28cb8564,
0x32e44171,0x14eb8562,0x30878470,0x32870871,0x32c44173,0x14cb8564,0x2c878470,0x32861073,
0x2a870871,0x30859474,0x30b44594,0x12bb8985,0x2b270867,0x3125946a,0x30b91d8a,0x26bb8985,
0x2a861073,0x2c859474,0x2cb44dd4,0x0ebb91c5,0x2cb925ca,0x22bb91c5,0x2b261069,0x2d25946a
};
const int numGroup56598 = 64;
int pan56598[numGroup56598] =
{
0x17645c2c,0x1d945c8f,0x1901c8eb,0x1f31c94e,0x25572e97,0x25963e17,0x2f17b312,0x2f35bad2,
0x1d01c92b,0x1f11c94c,0x17845c2e,0x19945c4f,0x25562e97,0x25973e17,0x2f37bad2,0x2f15b312,
0x1d995c8f,0x1c540f0f,0x1e91a28e,0x1f3bc94e,0x2e77b308,0x2e95bac8,0x11572d57,0x11963cd7,
0x19995c4f,0x18540ecf,0x1e71a28c,0x1f1bc94c,0x11562d57,0x11973cd7,0x2e97bac8,0x2e75b308,
0x19545c4b,0x1f845cae,0x1711c8cc,0x1d41c92f,0x2567b277,0x2585ba37,0x2f072f32,0x2f463eb2,
0x1ca1a26f,0x1d4bc92f,0x1f895cae,0x1e440f2e,0x2e672f28,0x2ea63ea8,0x1167b137,0x1185b8f7,
0x1165b137,0x1187b8f7,0x2ea73ea8,0x2e662f28,0x2f473eb2,0x2f062f32,0x2565b277,0x2587ba37,
0x1d545c8b,0x1f645cac,0x1731c8ce,0x1941c8ef,0x18a1a22f,0x194bc8ef,0x1f695cac,0x1e240f2c
};

void getPanSquares() {
//   -------------
  const int T = Sum2_1, F = MSum_1; int x[N][N]; x[2][2] = Mid_1;
  specialData *p = &which[kPan], *q = &which[kUltra];
  for (int i = 0; i < numGroup7; ++i) {
    int tmp = pan7[i];
    x[0][0] = i<4 ? 1 : i<8 ? 2 : i<12 ? 4 : 5; x[4][4] = T-x[0][0];
    x[1][3] = tmp & 0x1f; tmp >>= 5; x[3][1] = T-x[1][3];
    x[1][2] = tmp & 0x1f; tmp >>= 5; x[3][2] = T-x[1][2];
    x[1][1] = tmp & 0x1f; tmp >>= 5; x[3][3] = T-x[1][1];
    x[0][3] = tmp & 0x1f; tmp >>= 5; x[4][1] = T-x[0][3];
    x[0][2] = tmp & 0x1f; tmp >>= 5; x[4][2] = T-x[0][2];
    x[0][1] = tmp & 0x1f;            x[4][3] = T-x[0][1];
    x[1][0] = 23;                    x[3][4] = T-x[1][0];
    x[0][4] = F-x[0][0]-x[0][1]-x[0][2]-x[0][3]; x[4][0] = T-x[0][4];
    x[1][4] = F-   23  -x[1][1]-x[1][2]-x[1][3]; x[3][0] = T-x[1][4];
    x[2][0] = F-x[0][0]-   23  -x[3][0]-x[4][0]; x[2][4] = T-x[2][0];
    x[2][1] = F-x[0][1]-x[1][1]-x[3][1]-x[4][1]; x[2][3] = T-x[2][1];
    putSquare(x, &q->s, false); ++q->count;
    putSquare(x, &p->s, true);  p->count += 25;
  }

  for (int i = 0; i < numGroup13022; ++i) {
    int tmp = pan13022[i];
    x[3][3] = tmp & 0x1f; tmp >>= 5; x[4][4] = T-x[3][3];
    x[1][0] = tmp & 0x1f; tmp >>= 5; x[4][1] = T-x[1][0];
    x[0][4] = tmp & 0x1f; tmp >>= 5; x[3][1] = T-x[0][4];
    x[0][3] = tmp & 0x1f; tmp >>= 5; x[3][4] = T-x[0][3];
    x[0][1] = tmp & 0x1f; tmp >>= 5; x[1][4] = T-x[0][1];
    x[0][0] = tmp & 0x1f;            x[1][1] = T-x[0][0];
    x[0][2] = F-x[0][0]-x[0][1]-x[0][3]-x[0][4]; x[2][0] = T-x[0][2];
    x[2][1] = F-x[0][1]-x[1][1]-x[3][1]-x[4][1]; x[3][2] = T-x[2][1];
    x[3][0] = F-x[3][1]-x[3][2]-x[3][3]-x[3][4]; x[4][3] = T-x[3][0];
    x[4][0] = F-x[0][0]-x[1][0]-x[2][0]-x[3][0]; x[1][3] = T-x[4][0];
    x[4][2] = F-x[4][0]-x[4][1]-x[4][3]-x[4][4]; x[2][4] = T-x[4][2];
    x[1][2] = F-x[1][0]-x[1][1]-x[1][3]-x[1][4]; x[2][3] = T-x[1][2];
    putSquare(x, &p->s, true); p->count += 25;
  }
  for (int i = 0; i < numGroup13065; ++i) {
    int tmp = pan13065[i];
    x[4][0] = tmp & 0x1f; tmp >>= 5; x[4][4] = T-x[4][0];
    x[3][0] = tmp & 0x1f; tmp >>= 5; x[4][3] = T-x[3][0];
    x[1][1] = tmp & 0x1f; tmp >>= 5; x[3][1] = T-x[1][1];
    x[1][0] = tmp & 0x1f; tmp >>= 5; x[0][3] = T-x[1][0];
    x[0][1] = tmp & 0x1f; tmp >>= 5; x[1][4] = T-x[0][1];
    x[0][0] = tmp & 0x1f;            x[0][4] = T-x[0][0];
    x[0][2] = F-x[0][0]-x[0][1]-x[0][3]-x[0][4]; x[3][2] = T-x[0][2];
    x[3][3] = F-x[0][0]-x[1][1]- Mid-1 -x[4][4]; x[1][3] = T-x[3][3];
    x[1][2] = F-x[1][0]-x[1][1]-x[1][3]-x[1][4]; x[4][2] = T-x[1][2];
    x[2][0] = F-x[0][0]-x[1][0]-x[3][0]-x[4][0]; x[2][1] = T-x[2][0];
    x[4][1] = F-x[0][1]-x[1][1]-x[2][1]-x[3][1]; x[3][4] = T-x[4][1];
    x[2][3] = F-x[0][3]-x[1][3]-x[3][3]-x[4][3]; x[2][4] = T-x[2][3];
    putSquare(x, &p->s, true); p->count += 25;
  }
  for (int i = 0; i < numGroup56598; ++i) {
    int tmp = pan56598[i];
    x[1][4] = tmp & 0x1f; tmp >>= 5; x[4][3] = T-x[1][4];
    x[1][3] = tmp & 0x1f; tmp >>= 5; x[2][3] = T-x[1][3];
    x[1][0] = tmp & 0x1f; tmp >>= 5; x[3][4] = T-x[1][0];
    x[0][3] = tmp & 0x1f; tmp >>= 5; x[4][1] = T-x[0][3];
    x[0][2] = tmp & 0x1f; tmp >>= 5; x[0][0] = T-x[0][2];
    x[0][1] = tmp & 0x1f;            x[3][0] = T-x[0][1];
    x[0][4] = F-x[0][0]-x[0][1]-x[0][2]-x[0][3]; x[3][2] = T-x[0][4];
    x[3][3] = F-x[0][3]-x[1][3]-x[2][3]-x[4][3]; x[2][4] = T-x[3][3];
    x[4][4] = F-x[0][4]-x[1][4]-x[2][4]-x[3][4]; x[4][2] = T-x[4][4];
    x[1][2] = F-x[0][2]- Mid-1 -x[3][2]-x[4][2]; x[4][0] = T-x[1][2];
    x[1][1] = F-x[1][0]-x[1][2]-x[1][3]-x[1][4]; x[2][0] = T-x[1][1];
    x[2][1] = F-x[2][0]- Mid-1 -x[2][3]-x[2][4]; x[3][1] = T-x[2][1];
    putSquare(x, &p->s, true); p->count += 25;
  }
} // getPanSquares
//-------------------------------------------------------------------------------

void getAssocConcInlaidSquares(int partition) {
//   -------------------------
  int numAssoc = 0, numConc = 0;
  getAssoc3Combos(partition);
  if (numAssoc3Combos != 0) {
    getAssoc3UPermus();
    numAssoc = getAssoc3Squares();
  }
  getConc3Combos(partition); int x[N][N];
  if (numConc3Combos != 0) {
    getConc3UPermus();
    numConc = getConc3Square(x);
  }
  if ((numAssoc != 0) || (numConc != 0)) {
    if (get5Combos(partition) != 0) {
      get5UPermus(); get5UPermus15(); 
      if (numAssoc != 0) getAssoc5Squares();
      if (numConc != 0) {
        getConc5Squares(x);
        getInlaidCorner5Squares(partition, x);
        getInlaidSide5Squares(partition, x);
      }
    }
  }
} // getAssocConcInlaidSquares
//-------------------------------------------------------------------------------

void getSquares() {
//   ----------
  getPartitions(); int lastPartition = numPartitions - 1;
  Bordered = false;
  for (int partition = 0; partition < lastPartition; ++partition)
    getAssocConcInlaidSquares(partition);
  Bordered = true; getAssocConcInlaidSquares(lastPartition);
  getPanSquares();  
} // getSquares
//-------------------------------------------------------------------------------

const int bufSize = 128;
bool openDir() {
//   -------
  char buf[bufSize], msg[bufSize]; const char *baseName = "Order5Special";
  strcpy_s(buf, bufSize, baseName); int sub = 0;

  do {
    if (_mkdir(buf) != -1) break;
    if (errno != EEXIST) {
      sprintf_s(msg, bufSize, "Can't make folder %s", buf);
      perror(msg); return false;
    }
    sprintf_s(buf, bufSize, "%s_%d", baseName, ++sub);
  } while (true);
  if (_chdir(buf) == -1) {
    sprintf_s(msg, bufSize, "Can't open folder %s", buf);
    perror(msg); return false;
  }
  printf("Output files are in folder %s\n\n", buf);
  return true;
} // openDir
//--------------------------------------------------------------------------

int openFile(char *name) {
//  --------
  char buf[bufSize]; sprintf_s(buf, bufSize, "%s.txt", name);

  int wfd = -1;
  if ((wfd = _open(buf, _O_CREAT|_O_WRONLY, 0644)) == -1) {
    char msg[bufSize];
    sprintf_s(msg, bufSize, "\a\nCan't open for write %s", buf);
    perror(msg);
  }
  return wfd;
} // openFile
//--------------------------------------------------------------------------

void closeFiles() {
//   ----------
  for (int k = 0; k < numKinds; ++k) {
    specialData *f = &which[k];
    if (f->wfd != noFD) _close(f->wfd);
  }
} // closeFiles
//--------------------------------------------------------------------------

bool openFiles() {
//   ---------
  for (int k = 0; k < numKinds; ++k) {
    specialData *f = &which[k];
    if ((f->wfd = openFile(f->name)) == noFD) {
      closeFiles(); return false;
    }
  }
  return true;
} // openFiles
//--------------------------------------------------------------------------

bool writeSquares() {
//   ------------
  bool ok = true;
  for (int k = 0; k < numKinds; ++k) {
    specialData *f = &which[k];
    if (ok && (_write(f->wfd, f->buf, f->bufSize) != f->bufSize))
      { perror("\a\nError writing file.\n"); ok = false; }
    _close(f->wfd);
  }
  return ok;
} // writeSquares
//--------------------------------------------------------------------------

void printElapsedTime(time_t startTime) {
//   ---------------- 
  int et = (int)difftime(time(NULL), startTime);
  printf("\nelapsed time %2d:%02d\n", et/60, et%60);
} // printElapsedTime
//--------------------------------------------------------------------------

void makeSquares() {
//   -----------
  time_t startTime = time(NULL);

  if (openFiles()) {
    getSquares(); bool inCorrect = false;
    for (int k = 0; k < numKinds; ++k) {
      specialData *p = &which[k];
      if (p->count != p->number) { inCorrect = true; break; }
    }
    if (inCorrect) {
      printf("\a\nAn error occurred. Please report by email.\n");
    } else {
      if (writeSquares()) {
        printf("number of squares:\n\n");
        for (int k = 0; k < numKinds; ++k) {
          specialData *p = &which[k];
          printf("  %-12s  %6d\n",  p->name, p->count);
        }
      }
    }
  }
  printElapsedTime(startTime);
} // makeSquares
//-------------------------------------------------------------------------------

void outputLocalTime() {
//   ---------------
  time_t startTime = time(NULL); struct tm local;
  if (localtime_s(&local, &startTime) == 0) {
    char dateTime[100];
    size_t slen = strftime(dateTime, 100, "%A %Y-%m-%d %X %Z\n\n\0", &local);
    printf("\n%s", dateTime);
  }
} // outputLocalTime
//-------------------------------------------------------------------------------

int _tmain() {
//  ------
  outputLocalTime(); if (openDir()) makeSquares();

  printf("\n\nPress a key to close the console");
  while (!_kbhit()) Sleep(250);
  return 0;
} // _tmain