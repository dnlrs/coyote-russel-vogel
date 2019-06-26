#ifndef _modi_h_
#define _modi_h_


void allocModi(int*** modi,
               int** pathrows,
               int** pathcols,
               int nusers,
               int ncelle);





int findPathR(int startrow,   /* starting row coordinate */
              int startcol,   /* starting column coordinate */
              int destrow,    /* destination row */
              int destcol,    /* destination column */
              int** pathrows, /* vettore indici di riga del path */
              int** pathcols, /* vettore indici di colonna del path */
              int cidx, /* indice posizione corrente nel path */
              int noWay,
              int*** russelmatrix,
              int nsources,
              int ndest);

void squeezePath(int** pathrows,
                 int** pathcols,
                 int*  pathLen);


void clearPath(int** pathrows,
               int** pathcols,
               int nsources,
               int ndest);




#endif // _modi_h_
