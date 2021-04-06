/*
 *  File:    Franklin8.cpp
 *  Author:  S Harry White
 *  Created: 2013-11-05
 *  Updated: 2014-02-24
 */

// #include "stdafx.h"
#include <conio.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <time.h>
#include <Windows.h>

const int N     = 8,
          Nd2   = N/2,       //   4
          Nd2m1 = Nd2 - 1,   //   3
          NN    = N * N,     //  64
          NNd2  = NN/2,      //  32

// square numbers 0 to NN - 1
          S2 = NN - 1,       //  63
          S4 = S2 + S2,      // 126
          S8 = S4 + S4;      // 252

const int maxX2X4X6Combos = 225, squareVariations = 4096;
int numX2X4X6Combos, countMagic, countSemiMagic;

typedef unsigned int Uint;
Uint X2X4X6Combos[maxX2X4X6Combos][Nd2m1];
struct bools { bool used[NN]; }; bools allfree;
//--------------------------------------------------------------------------

void getX2X4X6Combos() {
//   ---------------
  bools v = allfree; numX2X4X6Combos = 0;

  for (int i = 1; i < NNd2; i++) {
    v.used[i] = true;
    for (int j = 1; j < (NNd2-1); j++) if (!v.used[j]) {
      for (int k = j+1; k < NNd2; k++) if (!v.used[k]) {
        if ((i == (j+k))) {
          Uint *p = X2X4X6Combos[numX2X4X6Combos++];
          p[0] = i; p[1] = j; p[2] = k;
        }
      }
    }
    v.used[i] = false;
  }
} // getX2X4X6Combos
//--------------------------------------------------------------------------

const int bufSize = 1024;
bool openDir(char *dir) {
//   -------
  char baseName[] = "Order8Franklin"; int fd = -1, sub = 0;
  strcpy_s(dir, bufSize, baseName);
  do {
    if ( (fd = _mkdir(dir)) != -1) break;
    if (errno != EEXIST) {
      sprintf_s(baseName, bufSize, "\a\nCan't make folder %s", dir);
      perror(baseName); return false;
    }
    sprintf_s(dir, bufSize, "%s_%d", baseName, ++sub);
  } while (true);
  printf("Output files are in folder %s\n", dir);
  return true;
} // openDir
//--------------------------------------------------------------------------

int openWrite(char *dir, char *prefix) {
//  ---------
  char fName[bufSize]; int wfd = -1;
  sprintf_s(fName, bufSize, "%s\\%sSquares.txt", dir, prefix);

  if ((wfd = _open(fName, _O_CREAT|_O_WRONLY, 0644)) == -1) {
    char msg[bufSize];
    sprintf_s(msg, bufSize, "\a\nCan't open for write %s", fName);
    perror(msg);
  }
  return wfd;
} // openWrite
//--------------------------------------------------------------------------

void outputLocalTime(FILE *wfp) {
//   ---------------
  time_t startTime = time(NULL); struct tm local;
  if (localtime_s(&local, &startTime) == 0) {
    char dateTime[100];
    size_t slen = strftime(dateTime, 100, "%A %Y-%m-%d %X %Z\n\n\0", &local);
    fprintf(wfp, "\n%s", dateTime);
  }
} // outputLocalTime
//--------------------------------------------------------------------------

void printElapsedTime(FILE *wfp, time_t startTime)
//   ----------------
{
  int et = (int)difftime(time(NULL), startTime);
  if (et > 0)
    fprintf(wfp, "\nelapsed time %d:%02d:%02d\n",
      et/3600, et%3600/60, et%60);
} // printElapsedTime
//--------------------------------------------------------------------------

const int bytesPerSquare = 193,
      numMagic = 368640, numSemiMagic = numMagic + numMagic,
      sizeMagic = numMagic*bytesPerSquare,
      sizeSemiMagic = numSemiMagic*bytesPerSquare;
char bufferMagic[sizeMagic], bufferSemiMagic[sizeSemiMagic];
char *sMagic = bufferMagic, *sSemiMagic = bufferSemiMagic, *sActive = NULL;

void printSquare(Uint **x) {
//   -----------
  char *s = sActive;
  for (int r = 0; r < N; ++r) {
    for (int c = 0; c < N; ++c) {
      Uint v = x[r][c]; if (c != 0) *s++ = ' ';
      if (v < 10) { *s++ = ' ';      *s++ = v+'0';
      } else      { *s++ = v/10+'0'; *s++ = v%10+'0'; }
    }
    *s++ = '\n';
  }
  *s++ = '\n'; sActive = s;
} // printSquare
//--------------------------------------------------------------------------

void swapRows(Uint **x, int a, int b) {
//   --------
  Uint *tmp = x[a]; x[a] = x[b]; x[b] = tmp;
  printSquare(x);
} // swapRows

void swapCols(Uint **x, int a, int b) {
//   --------
  for (int r = 0; r < N; ++r) {
    int tmp = x[r][a]; x[r][a] = x[r][b]; x[r][b] = tmp;
  }
  printSquare(x);
} // swapCols
//--------------------------------------------------------------------------

void swapSideRows(Uint **x, int a, int b, bool print) {
//   ------------
  int loop = 2;
  while(loop--) {
    Uint *tmp = x[a]; x[a] = x[b]; x[b] = tmp;
    a += 2; b += 2;
  }
  if (print) printSquare(x);
} // swapSideRows

void swapSideCols(Uint **x, int a, int b, bool print) {
//   ------------
  int loop = 2;
  while(loop--) {
    for (int r = 0; r < N; ++r) {
      int tmp = x[r][a]; x[r][a] = x[r][b]; x[r][b] = tmp;
    }
    a += 2; b += 2;
  }
  if (print) printSquare(x);
} // swapSideCols
//--------------------------------------------------------------------------

void swapCols46(Uint **x) {
//   ----------
  swapCols(x, 5, 7); swapCols(x, 4, 6); swapCols(x, 5, 7);
} // swapCols46

void swapCols13(Uint **x) {
//   ----------
  swapCols46(x); swapCols(x, 1, 3); swapCols46(x);
} // swapCols13

void swapCols02(Uint **x) {
//   ----------
  swapCols13(x); swapCols(x, 0, 2); swapCols13(x);
} // swapCols02

void swapRows57(Uint **x) {
//   ----------
  swapCols02(x); swapRows(x, 5, 7); swapCols02(x);
} // swapRows57

void swapRows46(Uint **x) {
//   ----------
  swapRows57(x); swapRows(x, 4, 6); swapRows57(x);
} // swapRows46

void swapRows13(Uint **x) {
//   ----------
  swapRows46(x); swapRows(x, 1, 3); swapRows46(x);
} // swapRows13

void swapRows02(Uint **x) {
//   ----------
  swapRows13(x); swapRows(x, 0, 2); swapRows13(x);
} // swapRows02

void swapCols1357(Uint **x) {
//   ------------
  swapSideCols(x, 1, 5, true);
  swapRows02(x);
  swapSideCols(x, 1, 5, false);
} // swapCols1357

void swapCols0246(Uint **x) {
//   ------------
  swapSideCols(x, 0, 4, true); swapRows02(x);
  swapCols1357(x);
  swapSideCols(x, 0, 4, false);
} // swapCols0246

void swapRows1357(Uint **x) {
//   ------------
  swapSideRows(x, 1, 5, true); swapRows02(x);
  swapCols0246(x); swapCols1357(x);
  swapSideRows(x, 1, 5, false);
} // swapRows1357

void swapRows0246(Uint **x) {
//   ------------
  swapSideRows(x, 0, 4, true); swapRows02(x);
  swapRows1357(x); swapCols0246(x); swapCols1357(x);
  swapSideRows(x, 0, 4, false);
} // swapRows0246

void swapSides(Uint **x) {
//   ---------
  swapRows0246(x); swapRows1357(x);
  swapCols0246(x); swapCols1357(x);
} // swapSides

void printVariations(Uint **x) {
//   ---------------
  printSquare(x); swapRows02(x); swapSides(x);
} // printVariations
//--------------------------------------------------------------------------

bool isMagic;
Uint xCopy[N][N], *xRows[N] =
  { xCopy[0], xCopy[1], xCopy[2], xCopy[3],
    xCopy[4], xCopy[5], xCopy[6], xCopy[7] };
void printSquares(Uint x[NN]) {
//   ------------
  Uint *p = x;
  for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c) {
    Uint v = *p++; xCopy[r][c] = (r+c)&1 ? S2+1-v : v+1;
  }
  if (isMagic) {
    sActive = sMagic;     countMagic     += squareVariations;
  } else {
    sActive = sSemiMagic; countSemiMagic += squareVariations;
  }
  printVariations(xRows);
  if (isMagic) sMagic = sActive; else sSemiMagic = sActive;
} // printSquares
//--------------------------------------------------------------------------

void extendX40X56(Uint x[NN], bool used[NN]) {
//   ------------
  Uint maxx56 = x[16]-1;
  for (x[56] = (x[16]>>1)+1; x[56] <= maxx56; ++x[56]) if (!used[S2-x[56]]) {
    x[57]  = x[1]+x[56];  if (!used[x[57]]) {
    x[59]  = x[3]+x[56];  if (!used[x[59]]) {
    x[61]  = x[5]+x[56];  if (!used[x[61]]) {
    x[63]  = x[7]+x[56];  if (!used[x[63]]) {
      used[S2-x[56]] = true; used[x[57]] = true; used[x[59]] = true;
      used[x[61]] = true; used[x[63]] = true;
    x[58]  = x[2] +x[56]; if (!used[S2-x[58]]) {
    x[60]  = x[4] +x[56]; if (!used[S2-x[60]]) {
    x[62]  = x[6] +x[56]; if (!used[S2-x[62]]) {
      used[S2-x[58]] = true; used[S2-x[60]] = true; used[S2-x[62]] = true;
    x[40] = x[16]-x[56];
      if ((x[40]<x[56]) && (x[40]>x[8]) && !used[S2-x[40]]) {
    x[41]  = x[1]+x[40];  if (!used[x[41]]) {
    x[43]  = x[3]+x[40];  if (!used[x[43]]) {
    x[45]  = x[5]+x[40];  if (!used[x[45]]) {
    x[47]  = x[7]+x[40];  if (!used[x[47]]) {
      used[S2-x[40]] = true; used[x[41]] = true; used[x[43]] = true;
      used[x[45]] = true; used[x[47]] = true;
    x[42]  = x[2] +x[40]; if (!used[S2-x[42]]) {
    x[44]  = x[4] +x[40]; if (!used[S2-x[44]]) {
    x[46]  = x[6] +x[40]; if (!used[S2-x[46]]) {
      printSquares(x);
    }
    }
    }
      used[S2-x[40]] = false; used[x[41]] = false; used[x[43]] = false;
      used[x[45]] = false; used[x[47]] = false;
    }
    }
    }
    }
  }    
      used[S2-x[58]] = false; used[S2-x[60]] = false; used[S2-x[62]] = false;
    }
    }
    }
      used[S2-x[56]] = false; used[x[57]] = false; used[x[59]] = false;
      used[x[61]] = false; used[x[63]] = false;
    }
    }
    }
    }
  }   
} // extendX40X56
//--------------------------------------------------------------------------

void extendX5X7 (Uint x[NN], bool used[NN]) {
//   ----------
  Uint maxx7 = x[2]-1;
  for (x[7] = (x[2]>>1)+1; x[7] <= maxx7; ++x[7]) if (!used[S2-x[7]]) {
    x[15]  = x[8]+x[7];   if (!used[x[15]]) {
    x[31]  = x[24]+x[7];  if (!used[x[31]]) {
      used[S2-x[7]] = true; used[x[15]] = true; used[x[31]] = true;
    x[23]  = x[7]+x[16];  if (!used[S2-x[23]]) {
    x[39]  = x[7]+x[32];  if (!used[S2-x[39]]) {
    x[55]  = x[7]+x[48];  if (!used[S2-x[55]]) {
      used[S2-x[23]] = true; used[S2-x[39]] = true; used[S2-x[55]] = true;
    x[5] = x[2]-x[7]; if ((x[5]>x[1]) && (x[5]<x[7]) && !used[S2-x[5]]) {
    x[13]  = x[8]+x[5];   if (!used[x[13]]) {
    x[29]  = x[24]+x[5];  if (!used[x[29]]) {
      used[S2-x[5]] = true; used[x[13]] = true; used[x[29]] = true;
    x[21]  = x[5]+x[16];  if (!used[S2-x[21]]) {
    x[37]  = x[5]+x[32];  if (!used[S2-x[37]]) {
    x[53]  = x[5]+x[48];  if (!used[S2-x[53]]) {
      used[S2-x[21]] = true; used[S2-x[37]] = true; used[S2-x[53]] = true;
      extendX40X56(x, used);
      used[S2-x[21]] = false; used[S2-x[37]] = false; used[S2-x[53]] = false;
    }
    }
    }
      used[S2-x[5]] = false; used[x[13]] = false; used[x[29]] = false;
    }
    }
    }
      used[S2-x[23]] = false; used[S2-x[39]] = false; used[S2-x[55]] = false;
    }
    }
    }
      used[S2-x[7]] = false; used[x[15]] = false; used[x[31]] = false;
    }
    }
  }
} // extendX5X7
//--------------------------------------------------------------------------

void extendX8X24(Uint x[NN], bool used[NN]) {
//   -----------
  for (x[24] = (x[16]>>1)+1; x[24] <= x[16]; ++x[24]) if (!used[S2-x[24]]) {
    x[25]  = x[1]+x[24];  if (!used[x[25]]) {
    x[27]  = x[3]+x[24];  if (!used[x[27]]) {
      used[S2-x[24]] = true; used[x[25]] = true; used[x[27]] = true;
    x[26]  = x[2] +x[24]; if (!used[S2-x[26]]) {
    x[28]  = x[4] +x[24]; if (!used[S2-x[28]]) {
    x[30]  = x[6] +x[24]; if (!used[S2-x[30]]) {
      used[S2-x[26]] = true; used[S2-x[28]] = true; used[S2-x[30]] = true;
    x[8] = x[16]-x[24]; if ((x[8]<x[24]) && !used[S2-x[8]]) {
    x[9]   = x[1]+x[8]; if (!used[x[9]]) {
    x[11]  = x[3]+x[8]; if (!used[x[11]]) {
      used[S2-x[8]] = true; used[x[9]] = true; used[x[11]] = true;
    x[10]  = x[2] +x[8]; if (!used[S2-x[10]]) {
    x[12]  = x[4] +x[8]; if (!used[S2-x[12]]) {
    x[14]  = x[6] +x[8]; if (!used[S2-x[14]]) {
      used[S2-x[10]] = true; used[S2-x[12]] = true; used[S2-x[14]] = true;
      extendX5X7(x, used);
      used[S2-x[10]] = false; used[S2-x[12]] = false; used[S2-x[14]] = false;
    }
    }
    }
      used[S2-x[8]] = false; used[x[9]] = false; used[x[11]] = false;
    }
    }
    }
      used[S2-x[26]] = false; used[S2-x[28]] = false; used[S2-x[30]] = false;
    }
    }
    }
      used[S2-x[24]] = false; used[x[25]] = false; used[x[27]] = false;
    }
    }
  }
} // extendX8X24
//--------------------------------------------------------------------------

void extendX1X3(Uint x[NN], bool used[NN]) {
//   ----------
  for (x[3] = (x[2]>>1)+1; x[3] <= x[2]; ++x[3]) if (!used[S2-x[3]]) {
    x[19]  = x[3]+x[16];  if (!used[S2-x[19]]) {
    x[35]  = x[3]+x[32];  if (!used[S2-x[35]]) {
    x[51]  = x[3]+x[48];  if (!used[S2-x[51]]) {
      used[S2-x[3]] = true;  used[S2-x[19]] = true;
      used[S2-x[35]] = true; used[S2-x[51]] = true;
    x[1] = x[2]-x[3]; if ((x[1]<x[3]) && !used[S2-x[1]]) {
    x[17]  = x[1]+x[16];  if (!used[S2-x[17]]) {
    x[33]  = x[1]+x[32];  if (!used[S2-x[33]]) {
    x[49]  = x[1]+x[48];  if (!used[S2-x[49]]) {
      used[S2-x[1]] = true;  used[S2-x[17]] = true;
      used[S2-x[33]] = true; used[S2-x[49]] = true;
      extendX8X24(x, used);
      used[S2-x[1]] = false;  used[S2-x[17]] = false;
      used[S2-x[33]] = false; used[S2-x[49]] = false;
    }
    }
    }
    }
      used[S2-x[3]] = false;  used[S2-x[19]] = false;
      used[S2-x[35]] = false; used[S2-x[51]] = false;
    }
    }
    }
  }
} // extendX1X3
//--------------------------------------------------------------------------

void extendX32X48(Uint x[NN], bool used[NN]) {
//   ------------
  Uint maxx48 = x[16]-1;
  for (x[48] = (x[16]>>1)+1; x[48] <= maxx48; ++x[48]) if (!used[x[48]]) {
    x[50] = x[2] +x[48]; if (!used[x[50]]) {
    x[52] = x[4] +x[48]; if (!used[x[52]]) {
    x[54] = x[6] +x[48]; if (!used[x[54]]) {
      used[x[48]] = true; used[x[50]] = true; used[x[52]] = true;
      used[x[54]] = true;
    x[32] = x[16]-x[48]; if ((x[32]<x[48]) && !used[x[32]]) {
    x[34] = x[2] +x[32]; if (!used[x[34]]) {
    x[36] = x[4] +x[32]; if (!used[x[36]]) {
    x[38] = x[6] +x[32]; if (!used[x[38]]) {
      used[x[32]] = true; used[x[34]] = true; used[x[36]] = true;
      used[x[38]] = true;
      extendX1X3(x, used);
      used[x[32]] = false; used[x[34]] = false; used[x[36]] = false;
      used[x[38]] = false;
    }
    }
    }
    }
      used[x[48]] = false; used[x[50]] = false; used[x[52]] = false;
      used[x[54]] = false;
    }
    }
    }
  }
} // extendX32X48
//--------------------------------------------------------------------------

void extendX16(Uint x[NN], bool used[NN]) {
//   ---------
  Uint maxx16 = S2-x[2];
  for (x[16] = x[2]+1; x[16] <= maxx16; ++x[16]) if (!used[x[16]]) {
    isMagic = (x[16] == maxx16);
    x[18] = x[2]+x[16]; if (!used[x[18]]) {
    x[20] = x[4]+x[16]; if (!used[x[20]]) {
    x[22] = x[6]+x[16]; if (!used[x[22]]) {
      used[x[16]] = true; used[x[18]] = true; used[x[20]] = true;
      used[x[22]] = true;
      extendX32X48(x, used);
      used[x[16]] = false; used[x[18]] = false; used[x[20]] = false;
      used[x[22]] = false;
    }
    }
    }
  }
} // extendX16
//--------------------------------------------------------------------------

void getSquares() {
//   ----------
  getX2X4X6Combos();
  struct bools v = allfree; Uint x[NN]; x[0] = 0; v.used[0] = true;

  for (int i = 0; i < numX2X4X6Combos; ++i) {
    Uint *p = X2X4X6Combos[i];
    v.used[x[2]=p[0]] = true; v.used[x[4]=p[1]] = true;
    v.used[x[6]=p[2]] = true;
    extendX16(x, v.used);
    v.used[x[2]] = false; v.used[x[4]] = false; v.used[x[6]] = false;
  }
} // getSquares
//--------------------------------------------------------------------------

void writeSquares(int wfdM, int wfdSM) {
//   ------------
  if (_write(wfdM, bufferMagic, sizeMagic) != sizeMagic)
    perror("Error writing magic squares: ");
  if (_write(wfdSM, bufferSemiMagic, sizeSemiMagic) != sizeSemiMagic)
    perror("Error writing semimagic squares: ");
} // writeSquares
//--------------------------------------------------------------------------
          
int main() {
//  ----
  outputLocalTime(stdout); char dir[bufSize];
  if (openDir(dir)) {
    int wfdM = openWrite(dir, "Magic"), wfdSM = openWrite(dir, "SemiMagic");
    if ((wfdM != -1) && (wfdSM != -1)) {
      time_t startTime = time(NULL);
      getSquares(); writeSquares(wfdM, wfdSM);
      printf("\nMagic Squares: %u Semi-magic Squares: %u\n",
             countMagic, countSemiMagic);
      printElapsedTime(stdout, startTime);
      _close(wfdM); _close(wfdSM);
    }
  }
  printf("\nPress a key to close the console.");
  while (!_kbhit()) Sleep(250);
  return 0;
} // main