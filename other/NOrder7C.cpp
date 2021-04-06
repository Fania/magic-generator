/*
 *  File:    NOrder7C.cpp
 *  Author:  S Harry White
 *  Created: 2010-02-05
 *  Updated: 2012-01-29
 */

#include "stdafx.h"
#include <conio.h>     // not CLR
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>   // not CLR

const int L  = 3,
          LL = L * L,             //   9

          M     = 5,
          MM    = M * M,          //  25
          Half5 = MM / 2,         //  12

          N    = 7,
          NN   = N * N,            //  49
          Half = NN / 2,           //  24

// square numbers 0 to NN - 1
          Sum2 = NN - 1,           //  48
          Sum3 = L * Half,         //  72
          Sum5 = M * Half,         // 120
          MSum = N * Half,         // 168

//Partitions of 0 .. NN-1 as MM/2 pairs and other (NN-MM)/2 pairs.
          num7Partitions = 2704156, // 24! / (12!12!)

//Partitions of 0 .. MM-1 as LL/2 pairs and other (MM-LL)/2 pairs.
//Partitions for each 7Partition.
          num5Partitions = 495;    // 12! / (4!8!) = 495

// Number of partitions = num7Partitions * num5Partitions = 1,338,557,220.
//---------------------------- start 3 data ---------------------------------

int numCenters;
int Centers[num5Partitions][LL-1];

// Combinations that do not include complementary pairs.
int num3Combos;
const int max3Combos = 4;
int threeCombos[max3Combos][L];

// Permutations of threeCombos, in which the third is greater than the first.
const int max3UPermus = 12;
int threeUPermus[max3UPermus][L];
int num3UPermus;

// threeUPermus having the third less than Half.
const int max3UPermusLH = 2;
int *threeUPermusLH[max3UPermusLH];
int num3UPermusLH;

// threeUPermus indexed by the first and third.
int threeUPermus13[NN][NN];
//---------------------------- start 5 data ---------------------------------

int fiveBorders[num5Partitions][MM-LL];
int fiveCenters[num7Partitions][MM-1];
int num5Centers;

// Combinations that do not include complementary pairs.
const int max5Combos = 52;
int fiveCombos[max5Combos][M];
int num5Combos;

// Permutations of fiveCombos, in which the fifth is greater than the first.
const int max5UPermus = 520;
int fiveUPermus[max5UPermus][M];
int num5UPermus;

// fiveUPermus having the fifth less than Half.
const int max5UPermusLH = 106;
int *fiveUPermusLH[max5UPermusLH], fiveUPermusGH[max5UPermusLH][M];
int num5UPermusLH;

// fiveUPermus indexed by the first and fifth.
const int max5UPermus15 = 11;
int num5UPermus15[NN][NN];
int *fiveUPermus15[NN][NN][max5UPermus15];
//---------------------------- start 7 data ---------------------------------

int sevenBorders[num7Partitions][NN-MM];

// Combinations that do not include complementary pairs.
const int max7Combos = 1730;
int sevenCombos[max7Combos][N];
int num7Combos;

// Permutations of sevenCombos, in which the seventh is greater than the first.
const int max7UPermus = 36330;
int sevenUPermus[max7UPermus][N];
int num7UPermus;

// sevenUPermus having the seventh less than Half.
const int max7UPermusLH = 7899;
int *sevenUPermusLH[max7UPermusLH], sevenUPermusGH[max7UPermusLH][N];
int num7UPermusLH;

// sevenUPermus indexed by the first and seventh
const int max7UPermus17 = 222;
int num7UPermus17[NN][NN];
int *sevenUPermus17[NN][NN][max7UPermus17];
//=============================== partitions ==============================

struct bools { bool used[NN]; };
struct bools allfree;

void putCenter(int a, int b, int c, int d) {
//   ---------
  int *p = Centers[numCenters++];

  p[0] = a;      p[1] = b;      p[2] = c;      p[3] = d;
  p[4] = Sum2-d; p[5] = Sum2-c; p[6] = Sum2-b; p[7] = Sum2-a;
} // putCenter
//---------------------------------------------------------------------------

void getCenters(int partition) {
//   ----------
  int *p = fiveCenters[partition-1];
  numCenters = 0;

  for (int i1 = 0; i1 < (Half5-3); i1++)
    for (int i2 = i1+1; i2 < (Half5-2); i2++)
      for (int i3 = i2+1; i3 < (Half5-1); i3++)
        for (int i4 = i3+1; i4 < Half5; i4++)
          putCenter(p[i1], p[i2], p[i3], p[i4]);
  //assert(numCenters == num5Partitions);
} // getCenters
//---------------------------------------------------------------------------

void get5Borders(int partition) {
//   -----------
  for (int i = 0; i < num5Partitions; i++) {
    struct bools v = allfree; int *p = Centers[i];
 
    for (int j = 0; j < (LL-1); j++) v.used[p[j]] = true; v.used[Half] = true;

    p = fiveBorders[i]; int *q = fiveCenters[partition-1], k = 0;
    for (int j = 0; j < (MM-1); j++) if (!v.used[q[j]]) p[k++] = q[j];
  }
} // get5Borders
//---------------------------------------------------------------------------

void get5Partitions(int partition) {
//   --------------
  getCenters(partition);
  get5Borders(partition);
} // get5Partitions
//---------------------------------------------------------------------------

void put5Center(int a, int b, int c, int d, int e, int f,
//   ----------
                int g, int h, int i, int j, int k, int l) {
  int *p = fiveCenters[num5Centers++];

  p[0] = a; p[1] = b; p[2] = c; p[3] = d; p[4] = e; p[5] = f;
  p[6] = g; p[7] = h; p[8] = i; p[9] = j; p[10] = k; p[11] = l;

  p[12] = Sum2-l; p[13] = Sum2-k; p[14] = Sum2-j; p[15] = Sum2-i;
  p[16] = Sum2-h; p[17] = Sum2-g; p[18] = Sum2-f; p[19] = Sum2-e;
  p[20] = Sum2-d; p[21] = Sum2-c; p[22] = Sum2-b; p[23] = Sum2-a; 
} // put5Center
//---------------------------------------------------------------------------

void get5Centers() {
//   -----------
  num5Centers = 0;

  for (int i1 = 0; i1 < (Half-11); i1++)
    for (int i2 = i1+1; i2 < (Half-10); i2++)
      for (int i3 = i2+1; i3 < (Half-9); i3++)
        for (int i4 = i3+1; i4 < (Half-8); i4++)
          for (int i5 = i4+1; i5 < (Half-7); i5++)
            for (int i6 = i5+1; i6 < (Half-6); i6++)
              for (int i7 = i6+1; i7 < (Half-5); i7++)
                for (int i8 = i7+1; i8 < (Half-4); i8++)
                  for (int i9 = i8+1; i9 < (Half-3); i9++)
                    for (int i10 = i9+1; i10 < (Half-2); i10++)
                      for (int i11 = i10+1; i11 < (Half-1); i11++)
                        for (int i12 = i11+1; i12 < Half; i12++)
        put5Center(i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12);
  //assert(num5Centers = num7Partitions);
} // get5Centers
//---------------------------------------------------------------------------

void getBorders() {
//   ----------
  for (int i = 0; i < num7Partitions; i++) {
    struct bools v = allfree;  int *p = fiveCenters[i];

    for (int j = 0; j < (MM-1); j++)  v.used[p[j]] = true; v.used[Half] = true;

    p = sevenBorders[i]; int k = 0;
    for (int j = 0; j < NN; j++) if (!v.used[j]) p[k++] = j;
  }
} // getBorders
//---------------------------------------------------------------------------


void getPartitions() {
//   -------------
  get5Centers();
  getBorders();
} // getPartitions
//=============================== start three ===============================

void put3Combo(int a, int b, int c) {
//   ---------
  int *p = threeCombos[num3Combos++];

  p[0] = a; p[1] = b; p[2] = c;
} // put3Combo
//---------------------------------------------------------------------------

void get3Combos(int partition5) {
//   ----------
  num3Combos = 0;

  int *p = Centers[partition5-1];
  for (int i = 0; i < (LL-3); i++)
    for (int j = i+1; j < (LL-2); j++)
      for (int k = j+1; k < (LL-1); k++)
        if ( (p[i] + p[j] + p[k] == Sum3) &&
           ((p[i] + p[j]) != Sum2) && ((p[i] + p[k]) != Sum2) &&
           ((p[j] + p[k]) != Sum2) ) put3Combo(p[i], p[j], p[k]);
} // get3Combos
//---------------------------------------------------------------------------

void put3UPermu(int a, int b, int c) {
//   ----------
  int *p = threeUPermus[num3UPermus++];
  p[0] = a; p[1] = b; p[2] = c;

  if (c < Half) threeUPermusLH[num3UPermusLH++] = p;
} // put3UPermu
//----------------------------------------------------------------------------

void get3UPermus() {
//   -----------
  num3UPermus = 0; num3UPermusLH = 0;

  for (int i = 0; i < num3Combos; i++) {
    int *p = threeCombos[i];

    put3UPermu(p[0], p[2], p[1]);
    put3UPermu(p[0], p[1], p[2]);

    put3UPermu(p[1], p[0], p[2]);
  }
} // get3UPermus 
//---------------------------------------------------------------------------

void get3UPermus13() {
//   -------------
  for (int i = 0; i < NN; i++)
    for (int j = 0; j < NN; j++)
      threeUPermus13[i][j] = -1;

  for (int i = 0; i < num3UPermus; i++) {
    int *p = threeUPermus[i];

    threeUPermus13[p[0]][p[2]] = p[1];
  }
} // get3UPermus13
//=============================== start five ===============================

void put5Combo(int a, int b, int c, int d, int e) {
//   ---------
  int *p = fiveCombos[num5Combos++];

  p[0] = a;  p[1] = b;  p[2] = c;  p[3] = d;  p[4] = e;
} // put5Combo
//--------------------------------------------------------------------------

void get5Combos(int partition5) {
//   ----------
  num5Combos = 0; struct bools v = allfree;

  int *p = fiveBorders[partition5-1];
  for (int i = 0; i < (MM-LL-4); i++) {
    v.used[Sum2-p[i]] = true;
    for (int j = i+1; j < (MM-LL-3); j++) if (!v.used[p[j]]) {
      v.used[Sum2-p[j]] = true;
      for (int k = j+1; k < (MM-LL-2); k++) if (!v.used[p[k]]) {
        v.used[Sum2-p[k]] = true;
	      for (int l = k+1; l < (MM-LL-1); l++) if (!v.used[p[l]]) {
          v.used[Sum2-p[l]] = true;
	        for (int m = l+1; m < (MM-LL); m++) if (!v.used[p[m]]) { 
	          if ( ((p[i] + p[j] + p[k] + p[l] + p[m]) == Sum5) )
              put5Combo(p[i], p[j], p[k], p[l], p[m]);
          }
          v.used[Sum2-p[l]] = false;
        }
        v.used[Sum2-p[k]] = false;
      }
      v.used[Sum2-p[j]] = false;
    }
    v.used[Sum2-p[i]] = false;
  }
} // get5Combos
//-----------------------------------------------------------------------

void put5UPermu(int a, int b, int c, int d, int e) {
//   ----------
  int *p;

  p = fiveUPermus[num5UPermus++];
  p[0] = a; p[1] = b; p[2] = c; p[3] = d; p[4] = e;

  if (e < Half) {
    fiveUPermusLH[num5UPermusLH] = p;
    p = fiveUPermusGH[num5UPermusLH++];
    p[0] = Sum2-a; p[1] = Sum2-b; p[2] = Sum2-c; p[3] = Sum2-d; p[4] = Sum2-e;
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
  for (int i = 0; i < NN; i++)
    for (int j = 0; j < NN; j++)
      num5UPermus15[i][j] = 0;

  for (int i = 0; i < num5UPermus; i++) {
    int *p = fiveUPermus[i];

    fiveUPermus15[p[0]][p[4]][(num5UPermus15[p[0]][p[4]])++] = p;
  }
} // get5UPermus15 
//=============================== start seven ==============================

void put7Combo(int a, int b, int c, int d, int e, int f, int g) {
//   ---------
  int *p = sevenCombos[num7Combos++];

  p[0] = a; p[1] = b; p[2] = c; p[3] = d; p[4] = e; p[5] = f; p[6] = g;
} // put7Combo
//--------------------------------------------------------------------------

void get7Combos(int partition) {
//   ----------
  num7Combos = 0;  struct bools v = allfree;

  int *p = sevenBorders[partition-1];
  for (int i = 0; i < (NN-MM-6); i++) {
    v.used[Sum2-p[i]] = true;
    for (int j = i+1; j < (NN-MM-5); j++) if (!v.used[p[j]]) {
      v.used[Sum2-p[j]] = true;
      for (int k = j+1; k < (NN-MM-4); k++) if (!v.used[p[k]]) {
        v.used[Sum2-p[k]] = true;
        for (int l = k+1; l < (NN-MM-3); l++) if (!v.used[p[l]]) {
          v.used[Sum2-p[l]] = true;
          for (int m = l+1; m < (NN-MM-2); m++) if (!v.used[p[m]]) {
            v.used[Sum2-p[m]] = true;
            for (int n = m+1; n < (NN-MM-1); n++) if (!v.used[p[n]]) {
              v.used[Sum2-p[n]] = true;
	            for (int o = n+1; o < (NN-MM); o++) if (!v.used[p[o]]) {
                if ( ((p[i] + p[j] + p[k] + p[l] + p[m] + p[n] + p[o]) == MSum) )
                  put7Combo(p[i], p[j], p[k], p[l], p[m], p[n], p[o]);
              }
              v.used[Sum2-p[n]] = false;
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
} // get7Combos
//-----------------------------------------------------------------------

void put7UPermu(int a, int b, int c, int d, int e, int f, int g) {
//   ----------
  int *p;

  p = sevenUPermus[num7UPermus++];
  p[0] = a; p[1] = b; p[2] = c; p[3] = d; p[4] = e; p[5] = f; p[6] = g;

  if (g < Half) {
    sevenUPermusLH[num7UPermusLH] = p;
    p = sevenUPermusGH[num7UPermusLH++];
    p[0] = Sum2-a; p[1] = Sum2-b; p[2] = Sum2-c; p[3] = Sum2-d;
    p[4] = Sum2-e; p[5] = Sum2-f; p[6] = Sum2-g;
  }
} // put7UPermu
//--------------------------------------------------------------------------

void get7UPermus() {
//   -----------
  num7UPermus = 0; num7UPermusLH = 0;

  for (int i = 0; i < num7Combos; i++) {
    int *p = sevenCombos[i];
    int p0 = p[0];  int p1 = p[1];  int p2 = p[2]; 
    int p3 = p[3];  int p4 = p[4];  int p5 = p[5];  int p6 = p[6];

    put7UPermu(p0, p2, p3, p4, p5, p6, p1);
    put7UPermu(p0, p1, p3, p4, p5, p6, p2);
    put7UPermu(p0, p1, p2, p4, p5, p6, p3);
    put7UPermu(p0, p1, p2, p3, p5, p6, p4);
    put7UPermu(p0, p1, p2, p3, p4, p6, p5);
    put7UPermu(p0, p1, p2, p3, p4, p5, p6);

    put7UPermu(p1, p0, p3, p4, p5, p6, p2);
    put7UPermu(p1, p0, p2, p4, p5, p6, p3);
    put7UPermu(p1, p0, p2, p3, p5, p6, p4);
    put7UPermu(p1, p0, p2, p3, p4, p6, p5);
    put7UPermu(p1, p0, p2, p3, p4, p5, p6);

    put7UPermu(p2, p0, p1, p4, p5, p6, p3);
    put7UPermu(p2, p0, p1, p3, p5, p6, p4);
    put7UPermu(p2, p0, p1, p3, p4, p6, p5);
    put7UPermu(p2, p0, p1, p3, p4, p5, p6);

    put7UPermu(p3, p0, p1, p2, p5, p6, p4);
    put7UPermu(p3, p0, p1, p2, p4, p6, p5);
    put7UPermu(p3, p0, p1, p2, p4, p5, p6);

    put7UPermu(p4, p0, p1, p2, p3, p6, p5);
    put7UPermu(p4, p0, p1, p2, p3, p5, p6);

    put7UPermu(p5, p0, p1, p2, p3, p4, p6);
  }
} // get7UPermus 
//--------------------------------------------------------------------------

void get7UPermus17() {
//   -------------
  for (int i = 0; i < NN; i++)
    for (int j = 0; j < NN; j++)
      num7UPermus17[i][j] = 0;

  for (int i = 0; i < num7UPermus; i++) {
    int *p = sevenUPermus[i];

    sevenUPermus17[p[0]][p[6]][(num7UPermus17[p[0]][p[6]])++] = p;
  }
} // get7UPermus17 
//--------------------------------------------------------------------------

int countCenter3Squares() {
//  -------------------
  //int count = 0;

  for (int i0 = 0; i0 < num3UPermusLH; i0++) {
    int *r1 = threeUPermusLH[i0];                    // first row 
    int c12 = threeUPermus13[r1[0]][Sum2-r1[2]];

    if ( (c12 >= 0) && (c12 != r1[1]) && (c12 != (Sum2-r1[1])) )
      return 1; //++count;
  }
  return 0;
} // countCenter3Squares
//--------------------------------------------------------------------------

int countOrder5Borders() {
//  ------------------
  int count = 0; bools v = allfree;

  for (int i0 = 0; i0 < num5UPermusLH; i0++) {
    int *r1 = fiveUPermusLH[i0];                    // first row
    int *R1 = fiveUPermusGH[i0];
    v.used[r1[1]] = true; v.used[r1[2]] = true; v.used[r1[3]] = true;
    v.used[R1[1]] = true; v.used[R1[2]] = true; v.used[R1[3]] = true;

    int i1Max = num5UPermus15[r1[0]][R1[4]];
    if (i1Max != 0) {
      int **p1  = fiveUPermus15[r1[0]][R1[4]];

      for (int i1 = 0; i1 < i1Max; i1++) {
        int *c1 = *(p1+i1);                          // first column
        if (!(v.used[c1[1]] || v.used[c1[2]] || v.used[c1[3]]))
          ++count;
      }
    }
    v.used[r1[1]] = false; v.used[r1[2]] = false; v.used[r1[3]] = false;
    v.used[R1[1]] = false; v.used[R1[2]] = false; v.used[R1[3]] = false;
  }
  return count;
} // countOrder5Borders 
//-------------------------------------------------------------------------------------

int countOrder7Borders() {
//  ------------------
  int count = 0; bools v = allfree;

  for (int i0 = 0; i0 < num7UPermusLH; i0++) {
    int *r1 = sevenUPermusLH[i0];                    // first row
    int *R1 = sevenUPermusGH[i0];
    v.used[r1[1]] = true; v.used[r1[2]] = true;
    v.used[r1[3]] = true; v.used[r1[4]] = true; v.used[r1[5]] = true;
    v.used[R1[1]] = true; v.used[R1[2]] = true;
    v.used[R1[3]] = true; v.used[R1[4]] = true; v.used[R1[5]] = true;

    int i1Max = num7UPermus17[r1[0]][R1[6]];
    if (i1Max != 0) {
      int **p1  = sevenUPermus17[r1[0]][R1[6]];

      for (int i1 = 0; i1 < i1Max; i1++) {
        int *c1 = *(p1+i1);                          // first column
        if (!(v.used[c1[1]] || v.used[c1[2]] || v.used[c1[3]] ||
            v.used[c1[4]] || v.used[c1[5]]))
          ++count;
      }
    }
    v.used[r1[1]] = false; v.used[r1[2]] = false;
    v.used[r1[3]] = false; v.used[r1[4]] = false; v.used[r1[5]] = false;
    v.used[R1[1]] = false; v.used[R1[2]] = false;
    v.used[R1[3]] = false; v.used[R1[4]] = false; v.used[R1[5]] = false;
  }
  return count;
} // countOrder7Borders 
//-------------------------------------------------------------------------------------

/*
int printCount;

int Max3Combos, Max3UPermus, Max3UPermusLH;
int Max5Combos, Max5UPermus, Max5UPermusLH, Max5UPermus15;
int Max7Combos, Max7UPermus, Max7UPermusLH, Max7UPermus17;
int zeroCombo3, zeroCount3, zeroCombo5, zeroCount5, zeroCombo7, zeroCount7;

void get3Maxs() {
//   --------
  if (num3Combos > Max3Combos) Max3Combos = num3Combos;
  if (num3UPermus > Max3UPermus) Max3UPermus = num3UPermus;
  if (num3UPermusLH > Max3UPermusLH) Max3UPermusLH = num3UPermusLH;
} // get3Maxs

void get5Maxs() {
//   --------
  if (num5Combos > Max5Combos) Max5Combos = num5Combos;
  if (num5UPermus > Max5UPermus) Max5UPermus = num5UPermus;
  if (num5UPermusLH > Max5UPermusLH) Max5UPermusLH = num5UPermusLH;
  for (int a = 0; a < NN; ++a)
    for (int b = 0; b < NN; ++b)
      if (num5UPermus15[a][b] > Max5UPermus15)
        Max5UPermus15 = num5UPermus15[a][b];
} // get5Maxs

void get7Maxs() {
//   --------
  if (num7Combos > Max7Combos) Max7Combos = num7Combos;
  if (num7UPermus > Max7UPermus) Max7UPermus = num7UPermus;
  if (num7UPermusLH > Max7UPermusLH) Max7UPermusLH = num7UPermusLH;
  for (int a = 0; a < NN; ++a)
    for (int b = 0; b < NN; ++b)
      if (num7UPermus17[a][b] > Max7UPermus17)
        Max7UPermus17 = num7UPermus17[a][b];
} // get7Maxs

void printMaxs(FILE *wfp) {
//   ---------
  fprintf(wfp,"Max3Combos %d Max3UPermus %d Max3UPermusLH %d\n",
          Max3Combos, Max3UPermus, Max3UPermusLH);
  fprintf(wfp,"Max5Combos %d Max5UPermus %d Max5UPermusLH %d Max5UPermus15 %d\n",
          Max5Combos, Max5UPermus, Max5UPermusLH, Max5UPermus15);
  fprintf(wfp,"Max7Combos %d Max7UPermus %d Max7UPermusLH %d Max7UPermus17 %d\n",
          Max7Combos, Max7UPermus, Max7UPermusLH, Max7UPermus17);
} // printMaxs

void printPartition(int partition, FILE *wfp) {
//   --------------
  int *p = fiveCenters[partition-1];
  fprintf(wfp, "5Center:");
  for (int a = 0; a < (MM-1); ++a) fprintf(wfp, " %2d", p[a]);
  fprintf(wfp, "\n");

  p = sevenBorders[partition-1];
  fprintf(wfp, "7Border:");
  for (int a = 0; a < (NN-MM); ++a) fprintf(wfp, " %2d", p[a]);
  fprintf(wfp, "\n\n");
} // printPartition

void printPartition5(int partition5, FILE *wfp) {
//   ---------------
  int *p = Centers[partition5-1];
  fprintf(wfp, "Center:");
  for (int a = 0; a < (LL-1); ++a) fprintf(wfp, " %2d", p[a]);
  fprintf(wfp, "\n");

  p = fiveBorders[partition5-1];
  fprintf(wfp, "Border:");
  for (int a = 0; a < (MM-LL); ++a) fprintf(wfp, " %2d", p[a]);
  fprintf(wfp, "\n\n");
} // printPartition5

void print3Combos(FILE *wfp) {
//   ------------
  for (int a = 0; a < num3Combos; ++a) {
    int *p = threeCombos[a];
    fprintf(wfp, "%2d %2d %2d\n", p[0], p[1], p[2]);
  }
  fprintf(wfp, "\n");
} // print3Combos

void print5Combos(FILE *wfp) {
//   ------------
  for (int a = 0; a < num5Combos; ++a) {
    int *p = fiveCombos[a];
    fprintf(wfp, "%2d %2d %2d %2d %2d\n", p[0], p[1], p[2], p[3], p[4]);
  }
  fprintf(wfp, "\n");
} // print5Combos

void print7Combos(FILE *wfp) {
//   ------------
  for (int a = 0; a < num7Combos; ++a) {
    int *p = sevenCombos[a];
    fprintf(wfp, "%2d %2d %2d %2d %2d %2d %2d\n",
                 p[0], p[1], p[2], p[3], p[4], p[5], p[6]);
  }
  fprintf(wfp, "\n");
} // print7Combos
*/

//--------------------------------------------------------------------------
typedef unsigned int Uint;
const Uint oneB = 1000000000, oneM = 1000000, oneT = 1000;

char *format_number(char *buf, int bufSize, Uint bb, Uint b, Uint c) {
//    -------------
  const int nbufSize = 20;
  char nbuf[nbufSize];

  if (bb == 0) {
    sprintf_s(buf, bufSize, "%u", b);
  } else {
    sprintf_s(buf, bufSize, "%u", bb);
    sprintf_s(nbuf, nbufSize, ",%03u,%03u,%03u", b/oneM, (b%oneM)/oneT, b%oneT);
    strcat_s(buf, bufSize, nbuf);
  }
  sprintf_s(nbuf, nbufSize, ",%03u,%03u,%03u", c/oneM, (c%oneM)/oneT, c%oneT);
  strcat_s(buf, bufSize, nbuf);
  return buf;
} // format_number

//void printResult(FILE *wfp, Uint totalB, Uint total,
////   -----------
//                            Uint TotalBB, Uint TotalB, Uint Total) {
void printResult(Uint TotalBB, Uint TotalB, Uint Total) {
//   -----------
  const int bSize = 30; char buf[bSize];
  
  format_number(buf, bSize, TotalBB, TotalB, Total);
  printf("\nNumber of squares is %s.\n", buf);
  //fprintf(wfp, "\nNumber of squares is %s.\n", buf);

  //format_number(buf, bSize, 0, totalB, total);
  //printf("(%s before rotations or permutations)\n", buf);
  //fprintf(wfp, "(%s before rotations or permutations)\n", buf);

} // printResult
//--------------------------------------------------------------------------

const Uint rotations = 8*8, perms5 = 6*6, perms7 = 120*120;
//void countSquares(FILE *wfp, bool consecutive) {
void countSquares(bool consecutive) {
//   ------------
  //Uint total = 0, totalB = 0; // not including permutations or rotations
  Uint Total = 0, TotalB = 0, TotalBB = 0; // including permutations and rotations

  getPartitions();  // 2,704,156
  int start5, start7;
  if (consecutive) {
    start5 = num5Partitions; start7 = num7Partitions;
  } else {
    start5 = start7 = 1;
  }
  //for (int partition = 873886; partition <= 873886; partition++) { // no 7combo
  for (int partition = start7; partition <= num7Partitions; partition++) {
    //printPartition(partition, wfp);
    //Uint count = 0; // not including permutations or rotations
    //Uint Count = 0, CountB = 0; // including permutations and rotations
    Uint count7; bool get7 = true;
    
    get5Partitions(partition); //495         (495 * 2,704,156 = 1,338,557,220)
    for (int partition5 = start5; partition5 <= num5Partitions; partition5++) {
      //printPartition5(partition5, wfp);
      get3Combos(partition5);
      if (num3Combos == 0) continue; //{ ++zeroCombo3; continue; } // print3Combos(wfp);
      get3UPermus(); get3UPermus13();   // get3Maxs();     
      int count3 = countCenter3Squares();
      if (count3 == 0) continue; //{ ++zeroCount3; continue; }
      get5Combos(partition5);
      if (num5Combos == 0) continue; //{ ++zeroCombo5; continue; } // print5Combos(wfp);
      get5UPermus(); get5UPermus15();   // get5Maxs();
      Uint count5 = countOrder5Borders();
      if (count5 == 0) continue; //{ ++zeroCount5; continue; }
      if (get7) {
        get7Combos(partition);
        // if (num7Combos == 0) break; //1 occurrence at partition 873,886
        //{ ++zeroCombo7; break; } // print7Combos(wfp);
        get7UPermus(); get7UPermus17(); // get7Maxs();
        count7 = countOrder7Borders();
        // if (count7 == 0) break; //{ ++zeroCount7; break; };
        get7 = false;
      }
      //Uint add = count5 * count7; total += add; // count += add;
      //if (total >= oneB) { ++totalB; total -= oneB; }

      Uint Count5 = count5*perms5, Count7 = count7*perms7;
      Uint OFlimit = (ULONG_MAX - oneB) / Count5;
      int loops = 1; 
      while (Count7 > OFlimit) {
        //assert((Count7 % 2) == 0);
        Count7 /= 2; loops += loops;
      }
      Uint add = Count5 * Count7; loops *= rotations;
      while (loops--) {
        //Count += add;
        //while (Count  >= oneB) { ++CountB;  Count -= oneB; }
        Total += add;
        while (Total  >= oneB) { ++TotalB;  Total  -= oneB; }
        while (TotalB >= oneB) { ++TotalBB; TotalB -= oneB; }
      }
    }
/*
    fprintf(wfp, "\nP %7d: count %9u\n", partition, count);
    fprintf(wfp, "           Count        %9u,%09u\n", CountB, Count);
    if (++printCount == 50000) {
      printf("P %7d: total %u,%09u,%09u\n", partition, TotalBB, TotalB, Total);
      fflush(wfp);
      printCount = 0;
    }
*/
  } 
  // printMaxs(wfp);
  //printResult(NULL, totalB, total, TotalBB, TotalB, Total);
  printResult(TotalBB, TotalB, Total);
/*
  printf("zeroCombo3 %d zeroCount3 %d zeroCombo5 %d zeroCount5 %d"
         " zeroCombo7 %d zeroCount7 %d\n",
    zeroCombo3, zeroCount3, zeroCombo5, zeroCount5, zeroCombo7, zeroCount7);
  fprintf(wfp, "zeroCombo3 %d zeroCount3 %d zeroCombo5 %d zeroCount5 %d"
               " zeroCombo7 %d zeroCount7 %d\n",
    zeroCombo3, zeroCount3, zeroCombo5, zeroCount5, zeroCombo7, zeroCount7);
*/
} // countSquares
//---------------------------------------------------------------------------

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

//FILE *openFile() {
////    --------
//  const int bufSize = 128;
//  char buf[bufSize];
//  const char *baseName = "C:\\Order7ConcentricMagicSquares";
//  sprintf_s(buf, bufSize, "%s.txt", baseName);
//  int sub = 0;
//
//  do {
//    if (_access_s(buf, 00) == ENOENT) {
//      break;
//    } else {
//      sprintf_s(buf, bufSize, "%s_%d.txt", baseName, ++sub);
//    }
//  } while (true);
//
//  FILE *wfp = NULL;
//  if (fopen_s(&wfp, buf, "w") == 0)
//    printf("Output file is %s\n", buf);
//  else {
//    char msg[bufSize];
//    sprintf_s(msg, bufSize, "\a\nCan't open for write %s", buf);
//    perror(msg);
//  }  
//  return wfp;
//} // openFile
//--------------------------------------------------------------------------

void outputLocalTime(time_t *startTime) {
//   ---------------
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

  printf("\nPress a key to close the console");
  //while (!Console::KeyAvailable) Thread::Sleep(250); // CLR
  while (!_kbhit()) Sleep(250);
  return 0;
} // _tmain