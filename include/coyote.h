#ifndef _coyote_h_
#define _coyote_h_

#define true  1
#define false 0




int isFeasible(int*** russelmatrix,
               int**** pos,
               int** tasks,
               int** sourcesidx,
               int** destsidx,
               int nsources,
               int ndest,
               int nusers,
               int time);



void readFile(char* file);



void allocBase();
void freeCoiote();












#endif // _coyote_h_
