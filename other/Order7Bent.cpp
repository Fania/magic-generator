/*
 * File:    Order7Bent.cpp
 * Author:  S Harry White
 * Created: 2012-09-17
 * Updated: 2021-03-16, tidy code.
 * http://budshaw.ca/f1Programs/f1-Order7Bent.txt
 */

/*
 *  Makes the 21,446 order 7 bent diagonal magic squares.
 *  Adapted from Quick Basic program 7X7BDM-H.BAS written by
 *  Miguel Angel Amela of Argentina.
 */

// #include "stdafx.h"
#include <conio.h>
#include <errno.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

typedef unsigned int Uint;

const int N=7, NN=N*N, NNm1=NN-1,

// sums for numbers 0 to NN - 1
          S1=NNm1/2,  //  24
          S2=S1+S1,   //  48
          S3=S1+S2,   //  72
          S4=S2+S2,   //  96
          S5=S1+S4,   // 120
          S7=S3+S4;   // 168

struct bools { bool free[NN]; }; const bool F=false, T=true;
bools allFree={
  T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T,T
};
//================================================ input ============================================

void get_rest_of_line(int c) { if (c!='\n') do { c=getchar(); } while (c!='\n'); }
//   ----------------

int getInt() { int n=0, c; scanf_s("%d", &n); c=getchar(); get_rest_of_line(c); return n; }
//  ------

//=============================================== output ============================================

const int bufSize=1024; char OutputFileName[bufSize];
FILE *openOutput(int a1) {
//    ----------
  char buf[bufSize], defaultName[bufSize]; int sub=0; FILE *wfp=NULL;
  if (a1==0 ) sprintf_s(defaultName, bufSize, "Bent%da1-all", N);
  else sprintf_s(defaultName, bufSize, "Bent%da1-%02d", N, a1);
  sprintf_s(buf, bufSize, "%s.txt", defaultName);

  do {
    if (_access_s(buf, 00)==ENOENT) break;
    else sprintf_s(buf, bufSize, "%s_%d.txt", defaultName, ++sub);
  } while (T);

  if (fopen_s(&wfp, buf, "w")==0) {
    printf("\nOutput file is %s\n", buf); strcpy_s(OutputFileName, bufSize, buf);
  } else {
    char msg[bufSize];
    sprintf_s(msg, bufSize, "\a\nCan't open for (int write %s", buf); perror(msg);
  } 
  return wfp;
} // openOutput

void outputLocalTime() {
//   ---------------
  time_t startTime=time(NULL); struct tm local;
  if (localtime_s(&local, &startTime)==0) {
    char dateTime[100];
    size_t slen=strftime(dateTime, 100, "%A %Y-%m-%d %X %Z\n\0", &local);
    printf("%s\n", dateTime);
  }
} // outputLocalTime

void printCountAndElapsedTime(int count, time_t startTime) {
//   ------------------------ 
  int et=(int)difftime(time(NULL), startTime);
  printf("%5d [%2d:%02d:%02d]\n", count, et/3600, et%3600/60, et%60);
} // printCountAndElapsedTime
//============================================= get squares ============================================

int getSquares(int a1, FILE *wfp) {
//  ----------
  time_t startTime=time(NULL); const int d4=S1; bools v=allFree; v.free[d4]=F;
  int count=0, Count=0, xa1=a1==0 ? NNm1 : a1-1, ya1=a1==0 ? 0 : xa1;

  for (a1=xa1; a1>=ya1; --a1) if (v.free[a1]) { v.free[a1]=F; printf("\na1: %2d\n", a1+1);
   for (int a7=a1+1; a7<NNm1; ++a7) if (v.free[a7]) { v.free[a7]=F;
    int a4=S3-a1-a7; if (((Uint)a4<NN) && v.free[a4]) { v.free[a4]=F;
    int g4=S2-a4; if (((Uint)g4<NN) && v.free[g4]) { v.free[g4]=F;

    for (int g1=a7+1; g1<NN; ++g1) if (v.free[g1]) { v.free[g1]=F;
     int g7=S4-a1-a7-g1; if ((g7>a1) && (g7<NN) && v.free[g7]) { v.free[g7]=F;
     int d1=a7+g7-d4; if (((Uint)d1<NN) && v.free[d1]) { v.free[d1]=F;
     int d7=S2-d1; if (((Uint)d7<NN) && v.free[d7]) { v.free[d7]=F;
     int b2c3=S3-a1, xb2, yb2;
     if (b2c3<=S2) { xb2=0; yb2=b2c3; } else { xb2=b2c3-S2; yb2=S2; }

     for (int b2=xb2; b2<=yb2; ++b2) if (v.free[b2]) { v.free[b2]=F;
      int c3=b2c3-b2; if (((Uint)c3<NN) && v.free[c3]) { v.free[c3]=F;
      int b6c5=S3-a7, xb6, yb6;
      if (b6c5<=S2) { xb6=0; yb6=b6c5; } else { xb6=b6c5-S2; yb6=S2; }

      for (int b6=xb6; b6<=yb6; ++b6) if (v.free[b6]) { v.free[b6]=F;
       int c5=b6c5-b6; if (((Uint)c5<NN) && v.free[c5]) { v.free[c5]=F;
       int f2e3=S3-g1, xf2, yf2;
       if (f2e3<=S2) { xf2=0; yf2=f2e3; } else { xf2=f2e3-S2; yf2=S2; }

       for (int f2=xf2; f2<=yf2; ++f2) if (v.free[f2]) { v.free[f2]=F;
        int e3=f2e3-f2; if (((Uint)e3<NN) && v.free[e3]) { v.free[e3]=F;
        int f6=S4-b2-b6-f2; if (((Uint)f6<NN) && v.free[f6]) { v.free[f6]=F;
        int e5=S3-f6-g7; if (((Uint)e5<NN) && v.free[e5]) { v.free[e5]=F;
        int b3b5=S4-b2-b6, xb3, yb3;
        if (b3b5<=S2) { xb3=0; yb3=b3b5; } else { xb3=b3b5-S2; yb3=S2; }

        for (int b3=xb3; b3<=yb3; ++b3) if (v.free[b3]) { v.free[b3]=F;
         int b5=b3b5-b3; if (((Uint)b5<NN) && v.free[b5]) { v.free[b5]=F;
         int f3=S4-b3-c3-e3; if (((Uint)f3<NN) && v.free[f3]) { v.free[f3]=F;
         int f5=S4-b5-c5-e5; if (((Uint)f5<NN) && v.free[f5]) { v.free[f5]=F;
         int c2c6=S4-c3-c5, xc2, yc2;
         if (c2c6<=S2) { xc2=0; yc2=c2c6; } else { xc2=c2c6-S2; yc2=S2; }

         for (int c2=xc2; c2<=yc2; ++c2) if (v.free[c2]) { v.free[c2]=F;
          int c6=c2c6-c2; if (((Uint)c6<NN) && v.free[c6]) { v.free[c6]=F;
          int e2=S4-b2-c2-f2; if (((Uint)e2<NN) && v.free[e2]) { v.free[e2]=F;
          int e6=S4-b6-c6-f6; if (((Uint)e6<NN) && v.free[e6]) { v.free[e6]=F;

          for (int a2=NNm1; a2>=0; --a2) if (v.free[a2]) { v.free[a2]=F;
           int g2d2=S3-a2, xg2, yg2;
           if (g2d2<=S2) { xg2=0; yg2=g2d2; } else { xg2=g2d2-S2; yg2=S2; }

           for (int g2=xg2;  g2<=yg2; ++g2) if (v.free[g2]) { v.free[g2]=F;
            int d2=g2d2-g2; if (((Uint)d2<NN) && v.free[d2]) { v.free[d2]=F;
            int d3=S5-c2-d1-d2-e2; if (((Uint)d3<NN) && v.free[d3]) { v.free[d3]=F;

            for (int a6=NNm1; a6>=0; --a6) if (v.free[a6]) { v.free[a6]=F;
             int b4=a2+a6-S1; if (((Uint)b4<NN) && v.free[b4]) { v.free[b4]=F;
             int c4=S7-g1-a2-b3-b5-a6-g7; if (((Uint)c4<NN) && v.free[c4]) { v.free[c4]=F;
             int g6d6=S3-a6, xg6, yg6;
             if (g6d6<=S2) { xg6=0; yg6=g6d6; } else { xg6=g6d6-S2; yg6=S2; }

             for (int g6=xg6; g6<=yg6; ++g6) if (v.free[g6]) { v.free[g6]=F;
              int d6=g6d6-g6; if (((Uint)d6<NN) && v.free[d6]) { v.free[d6]=F;
              int d5=S5-c6-d6-d7-e6; if (((Uint)d5<NN) && v.free[d5]) { v.free[d5]=F;
              int f4=g2+g6-S1; if (((Uint)f4<NN) && v.free[f4]) { v.free[f4]=F;
              int e4=S7-a1-g2-f3-f5-g6-a7; if (((Uint)e4<NN) && v.free[e4]) { v.free[e4]=F;
              int b1b7=S4-a2-a6, xb1, yb1;
              if (b1b7<=S2) { xb1=0; yb1=b1b7; } else { xb1=b1b7-S2; yb1=S2; }

              for (int b1=xb1; b1<=yb1; ++b1) if (v.free[b1]) { v.free[b1]=F;
               int b7=b1b7-b1; if (((Uint)b7<NN) && v.free[b7]) { v.free[b7]=F;
               int f1=a6+b7-g2; if (((Uint)f1<NN) && v.free[f1]) { v.free[f1]=F;
               int f7=a2+b1-g6; if (((Uint)f7<NN) && v.free[f7]) { v.free[f7]=F;
               int a3a5=S3-b4, xa3, ya3;
               if (a3a5<=S2) { xa3=0; ya3=a3a5; } else { xa3=a3a5-S2; ya3=S2; }

               for (int a3=xa3; a3<=ya3; ++a3) if (v.free[a3]) { v.free[a3]=F;
                int a5=a3a5-a3; if (((Uint)a5<NN) && v.free[a5]) { v.free[a5]=F;
                int g3=S7-a3-b4-c5-d6-e5-f4; if (((Uint)g3<NN) && v.free[g3]) { v.free[g3]=F;
                int g5=S7-a5-b4-c3-d2-e3-f4; if (((Uint)g5<NN) && v.free[g5]) { v.free[g5]=F;
                int c1e1=S3-d2, xc1, yc1;
                if (c1e1<=S2) { xc1=0; yc1=c1e1; } else { xc1=c1e1-S2; yc1=S2; }

                for (int c1=xc1; c1<=yc1; ++c1) if (v.free[c1]) { v.free[c1]=F;
                 int e1=c1e1-c1; if (((Uint)e1<NN) && v.free[e1]) { v.free[e1]=F;
                 int c7=S3-c1-c4; if (((Uint)c7<NN) && v.free[c7]) { v.free[c7]=F;
                 int e7=S4-b7-c7-f7; if (((Uint)e7<NN) && v.free[e7]) {
                  fprintf(wfp, "%2d %2d %2d %2d %2d %2d %2d\n", a1+1, a2+1, a3+1, a4+1, a5+1, a6+1, a7+1);
                  fprintf(wfp, "%2d %2d %2d %2d %2d %2d %2d\n", b1+1, b2+1, b3+1, b4+1, b5+1, b6+1, b7+1);
                  fprintf(wfp, "%2d %2d %2d %2d %2d %2d %2d\n", c1+1, c2+1, c3+1, c4+1, c5+1, c6+1, c7+1);
                  fprintf(wfp, "%2d %2d %2d %2d %2d %2d %2d\n", d1+1, d2+1, d3+1, d4+1, d5+1, d6+1, d7+1);
                  fprintf(wfp, "%2d %2d %2d %2d %2d %2d %2d\n", e1+1, e2+1, e3+1, e4+1, e5+1, e6+1, e7+1);
                  fprintf(wfp, "%2d %2d %2d %2d %2d %2d %2d\n", f1+1, f2+1, f3+1, f4+1, f5+1, f6+1, f7+1);
                  fprintf(wfp, "%2d %2d %2d %2d %2d %2d %2d\n\n", g1+1, g2+1, g3+1, g4+1, g5+1, g6+1, g7+1);
                  printCountAndElapsedTime(++count, startTime);
                 } v.free[c7]=T; } v.free[e1]=T; } v.free[c1]=T;
                } v.free[g5]=T; } v.free[g3]=T;
                } v.free[a5]=T; } v.free[a3]=T;
               } v.free[f7]=T; } v.free[f1]=T;
               } v.free[b7]=T; } v.free[b1]=T;
              } v.free[e4]=T; } v.free[f4]=T;
              } v.free[d5]=T; } v.free[d6]=T; } v.free[g6]=T;
             } v.free[c4]=T; } v.free[b4]=T; } v.free[a6]=T;
            } v.free[d3]=T; } v.free[d2]=T;} v.free[g2]=T;
           } v.free[a2]=T;
          } v.free[e6]=T; } v.free[e2]=T;
          } v.free[c6]=T; } v.free[c2]=T;
         } v.free[f5]=T; } v.free[f3]=T;
         } v.free[b5]=T; } v.free[b3]=T;
         } v.free[e5]=T; } v.free[f6]=T;
        } v.free[e3]=T; } v.free[f2]=T;
       } v.free[c5]=T; } v.free[b6]=T;
      } v.free[c3]=T; } v.free[b2]=T;
     } v.free[d7]=T; } v.free[d1]=T; }
      v.free[g7]=T; } v.free[g1]=T;
    } v.free[g4]=T; } v.free[a4]=T; } v.free[a7]=T;
   } v.free[a1]=T;
   Count+=count; count=0; 
  }
  putchar('\n'); printCountAndElapsedTime(Count, startTime);
  return Count;
} // getSquares
//============================================= main ============================================

int main() {
//  ----
  outputLocalTime(); printf("Enter a1, (0 for all): "); int a1=getInt();
  if ((a1<0)|(a1>49)) printf("\aError: %d is not 1 .. %d\n", a1, NN);
  else {
    int num=0; FILE *wfp=openOutput(a1);
    if (wfp!=NULL) {
      num=getSquares(a1, wfp);
      printf("Number of squares: %d\n", num); fclose(wfp);
    }
    if (num==0) remove(OutputFileName);
  }
  printf("\nPress a key to close the console.");
  while (!_kbhit()) Sleep(250);	return EXIT_SUCCESS;
} // main