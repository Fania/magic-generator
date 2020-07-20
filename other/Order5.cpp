/*
* File:    Order5.cpp
* Author:  S Harry White
* Created: 2009-07-09
* Updated: 2018-07-20
 */

/*
    Monday 2016-09-19 17:01:15 Newfoundland Daylight Time

    Output files are in folder Order5

    Center  1,25 number of squares   4365792, elapsed time  0:13
    Center  2,24 number of squares   5464716, elapsed time  0:15
    Center  3,23 number of squares   7659936, elapsed time  0:19
    Center  4,22 number of squares   7835348, elapsed time  0:19
    Center  5,21 number of squares   9727224, elapsed time  0:22
    Center  6,20 number of squares  10403516, elapsed time  0:24
    Center  7,19 number of squares  12067524, elapsed time  0:25
    Center  8,18 number of squares  12448644, elapsed time  0:27
    Center  9,17 number of squares  13890160, elapsed time  0:29
    Center 10,16 number of squares  13376136, elapsed time  0:29
    Center 11,15 number of squares  15735272, elapsed time  0:31
    Center 12,14 number of squares  15138472, elapsed time  0:31
    Center 13    number of squares  19079744, elapsed time  0:31

           total number of squares 275305224, elapsed time  5:15
 */

#include "stdafx.h"
#include <conio.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

const int N            =5,
          NN           =N*N,          // 25

// zero based
          Mid          =(NN-1) / 2,   // 12
          MagicConstant=Mid*N;        // 60

// Combinations of N chosen from 0 .. NN-1 that total to MagicConstant.

// Combinations that contain K where K is one of 0 .. Mid.
const int maxKCombos=302;
int KCombos[maxKCombos][N+1], numKCombos;

// Permutations of KCombos with K omitted.
const int maxCenterKPermus=7248;
int CenterKPermus[maxCenterKPermus][N], numCenterKPermus;

// CenterKPermus in which the third number is greater than the second.
const int maxCenterKUPermus=3624;
int *CenterKUPermus[maxCenterKUPermus], sizeCenterKUPermus;

struct spu { int *p; int u; };

const int maxCenterKPermusIJ=22;
// CenterKPermus indexed by the first and fourth.
spu CenterKPermus14[NN][NN][maxCenterKPermusIJ];
int sizeCenterKPermus14[NN][NN];

// CenterKPermus indexed by the second and third.
spu CenterKPermus23[NN][NN][maxCenterKPermusIJ];
int sizeCenterKPermus23[NN][NN];
//-------------------------------------------------------------------------------

// Combinations that do not contain K.
const int maxOKCombos=1150;
int OKCombos[maxOKCombos][N], numOKCombos;

// Permutations of OKCombos.
const int maxOKPermus=138000;  // 1150 x 5!
int OKPermus[maxOKPermus][N], numOKPermus;

const int maxOKPermusIJ=330;
// OKPermus in which the fourth is greater than the second,
// indexed by the second and fourth.
spu OKUPermus24[NN][NN][maxOKPermusIJ];
int sizeOKUPermus24[NN][NN];

// OKPermus indexed by the first, second and fifth.
const int maxOKPermusIJK=20;
spu OKPermus125[NN][NN][NN][maxOKPermusIJK];
int sizeOKPermus125[NN][NN][NN];

// OKPermus in which the fourth is greater than the second,
// indexed by the second, third and fourth.
spu OKUPermus234[NN][NN][NN][maxOKPermusIJK];
int sizeOKUPermus234[NN][NN][NN],

// Fourth of OKPermus indexed by the first, second, third and fifth.
    OKPermus1235[NN][NN][NN][NN],
// Third of OKPermus indexed by the first, second, fourth and fifth.
    OKPermus1245[NN][NN][NN][NN];
//============================================================================================


void putCombo(int K, int a, int b, int c, int d, int e) {
//   --------
  int *p;
  if ((a==K)||(b==K)||(c==K)||(d==K)||(e==K)) {
    p=KCombos[numKCombos++]; p[5]=((1<<a)|(1<<b)|(1<<c)|(1<<d)|(1<<e))&~(1<<K);
  } else p=OKCombos[numOKCombos++];
  p[0]=a; p[1]=b; p[2]=c; p[3]=d; p[4]=e;
} // putKCombo

void getCombos(int K) {
//   ---------
  numKCombos=0; numOKCombos=0;
  for (int i=0; i<(NN-4); i++)
    for (int j=i+1; j<(NN-3); j++)
      for (int k=j+1; k<(NN-2); k++)
        for (int l=k+1; l<(NN-1); l++)
          for (int m=l+1; m<NN; m++)
            if ((i+j+k+l+m)==MagicConstant) putCombo(K, i, j, k, l, m);
} // getCombos

void permuteCenterK_3(int K, int a, int b, int c, int d, int e, int u) {
//   ----------------
  if (c==K) {
    int *p=CenterKPermus[numCenterKPermus++]; p[0]=a; p[1]=b; p[2]=d; p[3]=e; spu *q;
    q=&(CenterKPermus14[a][e][(sizeCenterKPermus14[a][e])++]); q->p=p; q->u=(1<<b)|(1<<d);
    q=&(CenterKPermus23[b][d][(sizeCenterKPermus23[b][d])++]); q->p=p; q->u=(1<<a)|(1<<e);
    if (d>b) { CenterKUPermus[sizeCenterKUPermus++]=p; p[4]=u; }

    p=CenterKPermus[numCenterKPermus++]; p[0]=a; p[1]=b; p[2]=e; p[3]=d;
    q=&(CenterKPermus14[a][d][(sizeCenterKPermus14[a][d])++]); q->p=p; q->u=(1<<b)|(1<<e);
    q=&(CenterKPermus23[b][e][(sizeCenterKPermus23[b][e])++]); q->p=p; q->u=(1<<a)|(1<<d);
    if (e>b) { CenterKUPermus[sizeCenterKUPermus++]=p; p[4]=u; }
  } else if (d==K) {
    int *p=CenterKPermus[numCenterKPermus++]; p[0]=a; p[1]=b; p[2]=c; p[3]=e; spu *q;
    q=&(CenterKPermus14[a][e][(sizeCenterKPermus14[a][e])++]); q->p=p; q->u=(1<<b)|(1<<c);
    q=&(CenterKPermus23[b][c][(sizeCenterKPermus23[b][c])++]); q->p=p; q->u=(1<<a)|(1<<e);
    if (c>b) { CenterKUPermus[sizeCenterKUPermus++]=p; p[4]=u; }

    p=CenterKPermus[numCenterKPermus++]; p[0]=a; p[1]=b; p[2]=e; p[3]=c;
    q=&(CenterKPermus14[a][c][(sizeCenterKPermus14[a][c])++]); q->p=p; q->u=(1<<b)|(1<<e);
    q=&(CenterKPermus23[b][e][(sizeCenterKPermus23[b][e])++]); q->p=p; q->u=(1<<a)|(1<<c);
    if (e>b) { CenterKUPermus[sizeCenterKUPermus++]=p; p[4]=u; }
  } else if (e==K) {
    int *p=CenterKPermus[numCenterKPermus++]; p[0]=a; p[1]=b; p[2]=c; p[3]=d; spu *q;
    q=&(CenterKPermus14[a][d][(sizeCenterKPermus14[a][d])++]); q->p=p; q->u=(1<<b)|(1<<c);
    q=&(CenterKPermus23[b][c][(sizeCenterKPermus23[b][c])++]); q->p=p; q->u=(1<<a)|(1<<d);
    if (c>b) { CenterKUPermus[sizeCenterKUPermus++]=p; p[4]=u; }

    p=CenterKPermus[numCenterKPermus++]; p[0]=a; p[1]=b; p[2]=d; p[3]=c;
    q=&(CenterKPermus14[a][c][(sizeCenterKPermus14[a][c])++]); q->p=p; q->u=(1<<b)|(1<<d);
    q=&(CenterKPermus23[b][d][(sizeCenterKPermus23[b][d])++]); q->p=p; q->u=(1<<a)|(1<<c);
    if (d>b) { CenterKUPermus[sizeCenterKUPermus++]=p; p[4]=u; }
  }
} // permuteCenterK_

void permuteK_4(int K, int a, int b, int c, int d, int e, int u) {
//   ----------
  permuteCenterK_3(K, a, b, c, d, e, u); permuteCenterK_3(K, a, c, b, d, e, u);
  permuteCenterK_3(K, a, d, c, b, e, u); permuteCenterK_3(K, a, e, c, d, b, u);
} // permuteK_4

void getCenterKPermus(int K) {
//   ----------------
  numCenterKPermus=0; sizeCenterKUPermus=0;
  for (int i=0; i<NN; i++) for (int j=0; j<NN; j++) {
    sizeCenterKPermus14[i][j]=0; sizeCenterKPermus23[i][j]=0;
  }

  for (int i=0; i<numKCombos; i++) {
    int *p=KCombos[i], a=p[0], b=p[1], c=p[2], d=p[3], e=p[4], u=p[5];
    permuteK_4(K, a, b, c, d, e, u); permuteK_4(K, b, a, c, d, e, u); permuteK_4(K, c, b, a, d, e, u);
    permuteK_4(K, d, b, c, a, e, u); permuteK_4(K, e, b, c, d, a, u);
  }
} // getCenterKPermus
//----------------------------------------------------------------------------------------------------

void permuteOK_3(int a, int b, int c, int d, int e) {
//   -----------
  int *p, t; spu *q;

  p=OKPermus[numOKPermus++]; p[0]=a; p[1]=b; p[2]=c; p[3]=d; p[4]=e;
  q=&(OKPermus125[a][b][e][(sizeOKPermus125[a][b][e])++]); q->p=p; q->u=(1<<c)|(1<<d);
  OKPermus1235[a][b][c][e]=d; OKPermus1245[a][b][d][e]=c;
  if (d>b) {
    q=&(OKUPermus234[b][c][d][(sizeOKUPermus234[b][c][d])++]); q->p=p; q->u=t=(1<<a)|(1<<e);
    q=&(OKUPermus24[b][d][(sizeOKUPermus24[b][d])++]); q->p=p; q->u=t|(1<<c);
  }

  p=OKPermus[numOKPermus++]; p[0]=a; p[1]=b; p[2]=c; p[3]=e; p[4]=d;
  q=&(OKPermus125[a][b][d][(sizeOKPermus125[a][b][d])++]); q->p=p; q->u=(1<<c)|(1<<e);
  OKPermus1235[a][b][c][d]=e; OKPermus1245[a][b][e][d]=c;
  if (e>b) {
    q=&(OKUPermus234[b][c][e][(sizeOKUPermus234[b][c][e])++]); q->p=p; q->u=t=(1<<a)|(1<<d);
    q=&(OKUPermus24[b][e][(sizeOKUPermus24[b][e])++]); q->p=p; q->u=t|(1<<c);
  }

  p=OKPermus[numOKPermus++]; p[0]=a; p[1]=b; p[2]=d; p[3]=c; p[4]=e;
  q=&(OKPermus125[a][b][e][(sizeOKPermus125[a][b][e])++]); q->p=p; q->u=(1<<c)|(1<<d);
  OKPermus1235[a][b][d][e]=c; OKPermus1245[a][b][c][e]=d;
  if (c>b) {
    q=&(OKUPermus234[b][d][c][(sizeOKUPermus234[b][d][c])++]); q->p=p; q->u=t=(1<<a)|(1<<e);
    q=&(OKUPermus24[b][c][(sizeOKUPermus24[b][c])++]); q->p=p; q->u=t|(1<<d);
  }

  p=OKPermus[numOKPermus++]; p[0]=a; p[1]=b; p[2]=d; p[3]=e; p[4]=c;
  q=&(OKPermus125[a][b][c][(sizeOKPermus125[a][b][c])++]); q->p=p; q->u=(1<<d)|(1<<e);
  OKPermus1235[a][b][d][c]=e; OKPermus1245[a][b][e][c]=d;
  if (e>b) {
    q=&(OKUPermus234[b][d][e][(sizeOKUPermus234[b][d][e])++]); q->p=p; q->u=t=(1<<a)|(1<<c);
    q=&(OKUPermus24[b][e][(sizeOKUPermus24[b][e])++]); q->p=p; q->u=t|(1<<d);
  }

  p=OKPermus[numOKPermus++]; p[0]=a; p[1]=b; p[2]=e; p[3]=c; p[4]=d;
  q=&(OKPermus125[a][b][d][(sizeOKPermus125[a][b][d])++]); q->p=p; q->u=(1<<c)|(1<<e);
  OKPermus1235[a][b][e][d]=c; OKPermus1245[a][b][c][d]=e;
  if (c>b) {
    q=&(OKUPermus234[b][e][c][(sizeOKUPermus234[b][e][c])++]); q->p=p; q->u=t=(1<<a)|(1<<d);
    q=&(OKUPermus24[b][c][(sizeOKUPermus24[b][c])++]); q->p=p; q->u=t|(1<<e);
  }

  p=OKPermus[numOKPermus++]; p[0]=a; p[1]=b; p[2]=e; p[3]=d; p[4]=c;
  q=&(OKPermus125[a][b][c][(sizeOKPermus125[a][b][c])++]); q->p=p; q->u=(1<<d)|(1<<e);
  OKPermus1235[a][b][e][c]=d; OKPermus1245[a][b][d][c]=e;
  if (d>b) {
    q=&(OKUPermus234[b][e][d][(sizeOKUPermus234[b][e][d])++]); q->p=p; q->u=t=(1<<a)|(1<<c);
    q=&(OKUPermus24[b][d][(sizeOKUPermus24[b][d])++]); q->p=p; q->u=t|(1<<e);
  }
} // permuteOK_3

void permuteOK_4(int a, int b, int c, int d, int e) {
//   -----------
  permuteOK_3(a, b, c, d, e); permuteOK_3(a, c, b, d, e);
  permuteOK_3(a, d, c, b, e); permuteOK_3(a, e, c, d, b);
} // permuteOK_4

void getOKPermus() {
//   -----------
  numOKPermus=0;
  for (int i=0; i<NN; i++) for (int j=0; j<NN; j++) {
    sizeOKUPermus24[i][j]=0;
    for (int k=0; k<NN; k++) {
      sizeOKPermus125[i][j][k]=0; sizeOKUPermus234[i][j][k]=0;
      for (int l=0; l<NN; l++) {
        OKPermus1235[i][j][k][l]=-1; OKPermus1245[i][j][k][l]=-1;
      }
    }
  }

  for (int i=0; i<numOKCombos; i++) {
    int *p=OKCombos[i], a=p[0], b=p[1], c=p[2], d=p[3], e=p[4];
    permuteOK_4(a, b, c, d, e); permuteOK_4(b, a, c, d, e); permuteOK_4(c, b, a, d, e);
    permuteOK_4(d, b, c, a, e); permuteOK_4(e, b, c, d, a);
  }
} // getOKPermus 
//----------------------------------------------------------------------------------------

const int squareBytes=76, numSquares=1048576;
const int outSize=numSquares*squareBytes;
char outputBuffer1[outSize], outputBuffer2[outSize];
char *outPointer1=NULL, *outPointer2=NULL; int outSquares;
void writeSquares (
//   ------------
   const int a00, const int a01, const int a02, const int a03, const int a04,
   const int a10, const int a11, const int a12, const int a13, const int a14,
   const int a20, const int a21, const int a22, const int a23, const int a24,
   const int a30, const int a31, const int a32, const int a33, const int a34,
   const int a40, const int a41, const int a42, const int a43, const int a44)
{
  const int t0='1', t1='0'-9, t2='0'-19; char *s=outPointer1;

  // row 1
  if (a00<9)       { *s++=' '; *s++=t0+a00; }
  else if (a00<19) { *s++='1'; *s++=t1+a00; }
  else             { *s++='2'; *s++=t2+a00; } *s++=' ';
  if (a01<9)       { *s++=' '; *s++=t0+a01; }
  else if (a01<19) { *s++='1'; *s++=t1+a01; }
  else             { *s++='2'; *s++=t2+a01; } *s++=' ';
  if (a02<9)       { *s++=' '; *s++=t0+a02; }
  else if (a02<19) { *s++='1'; *s++=t1+a02; }
  else             { *s++='2'; *s++=t2+a02; } *s++=' ';
  if (a03<9)       { *s++=' '; *s++=t0+a03; }
  else if (a03<19) { *s++='1'; *s++=t1+a03; }
  else             { *s++='2'; *s++=t2+a03; } *s++=' ';
  if (a04<9)       { *s++=' '; *s++=t0+a04; }
  else if (a04<19) { *s++='1'; *s++=t1+a04; }
  else             { *s++='2'; *s++=t2+a04; } *s++='\n';

  // row 2
  if (a10<9)       { *s++=' '; *s++=t0+a10; }
  else if (a10<19) { *s++='1'; *s++=t1+a10; }
  else             { *s++='2'; *s++=t2+a10; } *s++=' ';
  if (a11<9)       { *s++=' '; *s++=t0+a11; }
  else if (a11<19) { *s++='1'; *s++=t1+a11; }
  else             { *s++='2'; *s++=t2+a11; } *s++=' ';
  if (a12<9)       { *s++=' '; *s++=t0+a12; }
  else if (a12<19) { *s++='1'; *s++=t1+a12; }
  else             { *s++='2'; *s++=t2+a12; } *s++=' ';
  if (a13<9)       { *s++=' '; *s++=t0+a13; }
  else if (a13<19) { *s++='1'; *s++=t1+a13; }
  else             { *s++='2'; *s++=t2+a13; } *s++=' ';
  if (a14<9)       { *s++=' '; *s++=t0+a14; }
  else if (a14<19) { *s++='1'; *s++=t1+a14; }
  else             { *s++='2'; *s++=t2+a14; } *s++='\n';

  // row 3
  if (a20<9)       { *s++=' '; *s++=t0+a20; }
  else if (a20<19) { *s++='1'; *s++=t1+a20; }
  else             { *s++='2'; *s++=t2+a20; } *s++=' ';
  if (a21<9)       { *s++=' '; *s++=t0+a21; }
  else if (a21<19) { *s++='1'; *s++=t1+a21; }
  else             { *s++='2'; *s++=t2+a21; } *s++=' ';
  if (a22  <9)     { *s++=' '; *s++=t0+a22; }
  else if (a22<19) { *s++='1'; *s++=t1+a22; }
  else             { *s++='2'; *s++=t2+a22; } *s++=' ';
  if (a23<9)       { *s++=' '; *s++=t0+a23; }
  else if (a23<19) { *s++='1'; *s++=t1+a23; }
  else             { *s++='2'; *s++=t2+a23; } *s++=' ';
  if (a24<9)       { *s++=' '; *s++=t0+a24; }
  else if (a24<19) { *s++='1'; *s++=t1+a24; }
  else             { *s++='2'; *s++=t2+a24; } *s++='\n';

  // row 4
  if (a30<9)       { *s++=' '; *s++=t0+a30; }
  else if (a30<19) { *s++='1'; *s++=t1+a30; }
  else             { *s++='2'; *s++=t2+a30; } *s++=' ';
  if (a31<9)       { *s++=' '; *s++=t0+a31; }
  else if (a31<19) { *s++='1'; *s++=t1+a31; }
  else             { *s++='2'; *s++=t2+a31; } *s++=' ';
  if (a32<9)       { *s++=' '; *s++=t0+a32; }
  else if (a32<19) { *s++='1'; *s++=t1+a32; }
  else             { *s++='2'; *s++=t2+a32; } *s++=' ';
  if (a33<9)       { *s++=' '; *s++=t0+a33; }
  else if (a33<19) { *s++='1'; *s++=t1+a33; }
  else             { *s++='2'; *s++=t2+a33; } *s++=' ';
  if (a34<9)       { *s++=' '; *s++=t0+a34; }
  else if (a34<19) { *s++='1'; *s++=t1+a34; }
  else             { *s++='2'; *s++=t2+a34; } *s++='\n';

  // row 5
  if (a40<9)       { *s++=' '; *s++=t0+a40; }
  else if (a40<19) { *s++='1'; *s++=t1+a40; }
  else             { *s++='2'; *s++=t2+a40; } *s++=' ';
  if (a41<9)       { *s++=' '; *s++=t0+a41; }
  else if (a41<19) { *s++='1'; *s++=t1+a41; }
  else             { *s++='2'; *s++=t2+a41; } *s++=' ';
  if (a42<9)       { *s++=' '; *s++=t0+a42; }
  else if (a42<19) { *s++='1'; *s++=t1+a42; }
  else             { *s++='2'; *s++=t2+a42; } *s++=' ';
  if (a43<9)       { *s++=' '; *s++=t0+a43; }
  else if (a43<19) { *s++='1'; *s++=t1+a43; }
  else             { *s++='2'; *s++=t2+a43; } *s++=' ';
  if (a44<9)       { *s++=' '; *s++=t0+a44; }
  else if (a44<19) { *s++='1'; *s++=t1+a44; }
  else             { *s++='2'; *s++=t2+a44; } *s++='\n';
  *s++='\n'; outPointer1=s;

  if (outPointer2!=NULL) {
    const int c0='0'+25, c1='0'+15, c2='0'+5; char *s=outPointer2;

    // row 1
    if (a00>15)     { *s++=' '; *s++=c0-a00; }
    else if (a00>5) { *s++='1'; *s++=c1-a00; }
    else            { *s++='2'; *s++=c2-a00; } *s++=' ';
    if (a01>15)     { *s++=' '; *s++=c0-a01; }
    else if (a01>5) { *s++='1'; *s++=c1-a01; }
    else            { *s++='2'; *s++=c2-a01; } *s++=' ';
    if (a02>15)     { *s++=' '; *s++=c0-a02; }
    else if (a02>5) { *s++='1'; *s++=c1-a02; }
    else            { *s++='2'; *s++=c2-a02; } *s++=' ';
    if (a03>15)     { *s++=' '; *s++=c0-a03; }
    else if (a03>5) { *s++='1'; *s++=c1-a03; }
    else            { *s++='2'; *s++=c2-a03; } *s++=' ';
    if (a04>15)     { *s++=' '; *s++=c0-a04; }
    else if (a04>5) { *s++='1'; *s++=c1-a04; }
    else            { *s++='2'; *s++=c2-a04; } *s++='\n';

    // row 2
    if (a10>15)     { *s++=' '; *s++=c0-a10; }
    else if (a10>5) { *s++='1'; *s++=c1-a10; }
    else            { *s++='2'; *s++=c2-a10; } *s++=' ';
    if (a11>15)     { *s++=' '; *s++=c0-a11; }
    else if (a11>5) { *s++='1'; *s++=c1-a11; }
    else            { *s++='2'; *s++=c2-a11; } *s++=' ';
    if (a12>15)     { *s++=' '; *s++=c0-a12; }
    else if (a12>5) { *s++='1'; *s++=c1-a12; }
    else            { *s++='2'; *s++=c2-a12; } *s++=' ';
    if (a13>15)     { *s++=' '; *s++=c0-a13; }
    else if (a13>5) { *s++='1'; *s++=c1-a13; }
    else            { *s++='2'; *s++=c2-a13; } *s++=' ';
    if (a14>15)     { *s++=' '; *s++=c0-a14; }
    else if (a14>5) { *s++='1'; *s++=c1-a14; }
    else            { *s++='2'; *s++=c2-a14; } *s++='\n';

    // row 3
    if (a20>15)     { *s++=' '; *s++=c0-a20; }
    else if (a20>5) { *s++='1'; *s++=c1-a20; }
    else            { *s++='2'; *s++=c2-a20; } *s++=' ';
    if (a21>15)     { *s++=' '; *s++=c0-a21; }
    else if (a21>5) { *s++='1'; *s++=c1-a21; }
    else            { *s++='2'; *s++=c2-a21; } *s++=' ';
    if (a22  >15)   { *s++=' '; *s++=c0-a22;   }
    else if (a22>5) { *s++='1'; *s++=c1-a22;   }
    else            { *s++='2'; *s++=c2-a22;   } *s++=' ';
    if (a23>15)     { *s++=' '; *s++=c0-a23; }
    else if (a23>5) { *s++='1'; *s++=c1-a23; }
    else            { *s++='2'; *s++=c2-a23; } *s++=' ';
    if (a24>15)     { *s++=' '; *s++=c0-a24; }
    else if (a24>5) { *s++='1'; *s++=c1-a24; }
    else            { *s++='2'; *s++=c2-a24; } *s++='\n';

    // row 4
    if (a30>15)     { *s++=' '; *s++=c0-a30; }
    else if (a30>5) { *s++='1'; *s++=c1-a30; }
    else            { *s++='2'; *s++=c2-a30; } *s++=' ';
    if (a31>15)     { *s++=' '; *s++=c0-a31; }
    else if (a31>5) { *s++='1'; *s++=c1-a31; }
    else            { *s++='2'; *s++=c2-a31; } *s++=' ';
    if (a32>15)     { *s++=' '; *s++=c0-a32; }
    else if (a32>5) { *s++='1'; *s++=c1-a32; }
    else            { *s++='2'; *s++=c2-a32; } *s++=' ';
    if (a33>15)     { *s++=' '; *s++=c0-a33; }
    else if (a33>5) { *s++='1'; *s++=c1-a33; }
    else            { *s++='2'; *s++=c2-a33; } *s++=' ';
    if (a34>15)     { *s++=' '; *s++=c0-a34; }
    else if (a34>5) { *s++='1'; *s++=c1-a34; }
    else            { *s++='2'; *s++=c2-a34; } *s++='\n';

    // row 5
    if (a40>15)     { *s++=' '; *s++=c0-a40; }
    else if (a40>5) { *s++='1'; *s++=c1-a40; }
    else            { *s++='2'; *s++=c2-a40; } *s++=' ';
    if (a41>15)     { *s++=' '; *s++=c0-a41; }
    else if (a41>5) { *s++='1'; *s++=c1-a41; }
    else            { *s++='2'; *s++=c2-a41; } *s++=' ';
    if (a42>15)     { *s++=' '; *s++=c0-a42; }
    else if (a42>5) { *s++='1'; *s++=c1-a42; }
    else            { *s++='2'; *s++=c2-a42; } *s++=' ';
    if (a43>15)     { *s++=' '; *s++=c0-a43; }
    else if (a43>5) { *s++='1'; *s++=c1-a43; }
    else            { *s++='2'; *s++=c2-a43; } *s++=' ';
    if (a44>15)     { *s++=' '; *s++=c0-a44; }
    else if (a44>5) { *s++='1'; *s++=c1-a44; }
    else            { *s++='2'; *s++=c2-a44; } *s++='\n';
    *s++='\n'; outPointer2=s;
  }
} // writeSquares
//-------------------------------------------------------------------------------

bool writeOut(int outBytes, int wfd, int wfdC) {
//   --------
  if (_write(wfd, outputBuffer1, outBytes)!=outBytes) return false;
  outPointer1=outputBuffer1; outSquares=0;
  if (wfdC!=-1) {
   if (_write(wfdC, outputBuffer2, outBytes)!=outBytes) return false;
   outPointer2=outputBuffer2;
  }
  return true;
} // writeOut
//-------------------------------------------------------------------------------

bool outputSquares
//   -------------
  (int wfd, int wfdC,
   const int a00, const int a01, const int a02, const int a03, const int a04,
   const int a10, const int a11, const int a12, const int a13, const int a14,
   const int a20, const int a21, const int a22, const int a23, const int a24,
   const int a30, const int a31, const int a32, const int a33, const int a34,
   const int a40, const int a41, const int a42, const int a43, const int a44) {

  writeSquares(a00, a01, a02, a03, a04,
               a10, a11, a12, a13, a14,
               a20, a21, a22, a23, a24,
               a30, a31, a32, a33, a34,
               a40, a41, a42, a43, a44);
  
  writeSquares(a11, a10, a12, a14, a13,
               a01, a00, a02, a04, a03,
               a21, a20, a22, a24, a23,
               a41, a40, a42, a44, a43,
               a31, a30, a32, a34, a33); // xform 2
  
  writeSquares(a44, a41, a42, a43, a40,
               a14, a11, a12, a13, a10,
               a24, a21, a22, a23, a20,
               a34, a31, a32, a33, a30,
               a04, a01, a02, a03, a00); // xform 1
  
  writeSquares(a11, a14, a12, a10, a13,
               a41, a44, a42, a40, a43,
               a21, a24, a22, a20, a23,
               a01, a04, a02, a00, a03,
               a31, a34, a32, a30, a33); // xform 2 of xform 1

  outSquares+=4;
  if ((outSquares==numSquares)&&!writeOut(outSize, wfd, wfdC)) return false;
  return true;
} // outputSquares
//------------------------------------------------------------------------------------------

// Used to disable some pairs of diagonal permutations.
struct bools {
  union {
    bool off[NN][NN][NN][NN][NN][NN];
    int align[(NN*NN*NN*NN*NN*NN+3)/4];
  } u;
};
bools noOff, diags;

// fill order: \dia, /dia, r 1, c 0, c 4, a[3][2], r 2, c 2, c 1, a[0][3], a[4][3]
//
//  1 21 19 23  5
//  9  2 10  6 11
// 12 17  0 18 14
// 13  7 16  3 15
//  8 22 20 24  4

int makeCenterKSquares(const int K, int wfd, int wfdC) {
//  ------------------
  int count=0, i0=sizeCenterKUPermus, **p0=CenterKUPermus; // B2>B1
  while (i0--) { // \ back diagonal B   
    int *B=*p0, B0=B[0], B1=B[1], B2=B[2], B3=B[3],
        i1=sizeCenterKUPermus, **p1=CenterKUPermus; // F2>F1
    while (i1--) { // / forward diagonal F
      int *F=*p1; if ( (F[1]>B1)&&(!(F[4]&B[4])) ) {
      int F0=F[0], F1=F[1], F2=F[2], F3=F[3];
      // F2>F1 and F1>B1, so F2>B1, (see second column below).

      if (diags.u.off[B0][B1][B2][F0][F1][F2]) { ++p1; continue; }

      // Count 4: the square, xform 1, xform 2, and xform 2 of xform 1.
      if (F3<F0) {  // xform 1: Turn off its xform 2 and skip.
        if (B3>B0) { // xform 2
          if (F3>B0) diags.u.off[B1][B0][B3][F2][F3][F0]=true;
          else       diags.u.off[F2][F3][F0][B1][B0][B3]=true;
        } else {    // xform 2, reverse B
          if (F3>B3) diags.u.off[B2][B3][B0][F2][F3][F0]=true;
          else       diags.u.off[F2][F3][F0][B2][B3][B0]=true;
        }
        ++p1; continue;
      } else { // Turn off xform 2.
		    if (B3>B0) {  // xform 2
          if (F0>B0) diags.u.off[B1][B0][B3][F1][F0][F3]=true;
          else       diags.u.off[F1][F0][F3][B1][B0][B3]=true;
		    } else {     // xform 2, reverse B
		      if (F0>B3) diags.u.off[B2][B3][B0][F1][F0][F3]=true;
          else       diags.u.off[F1][F0][F3][B2][B3][B0]=true;
		    }
      }

      int fUsed=F[4]|B[4];
      spu *p2=OKUPermus24[B1][F1]; int i2=sizeOKUPermus24[B1][F1];
      while (i2--) { if (!(p2->u&fUsed)) { // second row
        int *r1=p2->p, r1Used=p2->u|fUsed;
         
        spu *p3=OKPermus125[B0][r1[0]][F3]; int i3=sizeOKPermus125[B0][r1[0]][F3];
        while (i3--) { if (!(p3->u&r1Used)) { // first column;
          int *c0=p3->p, c0Used=p3->u|r1Used;
           
          spu *p4=OKPermus125[F0][r1[4]][B3]; int i4=sizeOKPermus125[F0][r1[4]][B3];
          while (i4--) { if (!(p4->u&c0Used)) { // fifth column
            int *c4=p4->p, c4Used=p4->u|c0Used, a32=OKPermus1245[c0[3]][F2][B2][c4[3]];
            if ( (a32>=0)&&(!(c4Used&(1<<a32))) ) { c4Used|=(1<<a32);
             
            spu *p5=CenterKPermus14[c0[2]][c4[2]]; int i5=sizeCenterKPermus14[c0[2]][c4[2]];
            while (i5--) { if (!(p5->u&c4Used)) { // third row            
              int *r2=p5->p, r2Used=p5->u|c4Used;
               
              spu *p6=CenterKPermus23[r1[2]][a32]; int i6=sizeCenterKPermus23[r1[2]][a32];
              while (i6--) { if (!(p6->u&r2Used)) { // third column
                int *c2=p6->p, c2Used=p6->u|r2Used;

                spu *p7=OKUPermus234[B1][r2[1]][F2]; int i7=sizeOKUPermus234[B1][r2[1]][F2];
                while (i7--) { if (!(p7->u&c2Used)) { // second column
                  int *c1=p7->p, c1Used=p7->u|c2Used, a03=OKPermus1235[B0][p7->p[0]][c2[0]][F0];

                  if ((a03>=0)&&!(c1Used&(1<<a03))) {
                    int a43=OKPermus1235[F3][c1[4]][c2[3]][B3];
                    if (!outputSquares(wfd, wfdC,
                           B0,    c1[0], c2[0], a03,   F0,
                           r1[0], B1,    r1[2], F1,    r1[4],
                           c0[2], r2[1], K,     r2[2], c4[2],
                           c0[3], F2,    a32,   B2,    c4[3],
                           F3,    c1[4], c2[3], a43,   B3)) return 0; ++count;
                  }
                } ++p7; }
              } ++p6; }
            } ++p5; }
          }} ++p4; }
        } ++p3; }
      } ++p2; }
    } ++p1; }
    ++p0;
  }
  if (!writeOut(outSquares*squareBytes, wfd, wfdC)) return 0; return count;
} // makeCenterKSquares 
//------------------------------------------------------------------------------------------

const int bufSize=128;
bool openDir() {
//   -------
  char buf[bufSize], msg[bufSize]; const char *baseName="Order5";
  strcpy_s(buf, bufSize, baseName); int sub=0;

  do {
    if (_mkdir(buf)!=-1) break;
    if (errno!=EEXIST) {
      sprintf_s(msg, bufSize, "Can't make folder %s", buf);
      perror(msg); return false;
    }
    sprintf_s(buf, bufSize, "%s_%d", baseName, ++sub);
  } while (true);
  if (_chdir(buf)==-1) {
    sprintf_s(msg, bufSize, "Can't open folder %s", buf);
    perror(msg); return false;
  }
  printf("Output files are in folder %s\n\n", buf);
  return true;
} // openDir

int openFile(int K) {
//  --------
  char buf[bufSize]; sprintf_s(buf, bufSize, "Center%d.txt", K);

  int wfd=-1;
  if ((wfd=_open(buf, _O_CREAT|_O_WRONLY, 0644))==-1) {
    char msg[bufSize];
    sprintf_s(msg, bufSize, "\a\nCan't open for write %s", buf);
    perror(msg);
  }
  return wfd;
} // openFile

bool openFiles(int K, int *wfdK, int *wfdCK) {
//   ---------
  if ((*wfdK=openFile(K+1))==-1) return false;
  if (K==Mid) *wfdCK =-1;
  else if ((*wfdCK=openFile(NN-K))==-1) return false;
  return true;
} // openFiles

void closeFiles(int wfd, int wfdC) {
//   ----------
  if (wfd!=-1) _close(wfd); if (wfdC!=-1) _close(wfdC);
} // wcloseFiles
//---------------------------------------------------------------------------------

void printElapsedTime(time_t startTime) {
//   ---------------- 
  int et=(int)difftime(time(NULL), startTime); printf("%2d:%02d\n", et/60, et%60);
} // printElapsedTime
//---------------------------------------------------------------------------------

void makeSquares() {
//   -----------
  int count=0; time_t startTime=time(NULL);
  int wfdK=-1, wfdCK =-1;

  for (int K=0; K <= Mid; ++K) if (openFiles(K, &wfdK, &wfdCK)) {
    time_t startTimeK=time(NULL);
    getCombos(K); getCenterKPermus(K); getOKPermus();

    outPointer1=outputBuffer1; outSquares=0;
    outPointer2=wfdCK==-1?NULL:outputBuffer2;
    int Kcount=makeCenterKSquares(K, wfdK, wfdCK);
    if (Kcount==0) { perror("\a\nError writing file"); break; }
    Kcount*=4; count+=Kcount;
    if (K==Mid) 
      printf("Center %2d    number of squares %9d, elapsed time ", K+1, Kcount);
    else {
      printf("Center %2d,%2d number of squares %9d, elapsed time ", K+1, NN-K, Kcount);
      count+=Kcount; diags=noOff;
    }
    printElapsedTime(startTimeK);
  }
  closeFiles(wfdK, wfdCK);
  printf("\n       total number of squares %9d, elapsed time ", count);
  printElapsedTime(startTime);
} // makeSquares
//-------------------------------------------------------------------------------

void outputLocalTime() {
//   --------------
  time_t startTime=time(NULL); struct tm local;
  if (localtime_s(&local, &startTime)==0) {
    char dateTime[100];
    size_t slen=strftime(dateTime, 100, "%A %Y-%m-%d %X %Z\n\n\0", &local);
    printf("\n%s", dateTime);
  }
} // outputLocalTime
//--------------------------------------------------------------------------

int main() {
//  ----
  outputLocalTime(); if (openDir()) makeSquares();
  printf("\n\nPress a key to close the console");
  while (!_kbhit()) Sleep(250); return 0;
} // main