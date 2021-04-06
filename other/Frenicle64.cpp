/*
 *  File:    Frenicle64.cpp
 *  Author:  S Harry White
 *  Created: 2009-12-28
 *  Updated: 2018-08-18
 */

/*
 *  Updated to 64-bit from Frenicle.cpp
 */

#include "stdafx.h"
#include <assert.h>
#include <conio.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

typedef signed   char      Sbyte;
typedef unsigned char      Ubyte;
typedef signed   short     Sshort;
typedef unsigned short     Ushort;
typedef unsigned int       Uint;
typedef unsigned long long Uint64;

const int byteBytes  = 1;
const int shortBytes = sizeof(short)/sizeof(char);
const int intBytes   = sizeof(int)/sizeof(char);

//#define min(A, B) ((A) < (B) ? (A) : (B))

int N,              // The order of the square.
    Nm1,            // N - 1
    Nm1N,           // (N - 1) * N
    Nm2,            // N - 2
    Nm2N,           // (N - 2) * N
    NN,             // N * N
    NNm1,           // NN - 1
    NNmN,           // NN - N
    NNmNm1,         // NN - N - 1
    NNmNp1,         // NN - N + 1
    NNm2,           // NN - 2
    NpNm1,          // N + N - 1

    numberBytes,    // 1, 2 or 4 byte integer for square numbers

    pointerSize = sizeof(void *);

bool signedNumbers;

void **blocks = NULL; const int startNumBlocks = 1;
int numBlocksAllocated, maxBlocksIncr, squaresPerBlock, squareSize; // NN * numberBytes

void **squares = NULL; int numSquaresAllocated, sIndex;
//-------------------------------------------------------------------------------

const Uint64 oneBB = 1000000000000000000;
const Uint oneB = 1000000000, oneM = 1000000, oneT = 1000;

const int fbufSize = 1024; char fbuf[fbufSize];
char *formatN(Uint64 n) {
//    -------
  const int nbufSize = 20; char nbuf[nbufSize];
  Uint bb = n / oneBB; n %= oneBB;
  Uint b  = n / oneB;  n %= oneB;

  if (bb == 0) {
    sprintf_s(fbuf, fbufSize, "%u", b);
  } else {
    sprintf_s(fbuf, fbufSize, "%u", bb);
    sprintf_s(nbuf, nbufSize, ",%03u,%03u,%03u", b/oneM, (b%oneM)/oneT, b%oneT);
    strcat_s(fbuf, fbufSize, nbuf);
  }
  sprintf_s(nbuf, nbufSize, ",%03u,%03u,%03u", n/oneM, (n%oneM)/oneT, n%oneT);
  strcat_s(fbuf, fbufSize, nbuf);
  return fbuf;
} // formatN
//--------------------------------------------------------------------------

void freeStore(int allocated) {
//   ---------
  if (blocks != NULL) {
    if ( (allocated != 0) ) printf(".. freeing squares\n");   
    for (int i = 0; i < allocated; i++) {
      free(blocks[i]);
    }
    free(blocks); blocks = NULL;
  }
  numBlocksAllocated = 0;
} // freeStore

const char *allocationFailed = "\nStorage allocation failed.\n\n";
bool reportAlloc = false;
bool allocateStore(int numNew) {
//   -------------
  bool ok; int numOld = numBlocksAllocated;
  union ovly { Uint64 i; void *p; };

  if ((numNew-numOld) > maxBlocksIncr) numNew = numOld+maxBlocksIncr;
  if (reportAlloc) {
    int nsq = numNew * squaresPerBlock,
        msq = nsq / 1000000,
        tsq = nsq % 1000000 / 1000,
        rsq = nsq % 1000;
    printf("    .. increasing squares to ");
    if (msq == 0)
      if (tsq == 0)
        printf("%11d\n", rsq);
      else
        printf("%7d,%03d\n", tsq, rsq);
    else
      printf("%3d,%03d,%03d\n", msq, tsq, rsq);
  } else
    reportAlloc = true;

  void **tmp = (void **)malloc(numNew * pointerSize);
  if (ok = (tmp != NULL)) {
    int allocated = numNew;
	   for (int i = 0; i < numOld; i++) tmp[i] = blocks[i];
    for(int i = numOld; i < numNew; i++) {
      void *p = malloc(squaresPerBlock * squareSize); 
      if (p == NULL) { ok = false; allocated = i; break; }
      tmp[i] = p;
    }
	   freeStore(0); blocks = tmp;
	   if (ok) {
      Uint64 numNew64 = numNew; // force type conversion in the arithmetic
      tmp = (void **)malloc(numNew64 * squaresPerBlock * pointerSize);
      if (ok = (tmp != NULL)) {
        for (int i = 0; i < sIndex; i++) tmp[i] = squares[i];
        free(squares); squares = tmp;
        int j = sIndex;
        for (int i = numOld; i < numNew; i++) {
          int k = 0; void *p = blocks[i];
          while (k++ < squaresPerBlock) {
            squares[j++] = p;
            ovly ip; ip.p = p; ip.i += NN * numberBytes; p = ip.p;           
          }
        }
	       numBlocksAllocated  = numNew;
        numSquaresAllocated = numNew * squaresPerBlock;
      }
    }
    if (!ok) { printf(allocationFailed); freeStore(allocated); }
  }
  if (ok) {
    Uint64 numNew64 = numNew,
           total = (numNew64 * pointerSize) +                  // blocks (ptrs)
                   (numNew64 * squaresPerBlock * squareSize) + // blocks (guts)
                   (numNew64 * squaresPerBlock * pointerSize); // squares (ptrs)
    printf("total bytes allocated %s\n", formatN(total));
  }
  return ok;
} // allocateStore
//============================================================================

Uint64 ncompares;
int cmpSquaresSbyte(const void *pp, const void *pq) {
//  ---------------
  Sbyte *p = *(Sbyte **)pp, *q = *(Sbyte **)pq; int i = 0;
  //if ((++ncompares & 0XFFFFFFF) == 0) printf("cmps %s\n", formatN(ncompares));
  while(i++ < NN) if (*p<*q) return -1; else if (*p++>*q++) return 1; return 0;
} // cmpSquaresSbyte

int cmpSquaresUbyte(const void *pp, const void *pq) {
//  ---------------
  Ubyte *p = *(Ubyte **)pp, *q = *(Ubyte **)pq; int i = 0;
  //if ((++ncompares & 0XFFFFFFF) == 0) printf("cmps %s\n", formatN(ncompares));
  while(i++ < NN) if (*p<*q) return -1; else if (*p++>*q++) return 1; return 0;
} // cmpSquaresUbyte

int cmpSquaresSshort(const void *pp, const void *pq) {
//  ----------------
  Sshort *p = *(Sshort **)pp, *q = *(Sshort **)pq; int i = 0;
  //if ((++ncompares & 0XFFFFFFF) == 0) printf("cmps %s\n", formatN(ncompares));
  while(i++ < NN) if (*p<*q) return -1; else if (*p++>*q++) return 1; return 0;
} // cmpSquaresSshort

int cmpSquaresUshort(const void *pp, const void *pq) {
//  ----------------
  Ushort *p = *(Ushort **)pp, *q = *(Ushort **)pq; int i = 0;
  //if ((++ncompares & 0XFFFFFFF) == 0) printf("cmps %s\n", formatN(ncompares));
  while(i++ < NN) if (*p<*q) return -1; else if (*p++>*q++) return 1; return 0;
} // cmpSquaresUshort

int cmpSquaresInt(const void *pp, const void *pq) {
//  -------------
  int *p = *(int **)pp, *q = *(int **)pq, i = 0;
  //if ((++ncompares & 0XFFFFFFF) == 0) printf("cmps %s\n", formatN(ncompares));
  while(i++ < NN) if (*p<*q) return -1; else if (*p++>*q++) return 1; return 0;
} // cmpSquaresInt

void sortSquares() {
//   -----------
  printf(".. sorting squares\n"); //clock_t t0=clock();
  //ncompares = 0;
  switch (numberBytes) {
    case 1:
      if (signedNumbers)
        qsort(squares, sIndex, sizeof(void *), cmpSquaresSbyte);
      else
        qsort(squares, sIndex, sizeof(void *), cmpSquaresUbyte);
      break;
    case 2:
      if (signedNumbers)
        qsort(squares, sIndex, sizeof(void *), cmpSquaresSshort);
      else
        qsort(squares, sIndex, sizeof(void *), cmpSquaresUshort);
      break;
    case 4:
      qsort(squares, sIndex, sizeof(void *), cmpSquaresInt);
    default: assert(false);
  }
  //printf("sort time: %.3f sec\n", (double)(clock()-t0) / CLOCKS_PER_SEC);
} // sortSquares
//=====================================================================

int fieldWidth(int i) {
//  ----------
  bool neg = i < 0;  if (neg) i = -i; int width = 1;
  while ((i = i / 10) != 0) ++width;
  if (width > 10) width = 10; // limited by fprintFW11
  return neg ? width + 1 : width;
} // fieldWidth
// --------------------------------------------------------------------

const int outSize = 100000000, noFD = -1, writeErr = -1;
char outBuffer[outSize]; int outBytes, squareBytes;

typedef void (*t_sprintFW)(char *s, int i);
void sprintFW1 (char *s, int i) { sprintf(s, "%1d",  i); }
void sprintFW2 (char *s, int i) { sprintf(s, "%2d",  i); }
void sprintFW3 (char *s, int i) { sprintf(s, "%3d",  i); }
void sprintFW4 (char *s, int i) { sprintf(s, "%4d",  i); }
void sprintFW5 (char *s, int i) { sprintf(s, "%5d",  i); }
void sprintFW6 (char *s, int i) { sprintf(s, "%6d",  i); }
void sprintFW7 (char *s, int i) { sprintf(s, "%7d",  i); }
void sprintFW8 (char *s, int i) { sprintf(s, "%8d",  i); }
void sprintFW9 (char *s, int i) { sprintf(s, "%9d",  i); }
void sprintFW10(char *s, int i) { sprintf(s, "%10d", i); }
void sprintFW11(char *s, int i) { sprintf(s, "%11d", i); }
static t_sprintFW sprintFW[] =
  {
    NULL,      sprintFW1, sprintFW2, sprintFW3, sprintFW4, sprintFW5,
    sprintFW6, sprintFW7, sprintFW8, sprintFW9, sprintFW10, sprintFW11
  };
//----------------------------------------------------------------------

int writeSquaresSbyte(int wfd, int fw) {
//  -----------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresSbyte(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
      int i = 0, colCount = 0; Sbyte *p = ((Sbyte **)squares)[sq];
      if (squareBytes > outSize) {
        while (i++ < NN) {
          if ((outBytes + fw + 2) >= outSize) {
            if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
            outBytes = 0; s = outBuffer;
          }
          sprintFW[fw](s, *p++); s += fw;
          if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
          outBytes += fw + 1;
        }
        *s++ = '\n'; ++outBytes;
      } else {
        if ((outBytes + squareBytes) >= outSize) {
          if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
          outBytes = 0; s = outBuffer;
        }
        while (i++ < NN) {
          sprintFW[fw](s, *p++); s += fw;
          if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';         
        }
        *s++ = '\n'; outBytes += squareBytes;
      }
    }
  }
  return countDuplicates;     
} // writeSquaresSbyte

int writeSquaresUbyteFW3(int wfd) {
//  --------------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresUbyte(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
      if ((outBytes + squareBytes) >= outSize) {
        if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
        outBytes = 0; s = outBuffer;
      }
      int i = 0, colCount = 0; Ubyte *p = ((Ubyte **)squares)[sq];
      while (i++ < NN) {
        int w = *p++;
        if (w < 10) {
         *s++ = ' '; *s++ = ' '; *s++ = '0' + w;
        } else {
          int v = w / 10;
          if (w < 100) {
            *s++ = ' '; w -= v*10; 
          } else {
            int u = v / 10;  w -= v*10; v -= u*10; *s++ = '0' + u;
          }
          *s++ = '0' + v; *s++ = '0'+ w;
        }
        if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
      }
      *s++ = '\n'; outBytes += squareBytes;
    }
  }
  return countDuplicates;     
} // writeSquaresUbyteFW3

int writeSquaresUbyteFW2(int wfd) {
//  --------------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresUbyte(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
      if ((outBytes + squareBytes) >= outSize) {
        if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
        outBytes = 0; s = outBuffer;
      }
      int i = 0, colCount = 0; Ubyte *p = ((Ubyte **)squares)[sq];
      while (i++ < NN) {
        int v = *p++;
        if (v < 10) {
         *s++ = ' '; *s++ = '0' + v;
        } else {
          int u = v / 10; v -= u*10; *s++ = '0' + u; *s++ = '0'+ v;
        }
        if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
      }
      *s++ = '\n'; outBytes += squareBytes;
    }
  }
  return countDuplicates;     
} // writeSquaresUbyteFW2

int writeSquaresLT10(int wfd) {
//  ----------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresUbyte(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
      if ((outBytes + squareBytes) >= outSize) {
        if (_write(wfd, outBuffer, outBytes) != outBytes) return -1;
        outBytes = 0; s = outBuffer;
      }
      int i = 0, colCount = 0; Ubyte *p = ((Ubyte **)squares)[sq];
      while (i++ < NN) {
        int v = *p++; *s++ = '0' + v;
        if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
      }
      *s++ = '\n'; outBytes += squareBytes;
    }
  }
  return countDuplicates;     
} // writeSquaresLT10

int writeSquaresLT20(int wfd) {
//  ----------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresUbyte(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
      if ((outBytes + squareBytes) >= outSize) {
        if (_write(wfd, outBuffer, outBytes) != outBytes) return -1;
        outBytes = 0; s = outBuffer;
      }
      int i = 0, colCount = 0; Ubyte *p = ((Ubyte **)squares)[sq];
      while (i++ < NN) {
        int v = *p++;
        if (v < 10)      { *s++ = ' '; *s++ = '0' + v; }
        else             { *s++ = '1'; *s++ = '0' - 10 + v; }
        if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
      }
      *s++ = '\n'; outBytes += squareBytes;
    }
  }
  return countDuplicates;     
} // writeSquaresLT20

int writeSquaresLT30(int wfd) {
//  ----------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresUbyte(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
      if ((outBytes + squareBytes) >= outSize) {
        if (_write(wfd, outBuffer, outBytes) != outBytes) return -1;
        outBytes = 0; s = outBuffer;
      }
      int i = 0, colCount = 0; Ubyte *p = ((Ubyte **)squares)[sq];
      while (i++ < NN) {
        int v = *p++;
        if (v < 10)      { *s++ = ' '; *s++ = '0' + v; }
        else if (v < 20) { *s++ = '1'; *s++ = '0' - 10 + v; }
        else             { *s++ = '2'; *s++ = '0' - 20 + v; }
        if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
      }
      *s++ = '\n'; outBytes += squareBytes;
    }
  }
  return countDuplicates;     
} // writeSquaresLT30

int writeSquaresUbyte(int wfd, int fw) {
//  -----------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresUbyte(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
      int i = 0, colCount = 0; Ubyte *p = ((Ubyte **)squares)[sq];
      if (squareBytes > outSize) {
        while (i++ < NN) {
          if ((outBytes + fw + 2) >= outSize) {
            if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
            outBytes = 0; s = outBuffer;
          }
          sprintFW[fw](s, *p++); s += fw;
          if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
          outBytes += fw + 1;
        }
        *s++ = '\n'; ++outBytes;
      } else {
        if ((outBytes + squareBytes) >= outSize) {
          if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
          outBytes = 0; s = outBuffer;
        }
        while (i++ < NN) {
          sprintFW[fw](s, *p++); s += fw;
          if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';         
        }
        *s++ = '\n'; outBytes += squareBytes;
      }
    }
  }
  return countDuplicates;     
} // writeSquaresUbyte

int writeSquaresSshort(int wfd, int fw) {
//  ------------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresSshort(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
      int i = 0, colCount = 0; Sshort *p = ((Sshort **)squares)[sq];
      if (squareBytes > outSize) {
        while (i++ < NN) {
          if ((outBytes + fw + 2) >= outSize) {
            if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
            outBytes = 0; s = outBuffer;
          }
          sprintFW[fw](s, *p++); s += fw;
          if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
          outBytes += fw + 1;
        }
        *s++ = '\n'; ++outBytes;
      } else {
        if ((outBytes + squareBytes) >= outSize) {
          if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
          outBytes = 0; s = outBuffer;
        }
        while (i++ < NN) {
          sprintFW[fw](s, *p++); s += fw;
          if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';         
        }
        *s++ = '\n'; outBytes += squareBytes;
      }
    }
  }
  return countDuplicates;
} // writeSquaresSshort

int writeSquaresUshortFW3(int wfd) {
//  ---------------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresUshort(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
      if ((outBytes + squareBytes) >= outSize) {
        if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
        outBytes = 0; s = outBuffer;
      }
      int i = 0, colCount = 0; Ushort *p = ((Ushort **)squares)[sq];
      while (i++ < NN) {
        int w = *p++;
        if (w < 10) {
         *s++ = ' '; *s++ = ' '; *s++ = '0' + w;
        } else {
          int v = w / 10;
          if (w < 100) {
            *s++ = ' '; w -= v*10; 
          } else {
            int u = v / 10;  w -= v*10; v -= u*10; *s++ = '0' + u;
          }
          *s++ = '0' + v; *s++ = '0'+ w;
        }
        if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
      }
      *s++ = '\n'; outBytes += squareBytes;
    }
  }
  return countDuplicates;     
} // writeSquaresUshortFW3

int writeSquaresUshort(int wfd, int fw) {
//  ------------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresUshort(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
     int i = 0, colCount = 0; Ushort *p = ((Ushort **)squares)[sq];
      if (squareBytes > outSize) {
        while (i++ < NN) {
          if ((outBytes + fw + 2) >= outSize) {
            if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
            outBytes = 0; s = outBuffer;
          }
          sprintFW[fw](s, *p++); s += fw;
          if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
          outBytes += fw + 1;
        }
        *s++ = '\n'; ++outBytes;
      } else {
        if ((outBytes + squareBytes) >= outSize) {
          if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
          outBytes = 0; s = outBuffer;
        }
        while (i++ < NN) {
          sprintFW[fw](s, *p++); s += fw;
          if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';         
        }
        *s++ = '\n'; outBytes += squareBytes;
      }
    }
  }
  return countDuplicates;
} // writeSquaresUshort

int writeSquaresInt(int wfd, int fw) {
//  ---------------
  int countDuplicates = 0; char *s = outBuffer;
  for (int sq = 0; sq < sIndex; sq++) {
    if ( (sq > 0) &&
       ( cmpSquaresInt(&squares[sq-1], &squares[sq]) == 0) ) {
      countDuplicates++;
    } else {
     int i = 0, colCount = 0; int *p = ((int **)squares)[sq];
      if (squareBytes > outSize) {
        while (i++ < NN) {
          if ((outBytes + fw + 2) >= outSize) {
            if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
            outBytes = 0; s = outBuffer;
          }
          sprintFW[fw](s, *p++); s += fw;
          if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';
          outBytes += fw + 1;
        }
        *s++ = '\n'; ++outBytes;
      } else {
        if ((outBytes + squareBytes) >= outSize) {
          if (_write(wfd, outBuffer, outBytes) != outBytes) return writeErr;
          outBytes = 0; s = outBuffer;
        }
        while (i++ < NN) {
          sprintFW[fw](s, *p++); s += fw;
          if (++colCount == N) { *s++ = '\n'; colCount = 0; } else *s++ = ' ';         
        }
        *s++ = '\n'; outBytes += squareBytes;
      }
    }
  }
  return countDuplicates;
} // writeSquaresInt

int smallestRead, biggestRead;
bool writeSquares(int wfd) {
//   ------------
  int countDuplicates = 0,
      sW = fieldWidth(smallestRead), bW = fieldWidth(biggestRead),
      fw = sW > bW ? sW : bW;
  
  squareBytes = NN * (fw+1) + 1; outBytes = 0;
  //printf("squareBytes %d\n", squareBytes);
  switch (numberBytes) {
    case 1:
      if (signedNumbers) {
        //puts("writeSquaresSbyte");
        countDuplicates = writeSquaresSbyte(wfd, fw);
      } else {
        if (squareBytes <= outSize) {
          if (fw == 1) {
            //puts("writeSquaresLT10");
            countDuplicates = writeSquaresLT10(wfd);
          } else if (fw == 2) {
            if (biggestRead<20) {
             //puts("writeSquaresLT20");
             countDuplicates = writeSquaresLT20(wfd);
            } else if (biggestRead<30) {
              //puts("writeSquaresLT30");
              countDuplicates = writeSquaresLT30(wfd);
            } else {
              //puts("writeSquaresUbyteFW2");
              countDuplicates = writeSquaresUbyteFW2(wfd);
            }
          } else if (fw == 3) {
            //puts("writeSquaresUbyteFW3");
            countDuplicates = writeSquaresUbyteFW3(wfd);
          } else {
            //puts("writeSquaresUbyte");
            countDuplicates = writeSquaresUbyte(wfd, fw);
          }
        } else {
          //puts("writeSquaresUbyte");
          countDuplicates = writeSquaresUbyte(wfd, fw);
        }
      }
      break;
    case 2:
      if (signedNumbers) {
        //puts("writeSquaresSshort");
        countDuplicates = writeSquaresSshort(wfd, fw);
      } else {
        if ((squareBytes <= outSize) && (fw == 3)) {
          //puts("writeSquaresUshortFW3");
          countDuplicates = writeSquaresUshortFW3(wfd);
        } else {
          //puts("writeSquaresUshort");
          countDuplicates = writeSquaresUshort(wfd, fw);
        }
      }
      break;
    case 4:
      //puts("writeSquaresInt");
      countDuplicates = writeSquaresInt(wfd, fw); break;
    default: assert(false);
  }
  if ((outBytes != 0) &&
      (_write(wfd, outBuffer, outBytes) != outBytes)) return false;
  if (countDuplicates > 0)
    printf("\nNumber of squares %d minus %d duplicates: %d.\n",
           sIndex, countDuplicates, sIndex - countDuplicates);
  else if (countDuplicates == 0)
    printf("\nNumber of squares: %d.\n", sIndex);
  return countDuplicates >= 0;
} // writeSquares
//-------------------------------------------------------------------------------

bool rotateSbyte(Sbyte *p, int corner)
//   -----------
{
  switch (corner) {
    case 0: // NW
      for (int r = 0; r < N; r++)
        for (int c = r+1; c < N; c++) {
          int i = r*N+c, j = c*N+r;
          if (p[i] == p[j]) continue; return p[i] < p[j];
        } break;
      case 1: // NE
      for (int r = 0; r < N; r++)
        for (int c = Nm2-r; c >= 0; c--) {
          int i = r*N+c, j = N*(N-c)-r-1;
          if (p[i] == p[j]) continue; return p[i] > p[j];
        } break;
      case 2: // SE
      for (int r = Nm1; r >= 0; r--)
        for (int c = r-1; c >= 0; c--) {
          int i = r*N+c, j = c*N+r;
          if (p[i] == p[j]) continue; return p[i] < p[j];
        } break;
      case 3: // SW
      for (int r = Nm1; r >= 0; r--)
        for (int c = N-r; c < N; c++) {
          int i = r*N+c, j = N*(N-c)-r-1;
          if (p[i] == p[j]) continue; return p[i] > p[j];
        } break;
    default: assert(false);
  }
  return true;
} // rotateSbyte

bool rotateUbyte(Ubyte *p, int corner)
//   -----------
{
  switch (corner) {
    case 0: // NW
      for (int r = 0; r < N; r++)
        for (int c = r+1; c < N; c++) {
          int i = r*N+c, j = c*N+r;
          if (p[i] == p[j]) continue; return p[i] < p[j];
        } break;
      case 1: // NE
      for (int r = 0; r < N; r++)
        for (int c = Nm2-r; c >= 0; c--) {
          int i = r*N+c, j = N*(N-c)-r-1;
          if (p[i] == p[j]) continue; return p[i] > p[j];
        } break;
      case 2: // SE
      for (int r = Nm1; r >= 0; r--)
        for (int c = r-1; c >= 0; c--) {
          int i = r*N+c, j = c*N+r;
          if (p[i] == p[j]) continue; return p[i] < p[j];
        } break;
      case 3: // SW
      for (int r = Nm1; r >= 0; r--)
        for (int c = N-r; c < N; c++) {
          int i = r*N+c, j = N*(N-c)-r-1;
          if (p[i] == p[j]) continue; return p[i] > p[j];
        } break;
    default: assert(false);
  }
  return true;
} // rotateUbyte

bool rotateSshort(Sshort *p, int corner)
//   ------------
{
  switch (corner) {
    case 0: // NW
      for (int r = 0; r < N; r++)
        for (int c = r+1; c < N; c++) {
          int i = r*N+c, j = c*N+r;
          if (p[i] == p[j]) continue; return p[i] < p[j];
        } break;
      case 1: // NE
      for (int r = 0; r < N; r++)
        for (int c = Nm2-r; c >= 0; c--) {
          int i = r*N+c, j = N*(N-c)-r-1;
          if (p[i] == p[j]) continue; return p[i] > p[j];
        } break;
      case 2: // SE
      for (int r = Nm1; r >= 0; r--)
        for (int c = r-1; c >= 0; c--) {
          int i = r*N+c, j = c*N+r;
          if (p[i] == p[j]) continue; return p[i] < p[j];
        } break;
      case 3: // SW
      for (int r = Nm1; r >= 0; r--)
        for (int c = N-r; c < N; c++) {
          int i = r*N+c, j = N*(N-c)-r-1;
          if (p[i] == p[j]) continue; return p[i] > p[j];
        } break;
    default: assert(false);
  }
  return true;
} // rotateSshort

bool rotateUshort(Ushort *p, int corner)
//   ------------
{
  switch (corner) {
    case 0: // NW
      for (int r = 0; r < N; r++)
        for (int c = r+1; c < N; c++) {
          int i = r*N+c, j = c*N+r;
          if (p[i] == p[j]) continue; return p[i] < p[j];
        } break;
      case 1: // NE
      for (int r = 0; r < N; r++)
        for (int c = Nm2-r; c >= 0; c--) {
          int i = r*N+c, j = N*(N-c)-r-1;
          if (p[i] == p[j]) continue; return p[i] > p[j];
        } break;
      case 2: // SE
      for (int r = Nm1; r >= 0; r--)
        for (int c = r-1; c >= 0; c--) {
          int i = r*N+c, j = c*N+r;
          if (p[i] == p[j]) continue; return p[i] < p[j];
        } break;
      case 3: // SW
      for (int r = Nm1; r >= 0; r--)
        for (int c = N-r; c < N; c++) {
          int i = r*N+c, j = N*(N-c)-r-1;
          if (p[i] == p[j]) continue; return p[i] > p[j];
        } break;
    default: assert(false);
  }
  return true;
} // rotateUshort

bool rotateInt(int *p, int corner)
//   ---------
{
  switch (corner) {
    case 0: // NW
      for (int r = 0; r < N; r++)
        for (int c = r+1; c < N; c++) {
          int i = r*N+c, j = c*N+r;
          if (p[i] == p[j]) continue; return p[i] < p[j];
        } break;
      case 1: // NE
      for (int r = 0; r < N; r++)
        for (int c = Nm2-r; c >= 0; c--) {
          int i = r*N+c, j = N*(N-c)-r-1;
          if (p[i] == p[j]) continue; return p[i] > p[j];
        } break;
      case 2: // SE
      for (int r = Nm1; r >= 0; r--)
        for (int c = r-1; c >= 0; c--) {
          int i = r*N+c, j = c*N+r;
          if (p[i] == p[j]) continue; return p[i] < p[j];
        } break;
      case 3: // SW
      for (int r = Nm1; r >= 0; r--)
        for (int c = N-r; c < N; c++) {
          int i = r*N+c, j = N*(N-c)-r-1;
          if (p[i] == p[j]) continue; return p[i] > p[j];
        } break;
    default: assert(false);
  }
  return true;
} // rotateInt
//-------------------------------------------------------------------------------

bool uniqueCornerSbyte(Sbyte *p, Sbyte *q, Sbyte *min, int minC) {
//   -----------------
  int numMinCorners = 0; Sbyte *x = NULL; bool prev = false;
  if (p[0]    == minC) ++numMinCorners;
  if (p[Nm1]  == minC) ++numMinCorners;
  if (p[NNm1] == minC) ++numMinCorners;
  if (p[NNmN] == minC) ++numMinCorners;

  if (numMinCorners > 1) {
    if (p[0] == minC) {
      prev = true;
      if ((p[1] < p[N]) || ((p[1] == p[N]) && rotateSbyte(p, 0)))
         for (int i = 0; i < NN; i++) min[i] = p[i];  // in Frenicle form
      else                   
        for (int r = 0; r < N; r++)   // reflect in YX
          for (int c = 0; c < N; c++)
            min[c*N+r] = p[r*N+c];
    }
    if (p[Nm1] == minC) {
      x = prev ? q : min;
      if ((p[NpNm1] < p[Nm2]) ||
          ((p[NpNm1] == p[Nm2]) && rotateSbyte(p, 1)))
        for (int r = 0; r < N; r++)   // rotate -90
          for (int c = 0; c < N; c++)
            x[(Nm1-c)*N+r] = p[r*N+c];
      else  
        for (int r = 0; r < N; r++)   // reflect in Y
          for (int c = 0; c < N; c++)
            x[r*N+Nm1-c] = p[r*N+c];
      if (prev) {
        if (cmpSquaresSbyte(&q, &min) < 0) { q = min; min = x; }
      } else {
        prev = true;
      }
    }
    if (p[NNm1] == minC) {
      x = prev ? q : min;
      if ((p[NNm2] < p[NNmNm1]) ||
          ((p[NNm2] == p[NNmNm1]) && rotateSbyte(p, 2)))
        for (int r = 0; r < N; r++)   // rotate 180
          for (int c = 0; c < N; c++)
            x[(Nm1-r)*N+Nm1-c] = p[r*N+c];
      else
        for (int r = 0; r < N; r++)   // reflect in XY
          for (int c = 0; c < N; c++)
            x[(Nm1-c)*N+Nm1-r] = p[r*N+c];
      if (prev) {
        if (cmpSquaresSbyte(&q, &min) < 0) { q = min; min = x; }
      } else {
        prev = true;
      }
    }
    if (p[NNmN] == minC ) {
      if ((p[Nm2N] < p[NNmNp1]) ||
          ((p[Nm2N] == p[NNmNp1]) && rotateSbyte(p, 3)))
        for (int r = 0; r < N; r++)   // rotate 90
          for (int c = 0; c < N; c++)
            q[c*N+Nm1-r] = p[r*N+c];
      else                         
        for (int r = 0; r < N; r++)  // reflect in X
          for (int c = 0; c < N; c++)
            q[(Nm1-r)*N+c] = p[r*N+c];
      if (cmpSquaresSbyte(&q, &min) < 0) min = q;
    }
    for (int i = 0; i < NN; i++) p[i] = min[i];
    return false;
  }
  return true;
} // uniqueCornerSbyte

bool uniqueCornerUbyte(Ubyte *p, Ubyte *q, Ubyte *min, int minC) {
//   -----------------
  int numMinCorners = 0; Ubyte *x = NULL; bool prev = false;
  if (p[0]    == minC) ++numMinCorners;
  if (p[Nm1]  == minC) ++numMinCorners;
  if (p[NNm1] == minC) ++numMinCorners;
  if (p[NNmN] == minC) ++numMinCorners;

  if (numMinCorners > 1) {
    if (p[0] == minC) {
      prev = true;
      if ((p[1] < p[N]) || ((p[1] == p[N]) && rotateUbyte(p, 0)))
         for (int i = 0; i < NN; i++) min[i] = p[i];  // in Frenicle form
      else                   
        for (int r = 0; r < N; r++)   // reflect in YX
          for (int c = 0; c < N; c++)
            min[c*N+r] = p[r*N+c];
    }
    if (p[Nm1] == minC) {
      x = prev ? q : min;
      if ((p[NpNm1] < p[Nm2]) ||
          ((p[NpNm1] == p[Nm2]) && rotateUbyte(p, 1)))
        for (int r = 0; r < N; r++)   // rotate -90
          for (int c = 0; c < N; c++)
            x[(Nm1-c)*N+r] = p[r*N+c];
      else  
        for (int r = 0; r < N; r++)   // reflect in Y
          for (int c = 0; c < N; c++)
            x[r*N+Nm1-c] = p[r*N+c];
      if (prev) {
        if (cmpSquaresUbyte(&q, &min) < 0) { q = min; min = x; }
      } else {
        prev = true;
      }
    }
    if (p[NNm1] == minC) {
      x = prev ? q : min;
      if ((p[NNm2] < p[NNmNm1]) ||
          ((p[NNm2] == p[NNmNm1]) && rotateUbyte(p, 2)))
        for (int r = 0; r < N; r++)   // rotate 180
          for (int c = 0; c < N; c++)
            x[(Nm1-r)*N+Nm1-c] = p[r*N+c];
      else
        for (int r = 0; r < N; r++)   // reflect in XY
          for (int c = 0; c < N; c++)
            x[(Nm1-c)*N+Nm1-r] = p[r*N+c];
      if (prev) {
        if (cmpSquaresUbyte(&q, &min) < 0) { q = min; min = x; }
      } else {
        prev = true;
      }
    }
    if (p[NNmN] == minC ) {
      if ((p[Nm2N] < p[NNmNp1]) ||
          ((p[Nm2N] == p[NNmNp1]) && rotateUbyte(p, 3)))
        for (int r = 0; r < N; r++)   // rotate 90
          for (int c = 0; c < N; c++)
            q[c*N+Nm1-r] = p[r*N+c];
      else                         
        for (int r = 0; r < N; r++)  // reflect in X
          for (int c = 0; c < N; c++)
            q[(Nm1-r)*N+c] = p[r*N+c];
      if (cmpSquaresUbyte(&q, &min) < 0) min = q;
    }
    for (int i = 0; i < NN; i++) p[i] = min[i];
    return false;
  }
  return true;
} // uniqueCornerUbyte

bool uniqueCornerSshort(Sshort *p, Sshort *q, Sshort *min, int minC) {
//   ------------------
  int numMinCorners = 0; Sshort *x = NULL; bool prev = false;
  if (p[0]    == minC) ++numMinCorners;
  if (p[Nm1]  == minC) ++numMinCorners;
  if (p[NNm1] == minC) ++numMinCorners;
  if (p[NNmN] == minC) ++numMinCorners;

  if (numMinCorners > 1) {
    if (p[0] == minC) {
      prev = true;
      if ((p[1] < p[N]) || ((p[1] == p[N]) && rotateSshort(p, 0)))
         for (int i = 0; i < NN; i++) min[i] = p[i];  // in Frenicle form
      else                   
        for (int r = 0; r < N; r++)   // reflect in YX
          for (int c = 0; c < N; c++)
            min[c*N+r] = p[r*N+c];
    }
    if (p[Nm1] == minC) {
      x = prev ? q : min;
      if ((p[NpNm1] < p[Nm2]) ||
          ((p[NpNm1] == p[Nm2]) && rotateSshort(p, 1)))
        for (int r = 0; r < N; r++)   // rotate -90
          for (int c = 0; c < N; c++)
            x[(Nm1-c)*N+r] = p[r*N+c];
      else  
        for (int r = 0; r < N; r++)   // reflect in Y
          for (int c = 0; c < N; c++)
            x[r*N+Nm1-c] = p[r*N+c];
      if (prev) {
        if (cmpSquaresSshort(&q, &min) < 0) { q = min; min = x; }
      } else {
        prev = true;
      }
    }
    if (p[NNm1] == minC) {
      x = prev ? q : min;
      if ((p[NNm2] < p[NNmNm1]) ||
          ((p[NNm2] == p[NNmNm1]) && rotateSshort(p, 2)))
        for (int r = 0; r < N; r++)   // rotate 180
          for (int c = 0; c < N; c++)
            x[(Nm1-r)*N+Nm1-c] = p[r*N+c];
      else
        for (int r = 0; r < N; r++)   // reflect in XY
          for (int c = 0; c < N; c++)
            x[(Nm1-c)*N+Nm1-r] = p[r*N+c];
      if (prev) {
        if (cmpSquaresSshort(&q, &min) < 0) { q = min; min = x; }
      } else {
        prev = true;
      }
    }
    if (p[NNmN] == minC ) {
      if ((p[Nm2N] < p[NNmNp1]) ||
          ((p[Nm2N] == p[NNmNp1]) && rotateSshort(p, 3)))
        for (int r = 0; r < N; r++)   // rotate 90
          for (int c = 0; c < N; c++)
            q[c*N+Nm1-r] = p[r*N+c];
      else                         
        for (int r = 0; r < N; r++)  // reflect in X
          for (int c = 0; c < N; c++)
            q[(Nm1-r)*N+c] = p[r*N+c];
      if (cmpSquaresSshort(&q, &min) < 0) min = q;
    }
    for (int i = 0; i < NN; i++) p[i] = min[i];
    return false;
  }
  return true;
} // uniqueCornerSshort

bool uniqueCornerUshort(Ushort *p, Ushort *q, Ushort *min, int minC) {
//   ------------------
  int numMinCorners = 0; Ushort *x = NULL; bool prev = false;
  if (p[0]    == minC) ++numMinCorners;
  if (p[Nm1]  == minC) ++numMinCorners;
  if (p[NNm1] == minC) ++numMinCorners;
  if (p[NNmN] == minC) ++numMinCorners;

  if (numMinCorners > 1) {
    if (p[0] == minC) {
      prev = true;
      if ((p[1] < p[N]) || ((p[1] == p[N]) && rotateUshort(p, 0)))
         for (int i = 0; i < NN; i++) min[i] = p[i];  // in Frenicle form
      else                   
        for (int r = 0; r < N; r++)   // reflect in YX
          for (int c = 0; c < N; c++)
            min[c*N+r] = p[r*N+c];
    }
    if (p[Nm1] == minC) {
      x = prev ? q : min;
      if ((p[NpNm1] < p[Nm2]) ||
          ((p[NpNm1] == p[Nm2]) && rotateUshort(p, 1)))
        for (int r = 0; r < N; r++)   // rotate -90
          for (int c = 0; c < N; c++)
            x[(Nm1-c)*N+r] = p[r*N+c];
      else  
        for (int r = 0; r < N; r++)   // reflect in Y
          for (int c = 0; c < N; c++)
            x[r*N+Nm1-c] = p[r*N+c];
      if (prev) {
        if (cmpSquaresUshort(&q, &min) < 0) { q = min; min = x; }
      } else {
        prev = true;
      }
    }
    if (p[NNm1] == minC) {
      x = prev ? q : min;
      if ((p[NNm2] < p[NNmNm1]) ||
          ((p[NNm2] == p[NNmNm1]) && rotateUshort(p, 2)))
        for (int r = 0; r < N; r++)   // rotate 180
          for (int c = 0; c < N; c++)
            x[(Nm1-r)*N+Nm1-c] = p[r*N+c];
      else
        for (int r = 0; r < N; r++)   // reflect in XY
          for (int c = 0; c < N; c++)
            x[(Nm1-c)*N+Nm1-r] = p[r*N+c];
      if (prev) {
        if (cmpSquaresUshort(&q, &min) < 0) { q = min; min = x; }
      } else {
        prev = true;
      }
    }
    if (p[NNmN] == minC ) {
      if ((p[Nm2N] < p[NNmNp1]) ||
          ((p[Nm2N] == p[NNmNp1]) && rotateUshort(p, 3)))
        for (int r = 0; r < N; r++)   // rotate 90
          for (int c = 0; c < N; c++)
            q[c*N+Nm1-r] = p[r*N+c];
      else                         
        for (int r = 0; r < N; r++)  // reflect in X
          for (int c = 0; c < N; c++)
            q[(Nm1-r)*N+c] = p[r*N+c];
      if (cmpSquaresUshort(&q, &min) < 0) min = q;
    }
    for (int i = 0; i < NN; i++) p[i] = min[i];
    return false;
  }
  return true;
} // uniqueCornerUshort

bool uniqueCornerInt(int *p, int *q, int *min, int minC) {
//   ---------------
  int numMinCorners = 0; int *x = NULL; bool prev = false;
  if (p[0]    == minC) ++numMinCorners;
  if (p[Nm1]  == minC) ++numMinCorners;
  if (p[NNm1] == minC) ++numMinCorners;
  if (p[NNmN] == minC) ++numMinCorners;

  if (numMinCorners > 1) {
    if (p[0] == minC) {
      prev = true;
      if ((p[1] < p[N]) || ((p[1] == p[N]) && rotateInt(p, 0)))
         for (int i = 0; i < NN; i++) min[i] = p[i];  // in Frenicle form
      else                   
        for (int r = 0; r < N; r++)   // reflect in YX
          for (int c = 0; c < N; c++)
            min[c*N+r] = p[r*N+c];
    }
    if (p[Nm1] == minC) {
      x = prev ? q : min;
      if ((p[NpNm1] < p[Nm2]) ||
          ((p[NpNm1] == p[Nm2]) && rotateInt(p, 1)))
        for (int r = 0; r < N; r++)   // rotate -90
          for (int c = 0; c < N; c++)
            x[(Nm1-c)*N+r] = p[r*N+c];
      else  
        for (int r = 0; r < N; r++)   // reflect in Y
          for (int c = 0; c < N; c++)
            x[r*N+Nm1-c] = p[r*N+c];
      if (prev) {
        if (cmpSquaresInt(&q, &min) < 0) { q = min; min = x; }
      } else {
        prev = true;
      }
    }
    if (p[NNm1] == minC) {
      x = prev ? q : min;
      if ((p[NNm2] < p[NNmNm1]) ||
          ((p[NNm2] == p[NNmNm1]) && rotateInt(p, 2)))
        for (int r = 0; r < N; r++)   // rotate 180
          for (int c = 0; c < N; c++)
            x[(Nm1-r)*N+Nm1-c] = p[r*N+c];
      else
        for (int r = 0; r < N; r++)   // reflect in XY
          for (int c = 0; c < N; c++)
            x[(Nm1-c)*N+Nm1-r] = p[r*N+c];
      if (prev) {
        if (cmpSquaresInt(&q, &min) < 0) { q = min; min = x; }
      } else {
        prev = true;
      }
    }
    if (p[NNmN] == minC ) {
      if ((p[Nm2N] < p[NNmNp1]) ||
          ((p[Nm2N] == p[NNmNp1]) && rotateInt(p, 3)))
        for (int r = 0; r < N; r++)   // rotate 90
          for (int c = 0; c < N; c++)
            q[c*N+Nm1-r] = p[r*N+c];
      else                         
        for (int r = 0; r < N; r++)  // reflect in X
          for (int c = 0; c < N; c++)
            q[(Nm1-r)*N+c] = p[r*N+c];
      if (cmpSquaresInt(&q, &min) < 0) min = q;
    }
    for (int i = 0; i < NN; i++) p[i] = min[i];
    return false;
  }
  return true;
} // uniqueCornerInt
//-------------------------------------------------------------------------------

void putInFrenicleFormSbyte(Sbyte *q, Sbyte *q2) {
//   ----------------------
  for (int i = 0; i < sIndex; i++) {
    bool inFrenicleForm = false;
    Sbyte *p = ((Sbyte **)squares)[i];
    Sbyte min1 = min(p[0], p[Nm1]), min2 = min(p[NNmN], p[NNm1]);
    Sbyte minC = min(min1, min2);

    if (uniqueCornerSbyte(p, q, q2, minC)) {
      if (p[0] == minC) {
        if ((p[1] < p[N]) || ((p[1] == p[N]) && rotateSbyte(p, 0)))
           inFrenicleForm = true;
        else                   
          for (int r = 0; r < N; r++)   // reflect in YX
            for (int c = 0; c < N; c++)
              q[c*N+r] = p[r*N+c];
      } else if (p[Nm1] == minC) {
        if ((p[NpNm1] < p[Nm2]) ||
            ((p[NpNm1] == p[Nm2]) && rotateSbyte(p, 1)))
          for (int r = 0; r < N; r++)   // rotate -90
            for (int c = 0; c < N; c++)
              q[(Nm1-c)*N+r] = p[r*N+c];
        else  
          for (int r = 0; r < N; r++)   // reflect in Y
            for (int c = 0; c < N; c++)
              q[r*N+Nm1-c] = p[r*N+c];      
      } else if (p[NNm1] == minC) {
        if ((p[NNm2] < p[NNmNm1]) ||
            ((p[NNm2] == p[NNmNm1]) && rotateSbyte(p, 2)))
          for (int r = 0; r < N; r++)   // rotate 180
            for (int c = 0; c < N; c++)
              q[(Nm1-r)*N+Nm1-c] = p[r*N+c];
        else
          for (int r = 0; r < N; r++)   // reflect in XY
            for (int c = 0; c < N; c++)
              q[(Nm1-c)*N+Nm1-r] = p[r*N+c];
      } else { // p[NNmN] == minC 
        if ((p[Nm2N] < p[NNmNp1]) ||
            ((p[Nm2N] == p[NNmNp1]) && rotateSbyte(p, 3)))
          for (int r = 0; r < N; r++)   // rotate 90
            for (int c = 0; c < N; c++)
              q[c*N+Nm1-r] = p[r*N+c];
        else                         
          for (int r = 0; r < N; r++)  // reflect in X
            for (int c = 0; c < N; c++)
              q[(Nm1-r)*N+c] = p[r*N+c];
      }

      if (!inFrenicleForm)
        for (int i = 0; i < NN; i++) p[i] = q[i];
    }
  }
} // putInFrenicleFormSbyte

void putInFrenicleFormUbyte(Ubyte *q, Ubyte *q2) {
//   ----------------------
  for (int i = 0; i < sIndex; i++) {
    bool inFrenicleForm = false;
    Ubyte *p = ((Ubyte **)squares)[i];
    Ubyte min1 = min(p[0], p[Nm1]), min2 = min(p[NNmN], p[NNm1]);
    Ubyte minC = min(min1, min2);

    if (uniqueCornerUbyte(p, q, q2, minC)) {
      if (p[0] == minC) {
        if ((p[1] < p[N]) || ((p[1] == p[N]) && rotateUbyte(p, 0)))
           inFrenicleForm = true;
        else                   
          for (int r = 0; r < N; r++)   // reflect in YX
            for (int c = 0; c < N; c++)
              q[c*N+r] = p[r*N+c];
      } else if (p[Nm1] == minC) {
        if ((p[NpNm1] < p[Nm2]) ||
            ((p[NpNm1] == p[Nm2]) && rotateUbyte(p, 1)))
          for (int r = 0; r < N; r++)   // rotate -90
            for (int c = 0; c < N; c++)
              q[(Nm1-c)*N+r] = p[r*N+c];
        else  
          for (int r = 0; r < N; r++)   // reflect in Y
            for (int c = 0; c < N; c++)
              q[r*N+Nm1-c] = p[r*N+c];      
      } else if (p[NNm1] == minC) {
        if ((p[NNm2] < p[NNmNm1]) ||
            ((p[NNm2] == p[NNmNm1]) && rotateUbyte(p, 2)))
          for (int r = 0; r < N; r++)   // rotate 180
            for (int c = 0; c < N; c++)
              q[(Nm1-r)*N+Nm1-c] = p[r*N+c];
        else
          for (int r = 0; r < N; r++)   // reflect in XY
            for (int c = 0; c < N; c++)
              q[(Nm1-c)*N+Nm1-r] = p[r*N+c];
      } else { // p[NNmN] == minC 
        if ((p[Nm2N] < p[NNmNp1]) ||
            ((p[Nm2N] == p[NNmNp1]) && rotateUbyte(p, 3)))
          for (int r = 0; r < N; r++)   // rotate 90
            for (int c = 0; c < N; c++)
              q[c*N+Nm1-r] = p[r*N+c];
        else                         
          for (int r = 0; r < N; r++)  // reflect in X
            for (int c = 0; c < N; c++)
              q[(Nm1-r)*N+c] = p[r*N+c];
      }

      if (!inFrenicleForm)
        for (int i = 0; i < NN; i++) p[i] = q[i];
    }
  }
} // putInFrenicleFormUbyte

void putInFrenicleFormSshort(Sshort *q, Sshort *q2) {
//   -----------------------
  for (int i = 0; i < sIndex; i++) {
    bool inFrenicleForm = false;
    Sshort *p = ((Sshort **)squares)[i];
    Sshort min1 = min(p[0], p[Nm1]), min2 = min(p[NNmN], p[NNm1]);
    Sshort minC = min(min1, min2);

    if (uniqueCornerSshort(p, q, q2, minC)) {
      if (p[0] == minC) {
        if ((p[1] < p[N]) || ((p[1] == p[N]) && rotateSshort(p, 0)))
           inFrenicleForm = true;
        else                   
          for (int r = 0; r < N; r++)   // reflect in YX
            for (int c = 0; c < N; c++)
              q[c*N+r] = p[r*N+c];
      } else if (p[Nm1] == minC) {
        if ((p[NpNm1] < p[Nm2]) ||
            ((p[NpNm1] == p[Nm2]) && rotateSshort(p, 1)))
          for (int r = 0; r < N; r++)   // rotate -90
            for (int c = 0; c < N; c++)
              q[(Nm1-c)*N+r] = p[r*N+c];
        else  
          for (int r = 0; r < N; r++)   // reflect in Y
            for (int c = 0; c < N; c++)
              q[r*N+Nm1-c] = p[r*N+c];      
      } else if (p[NNm1] == minC) {
        if ((p[NNm2] < p[NNmNm1]) ||
            ((p[NNm2] == p[NNmNm1]) && rotateSshort(p, 2)))
          for (int r = 0; r < N; r++)   // rotate 180
            for (int c = 0; c < N; c++)
              q[(Nm1-r)*N+Nm1-c] = p[r*N+c];
        else
          for (int r = 0; r < N; r++)   // reflect in XY
            for (int c = 0; c < N; c++)
              q[(Nm1-c)*N+Nm1-r] = p[r*N+c];
      } else { // p[NNmN] == minC 
        if ((p[Nm2N] < p[NNmNp1]) ||
            ((p[Nm2N] == p[NNmNp1]) && rotateSshort(p, 3)))
          for (int r = 0; r < N; r++)   // rotate 90
            for (int c = 0; c < N; c++)
              q[c*N+Nm1-r] = p[r*N+c];
        else                         
          for (int r = 0; r < N; r++)  // reflect in X
            for (int c = 0; c < N; c++)
              q[(Nm1-r)*N+c] = p[r*N+c];
      }

      if (!inFrenicleForm)
        for (int i = 0; i < NN; i++) p[i] = q[i];
    }
  }
} // putInFrenicleFormSshort

void putInFrenicleFormUshort(Ushort *q, Ushort *q2) {
//   -----------------------
  for (int i = 0; i < sIndex; i++) {
    bool inFrenicleForm = false;
    Ushort *p = ((Ushort **)squares)[i];
    Ushort min1 = min(p[0], p[Nm1]), min2 = min(p[NNmN], p[NNm1]);
    Ushort minC = min(min1, min2);

    if (uniqueCornerUshort(p, q, q2, minC)) {
      if (p[0] == minC) {
        if ((p[1] < p[N]) || ((p[1] == p[N]) && rotateUshort(p, 0)))
           inFrenicleForm = true;
        else                   
          for (int r = 0; r < N; r++)   // reflect in YX
            for (int c = 0; c < N; c++)
              q[c*N+r] = p[r*N+c];
      } else if (p[Nm1] == minC) {
        if ((p[NpNm1] < p[Nm2]) ||
            ((p[NpNm1] == p[Nm2]) && rotateUshort(p, 1)))
          for (int r = 0; r < N; r++)   // rotate -90
            for (int c = 0; c < N; c++)
              q[(Nm1-c)*N+r] = p[r*N+c];
        else  
          for (int r = 0; r < N; r++)   // reflect in Y
            for (int c = 0; c < N; c++)
              q[r*N+Nm1-c] = p[r*N+c];      
      } else if (p[NNm1] == minC) {
        if ((p[NNm2] < p[NNmNm1]) ||
            ((p[NNm2] == p[NNmNm1]) && rotateUshort(p, 2)))
          for (int r = 0; r < N; r++)   // rotate 180
            for (int c = 0; c < N; c++)
              q[(Nm1-r)*N+Nm1-c] = p[r*N+c];
        else
          for (int r = 0; r < N; r++)   // reflect in XY
            for (int c = 0; c < N; c++)
              q[(Nm1-c)*N+Nm1-r] = p[r*N+c];
      } else { // p[NNmN] == minC 
        if ((p[Nm2N] < p[NNmNp1]) ||
            ((p[Nm2N] == p[NNmNp1]) && rotateUshort(p, 3)))
          for (int r = 0; r < N; r++)   // rotate 90
            for (int c = 0; c < N; c++)
              q[c*N+Nm1-r] = p[r*N+c];
        else                         
          for (int r = 0; r < N; r++)  // reflect in X
            for (int c = 0; c < N; c++)
              q[(Nm1-r)*N+c] = p[r*N+c];
      }

      if (!inFrenicleForm)
        for (int i = 0; i < NN; i++) p[i] = q[i];
    }
  }
} // putInFrenicleFormUshort

void putInFrenicleFormInt(int *q, int *q2) {
//   --------------------
  for (int i = 0; i < sIndex; i++) {
    bool inFrenicleForm = false;
    int *p = ((int **)squares)[i];
    int min1 = min(p[0], p[Nm1]), min2 = min(p[NNmN], p[NNm1]);
    int minC = min(min1, min2);

    if (uniqueCornerInt(p, q, q2, minC)) {
      if (p[0] == minC) {
        if ((p[1] < p[N]) || ((p[1] == p[N]) && rotateInt(p, 0)))
           inFrenicleForm = true;
        else                   
          for (int r = 0; r < N; r++)   // reflect in YX
            for (int c = 0; c < N; c++)
              q[c*N+r] = p[r*N+c];
      } else if (p[Nm1] == minC) {
        if ((p[NpNm1] < p[Nm2]) ||
            ((p[NpNm1] == p[Nm2]) && rotateInt(p, 1)))
          for (int r = 0; r < N; r++)   // rotate -90
            for (int c = 0; c < N; c++)
              q[(Nm1-c)*N+r] = p[r*N+c];
        else  
          for (int r = 0; r < N; r++)   // reflect in Y
            for (int c = 0; c < N; c++)
              q[r*N+Nm1-c] = p[r*N+c];      
      } else if (p[NNm1] == minC) {
        if ((p[NNm2] < p[NNmNm1]) ||
            ((p[NNm2] == p[NNmNm1]) && rotateInt(p, 2)))
          for (int r = 0; r < N; r++)   // rotate 180
            for (int c = 0; c < N; c++)
              q[(Nm1-r)*N+Nm1-c] = p[r*N+c];
        else
          for (int r = 0; r < N; r++)   // reflect in XY
            for (int c = 0; c < N; c++)
              q[(Nm1-c)*N+Nm1-r] = p[r*N+c];
      } else { // p[NNmN] == minC 
        if ((p[Nm2N] < p[NNmNp1]) ||
            ((p[Nm2N] == p[NNmNp1]) && rotateInt(p, 3)))
          for (int r = 0; r < N; r++)   // rotate 90
            for (int c = 0; c < N; c++)
              q[c*N+Nm1-r] = p[r*N+c];
        else                         
          for (int r = 0; r < N; r++)  // reflect in X
            for (int c = 0; c < N; c++)
              q[(Nm1-r)*N+c] = p[r*N+c];
      }

      if (!inFrenicleForm)
        for (int i = 0; i < NN; i++) p[i] = q[i];
    }
  }
} // putInFrenicleFormInt

bool putInFrenicleForm() {
//   -----------------
  printf(".. putting squares in Frenicle form\n");
  void *copy1 = malloc(squareSize), *copy2 = malloc(squareSize);
  if ((copy1 == NULL) || (copy2 == NULL)) {
    printf(allocationFailed);
    if (copy1 != NULL) free(copy1); if (copy2 != NULL) free(copy2);
    return false;
  } else {
    switch (numberBytes) {
      case 1:
        if (signedNumbers)
          putInFrenicleFormSbyte((Sbyte *)copy1, (Sbyte *)copy2);
        else
          putInFrenicleFormUbyte((Ubyte *)copy1, (Ubyte *)copy2);
        break;
      case 2:
         if (signedNumbers)
          putInFrenicleFormSshort((Sshort *)copy1, (Sshort *)copy2);
        else
          putInFrenicleFormUshort((Ushort *)copy1, (Ushort *)copy2);
        break;
      case 4:
        putInFrenicleFormInt((int *)copy1, (int *)copy2);  break;
      default: assert(false);
    }
    free(copy1); free(copy2);
    return true;
  }
} // putInFrenicleForm
//---------------------------------------------------------------------------

enum readStatus { readAllocFail, readError, readOORM, readOORP, readOK} ;
const char *readErrorMsg = "\a\nError reading square from file.\n";
readStatus readSquaresSbyte(FILE *rfp, int incr) {
//         ---------------
  do {
    if ( (sIndex == numSquaresAllocated) &&
       (!allocateStore(numBlocksAllocated + numBlocksAllocated)) )
      return readAllocFail;
    int rv = 0; Sbyte *p = (Sbyte *)squares[sIndex++];
    if (incr == 0) {
      for (int i = 0; i < NN; i++) {
        int v;  rv = fscanf_s(rfp, "%ld", &v);
        if (rv != 1)
          if ( (rv == EOF) && (p == squares[--sIndex]) ) return readOK;
          else { printf(readErrorMsg); return readError; }
        if (v < smallestRead)
          if ( (smallestRead = v) < SCHAR_MIN) return readOORM; 
        if (v > biggestRead)
          if ( (biggestRead  = v) > SCHAR_MAX) return readOORP;
        *p++ = v;
      }
    } else {
      int count = 0, v[7];
      while (count < NN) {
        switch (incr) {
          case 7:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld %ld %ld",
                       &v[0], &v[1], &v[2], &v[3], &v[4], &v[5],
                       &v[6]) ) == incr) 
              for (int a = 0; a <= 6; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SCHAR_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 6:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) ) == incr)
              for (int a = 0; a <= 5; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SCHAR_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 5:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3], &v[4]) ) == incr)
              for (int a = 0; a <= 4; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SCHAR_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 4:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3]) ) == incr)
              for (int a = 0; a <= 3; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SCHAR_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 3:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld",
                         &v[0], &v[1], &v[2]) ) == incr)
              for (int a = 0; a <= 2; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SCHAR_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 2:
            if ( (rv = fscanf_s(rfp, "%ld %ld", &v[0], &v[1]) ) == incr)
              for (int a = 0; a <= 1; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SCHAR_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          default: assert(false);
        }
        if (rv != incr)
          if ( (rv == EOF) && (p == squares[--sIndex]) ) return readOK;
          else { printf(readErrorMsg); return readError; }
        count += incr; p += incr; 
      } 
    }
  } while (true);
  return readOK;
} // readSquaresSbyte

readStatus readSquaresUbyte(FILE *rfp, int incr) {
//         ----------------
  do {
    if ( (sIndex == numSquaresAllocated) &&
       (!allocateStore(numBlocksAllocated + numBlocksAllocated)) )
      return readAllocFail;

    int rv = 0; Ubyte *p = (Ubyte *)squares[sIndex++];
    if (incr == 0) {
      for (int i = 0; i < NN; i++) {
        int v;  rv = fscanf_s(rfp, "%ld", &v);
        if (rv != 1)
          if ( (rv == EOF) && (p == squares[--sIndex]) ) return readOK;
          else { printf(readErrorMsg); return readError; }
        if (v < smallestRead)
          if ( (smallestRead = v) < 0) return readOORM; 
        if (v > biggestRead)
          if ( (biggestRead  = v) > UCHAR_MAX) return readOORP;
        *p++ = v;
      }
    } else {
      int count = 0, v[7];
      while (count < NN) {
        switch (incr) {
          case 7:
           if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld %ld %ld",
                        &v[0], &v[1], &v[2], &v[3], &v[4], &v[5],
                        &v[6]) ) == incr) 
              for (int a = 0; a <= 6; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > UCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 6:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) ) == incr)
              for (int a = 0; a <= 5; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > UCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 5:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3], &v[4]) ) == incr)
              for (int a = 0; a <= 4; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > UCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 4:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3]) ) == incr)
              for (int a = 0; a <= 3; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > UCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 3:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld",
                         &v[0], &v[1], &v[2]) ) == incr)
              for (int a = 0; a <= 2; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > UCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 2:
            if ( (rv = fscanf_s(rfp, "%ld %ld", &v[0], &v[1]) ) == incr)
              for (int a = 0; a <= 1; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > UCHAR_MAX) return readOORP;
                p[a] = x;
              }
            break;
          default: assert(false);
        }
        if (rv != incr)
          if ( (rv == EOF) && (p == squares[--sIndex]) ) return readOK;
          else { printf(readErrorMsg); return readError; }
        count += incr; p += incr; 
      } 
    }
  } while (true);
  return readOK;
} // readSquaresUbyte

readStatus readSquaresSshort(FILE *rfp, int incr) {
//         -----------------
  do {
    if ( (sIndex == numSquaresAllocated) &&
       (!allocateStore(numBlocksAllocated + numBlocksAllocated)) )
      return readAllocFail;
    int rv = 0; Sshort *p = (Sshort *)squares[sIndex++];
    if (incr == 0) {
      for (int i = 0; i < NN; i++) {
        int v;  rv = fscanf_s(rfp, "%ld", &v);
        if (rv != 1)
          if ( (rv == EOF) && (p == squares[--sIndex]) ) return readOK;
          else { printf(readErrorMsg); return readError; }
        if (v < smallestRead)
          if ( (smallestRead = v) < SHRT_MIN) return readOORM; 
        if (v > biggestRead)
          if ( (biggestRead  = v) > SHRT_MAX) return readOORP;
        *p++ = v;
      }
    } else {
      int count = 0, v[7];
      while (count < NN) {
        switch (incr) {
          case 7:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3], &v[4], &v[5],
                         &v[6]) ) == incr) 
              for (int a = 0; a <= 6; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SHRT_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 6:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) ) == incr)
              for (int a = 0; a <= 5; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SHRT_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 5:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3], &v[4]) ) == incr)
              for (int a = 0; a <= 4; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SHRT_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 4:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3]) ) == incr)
              for (int a = 0; a <= 3; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SHRT_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 3:
              if ( (rv = fscanf_s(rfp, "%ld %ld %ld",
                           &v[0], &v[1], &v[2]) ) == incr)
              for (int a = 0; a <= 2; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SHRT_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 2:
            if ( (rv = fscanf_s(rfp, "%ld %ld", &v[0], &v[1]) ) == incr)
              for (int a = 0; a <= 1; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < SHRT_MIN) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > SHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          default: assert(false);
        }
        if (rv != incr)
          if ( (rv == EOF) && (p == squares[--sIndex]) ) return readOK;
          else { printf(readErrorMsg); return readError; }
        count += incr; p += incr; 
      }
    }
  } while (true);
  return readOK;
} // readSquaresSshort

readStatus readSquaresUshort(FILE *rfp, int incr) {
//         -----------------
  do {
    if ( (sIndex == numSquaresAllocated) &&
       (!allocateStore(numBlocksAllocated + numBlocksAllocated)) )
      return readAllocFail;
    int rv = 0; Ushort *p = (Ushort *)squares[sIndex++];
    if (incr == 0) {
      for (int i = 0; i < NN; i++) {
        int v;  rv = fscanf_s(rfp, "%ld", &v);
        if (rv != 1)
          if ( (rv == EOF) && (p == squares[--sIndex]) ) return readOK;
          else { printf(readErrorMsg); return readError; }
        if (v < smallestRead)
          if ( (smallestRead = v) < 0) return readOORM; 
        if (v > biggestRead)
          if ( (biggestRead  = v) > USHRT_MAX) return readOORP;
        *p++ = v;
      }
    } else {
      int count = 0, v[7];
      while (count < NN) {
        switch (incr) {
          case 7:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3], &v[4], &v[5],
                         &v[6]) ) == incr) 
              for (int a = 0; a <= 6; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > USHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 6:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) ) == incr)
              for (int a = 0; a <= 5; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > USHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 5:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3], &v[4]) ) == incr)
              for (int a = 0; a <= 4; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > USHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 4:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld",
                         &v[0], &v[1], &v[2], &v[3]) ) == incr)
              for (int a = 0; a <= 3; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > USHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 3:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld",
                         &v[0], &v[1], &v[2]) ) == incr)
              for (int a = 0; a <= 2; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > USHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          case 2:
            if ( (rv = fscanf_s(rfp, "%ld %ld", &v[0], &v[1]) ) == incr)
              for (int a = 0; a <= 1; a++) {
                int x = v[a];
                if (x < smallestRead)
                  if ( (smallestRead = x) < 0) return readOORM; 
                if (x > biggestRead)
                  if ( (biggestRead  = x) > USHRT_MAX) return readOORP;
                p[a] = x;
              }
            break;
          default: assert(false);
        }
        if (rv != incr)
          if ( (rv == EOF) && (p == squares[--sIndex]) ) return readOK;
          else { printf(readErrorMsg); return readError; }
        count += incr; p += incr; 
      }
    }
  } while (true);
  return readOK;
} // readSquaresUshort

readStatus readSquaresInt(FILE *rfp, int incr) {
//         --------------
  do {
    if ( (sIndex == numSquaresAllocated) &&
       (!allocateStore(numBlocksAllocated + numBlocksAllocated)) )
      return readAllocFail;
    int rv = 0; int *p = (int *)squares[sIndex++];
    if (incr == 0) {
      for (int i = 0; i < NN; i++) {
        int v;  rv = fscanf_s(rfp, "%ld", &v);
        if (rv != 1)
          if ( (rv == EOF) && (p == squares[--sIndex]) ) return readOK;
          else { printf(readErrorMsg); return readError; }
        *p++ = v;
        if (v < smallestRead) smallestRead = v;
        if (v > biggestRead)  biggestRead  = v;
      }
    } else {
      int count = 0;
      while (count < NN) {
        switch (incr) {
          case 7:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld %ld %ld",
                         &p[0], &p[1], &p[2], &p[3], &p[4], &p[5],
                         &p[6]) ) == incr)
              for (int a = 0; a <= 6; a++) {
                if (p[a] < smallestRead) smallestRead = p[a];
                if (p[a] > biggestRead)  biggestRead  = p[a];
              }
            break;
          case 6:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld %ld",
                         &p[0], &p[1], &p[2], &p[3], &p[4], &p[5]) ) == incr)
              for (int a = 0; a <= 5; a++) {
                if (p[a] < smallestRead) smallestRead = p[a];
                if (p[a] > biggestRead)  biggestRead  = p[a];
              }
            break;
          case 5:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld %ld",
                         &p[0], &p[1], &p[2], &p[3], &p[4]) ) == incr)
                for (int a = 0; a <= 4; a++) {
                  if (p[a] < smallestRead) smallestRead = p[a]; 
                  if (p[a] > biggestRead)  biggestRead  = p[a];
                }
            break;
          case 4:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld %ld",
                         &p[0], &p[1], &p[2], &p[3]) ) == incr)
              for (int a = 0; a <= 3; a++) {
                if (p[a] < smallestRead) smallestRead = p[a];
                if (p[a] > biggestRead)  biggestRead  = p[a];
              }
            break;
          case 3:
            if ( (rv = fscanf_s(rfp, "%ld %ld %ld",
                         &p[0], &p[1], &p[2]) ) == incr)
              for (int a = 0; a <= 2; a++) {
                if (p[a] < smallestRead) smallestRead = p[a];
                if (p[a] > biggestRead)  biggestRead  = p[a];
              }
            break;
          case 2:
            if ( (rv = fscanf_s(rfp, "%ld %ld", &p[0], &p[1]) ) == incr)
              for (int a = 0; a <= 1; a++) {
                if (p[a] < smallestRead) smallestRead = p[a];
                if (p[a] > biggestRead) biggestRead   = p[a];
              }
            break;
          default: assert(false);
        }
        if (rv != incr)
          if ( (rv == EOF) && (p == squares[--sIndex]) ) return readOK;
          else { printf(readErrorMsg); return readError; }
        count += incr; p += incr; 
      }
    }
  } while (true);
  return readOK;
} // readSquaresInt
//-------------------------------------------------------------------------------

void initGlobals(int numBytes, bool signedNums) {
//   -----------
  Nm1    = N - 1; 
  Nm1N   = Nm1 * N;
  Nm2    = N - 2;
  Nm2N   = (N - 2) * N;
  NN     = N * N;
  NNm1   = NN - 1;
  NNmN   = NN - N;
  NNmNm1 = NNmN - 1;
  NNmNp1 = NNmN + 1;
  NNm2   = NN - 2;
  NpNm1  = N + N - 1;

  numBlocksAllocated  = 0;
  numSquaresAllocated = 0;
  sIndex = 0;

  smallestRead = LONG_MAX;
  biggestRead  = LONG_MIN;

  signedNumbers = signedNums;

  numberBytes =
    (numBytes == 0)
      ? (NN <= UCHAR_MAX ? byteBytes
                         : NN <= USHRT_MAX ? shortBytes : intBytes) 
      : numBytes;
  squareSize  = NN * numberBytes;

  const int OneHundredMillion = 100000000;
  const int FiftyThousand     = 50000;
  squaresPerBlock =  OneHundredMillion / squareSize;
  if (squaresPerBlock > FiftyThousand)
    squaresPerBlock = FiftyThousand;
  else if (squaresPerBlock < 1)
    squaresPerBlock = 1;
  maxBlocksIncr =
    oneB/(pointerSize+squaresPerBlock*(squareSize+pointerSize));
  reportAlloc = false;
} // initGlobals
//-------------------------------------------------------------------------------

bool readSquares(FILE *rfp) {
//   -----------
  char *startMsg = ".. reading square numbers as ";
  printf("\n%s%s\n", startMsg, numberBytes == byteBytes
         ? "unsigned bytes"
         : numberBytes == shortBytes ? "unsigned shorts"
                                     : "signed ints");

  int incr = 0;
  for (int i = 7; i > 1; i--) 
    if ((N % i) == 0) { incr = i; break; }
    
  readStatus rc = readError; bool signedNums = signedNumbers;
  do {
    enum retry_t {tryNone, trySbyte, tryUbyte, trySshort, tryUshort, tryInt};
    retry_t retry = tryNone;
    switch (numberBytes) {
      case 1:
        if (signedNums) {
          rc = readSquaresSbyte(rfp, incr);
          if ( (rc == readOORM) || (rc == readOORP) ) {
            if ( (smallestRead < SHRT_MIN) || (biggestRead > SHRT_MAX) ) {
              retry = tryInt; numberBytes = intBytes;
            } else {
              retry = trySshort; numberBytes = shortBytes;
            }
          }
        } else { // not signedNums
          rc = readSquaresUbyte(rfp, incr);
          if (rc == readOORM) {
            signedNums = true;
            if ( (smallestRead >= SCHAR_MIN) && (biggestRead <= SCHAR_MAX) ) {
              retry = trySbyte; numberBytes = 1; 
            } else if ( (smallestRead >= SHRT_MIN) &&
                        (biggestRead <= SHRT_MAX) ) {
              retry = trySshort; numberBytes = shortBytes;
            } else {
              retry = tryInt; numberBytes = intBytes;
            }
          } else if (rc == readOORP) {
            if (biggestRead <= USHRT_MAX) {
              retry = tryUshort; numberBytes = shortBytes;
            } else {
              retry = tryInt; numberBytes = intBytes; signedNums = true;
            }
          }
        }
        break;
      case 2:
        if (signedNums) {
          rc = readSquaresSshort(rfp, incr);
          if ( (rc == readOORM) || (rc == readOORP) ) {
            retry = tryInt; numberBytes = intBytes;
          }
        } else {  // not signedNums
          rc = readSquaresUshort(rfp, incr);
          signedNums = true;
          if (rc == readOORM) {     
            if ( (smallestRead >= SCHAR_MIN) && (biggestRead <= SCHAR_MAX) ) {
              retry = trySbyte; numberBytes = 1; 
            } else if ( (smallestRead >= SHRT_MIN) &&
                        (biggestRead <= SHRT_MAX) ) {
              retry = trySshort; numberBytes = shortBytes;
            } else {
              retry = tryInt; numberBytes = intBytes;
            }
          } else if (rc == readOORP) {
            retry = tryInt; numberBytes = intBytes;
          }
        }
        break;
      case 4: rc = readSquaresInt(rfp, incr); break;
      default: assert(false);
    }

    if (retry == tryNone) {
      break;
    } else {
      printf("number out of range\n");
      freeStore(numBlocksAllocated);
      free(squares); squares = NULL;
      initGlobals(numberBytes, signedNums);
      if ( allocateStore(startNumBlocks) ) {
        rewind(rfp);
        switch (retry) {
          case trySbyte:
            printf("%ssigned bytes\n", startMsg); break;
          case tryUbyte: assert(false);
          case trySshort:
            printf("%ssigned shorts\n", startMsg); break;
          case tryUshort:
            printf("%sunsigned shorts\n", startMsg); break;
          case tryInt:
            printf("%ssigned ints\n", startMsg); break;
          default: assert(false);
        }
      } else {
        rc = readAllocFail;
        break;
      }
    }
  } while (true);

  if ((rc == readError) && (sIndex > 1)) {
    --sIndex; rc = readOK;
  }
  return rc == readOK;
} // readSquares
//-------------------------------------------------------------------------------

void get_rest_of_line(int c) {
//   ----------------
  if (c != '\n') do { c = getchar(); } while (c != '\n');
}

bool getY() {
//   ----
  int c;  do { c = getchar(); } while ((c == ' ') || (c == '\t') || (c == '\n'));
  get_rest_of_line(c);  return (c == 'Y') || (c == 'y');
}

bool getYorOrder(int *n) { // if user inputs size instead of 'y' or 'n', use it
//   -----------
  bool result = false; int c; *n = 0;

  do { c = getchar(); } while ((c == ' ') || (c == '\t') || (c == '\n') );
  if ( (c == 'Y') || (c == 'y') )
    result = true;
  else if ( (c != 'N') && (c != 'n') )
    if ( ('1' <= c) && (c <= '9') ) {
      int i = c - '0';
      while ( ('0' <= (c = getchar())) && (c <= '9') )
        i = i*10 + c - '0';
      *n = i; result = true;
    }   
  get_rest_of_line(c);
  return result;
} // getYorOrder

int getSize() {
//  -------
  int n = 0;  int unused = scanf_s("%d", &n);
  int c = getchar(); get_rest_of_line(c);
  return n;
}

bool getFileName(char *buf, int size) {
//   -----------
  int c, i = 0; char *s = buf;

  do { c = getchar(); }
  while ((c == ' ') || (c == '\t') || (c == '\n') ); *s = c;
  while (i++ < size) if ( (*++s = getchar()) == '\n') break;
  if (*s != '\n') {
    printf("\nFile name too long.\n"); get_rest_of_line(*s); return false;
  }
  *s = '\0';
  return true;
} // getFileName
//--------------------------------------------------------------------------

const int bufSize = 1024;
FILE *openInput(char *buf, int size) {
//    ---------
  char *rFname = NULL;

  do {
    printf("\nEnter the name of the squares file: ");
    if (getFileName(buf, size - 4)) {  // reserve 4 to add .txt if needed
      rFname = buf; break;
    } else {
       printf("\a\nCan't read the file name. Try again? y (yes) or n (no) ");
	   if (!getY()) break;
    }
  } while (true);

  FILE *rfp = NULL;
  if (rFname != NULL) {
    char *s = buf; bool txt = false;
    while (*s++ != '\0');
    while (--s != buf)
      if (*s == '.') {
        txt = (*++s == 't') && (*++s == 'x') && (*++s == 't') && (*++s == '\0');
        break;
      }
    if (!txt)  // no .txt entered, add it
      strcat_s(buf, size, ".txt");
    if (fopen_s(&rfp, rFname, "r") != 0) {
      const int msgSize = bufSize + 50; char msg[msgSize];
      sprintf_s(msg, msgSize, "\a\nCan't open for read %s", buf);
      perror(msg);
    }
  }
  return rfp;
} // openInput
//--------------------------------------------------------------------------

int openOutput(char* inFname) {
//  ----------
  const int outSize = bufSize + 50; char buf[outSize];
  const int defaultSize = bufSize + 25; char defaultName[defaultSize];
  char *s = inFname; while (*s++ != '\0');
  while (--s != inFname) if (*s == '.') { *s = '\0'; break; } // remove .txt
  sprintf_s(defaultName, defaultSize, "%sFrenicle", inFname);
  sprintf_s(buf, outSize, "%s.txt", defaultName);
  int wfd = noFD; int sub = 0;

  do {
    if (_access_s(buf, 00) == ENOENT) {
      break;
    } else {
      sprintf_s(buf, outSize, "%s_%d.txt", defaultName, ++sub);
    }
  } while (true);

  if ((wfd = _open(buf, _O_CREAT|_O_WRONLY, 0644)) == noFD) {
    char msg[outSize];
    sprintf_s(msg, outSize, "\a\nCan't open for write %s", buf);
    perror(msg);
  } else
    printf(".. writing squares to file %s\n", buf);
  return wfd;
} // openOutput
//--------------------------------------------------------------------------

void outputLocalTime() {
//   --------------
  time_t startTime = time(NULL); struct tm local;
  localtime_s(&local, &startTime);  char dateTime[100];
  size_t slen = strftime(dateTime, 100, "%A %Y-%m-%d %X %Z\n\n\0", &local);

  printf(dateTime);
} // outputLocalTime
//-----------------------------------------------------------------------------

void printElapsedTime(time_t startTime) {
//   ----------------
  int et = (int)difftime(time(NULL), startTime);
  if (et > 0)
    printf("\nelapsed time %d:%02d:%02d\n", et/3600, et%3600/60, et%60);
} // printElapsedTime
//-----------------------------------------------------------------------------

//void printSizes()
////   ----------
//{
//  printf("int %d long long %d ptr %d size_t %d\n",
//    sizeof(int), sizeof(long long), sizeof(const void *), sizeof(size_t));
//} // printSizes
//--------------------------------------------------------------------------

int main() {
//  ----
  bool another = true, inputSize = true, noWriteError;
  char *input_order = "\nInput the order of the squares: ";
  const int inSize = bufSize + 4; // +4 to add .txt if needed
  char buf[inSize];

  outputLocalTime(); // printSizes();
  do {
    if (inputSize) { printf(input_order); N = getSize(); }
    if (N <= 0) {
      printf("\aOrder must be a positive integer.\n\n");
    } else {
      initGlobals(0, (N*N) > USHRT_MAX);
      FILE *rfp = openInput(buf, inSize);

      if (rfp != NULL) {
        time_t startTime = time(NULL);
        if ( allocateStore(startNumBlocks) ) {
          if (readSquares(rfp)) {
            if (putInFrenicleForm()) {
              sortSquares();
              int wfd = openOutput(buf);

              if (wfd != noFD) {
                noWriteError = writeSquares(wfd);
                _close(wfd);
              }
            }
          }
          freeStore(numBlocksAllocated);
          free(squares); squares = NULL;
        }
        fclose(rfp);
        printElapsedTime(startTime);
      }
   	}
    if (noWriteError) {
      printf("\nContinue? input y (yes) or n (no) or the order of the squares: ");
      if (getYorOrder(&N)) inputSize = (N == 0); else another = false;
    } else {
      perror("\a\nError writing file"); another = false;
    }
  } while (another);

  printf("\n\nPress a key to close the console.");
  while (!_kbhit()) Sleep(250);
  return 0;
} // main