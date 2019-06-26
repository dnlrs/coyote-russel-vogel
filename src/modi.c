
#include "coyote.h"

struct s_modi{
    int** modi;         /* matrice dei dei pesi per MODI                    */

    int* pathrows;      /* vettore indici di riga del path                  */
    int* pathcols;      /* vettore indici di colonna del path               */
};
static struct s_modi* modiData;



typedef struct tabuNode {
    int row;
    int column;
} tabuN;

tabuN tabuNodes[ntaboes];
int tabuI=0;


int isTabu(int r, int c) {
    int i;

    for ( i=0; i<tabuI; i++ )
        if ( tabuNodes[i].row == r && tabuNodes[i].column == c )
            return true;

    return false;
}

void addTabu(int r, int c) {
    tabuNodes[tabuI].row=r;
    tabuNodes[tabuI].column=c;

    tabuI++;
}

void initTabu() {
    int i;

    for (i=0; i<ntaboes; i++) {
        tabuNodes[i].row    =-1;
        tabuNodes[i].column =-1;
        }
    tabuI=0;
}





void initMODI(nusers, ncelle) {

    modiData=(struct s_modi*) malloc(sizeof(struct s_modi));
    assert( modiData );

    /* //////////////////////////////////////////////////////////////////// **
    ** Allocazione e preparazione della struttura dati per MODI             **
    ** //////////////////////////////////////////////////////////////////// */
    allocModi(&(modiData->modi),
              &(modiData->pathrows),
              &(modiData->pathcols),
              nusers, ncelle);

}


void modi() {

    /* //////////////////////////////////////////////////////////////////// **
    ** Strutture dati                                                       **
    ** //////////////////////////////////////////////////////////////////// */


    int** modi;         /* matrice dei dei pesi per MODI                    */

    int* pathrows;      /* vettore indici di riga del path                  */
    int* pathcols;      /* vettore indici di colonna del path               */
    int  pathLen;
    int  canBeImproved; /* dice se la basic solution can be improved        */
    int modicycle;


    /* ==================================================================== */
    /* -------------------------------------------------------------------- */
    /* ==================================================================== */
    /*                              MODI                                    */
    /* ==================================================================== */
    /* ==================================================================== */
    canBeImproved=true;
    modicycle=0;
    while ( canBeImproved ) {
        modicycle++;
        #ifdef _debugPrintStepByStepModi
        fprintf(stderr, "\nIterazioni MODI corrente: %4d\n", modicycle);
        #endif // _debugPrintStepByStepModi
        /* inizializzazione vettori "u" e "v"                               */
        for ( source=0; source<nsources; source++ )
            u[source]=-1;
        for ( dest=0; dest<ndest; dest++ )
            v[dest]=-1;

        /* per tutte le variabili della Basic Solution imposto il modi cost a 0 **
        ** NB: trascuro l'ultima colonna                                        */
        int nx=0;
        for ( source=0; source<nsources; source++ )
            for ( dest=0; dest<ndest; dest++ )
                if ( russelmatrix[source][dest]!=-1 ) {
                    nx++;
                    modi[source][dest]=0;
                    }
                else
                    modi[source][dest]=-1;



        #ifdef _debugPrintStepByStepModi
        fprintf(stderr, "\nNX: %4d\n", nx);
        #endif // _debugPrintStepByStepModi


        int primo=true;
        while ( nx>0 ) {
                nx--;
            #ifdef _debugPrintStepByStepModi
            fprintf(stderr, "Mancano %d variabili a cui calcolare u e v\n", nx);
            #endif // _debugPrintStepByStepModi
            for ( source=0; source<nsources; source++ )
                for ( dest=0; dest<ndest; dest++ )
                    if ( russelmatrix[source][dest]!=-1 ) {
                        /* se non ho ne' "u" ne' "v" per la variabile           */
                        if ( v[dest]==-1 && u[source]==-1 ) {
                            /* se e' la prima che trovo metto u=0               */
                            if ( primo ) {
                                u[source]=0;
    //                            v[dest]=0;
                                primo=false;
                                }
                            else
                                continue;
                            }

                        /* se ho "u" ma non "v"                                 */
                        if ( v[dest]==-1 && u[source]!=-1 ) {
                            v[dest]=russelcosts[source][dest]-u[source];

                            #ifdef _debugPrintStepByStepModi
                            fprintf(stderr, "R %2d C %2d: v %6d (%6d - %6d)\n",
                                    source, dest, v[dest],
                                    russelcosts[source][dest], u[source]);
                            #endif // _debugPrintStepByStepModi
                            }

                        /* se ho "v" ma non "u"                                 */
                        if ( v[dest]!=-1 && u[source]==-1 ) {
                            u[source]=russelcosts[source][dest]-v[dest];

                            #ifdef _debugPrintStepByStepModi
                            fprintf(stderr, "R %2d C %2d: u %6d (%6d - %6d)\n",
                                    source, dest, u[source],
                                    russelcosts[source][dest], v[dest]);
                            #endif // _debugPrintStepByStepModi
                            }

                        /* se li ho gia' (sia "u" che "v")                      */
    //                    if ( v[dest]!=-1 && u[source]!=-1 )
    //                        nx--;
                        }



            #ifdef _debugPrintStepByStepModi
            fprintf(stderr,
                    "====================================================================================================\n");
            fprintf(stderr, " MODI MATRIX\n");
            fprintf(stderr,
                    "====================================================================================================\n");

            fprintf(stderr, " [%2d] ;", itera);
            for (j=0; j<ndest;j++)
                fprintf(stderr, "  %2d  ;", j);
            fprintf(stderr, "supply;");
            fprintf(stderr, "   u  ;\n");

            for (i=0; i<nsources; i++) {
                fprintf(stderr, "  %2d  ;", i);
                for (j=0; j<ndest; j++) {
                    if (modi[i][j]!=-1)
                        fprintf(stderr, "%6d;", modi[i][j]);
                    else
                        fprintf(stderr, "      ;");
                }
                fprintf(stderr, "  %4d;", supply[i]);
                fprintf(stderr, "%6d;", u[i]);

                fprintf(stderr, "\n");
            }

            fprintf(stderr, "demand;");
            for (j=0; j<ndest;j++)
                fprintf(stderr, "%6d;", demand[j]);
            fprintf(stderr, "\n");
            fprintf(stderr, "   v  ;");
            for (j=0; j<ndest;j++)
                fprintf(stderr, "%6d;", v[j]);
            fprintf(stderr, "\n");
        //        fprintf(stderr, "\n");
            fprintf(stderr,
                    "====================================================================================================\n");
            fprintf(stderr,
                    "====================================================================================================\n");
            fprintf(stderr,
                    "====================================================================================================\n");
            #endif // _debugPrintStepByStepModi
            }

        rmax=-1;
        cmax=-1;
        maxdiff=INT_MAX;
        for ( source=0; source<nsources; source++ )
            for ( dest=0; dest<ndest; dest++ )
                if ( u[source]+v[dest]!=russelcosts[source][dest] ) {
                    modi[source][dest]=russelcosts[source][dest]-u[source]-v[dest];

                    if ( modi[source][dest]<maxdiff ) {
                        maxdiff=modi[source][dest];
                        rmax=source;
                        cmax=dest;
                        }
                    }
        #ifdef _debugPrintStepByStepModi
            fprintf(stderr, "BEST IMPROVEMENT %d, row %4d column %4d\n\n", maxdiff, rmax, cmax);
        #endif // _debugPrintStepByStepModi

        if ( maxdiff>0 )
            canBeImproved=false;


        if ( !canBeImproved ) {
            #ifdef _debugPrintStepByStepModi
                fprintf(stderr, "CANNOT BE IMPROVED!\n");
            #endif // _debugPrintStepByStepModi
            continue;
            }
        else{
            #ifdef _debugPrintStepByStepModi
                fprintf(stderr, "CAN BE IMPROVED!\n");
            #endif // _debugPrintStepByStepModi
            }

        /* -------------------------------------------------------------------- */
        /* ricerca di un path                                                   */
        /* -------------------------------------------------------------------- */
        initTabu();
        clearPath(&pathrows, &pathcols, nsources, ndest);


        pathrows[0]=rmax;
        pathcols[0]=cmax;

        rs = findPathR(rmax, /* starting row coordinate                         */
                       cmax, /* starting column coordinate                      */
                       rmax, /* destination row coordinate                      */
                       cmax, /* destination column coordinate                   */
                       &pathrows, /* vettore indici di riga del path            */
                       &pathcols, /* vettore indici di colonna del path         */
                       1,    /* la prima posizione e' la starting cell          */
                       -1,   /* inizialmente nessuna direzione e' vietata       */
                       &russelmatrix,
                       nsources,
                       ndest);

        if ( rs ) {
            #ifdef _debugPrintSqueezePath
            fprintf(stderr, "PATH FOUND:\n");
            i=0;
            while ( pathrows[i]!=-1 ) {
                fprintf(stderr, "(%3d,%3d)\n", pathrows[i], pathcols[i]);
                i++;
                }
            #endif // _debugPrintSqueezePath

            squeezePath(&pathrows, &pathcols, &pathLen);

            #ifdef _debugPrintSqueezePath
            fprintf(stderr, "Squeezed path:\n");
             i=0;
            while ( pathrows[i]!=-1 ) {
                fprintf(stderr, "(%3d,%3d)\n", pathrows[i], pathcols[i]);
                i++;
                }
            #endif // _debugPrintSqueezePath
            }
        else {
            fprintf(stderr, "PROBLEMS: NO PATH FOUND:\n");
            canBeImproved=false;
            continue;
            }


        /* -------------------------------------------------------------------- */
        /* spostamento task                                                     */
        /* -------------------------------------------------------------------- */
        /* trova la massima quantita' di task che possono essere spostati       **
        ** la massima quantita' di task che possono essere spostati corrisponde **
        ** al valore piu' grande tra quelli delle celle donatrici               **
        **                                                                      **
        ** per la codifica del problema, le celle donatrici si trovano agli     **
        ** indici dispari del vettore che contiene il path                      */

        #ifdef _debugPrintStepByStepModi
        fprintf(stderr, "\n\n---===--- pathLen is %3d ---====---\n\n", pathLen);
        #endif // _debugPrintStepByStepModi

        k=INT_MAX;
        for ( i=1; i<pathLen-1; i++ ) {
            source  =pathrows[i];
            dest    =pathcols[i];

            if ( i%2==1 )
                if ( russelmatrix[source][dest]<k )
                    k=russelmatrix[source][dest];
            }

        #ifdef _debugPrintStepByStepModi
        fprintf(stderr, "\n\n---===--- Rialloco %3d tasks---====---\n\n", k);
        #endif // _debugPrintStepByStepModi

        /* aggiungi/togli task secondo la regola delle celle recipienti e       **
        ** donatrici                                                            */
        for ( i=0; i<pathLen-1; i++ ) {
            source  =pathrows[i];
            dest    =pathcols[i];

            if ( i%2==1 ) {
                russelmatrix[source][dest] -= k;
                if ( russelmatrix[source][dest]==0 )
                    russelmatrix[source][dest]=-1;
                }
            else {
                if ( russelmatrix[source][dest]==-1 )
                    russelmatrix[source][dest] = k;
                else
                    russelmatrix[source][dest] += k;
                }
            }


        #ifdef _debugPrintStepByStepModi
        fprintf(stderr,
                "====================================================================================================\n");
        fprintf(stderr, " FINAL MODI MATRIX\n");
        fprintf(stderr,
                "====================================================================================================\n");

        fprintf(stderr, " [%2d] ;", itera);
        for (j=0; j<ndest;j++)
            fprintf(stderr, "  %2d  ;", j);
        fprintf(stderr, "supply;");
        fprintf(stderr, "   u  ;\n");

        for (i=0; i<nsources; i++) {
            fprintf(stderr, "  %2d  ;", i);
            for (j=0; j<ndest; j++) {
                if (modi[i][j]!=-1)
                    fprintf(stderr, "%6d;", modi[i][j]);
                else
                    fprintf(stderr, "      ;");
            }
            fprintf(stderr, "  %4d;", supply[i]);
            fprintf(stderr, "%6d;", u[i]);

            fprintf(stderr, "\n");
        }

        fprintf(stderr, "demand;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%6d;", demand[j]);
        fprintf(stderr, "\n");
        fprintf(stderr, "   v  ;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%6d;", v[j]);
        fprintf(stderr, "\n");
    //        fprintf(stderr, "\n");
        fprintf(stderr,
                "====================================================================================================\n");
        fprintf(stderr,
                "====================================================================================================\n");
        fprintf(stderr,
                "====================================================================================================\n");
        #endif // _debugPrintStepByStepModi

        }

}



void allocModi(int*** modi,
               int** pathrows,
               int** pathcols,
               int nusers,
               int ncelle) {
    int i, j;
    /* max nr. di sorgenti      = nusers*ncelle                             **
    ** max nr. di destinazioni  = ncelle                                    */
    int len=(nusers*ncelle) + ncelle;

    /* indici di riga per il path dello stepping stone method (o MODI)      */
    *pathrows=(int*) malloc(len*sizeof(int));
    assert( *pathrows );

    /* indici di riga per il path dello stepping stone method (o MODI)      */
    *pathcols=(int*) malloc(len*sizeof(int));
    assert( *pathcols );

    for ( i=0; i<len; i++ ) {
        (*pathrows)[i]=-1;
        (*pathcols)[i]=-1;
        }

    /* matrice dei pesi                                                     */
    *modi=(int**) malloc(len*sizeof(int*));
    assert ( *modi );

    /* inizializzazione matrici costi e assegnazioni                        */
    for ( i=0; i<len; i++ ) {
        (*modi)[i] =(int*) malloc((ncelle+1)*sizeof(int));
        assert ( (*modi)[i] );

        for ( j=0; j<(ncelle+1); j++ )
            (*modi)[i][j]=-1;
        }
}


/* Direction encoding:                                                      **
**  sinistra = 0                                                            **
**  sopra    = 1                                                            **
**  destra   = 2                                                            **
**  sotto    = 3                                                            **
**                                                                          */
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
              int ndest) {

    int i,j;
    int rs;
    #ifdef _debugPrintStepByStepPrintFindPath
    int p;
    #endif // _debugPrintStepByStepPrintFindPath



    if ( isTabu(startrow, startcol) ) {
        #ifdef _debugPrintStepByStepPrintFindPath
        for ( p=0; p<cidx; p++ )
            fprintf(stderr, "  ");
        fprintf(stderr, "...isTabu\n");
        #endif // _debugPrintStepByStepPrintFindPath
        return false;
    }


    /* go left                                                              */
    #ifdef _debugPrintStepByStepPrintFindPath
    if ( noWay!=0 ) {
        for ( p=0; p<cidx; p++ )
            fprintf(stderr, "  ");
        fprintf(stderr, "I GO LEFT\n");
        }
    #endif // _debugPrintStepByStepPrintFindPath

    i=startrow;
    j=startcol-1;
    rs=false;
    if ( noWay!=0 )
        while ( j>=0 ) {
            if ( (*russelmatrix)[i][j]!=-1 ) {
                (*pathrows)[cidx]=i;
                (*pathcols)[cidx]=j;

                #ifdef _debugPrintStepByStepPrintFindPath
                for ( p=0; p<cidx; p++ )
                    fprintf(stderr, "  ");
                fprintf(stderr, "I check (%3d,%3d)\n", i, j);
                #endif // _debugPrintStepByStepPrintFindPath

                if ( i==destrow && j==destcol && cidx>1 )
                    return true;

                rs = findPathR(i, j, destrow, destcol,
                               pathrows, pathcols, cidx+1, 2,
                               russelmatrix, nsources, ndest);
                if (rs)
                    return true;
                }
            if ( i==destrow && j==destcol && cidx>1 ) {
                (*pathrows)[cidx]=i;
                (*pathcols)[cidx]=j;
                return true;
                }
            j--;
            }


    /* go up */
    #ifdef _debugPrintStepByStepPrintFindPath
    if ( noWay!=1 ) {
        for ( p=0; p<cidx; p++ )
            fprintf(stderr, "  ");
        fprintf(stderr, "I GO UP\n");
        }
    #endif // _debugPrintStepByStepPrintFindPath


    i=startrow-1;
    j=startcol;
    rs=false;
    if ( noWay!=1 )
        while ( i>=0 ) {
            if ( (*russelmatrix)[i][j]!=-1 ) {
                (*pathrows)[cidx]=i;
                (*pathcols)[cidx]=j;

                #ifdef _debugPrintStepByStepPrintFindPath
                for ( p=0; p<cidx; p++ )
                    fprintf(stderr, "  ");
                fprintf(stderr, "I check (%3d,%3d)\n", i, j);
                #endif // _debugPrintStepByStepPrintFindPath


                if ( i==destrow && j==destcol && cidx>1 )
                    return true;

                rs = findPathR(i, j, destrow, destcol,
                               pathrows, pathcols, cidx+1, 3,
                               russelmatrix, nsources, ndest);
                if (rs)
                    return true;
                }
            if ( i==destrow && j==destcol && cidx>1 ) {
                (*pathrows)[cidx]=i;
                (*pathcols)[cidx]=j;
                return true;
                }
            i--;
            }


    /* go right */

    #ifdef _debugPrintStepByStepPrintFindPath
    if ( noWay!=2 ) {
        for ( p=0; p<cidx; p++ )
            fprintf(stderr, "  ");
        fprintf(stderr, "I GO RIGHT\n");
        }
    #endif // _debugPrintStepByStepPrintFindPath

    i=startrow;
    j=startcol+1;
    rs=false;
    if ( noWay!=2 )
        while ( j<ndest ) {
            if ( (*russelmatrix)[i][j]!=-1 ) {
                (*pathrows)[cidx]=i;
                (*pathcols)[cidx]=j;
                #ifdef _debugPrintStepByStepPrintFindPath
                for ( p=0; p<cidx; p++ )
                    fprintf(stderr, "  ");
                fprintf(stderr, "I check (%3d,%3d)\n", i, j);
                #endif // _debugPrintStepByStepPrintFindPath

                if ( i==destrow && j==destcol && cidx>1 )
                    return true;

                rs = findPathR(i, j, destrow, destcol,
                               pathrows, pathcols, cidx+1, 0,
                               russelmatrix, nsources, ndest);
                if (rs)
                    return true;
                }
            if ( i==destrow && j==destcol && cidx>1 ) {
                (*pathrows)[cidx]=i;
                (*pathcols)[cidx]=j;
                return true;
                }
            j++;
            }


    /* go down */
    #ifdef _debugPrintStepByStepPrintFindPath
    if ( noWay!=3 ){
        for ( p=0; p<cidx; p++ )
            fprintf(stderr, "  ");
        fprintf(stderr, "I GO DOWN\n");
        }
    #endif // _debugPrintStepByStepPrintFindPath

    i=startrow+1;
    j=startcol;
    rs=false;
    if ( noWay!=3 )
        while ( i<nsources ) {
            if ( (*russelmatrix)[i][j]!=-1 ) {
                (*pathrows)[cidx]=i;
                (*pathcols)[cidx]=j;

                #ifdef _debugPrintStepByStepPrintFindPath
                for ( p=0; p<cidx; p++ )
                    fprintf(stderr, "  ");
                fprintf(stderr, "I check (%3d,%3d)\n", i, j);
                #endif // _debugPrintStepByStepPrintFindPath

                if ( i==destrow && j==destcol && cidx>1 )
                    return true;

                rs = findPathR(i, j, destrow, destcol,
                               pathrows, pathcols, cidx+1, 1,
                               russelmatrix, nsources, ndest);
                if (rs)
                    return true;
                }
            if ( i==destrow && j==destcol && cidx>1 ) {
                (*pathrows)[cidx]=i;
                (*pathcols)[cidx]=j;
                return true;
                }
            i++;
            }

    /* backtrack */
    (*pathrows)[cidx]=-1;
    (*pathcols)[cidx]=-1;

    addTabu(startrow, startcol);
    return false;
}

void squeezePath(int** pathrows,
                 int** pathcols,
                 int*  pathLen) {

    int i, idx;

    int lr;
    int lc;
    int nr;
    int occ;

    nr=0;
    while ( (*pathrows)[nr]!=-1 )
        nr++;



    lr=(*pathrows)[0];
    idx=0;
    occ=1;
    for ( i=1; i<nr; i++ ) {
        if ( (*pathrows)[i]==lr ) {
            occ++;
            if ( occ==2 )
                idx++;
            (*pathrows)[idx]=(*pathrows)[i];
            (*pathcols)[idx]=(*pathcols)[i];
            }
        else {
            occ=1;
            idx++;
            (*pathrows)[idx]=(*pathrows)[i];
            (*pathcols)[idx]=(*pathcols)[i];
            lr=(*pathrows)[i];
            }
        }

    nr=idx+1;

    #ifdef _debugPrintSqueezePath
    fprintf(stderr, "Row Squeezed path:\n");
    i=0;
    while ( (*pathrows)[i]!=-1 ) {
        fprintf(stderr, "(%3d,%3d)\n", (*pathrows)[i], (*pathcols)[i]);
        i++;
        }
    #endif // _debugPrintSqueezePath

    lc=(*pathcols)[0];
    idx=0;
    occ=1;
    for ( i=1; i<nr; i++ ) {
        if ( (*pathcols)[i]==lc ) {
            occ++;
            if ( occ==2 )
                idx++;
            (*pathrows)[idx]=(*pathrows)[i];
            (*pathcols)[idx]=(*pathcols)[i];
            }
        else {
            occ=1;
            idx++;
            (*pathrows)[idx]=(*pathrows)[i];
            (*pathcols)[idx]=(*pathcols)[i];
            lc=(*pathcols)[i];
            }
        }

    *pathLen=idx+1;

    while ( (*pathrows)[++idx]!=-1 ) {
        (*pathrows)[idx]=-1;
        (*pathcols)[idx]=-1;
        }

    #ifdef _debugPrintSqueezePath
    fprintf(stderr, "Column Squeezed path:\n");
    i=0;
    while ( (*pathrows)[i]!=-1 ) {
        fprintf(stderr, "(%3d,%3d)\n", (*pathrows)[i], (*pathcols)[i]);
        i++;
        }
    #endif // _debugPrintSqueezePath
}

void clearPath(int** pathrows,
               int** pathcols,
               int nsources,
               int ndest) {
    int i;

    for ( i=0; i<(nsources+ndest); i++ ) {
        (*pathrows)[i]=-1;
        (*pathcols)[i]=-1;
    }
}

