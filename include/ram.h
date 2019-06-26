#ifndef _ram_h_
#define _ram_h_


void allocRussell(int** supply,
                  int** demand,
                  int** sourcesidx,
                  int** destsidx,
                  int** u,
                  int** v,
                  int*** russelcosts,
                  int*** russelmatrix,
                  int*** russelweights,
                  int nusers,
                  int ncelle);


#endif // _ram_h_
