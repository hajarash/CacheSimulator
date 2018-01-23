// Note that the code looks messy because we needed to optimize the locality //
// -Arash
#include <stdio.h>
#include "life.h"

# define BITMAP_SIZE 8
// You are only allowed to change the contents of this file with respect 
// to improving the performance of this program



// If you need to initialize some data structures for your implementation
// of life then make changes to the following function, otherwise 
// do not change this function.

void initLife() {

}

//bool validRowCol(int row, int col) {
//    return (row < N && row >= 0 && col < N && col>= 0);
//}

// When completed this file will contain several versions of the life() function and 
// conditional compilation will be used to determine which version runs.  Each 
// version will be surrounded with  an ifdef, undef, and endif as illustrated 
// immediately below this comment block. To select the version of the life function 
// to use, put the #define USE USE immediately infront of the version of the life() 
// to compile.  Your version of life() must produce a correct result. 

// You are required to document the changes you make in the README.txt file. For each entry
// in the README.txt file there is to be a version of the matching  life() function here such
// that the markers can see, and run if needed,  the actual code that you used 
// to produce your results for the logged change to the life function.  






/*
 17TH IMPLEMENTATION
 */


//ARASH
#define USE
#ifdef USE

int life(long oldWorld[N][N], long newWorld[N][N]) {
    
    int i, j;
    int alive = 0;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            newWorld[i][j] = checkHealth(0 + oldWorld[(i -1 + N) % N][(j - 1 + N ) % N] + oldWorld[(i -1 + N) % N][j] + oldWorld[(i - 1 + N) % N][(j + 1 + N ) % N]  + oldWorld[i][(j - 1 + N ) % N] + oldWorld[i][(j + 1  + N ) % N]  + oldWorld[(i + 1 + N) % N][(j - 1  + N ) % N]+ oldWorld[(i + 1 + N) % N][j]   + oldWorld[(i + 1 + N) % N][(j + 1  + N ) % N],  oldWorld[i][j]);
            if(newWorld[i][j]){
                alive++;
            }
        }
    }
    return alive;
}
#undef USE
#endif

//ARASH


