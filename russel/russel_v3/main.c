#include <assert.h>
#include <math.h>
#include <tgmath.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>
#include <limits.h>


#define true  1
#define false 0

#define colonna 0
#define riga    1

#define minus   0
#define plus    1

#define ntaboes 100000

//#define _debug

//#define _debugVerbose             /* stampa info cu cio' che sta facendo  */

#define _debugPrintCostsMatrix    /* stampa la matrice dei costi per VAM  */
#define _debugPrintStepByStep     /* stampa matrice assegnazioni each time*/
#define _debugPrintStepByStepModi     /* stampa matrice MODI each time*/
#define _debugPrintStepByStepPrintFinal     /* stampa matrice MODI each time*/
// #define _debugFastFindPath
// #define _debugFastFindPathR

//#define _debugPrintStepByStepPrintFindPath     /* stampa operazioni di path finding */
// #define _debugPrintAssignments    /* verbose info sulle assegnazioni      */
//#define _debugPrintSqueezePath
//#define _debugPrintSqueezePathA
//#define _debugAggiustaR

#define min(A, B) ((A) < (B) ? (A) : (B))
#define max(A, B) ((A) < (B) ? (B) : (A))





typedef struct s_cella {
    int row;
    int column;
} t_cella;

int sbNCelle=0;
t_cella searchByRow[ntaboes];
t_cella searchByCol[ntaboes];

int searchByIndexesRow[ntaboes];
int searchByIndexesCol[ntaboes];

void initSearchWays() {
    int i;

    for ( i=0; i<ntaboes; i++ ) {
        searchByRow[i].row    = -1;
        searchByCol[i].row    = -1;
        searchByRow[i].column = -1;
        searchByCol[i].column = -1;
        searchByIndexesRow[i] = -1;
        searchByIndexesCol[i] = -1;
        }
}


/* per aggiornaR() */
typedef struct s_tabuE {
    int n;
} tabuE;


tabuE tabuRows[ntaboes];
int nTR=0;
tabuE tabuCols[ntaboes];
int nTC=0;

int isTabuR(int n) {
    int i;

    for ( i=0; i<nTR; i++ )
        if ( tabuRows[i].n==n )
            return true;
    return false;
}

int isTabuC(int n) {
    int i;

    for ( i=0; i<nTR; i++ )
        if ( tabuCols[i].n==n )
            return true;
    return false;
}


void initTabuR(){
    nTR=0;
}

void addTabuR(int n) {
    tabuRows[nTR].n=n;
    fprintf(stderr, "ntr is %d\n", nTR+1);
    nTR++;
}

void addTabuC(int n) {
    tabuCols[nTC].n=n;
    nTC++;
}

void rmTabuR() {
    nTR--;
}

void rmTabuC(int n) {
    int i;
    for ( i=0; i<nTC; i++ )
        if ( tabuCols[i].n==n )
            tabuCols[i].n=-1;
//    nTC--;
}




/* per findPathR() */
typedef struct tabuNode {
    int row;
    int column;
} tabuN;

tabuN tabuNodes[ntaboes];
int tabuI=0;

void initTabu() {
    int i;

    for (i=0; i<ntaboes; i++) {
        tabuNodes[i].row    =-1;
        tabuNodes[i].column =-1;
        }
    tabuI=0;
}

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

void rmTabu() {
    tabuI--;
    tabuNodes[tabuI].row=-1;
    tabuNodes[tabuI].column=-1;
}




/* per il ciclo MODI */
tabuN tabuNodesModi[ntaboes];
int tabuM=0;

void initTabuM() {
    int i;

    for (i=0; i<ntaboes; i++) {
        tabuNodesModi[i].row    =-1;
        tabuNodesModi[i].column =-1;
        tabuRows[i].n=-1;
        tabuCols[i].n=-1;
        }
    tabuI=0;
    nTR=0;
    nTC=0;
}

int isTabuM(int r, int c) {
    int i;

    for ( i=0; i<tabuM; i++ )
        if ( tabuNodesModi[i].row == r && tabuNodesModi[i].column == c )
            return true;

    return false;
}

void addTabuM(int r, int c) {
    tabuNodesModi[tabuM].row=r;
    tabuNodesModi[tabuM].column=c;

    tabuM++;
}

void rmTabuM(int r, int c) {
    tabuM--;
    tabuNodesModi[tabuM].row    = -1;
    tabuNodesModi[tabuM].column = -1;
}



void sspace(int n) {
    int i;
    for (i=0; i<n-1; i++)
        fprintf(stderr, "  ");
    fprintf(stderr, "|| ");
}


void allocBase(int** tasks,
               int***** costs,
               int**** pos,
               int** tpuser,
               int ncelle,
               int nusers,
               int nintervalli);

void readFile(int** tasks,
              int***** costs,
              int**** pos,
              int** tpuser,
              char* file);

void allocRussell(int** supply,
                  int** demand,
                  int** sourcesidx,
                  int** destsidx,
                  int** timeidx,
                  int** u,
                  int** v,
                  int*** russelcosts,
                  int*** russelmatrix,
                  int*** russelweights,
                  int nusers,
                  int ncelle,
                  int nintervalli);

void allocModi(int*** modi,
               int** pathrows,
               int** pathcols,
               int nusers,
               int ncelle,
               int nintervalli);

int isFeasible(int*** russelmatrix,
               int**** pos,
               int** tasks,
               int** sourcesidx,
               int** destsidx,
               int** timeidx,
               int** tpuser,
               int nsources,
               int ndest,
               int nusers);

int solveConflicts(int startrow,
                   int startcol,
                   int** pathrows,
                   int** pathcols,
                   int*** russelmatrix,
                   int*** russelcosts,
                   int*** modimatrix,
                   int** tpuser,
                   int nsources,
                   int ndest,
                   int nusers);



int fastFindPathR(int startrow,   /* starting row coordinate */
                  int startcol,   /* starting column coordinate */
                  int destrow,    /* destination row */
                  int destcol,    /* destination column */
                  int** pathrows, /* vettore indici di riga del path */
                  int** pathcols, /* vettore indici di colonna del path */
                  int cidx,       /* indice posizione corrente nel path */
                  int TasksToMove,
                  int*** russelmatrix,
                  int** tpuser,
                  int nusers,
                  int nsources,
                  int ndest,
                  int payAttention,
                  int depAllowed);


// Fast Find Path Recursive RELAXED
int fastFindPathRR(int startrow,   /* starting row coordinate */
                   int startcol,   /* starting column coordinate */
                   int destrow,    /* destination row */
                   int destcol,    /* destination column */
                   int** pathrows, /* vettore indici di riga del path */
                   int** pathcols, /* vettore indici di colonna del path */
                   int cidx,       /* indice posizione corrente nel path */
                   int TasksToMove,
                   int currentCost, /* costo corrente di tutte le celle aggiunte */
                   int maxCost,    /* costo massimo al quale accetto una nuova cella */
                   int*** russelmatrix,
                   int*** modimatrix,
                   int** tpuser,
                   int nusers,
                   int nsources,
                   int ndest,
                   int payAttention);

int findPathR(int startrow,   /* starting row coordinate */
              int startcol,   /* starting column coordinate */
              int destrow,    /* destination row */
              int destcol,    /* destination column */
              int** pathrows, /* vettore indici di riga del path */
              int** pathcols, /* vettore indici di colonna del path */
              int cidx,       /* indice posizione corrente nel path */
              int noWay,
              int*** russelmatrix,
              int*** russelcosts,
              int nsources,
              int ndest);


int getUVR(int startrow,
           int startcol,
           int* nx,
           int cidx,
           int*** russelmatrix,
           int*** russelcosts,
           int**  u,
           int**  v,
           int nsources,
           int ndest);

// vecchia versione sotto
int aggiustaR(int startrow,     /* starting row coordinate */
              int startcol,     /* starting column coordinate */
              int destrow,      /* destination row            */
              int destcol,      /* destination column         */
              int** pathrows,   /* vettore indici di riga del path */
              int** pathcols,   /* vettore indici di colonna del path */
              int cidx,         /* indice posizione corrente nel path */
              int* k,           /* numero di tasks da spostare */
              int* kmax,        /* massimo valore per k (fase 0) */
              int verso,        /* sto andando sulle righe o sulle colonne? */
              int costo,        /* costo del path fino a questo punto */
              int costomax,     /* la somma del path deve fare meglio di cosi' */
              int segno,
              int*** russelmatrix,
              int*** russelcosts,
              int*** modimatrix,
              int nsources,
              int ndest,
              int nusers);




int getNextCandidate(int startrow,
                     int startcol,
                     int TTM,
                     int TPU,
                     int* minCost,
                     int*** russelmatrix,
                     int*** modimatrix,
                     int** tpuser,
                     int nsources,
                     int ndest,
                     int nusers);
//
//
//int aggiustaR(int startrow,   /* starting row coordinate */
//              int startcol,   /* starting column coordinate */
//              int** pathrows, /* vettore indici di riga del path */
//              int** pathcols, /* vettore indici di colonna del path */
//              int cidx,       /* indice posizione corrente nel path */
//              int k,          /* numero di tasks da spostare */
//              int verso,      /* sto andando sulle righe o sulle colonne? */
//              int costo,      /* costo del path fino a questo punto */
//              int costomax, /* la somma del path deve fare meglio di cosi' */
//              //int segno,
//              int*** russelmatrix,
//              int*** russelcosts,
//              int nsources,
//              int ndest,
//              int nusers);

void squeezePath(int** pathrows,
                 int** pathcols,
                 int* pathLen);


void clearPath(int** pathrows,
               int** pathcols,
               int nsources,
               int ndest);

int main(int argc, char* argv[])
{
    /* //////////////////////////////////////////////////////////////////// **
    ** Strutture dati                                                       **
    ** //////////////////////////////////////////////////////////////////// */

    /*                                                                      **
    ** ... di base.                                                         **
    **                                                                      */
    FILE *f;
    /* matrice dei costi                                                    **
    **                                                                      **
    ** [ncelle][ncelle][nusers][nintervalli]                                */
    int**** costs;
    /* tasks per cella                                                      **
    **                                                                      **
    ** [ncelle]                                                             */
    int* tasks;
    /* posizione degli users nelle celle                                    **
    **                                                                      **
    ** [ncelle][nusers][nintervalli]                                        */
    int*** pos;

    int* tpuser;

    /* time is $$                                                           */
    clock_t end;
    clock_t start;
    /*
    ** 'nusers' numero di users
    ** 'nintervalli'
    ** 'ncelle'
    */
    int nusers;         /* numero di users                                  */
    int nintervalli;    /* numero di intervalli                             */
    int ncelle;         /* numero di celle                                  */
    /* indici per i cicli                                                   */
    int user;           /* indice per l'user                                */
    int tpu;            /* tasks per user                                   */
    int time;           /* indice per il tempo                              */
    int cella;          /* indice per la cella                              */
    int i, j, k;        /* indici vari utili                                */

    int flag;           /* useful boolean flag                              */
    int rs;             /* result status                                    */

    float obj=0;

    /*                                                                      **
    ** ... per RAM                                                          **
    **                                                                      */
    int nsources;       /* numero di sorgenti                               */
    int ndest;          /* numero di destinazioni                           */
    int totsupply;      /* fornitura totale                                 */
    int totdemand;      /* domanda totale                                   */

    int dest;           /* indice per le destinazioni                       */
    int source;         /* indice per le sorgenti                           */
    //int avg;
    //int ne;


    int* supply;        /* vettore delle forniture (da sorgenti)            */
    int* demand;        /* vettore delle richieste (per destinazioni)       */

    int* sourcesidx;    /* indice delle sorgenti nella matrice originale    */
    int* destsidx;      /* indice delle destinazioni nella matrice originale*/
    int* timeidx;

    int** russelcosts;  /* matrice di Russell con i costi                     */
    int** russelmatrix; /* matrice di Russell con le soluzioni                */
    int** russelweights; /* matrice di Russell con i pesi di ogni variabile   */


    int* u;             /* largest unit costs for row                       */
    int* v;             /* largest unit costs for columns                   */


    float cost;         /* variabile ausiliaria per il costo                */
//    int costi;          /* variabile ausiliaria per il costo                */
    int ccost;          /* variabile ausiliaria per il costo                */

    int itera;          /* variabile per le ~(m+n-1) iterazioni del RAM     */
    int cmin;           /* variabile aus. per il costo min nella colonna    */
    int rmin;           /* variabile aus. per il costo min nella riga       */
    int cmax;           /* variabile aus. pos. max. weight colonna          */
    int rmax;           /* variabile aus. pos. max. weight riga             */

    int maxdiff;        /* var. aus. per trovare la _differenza_ maggiore   */

    int assignment;     /* var. aus. per stabilire l'assegnazione           */
    int destsLeft;      /* destinazioni rimaste per il ciclo Russel         */
    int sourcesLeft;    /* sorgenti rimaste per il ciclo Russel             */


    /*                                                                      **
    ** ... per MODI                                                         **
    **                                                                      */
    int** modi;         /* matrice dei dei pesi per MODI                    */

    int* pathrows;      /* vettore indici di riga del path                  */
    int* pathcols;      /* vettore indici di colonna del path               */
    int  pathLen;
    int  canBeImproved; /* dice se la basic solution can be improved        */
    int modicycle;
    int profit;
    int cm;
    int rm;
    int nx;
    int nxr;
    int mnxr;
    int rx;
//    int cbc;    /* current best cost                                        */


    /* //////////////////////////////////////////////////////////////////// **
    ** Elaborazione                                                         **
    ** //////////////////////////////////////////////////////////////////// */

    /* passo il nome del file come parametro */
    if ( argc<2 ) {
        fprintf(stderr, "manca il file.\n");
        return -1;
        }

    /* leggo il numero di celle, users e intervalli per un'allocazione      **
    ** della memoria consapevole.                                           */
    f=fopen(argv[1], "r");
    assert(f!=NULL);
    fscanf(f, "%d", &ncelle);      /* leggo nr. celle      */
    fscanf(f, "%d", &nintervalli); /* leggo nr. intervalli */
    fscanf(f, "%d", &nusers);      /* leggo nr. genti      */
    fclose(f);


    /* //////////////////////////////////////////////////////////////////// **
    ** Allocazione e preparazione della struttura dati di base              **
    ** //////////////////////////////////////////////////////////////////// */
    allocBase(&tasks, &costs, &pos, &tpuser,
              ncelle, nusers, nintervalli);

    #ifdef _debugVerbose
    fprintf(stderr, "Numero di celle:      %4d\n", ncelle);
    fprintf(stderr, "Numero di users:      %4d\n", nusers);
    fprintf(stderr, "Numero di intervalli: %4d\n", nintervalli);
    #endif // _debugVerbose

    /* //////////////////////////////////////////////////////////////////// **
    ** Allocazione e preparazione della struttura dati per Russel           **
    ** //////////////////////////////////////////////////////////////////// */
    allocRussell(&supply, &demand, &sourcesidx, &destsidx, &timeidx, &u, &v,
                 &russelcosts, &russelmatrix, &russelweights,
                 nusers, ncelle, nintervalli);

    /* //////////////////////////////////////////////////////////////////// **
    ** Allocazione e preparazione della struttura dati per MODI             **
    ** //////////////////////////////////////////////////////////////////// */
    allocModi(&modi, &pathrows, &pathcols,
              nusers, ncelle, nintervalli);

    /* //////////////////////////////////////////////////////////////////// **
    ** Lettura dei dati dal file                                            **
    ** //////////////////////////////////////////////////////////////////// */
    readFile(&tasks, &costs, &pos, &tpuser, argv[1]);

    #ifdef _debugVerbose
    fprintf(stderr, "Caricamento dei dati completato.\n");
    #endif // _debugVerbose

    /* //////////////////////////////////////////////////////////////////// **
    ** Inizio elaborazione                                                  **
    ** //////////////////////////////////////////////////////////////////// */
    start=clock();

    /*
    ** Russell's Approximation Method
    ** ------------------------------
    **
    ** Per ogni riga    "i" si trova il costo massimo e lo si salva in u[i].
    ** Per ogni colonna "j" si trova il costo massimo e lo si salva in v[j].
    **
    ** Per ogni varibile non ancora inizializzata, si calcola
    **                  d[i][j] = c[i][j]-u[i]-v[j]
    **
    ** Si esegue l'assegnazione per la cella che ha il d[i][j] massimo
    ** assoluto (il piu' negativo).
    **
    ** Assegnazione
    ** ------------
    **
    ** Si assegna il massimo tra la domanda e la fornitura disponibile.
    **
    */


    /* ==================================================================== */
    /*      INIZIALIZZA SORGENTI E DI DESTINAZIONI                          */
    /* ==================================================================== */

    /* -------------------------------------------------------------------- */
    /*      DESTINAZIONI                                                    */
    /* -------------------------------------------------------------------- */

    #ifdef _debugVerbose
    fprintf(stderr, "Cerco le destinazioni... dest [tasks] -> original dest\n");
    #endif // _debugVerbose


    dest=0;
    totdemand=0;
    for ( cella=0; cella<ncelle; cella++ )
        if ( tasks[cella]>0 ) {
            demand[dest]   =tasks[cella]; /* imposta la richiesta per la    **
                                          ** destinazione                   */
            totdemand     +=tasks[cella]; /* incrementa il numero totale    **
                                          ** di tasks                       */
            destsidx[dest] =cella;        /* salva la corrispondenza per    **
                                          ** la destinazione                */
            #ifdef _debugVerbose
            fprintf(stderr, "Nuova destinazione: %4d [%4d] -> %4d.\n",
                   dest, demand[dest], destsidx[dest]);
            #endif // _debugVerbose

            dest++;
            }

    ndest=dest+1; /* la dummy-destination                                   */

    #ifdef _debugVerbose
    fprintf(stderr, "Ho inizializzato le destinazioni.\n");
    #endif // _debugVerbose


    /* -------------------------------------------------------------------- */
    /*      SORGENTI                                                        */
    /* -------------------------------------------------------------------- */
    #ifdef _debugVerbose
    fprintf(stderr,
            "Cerco le sorgenti. sorg [tipo user] [n. users] [tasks] -> sorg orig.\n");
    #endif // _debugVerbose


    totsupply=0;
    source=0;
    /* in questo contesto "flag" e' TRUE se la cella corrente contiene      **
    ** almeno un tipo di user diverso da zero                               */
    for ( time=0; time<nintervalli; time++ )
        for ( cella=0; cella<ncelle; cella++ ) {

            flag=false;
            for ( user=0; user<nusers; user++ )
                if ( pos[cella][user][time]!=0 ) {
                    flag=true;
                    break;
                }

            if ( !flag )
                continue;

            /* Riempi le prossime nusers sources con i task disponibili.    **
            ** NB: considero i tasks non gli utenti                         */
            for ( user=0; user<nusers; user++ ) {
                tpu = tpuser[user];

                supply[source+user]     = pos[cella][user][time]*tpu;
                totsupply              += pos[cella][user][time]*tpu;
                sourcesidx[source+user] = cella;
                timeidx[source+user]    = time;

                #ifdef _debugVerbose
                fprintf(stderr,
                        "Nuova sorgente: %4d [%4d] [%4d] [%4d] -> %4d.\n",
                       source+user, user, pos[cella][user][time],
                       supply[source+user], sourcesidx[source]);
                #endif // _debugVerbose
            }
            /* avanza di nusers sorgenti                                    */
            source += nusers;
        }
    nsources=source;

    #ifdef _debugVerbose
    fprintf(stderr,
            "Ho inizializzato le sorgenti. (supply %6d e demand %6d)\n",
           totsupply, totdemand);
    #endif // _debugVerbose

    /* calcola la differenza tra domanda e fornitura e assegnala alla       **
    ** dummy-destination                                                    */
    if ( totsupply>totdemand )
        demand[ndest-1]=totsupply-totdemand;
    else
        demand[ndest-1]=0;



    /* ==================================================================== */
    /*      COSTI E MATRICI DELLE VARIABILI                                 */
    /* ==================================================================== */

    maxdiff=INT_MIN;
    for ( source=0; source<nsources; source++ ) {
        i=sourcesidx[source];
        user=source%nusers;
        tpu=tpuser[user];
        time=timeidx[source];

        for ( dest=0; dest<ndest-1; dest++ ) {
            j=destsidx[dest];

            /* normalizzo il costo dello spostamento sorgente-destinazione  **
            ** per il numero di task che puo' svolgere l'utente della       **
            ** sorgente                                                     */
            cost=(float) costs[i][j][user][time] / tpu;

            russelcosts[source][dest]=(int) (cost*100);

            /* inizializzo anche la matrice degli asegnamenti               */
            russelmatrix[source][dest]=-1;

            /* trovo il massimo costo dalla matrice per asegnarlo alla      **
            ** dummy-destination                                            */
            if ( russelcosts[source][dest]>maxdiff )
                maxdiff=russelcosts[source][dest];
            }
        }

    /* inizializzo i costi della dummy-destination con il costo massimo in  **
    ** assoluto                                                             */
    for ( source=0; source<nsources; source++ )
        russelcosts[source][ndest-1]=maxdiff+1;

    /* porto al massimo il costo per spostamenti che implicano come         **
    ** sorgente e come destinazione la stessa cella                         */
    for ( source=0; source<nsources; source++ )
        for ( dest=0; dest<ndest; dest++)
            if ( sourcesidx[source]==destsidx[dest] )
                russelcosts[source][dest] = maxdiff+1;


    #ifdef _debugPrintCostsMatrix
    /* stampa matrice dei costi per debug                                   */
    fprintf(stderr, "COSTI ;");
    for (j=0; j<ndest;j++)
        fprintf(stderr, " %4d ;", j);
    fprintf(stderr, "\n");

    for (i=0; i<nsources; i++) {
        fprintf(stderr, " %4d ;", i);
        for (j=0; j<ndest; j++)
            fprintf(stderr, " %4d ;", russelcosts[i][j]);
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    #endif // _debugPrintCostsMatrix

    /* ==================================================================== */
    /*      RUSSELL'S APPROXIMATION METHOD                                  */
    /* ==================================================================== */
    #ifdef _debugVerbose
    fprintf(stderr, "Inizio elaborazione RAM::\n");
    #endif // _debugVerbose

    /* NOTA: i vettori 'u' e 'v' sono stati inizializzati a INT_MIN durante **
    ** l'allocazione                                                        */

    /* trovo i massimi di riga e colonna                                    */
    for ( source=0; source<nsources; source++ )
        for ( dest=0; dest<ndest; dest++ ) {
            ccost=russelcosts[source][dest];

            #ifdef _debugVerbose
            fprintf(stderr, "Sorgente %4d - destinazione %4d => costo %4d\n",
                   source, dest, ccost);
            #endif // _debugVerbose

            /* controllo se ho trovato un massimo per la riga "source"      */
            if ( ccost>u[source] )
                u[source]=ccost;

            /* controllo se ho trovato un massimo per la colonna "dest"     */
            if ( ccost>v[dest] )
                v[dest]=ccost;
            }

    maxdiff=INT_MAX;
    rmax=-1;
    cmax=-1;
    /* calcola le delta per ogni cella d[i][j] = c[i][j]-u[i]-v[j]          */
    for ( source=0; source<nsources; source++ )
        for ( dest=0; dest<ndest; dest++ ) {

            ccost=russelcosts[source][dest];
            russelweights[source][dest]=ccost-u[source]-v[dest];

            // TODO: togliere la ricerca del max min
            if ( russelweights[source][dest]<maxdiff ) {
                maxdiff=russelweights[source][dest];
                rmax=source;
                cmax=dest;
                }
            else
            if ( russelweights[source][dest]==maxdiff ) {
                if ( russelcosts[source][dest]>russelcosts[rmax][cmax] ) {
                    rmax=source;
                    cmax=dest;
                    }
                }
            }



    /* ==================================================================== */
    /* ==================================================================== */
    /*                           CICLO RUSSELL                              */
    /* ==================================================================== */
    /* ==================================================================== */

    #ifdef _debugPrintStepByStep
    fprintf(stderr, "Iterazioni (e quindi variabili): %4d\n", nsources+ndest-1);
    #endif // _debugPrintStepByStep

    flag = false;
    sourcesLeft = nsources;
    destsLeft   = ndest;
    itera=nsources+ndest-1;
    while ( itera>0 ) {
        itera--;

        /* ---------------------------------------------------------------- */
        /* flag=0 ci sono ancora piu' di due destinazioni e sorgenti        **
        ** flag=1 e' rimasta una sola destinazione                          **
        ** flag=2 e' rimasta una sola sorgente                              **
        ** ---------------------------------------------------------------- */
        if ( flag==1 ) {

            /* trova l'ultima destinazione rimasta                          */
            for ( j=0; j<ndest; j++ )
                if ( demand[j]>-1 ) {
                    dest=j;
                    break;
                    }
            /* assegnali tutto quello che e' rimasto                        */
            for ( source=0; source<nsources; source++ ) {
                if ( supply[source]==-1 )
                    continue;

                /* se la domanda >= della fornitura, posso spostare tutta   **
                ** la fornitura rimasta                                     **
                ** se la domanda < della fornitura, sposto solamente quando **
                ** la destinazione chiede                                   */
                if ( demand[dest] < supply[source] )
                    assignment=demand[dest];
                else
                    assignment=supply[source];

                if ( russelmatrix[source][dest]==-1 )
                    russelmatrix[source][dest]  = assignment;
                else
                    russelmatrix[source][dest] += assignment;

                supply[source] -= assignment;
                demand[dest]   -= assignment;

                if ( supply[source]==0 )
                    supply[source]=-1;

                if ( demand[dest]==0 ) {
                    demand[dest]=-1;
                    break;
                    }
                }
            itera=0;
            continue;
            }

        else if ( flag==2 ) {

            /* trova l'ultima sorgente                                      */
            for ( i=0; i<nsources; i++ )
                if ( supply[i]>-1 ) {
                    source=i;
                    break;
                    }
            /* distribuisci tutto quello che e' rimasto                     */
            for ( dest=0; dest<ndest; dest++ ) {
                if ( demand[dest]==-1 )
                    continue;

                if ( demand[dest]<supply[source] )
                    assignment = demand[dest];
                else
                    assignment = supply[source];

                if ( russelmatrix[source][dest]==-1 )
                    russelmatrix[source][dest]  = assignment;
                else
                    russelmatrix[source][dest] += assignment;

                supply[source] -= assignment;
                demand[dest]   -= assignment;

                if ( demand[dest]==0 )
                    demand[dest]=-1;

                if ( supply[source]==0 ) {
                    supply[source]=-1;
                    break;
                    }

                }
            itera=0;
            continue;
            }
        /* ---------------------------------------------------------------- */

        /* RICERCA DEL MASSIMO MINIMO ===================================== */
        maxdiff=INT_MAX;
        rmax=-1;
        cmax=-1;
        for ( source=0; source<nsources; source++ ) {
            if ( supply[source]==-1 )
                continue;

            for ( dest=0; dest<ndest; dest++ ) {
                if ( demand[dest]==-1 )
                    continue;

                if ( russelweights[source][dest]<maxdiff ) {
                    maxdiff=russelweights[source][dest];
                    rmax=source;
                    cmax=dest;
                    }
//                else
//                if ( russelweights[source][dest]==maxdiff )
//                    if ( russelcosts[source][dest]<russelcosts[rmax][cmax] ) {
//                        rmax=source;
//                        cmax=dest;
//                        }
                }
            }

            #ifdef _debugVerbose
            fprintf(stderr, "RAM: massimo minimo (%4d) in (%3d,%3d)\n",
                   maxdiff, source, dest);
            #endif // _debugVerbose

        /* FINE RICERCA DEL MASSIMO MINIMO ================================ */

        // serve ancora?????????????????????? //TODO: magari togliere
//        if ( rmax==-1 && cmax==-1 )
//            continue;


        /* ASSEGNAZIONE =================================================== */
        source  = rmax;
        dest    = cmax;

        /* se la domanda < della fornitura sposto solamente lo stretto      **
        ** necessario                                                       **
        ** se la domanda >= della fornitura, posso tranquillamente spostare **
        ** tutta la fornitura rimasta                                       */
        if ( demand[dest] < supply[source] )
            assignment=demand[dest];
        else
            assignment=supply[source];

        #ifdef _debugPrintAssignments
        fprintf(stderr, "RAM: assegno %d tasks in (%3d,%3d)\n",
                assignment, source, dest);
        #endif // _debugPrintAssignments

        if ( russelmatrix[source][dest]==-1 )
            russelmatrix[source][dest]  = assignment;
        else
            russelmatrix[source][dest] += assignment;

        supply[source] -= assignment;
        demand[dest]   -= assignment;



        if ( supply[source]==0 && demand[dest]==0 ) {
            supply[source]  = -1;
            u[source]       = -1;

            sourcesLeft--;
            if ( sourcesLeft==1 ) {
                flag=2;
                }

//            if ( itera==1 ) {
//                demand[dest]    =-1;
//                v[dest]       =-1;
//                }
            }

        else {
            if ( supply[source]==0 ) {
                supply[source]  = -1;
                u[source]       = -1;

                sourcesLeft--;
                if ( sourcesLeft==1 )
                    flag=2;
                }

            else if ( demand[dest]==0 ) {
                demand[dest]    =-1;
                v[dest]         =-1;

                destsLeft--;
                if ( destsLeft==1 )
                    flag=1;

                }
            }


//        /* AGGIORNAMENTO FORNITURE / DOMANDA                                */
//        /* situazione degenere                                              */
//        if ( (supply[source]==0 && demand[dest]==0) || supply[source]==0 ) {
//            supply[source]  = -1;
//            u[source]       = -1;
//
//            if ( itera==1 && demand[dest]==0 ) {
//                demand[dest]    =-1;
//                v[dest]       =-1;
//                }
//            }
//        else
//            if ( demand[dest]==0 ) {
//                demand[dest]    =-1;
//                v[dest]       =-1;
//                flag--;
//                }



        /* AGGIORNAMENTO ================================================== */
        /* se viene eliminata una destinazione, non devo aggiornare nulla   **
        ** in quanto il massimo per la riga si trovera' sempre nella dummy  **
        ** destination.                                                     **
        ** se viene eliminata una riga bisogna:                             **
        **   1. controllare se qualche colonna aveva il massimo proprio su  **
        **      quella riga;                                                **
        **   2. se 1 e' vera, per quella colonna ricalcolare le             **
        **      delta[i][j]                                                 */
        if ( supply[source]==-1 && flag==false ) {
            for ( dest=0; dest<ndest-1; dest++ ) {
                ccost=russelcosts[source][dest];

                /* se la colonna aveva il massimo su quella riga            */
                if ( v[dest]==ccost ) {
                    v[dest]=-1;

                    /* aggiorno il massimo per la colonna                   */
                    for ( i=0; i<nsources; i++ ) {
                        if ( supply[i]==-1 )
                            continue;

                        ccost=russelcosts[i][dest];
                        if ( ccost>v[dest] )
                            v[dest]=ccost;
                        }

                    /* aggiorno i delta[i][j] per la colonna               */
                    for ( i=0; i<nsources; i++ ) {
                        if ( supply[i]==-1 )
                            continue;

                        ccost=russelcosts[i][dest];
                        russelweights[i][dest]=ccost-u[i]-v[dest];
                        }
                    }
                }
            }
        /* FINE AGGIORNAMENTO ============================================= */

        #ifdef _debugPrintStepByStep
        fprintf(stderr, " DOPO L'ASSEGNAZIONE\n");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "=======");
        fprintf(stderr, "\n");

        fprintf(stderr, " [%2d] ;", itera);
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", j);
        fprintf(stderr, "supply;");
        fprintf(stderr, "   u;\n");

        for (i=0; i<nsources; i++) {
            fprintf(stderr, "  %4d;", i);
            for (j=0; j<ndest; j++) {
                if (russelmatrix[i][j]!=-1)
                    fprintf(stderr, "%4d;", russelmatrix[i][j]);
                else
                    fprintf(stderr, "    ;");
            }
            fprintf(stderr, "  %4d;", supply[i]);
            fprintf(stderr, "%4d;", u[i]);

            fprintf(stderr, "\n");
        }

        fprintf(stderr, "demand;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", demand[j]);
        fprintf(stderr, "\n");
        fprintf(stderr, "   v  ;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", v[j]);
        fprintf(stderr, "\n");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "=======");
        fprintf(stderr, "\n");
        #endif // _debugPrintStepByStep
        } // while itera>0





    #ifdef _debugPrintStepByStepPrintFinal
    fprintf(stderr, " BEFORE MODI\n");
    for (j=0; j<ndest;j++)
        fprintf(stderr, "=======");
    fprintf(stderr, "\n");

    fprintf(stderr, " [%2d] ;", itera);
    for (j=0; j<ndest;j++)
        fprintf(stderr, "  %2d  ;", j);
    fprintf(stderr, "supply;");
    fprintf(stderr, "   u  ;\n");

    for (i=0; i<nsources; i++) {
        fprintf(stderr, "  %2d  ;", i);
        for (j=0; j<ndest; j++) {
            if (russelmatrix[i][j]!=-1)
                fprintf(stderr, "%6d;", russelmatrix[i][j]);
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
    for (j=0; j<ndest;j++)
        fprintf(stderr, "=======");
    fprintf(stderr, "\n");
    for (j=0; j<ndest;j++)
        fprintf(stderr, "=======");
    fprintf(stderr, "\n");
    #endif // _debugPrintStepByStepPrintFinal





    /* ==================================================================== */
    /* ==================================================================== */
    /*                            CICLO MODI                                */
    /* ==================================================================== */
    /* ==================================================================== */
    initTabuM();

    canBeImproved=true;
    modicycle=0;
    while ( canBeImproved ) {
        modicycle++;

        if ( modicycle>500 ) {
            canBeImproved=false;
            continue;
            }

        #ifdef _debugPrintStepByStepModi
        fprintf(stderr, "\nMODI: iterazione %3d\n", modicycle);
        #endif // _debugPrintStepByStepModi

        /* per tutte le variabili della Basic Solution imposto il modi      **
        ** cost a 0                                                         */
        /* contemporaneamente                                               **
        **   - conto il numero di variabili base totali ( 'nx' )            **
        **   - conto il numero di spostamenti da ogni sorgente ( 'nxr' )    **
        **   - memorizzo la sorgente che ha fatto piu' spostamenti          **
        **     di tutte ( spostamenti 'mnxr' alla riga 'rx' )               **
        **                                                                  **
        ** Questo mi serve perche' poi comincio a calcolare 'u' e 'v' delle **
        ** variabili base a partire dalla riga che ha piu' spostamenti      **
        ** verso le destinazioni (escluso quello verso la dummy-destination)*/
        nx=0;
        nxr=0;  /* Numero Variabili per Riga corrente                       */
        mnxr=0; /* Massimo Numero di Variabili per Riga finora              */
        rx=-1;  /* Riga con il max. num. Variabili                          */
        for ( source=0; source<nsources; source++ ) {
            nxr=0;
            for ( dest=0; dest<ndest; dest++ )
                if ( russelmatrix[source][dest]!=-1 ) {
                    modi[source][dest]=0;

                    if ( dest<ndest-1 )
                        nxr++;
                    }
                else
                    modi[source][dest]=-1;

            if ( nxr > mnxr ) {
                mnxr=nxr;
                rx=source;
                }

            }

        #ifdef _debugPrintStepByStepModi
        fprintf(stderr,
                "MODI: sorgente di partenza %3d (%3d spostamenti)\n",
                rx, mnxr);
        #endif // _debugPrintStepByStepModi

        /* INIZIALIZZAZAIONE VETTORI 'u' E 'v' ============================= */
        for ( source=0; source<nsources; source++ )
            u[source]=-1;
        for ( dest=0; dest<ndest; dest++ )
            v[dest]=-1;

        u[rx] = 0;
        nx    = 0;
        /* ricalcolo dei vettori "u" e "v"                                  */
        rs = getUVR(rx, 0, &nx, 0,
                    &russelmatrix, &russelcosts,
                    &u, &v, nsources, ndest);
        /* ================================================================ */


        /* RICALCOLO DELTA MATRICE MODI =================================== */
        /* ricalcolo dei coefficienti per tutta la matrice modi, escluse le **
        ** variabili base                                                   */
        /* contemporaneamente trovo la nuova variabile entrante (quella con **
        ** il delta piu' negativo in assoluto)                              */
        rmax=-1;
        cmax=-1;
        maxdiff=INT_MAX;
        for ( source=0; source<nsources; source++ )
            for ( dest=0; dest<ndest; dest++ ) {

                if ( russelmatrix[source][dest]!=-1 )
                    continue;

                modi[source][dest]=russelcosts[source][dest]-u[source]-v[dest];


               if ( isTabuM(source, dest) )
                   continue;

//                /* se trovo un delta uguale ma il costo della cella         **
//                ** e' maggiore, significa che, anche se nel peggiore dei    **
//                ** casi pago un delta uguale, in questo caso il costo       **
//                ** assoluto sara' maggiore. Magari conviene prendere questa */
//                if ( modi[source][dest]==maxdiff && maxdiff<20 ) {
//                    if ( russelcosts[source][dest]>russelcosts[rmax][cmax] ) {
//                        rmax=source;
//                        cmax=dest;
//                        }
//                    }

                if ( modi[source][dest]<maxdiff ) {
                    maxdiff=modi[source][dest];
                    rmax=source;
                    cmax=dest;
                    }

                // if ( maxdiff<0 )
                //     break;

            }
        /* ================================================================ */



        #ifdef _debugPrintStepByStepModi
        if ( maxdiff<0 )
            fprintf(stderr, "MODI: best improvement %d in (%3d,%3d)\n",
                    maxdiff, rmax, cmax);
        #endif // _debugPrintStepByStepModi


        if ( maxdiff>=0 ) { // ora non gestisco piu' i 0ri nella modi
            #ifdef _debugPrintStepByStepModi
            fprintf(stderr, "Maxdiff POSITIVO!\n");
            #endif // _debugPrintStepByStepModi
            canBeImproved=false;
            }



        if ( !canBeImproved ) {
            #ifdef _debugPrintStepByStepModi
                fprintf(stderr, "MODI: solution CANNOT be improved!\n");
            #endif // _debugPrintStepByStepModi
            continue;
            }
            #ifdef _debugPrintStepByStepModi
        else{
                fprintf(stderr, "MODI: solution CAN be improved!\n");
            }
            #endif // _debugPrintStepByStepModi

        //addTabuM(rmax, cmax);
        /* SETTING OFF CHAIN REACTION ===================================== */
        initTabu();
        clearPath(&pathrows, &pathcols, nsources, ndest);


        pathrows[0]=rmax;
        pathcols[0]=cmax;


        rs = findPathR(rmax, /* starting row coordinate                     */
                       cmax, /* starting column coordinate                  */
                       rmax, /* destination row coordinate                  */
                       cmax, /* destination column coordinate               */
                       &pathrows, /* vettore indici di riga del path        */
                       &pathcols, /* vettore indici di colonna del path     */
                       1,  /* la prima posizione e' la starting cell        */
                       -1, /* inizialmente nessuna direzione e' vietata     */
                       &russelmatrix,
                       &russelcosts,
                       nsources,
                       ndest);

        if ( rs ) {
            #ifdef _debugPrintStepByStepModi
            fprintf(stderr, "MODI: path found:\n");
            i=0;
            while ( pathrows[i]!=-1 ) {
                fprintf(stderr, "    (%3d,%3d)\n", pathrows[i], pathcols[i]);
                i++;
                }
            #endif // _debugPrintStepByStepModi
            }
        else {
                #ifdef _debugPrintStepByStepModi
            fprintf(stderr, "PROBLEMS: NO PATH FOUND:\n");
                #endif // _debugPrintStepByStepModi
           addTabuM(rmax, cmax);
            //canBeImproved=false;
            continue;
            }
        /* ================================================================ */


        /* SPOSTAMENTO TASK =============================================== */
        /* trova la massima quantita' di task che possono essere spostati;  **
        ** la massima quantita' di task che possono essere spostati         **
        ** corrisponde al valore piu' piccolo tra quelli delle celle        **
        ** donatrici                                                        **
        **                                                                  **
        ** per la codifica del problema, le celle donatrici si trovano agli **
        ** indici dispari del vettore che contiene il path                  */

        cm=-1;  /* indice di colonna della variabile piu' piccola           */
        rm=-1;  /* indice di riga della cordinata piu' piccole              */
        profit=0;
        pathLen=0;
        k=INT_MAX;
        while (pathrows[pathLen]!=-1) {
            source  =pathrows[pathLen];
            dest    =pathcols[pathLen];

            if ( pathLen%2==1 ) {
                profit -= russelcosts[source][dest];
                if ( russelmatrix[source][dest]<k ) {
                    k=russelmatrix[source][dest];
                    rm=source;
                    cm=dest;
                    }
                }
            else
                profit += russelcosts[source][dest];

            pathLen++;
            }


        #ifdef _debugPrintStepByStepModi
        fprintf(stderr,
                "MODI: pathLen %3d, rialloco %d tasks "
                "(%3d,%3d) -> (%3d,%3d), profit %d\n",
                pathLen, k, rm, cm, rmax, cmax, profit);
        #endif // _debugPrintStepByStepModi

        /* aggiungi/togli task secondo la regola delle celle recipienti e       **
        ** donatrici                                                            */
        for ( i=0; i<pathLen; i++ ) {
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
        fprintf(stderr, " NEW MODI MATRIX\n");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "=======");
        fprintf(stderr, "\n");
        fprintf(stderr, " [%2d] ;", itera);
        for (j=0; j<ndest;j++)
            fprintf(stderr, "  %2d  ;", j);
        fprintf(stderr, "   u  ;\n");

        for (i=0; i<nsources; i++) {
            fprintf(stderr, "  %2d  ;", i);
            for (j=0; j<ndest; j++) {
                if (modi[i][j]!=-1)
                    fprintf(stderr, "%6d;", modi[i][j]);
                else
                    fprintf(stderr, "      ;");
            }
            fprintf(stderr, "%6d;", u[i]);

            fprintf(stderr, "\n");
        }

        fprintf(stderr, "   v  ;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%6d;", v[j]);
        fprintf(stderr, "\n");
    //        fprintf(stderr, "\n");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "=======");
        fprintf(stderr, "\n");
        #endif // _debugPrintStepByStepModi
        #ifdef _debugPrintStepByStepModi
        fprintf(stderr, " NEW ASSIGNMENTS\n");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "=======");
        fprintf(stderr, "\n");
        fprintf(stderr, " [%2d] ;", itera);
        for (j=0; j<ndest;j++)
            fprintf(stderr, "  %2d  ;", j);
        fprintf(stderr, "   u  ;\n");

        for (i=0; i<nsources; i++) {
            fprintf(stderr, "  %2d  ;", i);
            for (j=0; j<ndest; j++) {
                if (russelmatrix[i][j]!=-1)
                    fprintf(stderr, "%6d;", russelmatrix[i][j]);
                else
                    fprintf(stderr, "      ;");
            }
            fprintf(stderr, "%6d;", u[i]);

            fprintf(stderr, "\n");
        }
        fprintf(stderr, "   v  ;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%6d;", v[j]);
        fprintf(stderr, "\n");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "=======");
        fprintf(stderr, "\n");
        #endif // _debugPrintStepByStepPrintFinal

        } // while ( canBeImproved )...

    /* ==================================================================== */
    /* ==================================================================== */
    #ifdef _debugPrintStepByStepPrintFinal

    fprintf(stderr, " NEW MODI MATRIX\n");
    for (j=0; j<ndest;j++)
        fprintf(stderr, "=======");
    fprintf(stderr, "\n");
    fprintf(stderr, " [%2d] ;", itera);
    for (j=0; j<ndest;j++)
        fprintf(stderr, "  %2d  ;", j);
    fprintf(stderr, "   u  ;\n");

    for (i=0; i<nsources; i++) {
        fprintf(stderr, "  %2d  ;", i);
        for (j=0; j<ndest; j++) {
            if (modi[i][j]!=-1)
                fprintf(stderr, "%6d;", modi[i][j]);
            else
                fprintf(stderr, "      ;");
        }
        fprintf(stderr, "%6d;", u[i]);

        fprintf(stderr, "\n");
    }
    for (j=0; j<ndest;j++)
        fprintf(stderr, "=======");
    fprintf(stderr, "\n");

    fprintf(stderr, " AFTER MODI, BEFORE ADJUSTMENT\n");
    for (j=0; j<ndest;j++)
        fprintf(stderr, "=======");
    fprintf(stderr, "\n");
    fprintf(stderr, " [%2d] ;", itera);
    for (j=0; j<ndest;j++)
        fprintf(stderr, "  %2d  ;", j);
    fprintf(stderr, "\n");

    for (i=0; i<nsources; i++) {
        fprintf(stderr, "  %2d  ;", i);
        for (j=0; j<ndest; j++) {
            if (russelmatrix[i][j]!=-1)
                fprintf(stderr, "%6d;", russelmatrix[i][j]);
            else
                fprintf(stderr, "      ;");
        }
        fprintf(stderr, "\n");
    }
    for (j=0; j<ndest;j++)
        fprintf(stderr, "=======");
    fprintf(stderr, "\n");

    #endif // _debugPrintStepByStepPrintFinal
    /* ==================================================================== */
    /* Aggiustamento                                                        */
    /* ==================================================================== */

    for ( dest=0; dest<ndest; dest++ ) {
        v[dest]=0;
        u[dest]=0;
        }



    /* Preparo i percorsi                                                   */
    /* Percorso per righe                                                   */
    i=0;
    for ( source=0; source<nsources; source++ ) {
        searchByIndexesRow[source]=i;

        for ( dest=0; dest<ndest; dest++ )
            if ( russelmatrix[source][dest]>=0 ) {
                searchByRow[i].row    = source;
                searchByRow[i].column = dest;
                i++;
                }
            }

    /* Percorso per colonne                                                 */
    i=0;
    for ( dest=0; dest<ndest; dest++ ) {
        searchByIndexesCol[dest]=i;

        for ( source=0; source<nsources; source++ ) {
            tpu = tpuser[(source%nusers)];

            if ( russelmatrix[source][dest]>=0 ) {

                searchByCol[i].row    = source;
                searchByCol[i].column = dest;
                i++;

                if ( russelmatrix[source][dest]%tpu!=0 ) {
                    v[dest]++;
                    u[dest] = source;
                    }
                }


            }
        }
    sbNCelle=i;


    fprintf(stderr, " CVIV ;");
    for ( dest=0; dest<ndest; dest++ )
        fprintf(stderr, "%6d;", v[dest]);
    fprintf(stderr, "\n");
    fprintf(stderr, "source;");
    for ( dest=0; dest<ndest; dest++ )
        fprintf(stderr, "%6d;", u[dest]);
    fprintf(stderr, "\n");


    /* FASE 1                                                               **
    ** Per tutte quelle celle che violano il vincolo ma che sono da sole    **
    ** sulla colonna, non resta che:                                        **
    **   1. Allocare una nuova cella, che non violi i vincoli e che abbia   **
    **      il prezzo piu' basso possibile                                  **
    **   2. Aggiungere task fino a far rispettare i vincoli se cio' che     **
    **      pago sovrabbondando con i task e' inferiore a cio' che pagherei **
    **      spostando quelli in eccesso (a patto di avere disponibilita')   */

    /* DATI CVIV                                                            */
    int ttm;            // TaskToMove
    int ottm;           // OriginalTaskToMove
    int tta;            // TaskToAdd
    int icost = -1;     // costo cella con violazione
    int costA = -1;     // costo abbondando

    rmax      = -1;     // riga cella con violazione
    user      = -1;     // user cella con violazione
    tpu       = -1;     // TaskPerUser cella con violazione

    /* DATI cella corrente nell'iterazione per colonna                      */
    int cuser = -1;     // current user
    int ctpu  = -1;     // current TaskPerUser
    ccost     = -1;     // current cost

    rmin  = -1;         // current riga con costo minimo
    cmin  = INT_MAX;    // costo minimo

    int canA  = false;
    int r, c;
    int mcm;

    int cvivSolved;

    for ( j=0; j<ndest-1; j++ ) {
        if ( v[j]==1 ) {

            cmin  =INT_MAX;                     // costo minimo
            rmax  = u[j];                       // indice di riga CVIV
            user  = rmax%nusers;                // tipo user CVIC
            tpu   = tpuser[user];               // TPU CVIV
            icost  = russelcosts[rmax][j];      // costo CVIV
            ttm   = russelmatrix[rmax][j]%tpu;  // Task To Move
            tta   = tpu - ttm;                  // Task To Add
            ottm  = ttm;
            fprintf(stderr, "\n ttm %d\n", ttm);
            fprintf(stderr, " tpu %d\n", tpu);
            fprintf(stderr, "icost %d\n", icost);
            /* Se la cella si e' messa a posto grazie a poteri              **
            ** sovrannaturali la ignoro                                     */
            if ( ttm==0 )
                continue;


            /* Per avere un confronto reale tra il costo di spostare i task
            ** in eccesso e quello di sovrabbondare per far rispettare il
            ** TPU, dovremmo sottrarre al primo costo il guagno dal
            ** togliere i task in ecceso dalla CVIV. Per evitare cio'
            ** faccio la cosa contraria e considero il secondo costo come
            ** il costo di spostare TPU tasks nella CVIV.
            **
            ** Ricordo che
            **                 tta + ttm = tpu
            **
            ** In un primo momento inoltre considero il secondo costo
            ** solamente se ho disponibilita' di TTA task nel deposito
            ** della CVIV.                                                  */
            if ( russelmatrix[rmax][ndest-1]>=tta )
                // costA = cost*tpu;
                costA = icost*tta;
            else
                costA = INT_MAX;

            fprintf(stderr, "Cella (%3d,%3d), unica sulla colonna\n", rmax, j);
            initTabuM();
            addTabuM(rmax, j);
            cvivSolved = false;
            while ( !cvivSolved ) {
                ttm=ottm;


                source = getNextCandidate(rmax, j, ttm, tpu, &cmin,
                                          &russelmatrix, &modi, &tpuser,
                                          nsources, ndest, nusers);

                cuser  = source%nusers;
                ctpu   = tpuser[cuser];
                addTabuM(source, j);

                /* Prima di procedere devo capire quanti task ho deciso di      **
                ** spostare.                                                    */
                if ( russelmatrix[source][j]==-1 ) {
                    if ( (ttm%ctpu)!=0 ) {
                        if ( ((ttm+tpu)%ctpu)==0 )
                            ttm += tpu;
                        else
                            fprintf(stderr, "WHAT THE TPU TTM HACK?!\n");
                        }
                    }
                else {
                    if ( russelmatrix[source][j]>=0 ) {
                        if ( ((russelmatrix[source][j]+ttm)%ctpu)!=0 ) {
                            if ( ((russelmatrix[source][j]+ttm+tpu)%ctpu)==0 )
                                ttm += tpu;
                            else
                                fprintf(stderr, "WHAT THE TPU TTM HACK SECOND?!\n");
                            }
                        }
                    }

                fprintf(stderr, "ha come minimo (%3d,%3d), costo %d (ttm %d)\n",
                        source, j, cmin, ttm);
                /* NON dimentichiamoci che, se abbondare conviene piu' di       **
                ** togliere i task in eccesso, allora con fare DECISO           **
                ** abbondiamo                                                   **
                **        "Melius abundare quam deficere".                      **
                **                                                              */
                fprintf(stderr, "costA %d, cmin %d, ttm %d cost %d\n",
                        costA, cmin, ttm, icost);

                // if ( costA<cmin && tpu*costA<cmin ) {
                if ( costA <= cmin ) {
                    fprintf(stderr, "Melius abundare quam deficere.\n");
                    fprintf(stderr, "costa %d, cmin %d.\n", costA, cmin);
                    cmin = costA;
                    rmin = rmax;

                    russelmatrix[rmax][j] += tta;
                    russelmatrix[rmax][ndest-1] -= tta;

                    if ( russelmatrix[rmax][ndest-1]==0 )
                        russelmatrix[rmax][ndest-1] =-1;

                    for ( i=0; i<nsources; i++ ) {
                        if ( russelmatrix[i][j]==-1 )
                            continue;

                        cuser = i%nusers;
                        ctpu  = tpuser[cuser];
                        if ( ((russelmatrix[i][j]-tta)%ctpu)==0 ) {
                            russelmatrix[i][j] -= tta;

                            if ( russelmatrix[i][ndest-1]==-1 )
                                russelmatrix[i][ndest-1]  = tta;
                            else
                                russelmatrix[i][ndest-1] += tta;
                            break;
                            }
                        }

                    cvivSolved = true;
                    continue;
                    }


                /* Procedimento e casi                                          **
                ** -------------------                                          **
                **
                ** CACM  - Cella A Costo Minimo
                ** CVIV  - Cella Violante I Vincoli
                ** CNVIV - Cella Non Violante I Vincoli
                ** TTM   - Task To Move
                **
                **
                ** CASO 1                                                       **
                ** ============================================                 **
                ** La cella a costo minimo ha task nel deposito                 **
                ** --------------------------------------------                 **
                ** Soluzione 1 [C1S1]
                ** Controllo se esiste un path che parte dalla CACM, passa
                ** dalla CVIV e poi passa solamente attraverso celle gia'
                ** occupate senza che cio' introduca nuove violazioni, anzi,
                ** mi conviene passare nella cella occupata se:
                **   la cella era una CVIV e risolvo la violazione, oppure,
                **   la cella era una CVIV e non risolvo la violazione
                **
                ** Mi conviene la soluzione 1 rispetto alla 2 perche' non
                ** introduco altri task nella soluzione ma li rialloco, il
                ** costo complessivo non cresce significativamente o comunque
                ** meno che nella soluzione 2.
                **
                ** Regole per costruire il path:
                **    - e' vietato passare dal deposito se questo non ha task;
                **    - se mi trovo nel deposito della riga non m'importa se
                **      creo violazioni (significa che probabilmente ci sono
                **      altre violazioni sulla riga)
                **    - se passo da una CVIV, la accetto se:
                **         * risolvo i vincoli
                **         * non risolvo i vincoli
                **    - se passo da una CNVIV la accetto se:
                **         * non introduco violazioni
                **    - il TTM e' fisso, non puo' cambiare lungo il percorso
                **
                **
                ** Soluzione 2 [C1S2]
                **  1. Sposto i TTM dal deposito della CACM alla CACM;
                **  2. Tolgo i task dalla CVIV e li aggiungo al deposito
                **     della CVIV.
                **
                ** Eventualmente ci fosse qualche cella sulla stessa riga della
                ** CVIV che e' a sua volta una CVIV e ha necessita' di
                ** esattamente i TTM per completare i suoi TPU, conviene
                ** spostarli li'?
                ** Si', l'esito della Soluzione 1 risponde a questa domanda.
                **
                **
                **
                ** CASO 2                                                       **
                ** ================================================             **
                ** La cella a costo minimo NON ha task nel deposito             **
                ** ------------------------------------------------             **
                ** (e' compresa la situazione di quando il deposito ha tasks
                **  ma non abbastanza da soddisfare lo spostamento)
                **
                ** Soluzione 0 [C2S0] -- NO! ERRORE! FARE CIO' SIGNIFICA
                **                       ABBONDARE E QUESTO LO ABBIAMO GIA'
                **                       SCARTATO NEL MOMENTO IN CUI ABBIAMO
                **                       CONFRONTATO IL COSTO DI ABBONDARE CON
                **                       IL COSTO MINIMO!
                // ** Dato che il deposito della CACM e' vuoto, faccio il giro
                // ** nell'altro senso, spostando i TTA.
                // ** Le caratteristiche di questo path sono le stesse del path di
                // ** [C1S1]:
                // **   - e' vietato passare dal deposito se non ha task
                // **   - nei depositi non si controllano le violazioni
                // **   - tutte le celle all'interno del path devono essere CNVIV
                // **     o diventarlo dopo l'attraversamento
                // **   - i TTA sono fissi
                // **
                // ** [WARN] Il path e' valido SSE la CVIV originaria vi e'
                // **        compresa
                **
                **
                **
                ** Soluzione 1 [C2S1]
                ** Cerco un path come per il [C1S1].
                ** Le regole sono le stesse di [C1S1] tranne che in questo caso
                ** ammetto il passaggio dal deposito della CVIV anche se non ha
                ** tasks perche' sono sicuro che non puo' semplicemente fare il
                ** giro dai depositi (non avendo la CACM task nel deposito).
                ** Il TTM continua ad essere fisso.
                ** Il path parte dalla CACM, passa dalla CVIV e continua secondo
                ** le regole sopra definite.
                **
                ** In questo modo, eventualmente, risolvo la violazione con uno
                ** spostamento di task.
                **
                **
                ** Soluzione 2 [C2S2]
                ** Cerco un path per spostare task nel desposito della CACM.
                ** Regole per la costruzione del path:
                **    - I TTM possono essere variaibli in base alle richieste
                **      delle CNVIV per non violare i vincoli;
                **    - Se passo per una CVIV non me ne curo, e la accetto a
                **      prescinedere;
                **    - Si passa solamente per le celle base occupate;
                ** Lo scopo e' creare disponibilita' nel deposito della CACM
                ** senza rovinare le celle che rispettano i vincoli.
                **
                ** Creata la disponibilita', si procede come nella [C1S2]:
                **  1. Sposto i TTM dal deposito della CACM alla CACM;
                **  2. Tolgo i task dalla CVIV e li aggiungo al deposito
                **     della CVIV.
                **
                ** [NOTA] Dopo aver spostato i task nel deposito della CACM e
                **        aver eseguito la [C2S2], puo' capitare che altre CVIV
                **        abbiano bisogno di spostare task sulla riga della
                **        corrente CACM; In tal caso pero' ci ritroverremmo nel
                **        CASO 1. In tal caso, non riuscendo a costruire un path
                **        alternativo, (non usa il deposito della CVIV e non vede
                **        il deposito della CACM corrente come variabile di base),
                **        procedera' con la [C1S2], la quale va in ogni caso bene.
                **
                */



                if ( russelmatrix[source][ndest-1]>=ttm ) { // CASO 1
                    clearPath(&pathrows, &pathcols, nsources, ndest);
                    initTabu();

                    pathrows[0]=source;
                    pathcols[0]=j;

                    // pathrows[1]=rmax;
                    // pathcols[1]=j;
                    // addTabu(rmax, j);

                    rs = fastFindPathR(source, j,
                                       source, j,
                                       &pathrows, &pathcols,
                                       1, ttm,
                                       &russelmatrix, &tpuser,
                                       nusers, nsources, ndest,
                                       true,    // payAttention
                                       false);  // depAllowed


                    // rs = fastFindPathRR(source, j,
                    //                     source, j,
                    //                     &pathrows, &pathcols,
                    //                     1, ttm,
                    //                     modi[source][j], // current cost
                    //                     modi[source][j], // max cost allowed
                    //                     &russelmatrix, &modi, &tpuser,
                    //                     nusers, nsources, ndest,
                    //                     true);

                    if ( rs ) { // Soluzione 1 [C1S1]
                        fprintf(stderr, "[C1S1] Path found\n");
                        for ( i=0; pathrows[i]!=-1; i++ ) {
                            r = pathrows[i];
                            c = pathcols[i];

                            if ( (i%2)==0 ) { // indice pari -> +
                                fprintf(stderr, "Aggiungo a  (%3d,%3d)\n", r, c);
                                if ( russelmatrix[r][c]==-1 )
                                    russelmatrix[r][c]  = ttm;
                                else
                                    russelmatrix[r][c] += ttm;

                                }
                            else { // indice dispari -> -
                                fprintf(stderr, "Sottraggo a (%3d,%3d)\n", r, c);
                                russelmatrix[r][c] -= ttm;
                                }
                            }
                        // continue;
                        cvivSolved = true;
                        }
                    else { // Soluzione 2 [C1S2]
                        fprintf(stderr, "[C1S1] Path NOT found\n");
                        fprintf(stderr, "[C1S2] Appliable\n");
                        russelmatrix[source][ndest-1] -= ttm;

                        if ( russelmatrix[source][j]==-1 )
                            russelmatrix[source][j]  = ttm;
                        else
                            russelmatrix[source][j] += ttm;

                        russelmatrix[rmax][j] -= ttm;

                        if ( russelmatrix[rmax][ndest-1]==-1 )
                            russelmatrix[rmax][ndest-1]  = ttm;
                        else
                            russelmatrix[rmax][ndest-1] += ttm;

                        // continue;
                        cvivSolved = true;
                        }



                    // EMERGENZA CASO 1?

                    }
                else {  // CASO 2

                    clearPath(&pathrows, &pathcols, nsources, ndest);
                    initTabu();

                    pathrows[0]=source;
                    pathcols[0]=j;

                    // addTabu(source, j)
                    //
                    // pathrows[1]=rmax;
                    // pathcols[1]=j;

                    // rs = fastFindPathR(source, ndest-1,
                    //                    source, ndest-1,
                    //                    &pathrows, &pathcols,
                    //                    1, tta,
                    //                    &russelmatrix, &tpuser,
                    //                    nusers, nsources, ndest,
                    //                    true,   // payAttention
                    //                    false); // depAllowed

                    rs = fastFindPathRR(source, j,
                                        source, j,
                                        &pathrows, &pathcols,
                                        1, ttm,
                                        modi[source][j], // current cost
                                        // modi[source][ndest-1], // max cost allowed
                                        (costA < modi[source][ndest-1] ? costA : modi[source][ndest-1]), // max cost allowed
                                        &russelmatrix, &modi, &tpuser,
                                        nusers, nsources, ndest,
                                        true); // payAttention
                    fprintf(stderr, "MaxAllowed %d, current %d\n",
                            modi[source][ndest-1], modi[source][j]);

                    // flag = false;
                    // if ( rs ) {
                    //     fprintf(stderr, "RR - sol found\n");
                    //     for ( i=0; pathrows[i]!=-1; i++ ) {
                    //         if ( rmax==pathrows[i] && j==pathcols[i] ) {
                    //             flag = true;
                    //             break;
                    //             }
                    //         }
                    //     }
                    //
                    // clearPath(&pathrows, &pathcols, nsources, ndest);
                    // initTabu();
                    //
                    // pathrows[0]=source;
                    // pathcols[0]=j;
                    //
                    // pathrows[1]=rmax;
                    // pathcols[1]=j;
                    //
                    // rs = fastFindPathR(rmax, j,
                    //                    source, j,
                    //                    &pathrows, &pathcols,
                    //                    2, ttm,
                    //                    &russelmatrix, &tpuser,
                    //                    nusers, nsources, ndest,
                    //                    true, // payAttention
                    //                    true); // depAllowed
                    // rs=false;
                    if ( rs ) { // Soluzione 1 [C2S2]
                        fprintf(stderr, "[C2S1] RR - Path found\n");
                        for ( i=0; pathrows[i]!=-1; i++ ) {
                            r = pathrows[i];
                            c = pathcols[i];

                            if ( (i%2)==0 ) { // indice pari -> +
                                fprintf(stderr, "Aggiungo  %2d a (%3d,%3d)\n", ttm, r, c);
                                if ( russelmatrix[r][c]==-1 )
                                    russelmatrix[r][c]  = ttm;
                                else
                                    russelmatrix[r][c] += ttm;

                                }
                            else { // indice dispari -> -
                                fprintf(stderr, "Sottraggo %2d a (%3d,%3d)\n", ttm, r, c);
                                russelmatrix[r][c] -= ttm;
                                }
                            }
                        cvivSolved = true;
                        }
                    else { // Soluzione 2 [C2S2]
                        fprintf(stderr, "[C2S1] RR - Path NOT found\n");

                        if ( (tta*russelcosts[rmax][j]-tta*russelcosts[source][j])<=cmin &&
                             (russelmatrix[source][j]-tta)%ctpu == 0          &&
                             (russelmatrix[source][j]-tta)      >= 0          &&
                              russelmatrix[rmax][ndest-1]       >= tta      ) {

                            fprintf(stderr, "[C2S2] Applied\n");

                            russelmatrix[rmax][j]       += tta;
                            russelmatrix[rmax][ndest-1] -= tta;

                            if ( russelmatrix[rmax][ndest-1]==0 )
                                russelmatrix[rmax][ndest-1] =-1;

                            russelmatrix[source][j] -= tta;

                            if ( russelmatrix[source][j]==0 )
                                russelmatrix[source][j]=-1;

                            if ( russelmatrix[source][ndest-1]==-1 )
                                russelmatrix[source][ndest-1]  = tta;
                            else
                                russelmatrix[source][ndest-1] += tta;

                            cvivSolved = true;
                            continue;
                            }















                        clearPath(&pathrows, &pathcols, nsources, ndest);
                        initTabu();

                        pathrows[0]=source;
                        pathcols[0]=ndest-1;

                        // addTabu(rmax, j);
                        mcm = 0;
                        rs = fastFindPathR(source, ndest-1,
                                           source, ndest-1,
                                           &pathrows, &pathcols,
                                           1, 0,
                                           &russelmatrix, &tpuser,
                                           nusers, nsources, ndest,
                                           true, // payAttention
                                           false); // depAllowed
                        rs=false;
                        if ( rs ) {
                            /* trovo quanti task posso spostare attraverso  **
                            ** il path senza creare casini:                **
                            **  se la cella corrente e' CNVIV, allora       **
                            **  controllo se gli va bene l'mcm trovato e lo **
                            **  aggiusto di conseguenza;                    **
                            **  se la cella corrente e' CVIV, non me ne     **
                            **  curo, qualsiasi mcm gli andra' bene         */
                            fprintf(stderr, "[C2S2] Path found\n");
                            mcm =-1;
                            for ( i=0; pathrows[i]!=-1; i++ ) {
                                r = pathrows[i];
                                c = pathcols[i];
                                fprintf(stderr, " ->  (%3d,%3d)\n", r, c);

                                cuser = r%nusers;
                                ctpu  = tpuser[cuser];

                                if ( russelmatrix[r][c]%ctpu!=0 )
                                    continue;

                                if ( mcm==-1 )
                                    mcm = ctpu;

                                if ( mcm%ctpu!=0 )
                                    mcm = mcm*ctpu;
                                fprintf(stderr, "user %d tpu %d mcm %d\n",
                                        cuser, ctpu, mcm);
                                }

                            for ( i=0; pathrows[i]!=-1; i++ ) {
                                r = pathrows[i];
                                c = pathcols[i];

                                if ( (i%2)==0 ) { // indice pari -> +
                                    fprintf(stderr, "Aggiungo  %2d a (%3d,%3d)\n", mcm, r, c);
                                    if ( russelmatrix[r][c]==-1 )
                                        russelmatrix[r][c]  = mcm;
                                    else
                                        russelmatrix[r][c] += mcm;

                                    }
                                else { // indice dispari -> -
                                    fprintf(stderr, "Sottraggo %2d a (%3d,%3d)\n", mcm, r, c);
                                    russelmatrix[r][c] -= mcm;
                                    }
                                }
                            cvivSolved = true;

                            }
                        else {
                            fprintf(stderr, "[C2S2] No Path found\n");

                            }

                        }
                    }


                }







            // clearPath(&pathrows, &pathcols, nsources, ndest);
            // initTabu();
            //
            // pathrows[0]=source;
            // pathcols[0]=j;
            //
            // // pathrows[1]=rmax;
            // // pathcols[1]=j;
            //
            // rs = fastFindPathR(source, j,
            //                    source, j,
            //                    &pathrows, &pathcols,
            //                    1, ttm,
            //                    &russelmatrix, &tpuser,
            //                    nusers, nsources, ndest,
            //                    true);
            // if ( rs ) {
            //
            //     fprintf(stderr, "\n[FFPR] Direct path Found:\n");
            //     k=0;
            //     while ( pathrows[k]!=-1 ) {
            //         fprintf(stderr, "    (%3d,%3d)\n", pathrows[k], pathcols[k]);
            //         k++;
            //         }
            //
            //     }
            // else {
            //
            //     fprintf(stderr, "\n[FFPR] [!] NO Direct path found\n");
            //
            //     clearPath(&pathrows, &pathcols, nsources, ndest);
            //     initTabu();
            //
            //     pathrows[0]=source;
            //     pathcols[0]=ndest-1;
            //
            //     rs = fastFindPathR(source, ndest-1,
            //                        source, ndest-1,
            //                        &pathrows, &pathcols,
            //                        1, ttm,
            //                        &russelmatrix, &tpuser,
            //                        nusers, nsources, ndest,
            //                        false);
            //     if ( rs ) {
            //
            //        fprintf(stderr, "\n[FFPR] InDirect path Found:\n");
            //        k=0;
            //        while ( pathrows[k]!=-1 ) {
            //            fprintf(stderr, "    (%3d,%3d)\n", pathrows[k], pathcols[k]);
            //            k++;
            //            }
            //
            //        }
            //     else
            //        fprintf(stderr, "\n[FFPR] [!] NO InDirect path found\n");
            //
            //     }
            //
            //
            //
            // fprintf(stderr, "\n-----------------------------------------\n\n");
            //







            }  // if ( v[j]==1 )
        } // for ( j=0; j<ndest; j++ )





























    /* ==================================================================== */
    /* -------------------------------------------------------------------- */
    /* ==================================================================== */




    #ifdef _debugPrintStepByStepPrintFinal
    fprintf(stderr,
            "----------------------------------------------------------------------------------------------------\n");
    fprintf(stderr, " FINAL MATRIX AFTER ADJUSTMENT\n");
    fprintf(stderr,
            "----------------------------------------------------------------------------------------------------\n");

    fprintf(stderr, " tpu  ;");
    fprintf(stderr, "      ;");
    for (j=0; j<ndest;j++)
        fprintf(stderr, "  %2d  ;", j);

    fprintf(stderr, "\n");
    for (i=0; i<nsources; i++) {
        fprintf(stderr, "  %2d  ;", i%nusers+1);
        fprintf(stderr, "  %2d  ;", i);
        for (j=0; j<ndest; j++) {
            if (russelmatrix[i][j]!=-1)
                fprintf(stderr, "%6d;", russelmatrix[i][j]);
            else
                fprintf(stderr, "      ;");
            }
        fprintf(stderr, "\n");
    }

    fprintf(stderr, "\n");
    fprintf(stderr,
            "----------------------------------------------------------------------------------------------------\n");
    fprintf(stderr,
            "----------------------------------------------------------------------------------------------------\n");
    #endif // _debugPrintStepByStepPrintFinal


    end=clock();

    if ( isFeasible(&russelmatrix, &pos, &tasks,
                    &sourcesidx, &destsidx, &timeidx, &tpuser,
                    nsources, ndest, nusers) ) {
        obj=0;
        for ( source=0; source < nsources; source++ )
            for ( dest=0; dest<ndest-1; dest++ )
                if ( russelmatrix[source][dest]!=-1 ) {
                    user    = source%nusers;
                    i       = sourcesidx[source];
                    j       = destsidx[dest];
                    time    = timeidx[source];
                    tpu     = tpuser[user];
//                    k =(int) ceil((float)  russelmatrix[source][dest] / (user+1));

                    k = floor((float)russelmatrix[source][dest] / tpu);
                    obj += k * costs[i][j][user][time];
                    }

//        printf("Istanza %s.\n", argv[1]);
//        printf("Tempo: %f\n", ((double)end-start)/CLOCKS_PER_SEC);
//        printf("Costo: %f\n", obj);
//        printf("ITERAZIONE;Istanza;OF;Time\n");

        printf("%s;%d;%f\n",basename(argv[1]),
               (int) obj,(((double)end-start)/CLOCKS_PER_SEC));
        fprintf(stderr, "%s;%f;%f\n",basename(argv[1]),
               obj, (((double)end-start)/CLOCKS_PER_SEC));
        }
    else {
        fprintf(stdout, "%s;-;-;SOLUTION UNFEASIBLE\n",
                basename(argv[1]));
        fprintf(stderr, "%s;-;-;SOLUTION UNFEASIBLE\n",
                basename(argv[1]));

        }
    #ifdef _debugVerbose
    fprintf(stderr, "\n\n");
    #endif // _debugVerbose


    fprintf(stderr, "\n\n");

    #ifdef _debugPrintStepByStepModi
    fprintf(stderr, "\nIterazioni MODI compiute: %4d\n", modicycle);
    #endif // _debugPrintStepByStepModi



    /* //////////////////////////////////////////////////////////////////// **
    **                                                                      **
    ** Libera tutti                                                         **
    **                                                                      **
    ** //////////////////////////////////////////////////////////////////// */

    free(tasks);

    for (i=0; i<ncelle; i++) {
        for (j=0; j<ncelle; j++) {
            for (k=0; k<nusers; k++)
                free(costs[i][j][k]);
            free(costs[i][j]);
            }
        free(costs[i]);
        }
    free(costs);

    for (i=0; i<ncelle; i++) {
        for (j=0; j<nusers; j++)
            free(pos[i][j]);
        free(pos[i]);
        }
    free(pos);

    free(supply);
    free(sourcesidx);
    free(u);
    free(demand);
    free(destsidx);
    free(timeidx);
    free(v);

    for ( source=0; source<nsources; source++ ) {
        free(russelcosts[source]);
        free(russelmatrix[source]);
        free(russelweights[source]);
        free(modi[source]);
        }
    free(russelcosts);
    free(russelmatrix);
    free(russelweights);
    free(modi);

    return 0;

}


int getNextCandidate(int startrow,
                     int startcol,
                     int TTM,
                     int TPU,
                     int* minCost,
                     int*** russelmatrix,
                     int*** modimatrix,
                     int** tpuser,
                     int nsources,
                     int ndest,
                     int nusers) {

    int source;
    int r;
    int c=startcol;

    int bMinCost;
    int bMinRow;
    int cCost;
    int cUser;
    int cTPU;

    int rTTM; // Real TasksToMove


    bMinCost = INT_MAX;
    r        = -1;
    for ( r=0; r<nsources; r++ ) {
        if ( isTabuM(r, c) )
            continue;

        cUser = r%nusers;
        cTPU  = (*tpuser)[cUser];

        // La cella e' occupata
        if ( (*russelmatrix)[r][c]>=0 ) {

            rTTM = -1;
            // Controllo compatibilita'
            if ( (((*russelmatrix)[r][c]+TTM)%cTPU)==0 )
                rTTM = TTM;
            else if ( (((*russelmatrix)[r][c]+TTM+TPU)%cTPU)==0 )
                rTTM = TTM + TPU;

            if ( rTTM==-1 )
                continue;

            // Definizione costo
            if ( (*russelmatrix)[r][ndest-1]>=rTTM )
                cCost = rTTM * (*modimatrix)[r][c];
            else
                // cCost = rTTM * (*modimatrix)[r][c];
                cCost = rTTM * (*modimatrix)[r][ndest-1];
            fprintf(stderr, "cCost is %d\n", cCost);
            // Confronto con il costo minimo trovato finora
            if ( cCost<bMinCost ) {
                bMinCost = cCost;
                bMinRow  = r;
                }

            }

        else { // La cella non e' occupata
            rTTM = -1;

            // Controllo compatibilita'
            if ( (TTM%cTPU)==0 )
                rTTM = TTM;
            else if ( ((TTM+TPU)%cTPU)==0 )
                rTTM = TTM + TPU;

            if ( rTTM==-1 )
                continue;

            // Definizione costo
            if ( (*modimatrix)[r][c]==0 )
                cCost = 0;
            else
                cCost = rTTM * (*modimatrix)[r][c];

            // Confronto con il costo minimo trovato finora
            if ( cCost<bMinCost ) {
                bMinCost = cCost;
                bMinRow  = r;
                }
            }
        }

    fprintf(stderr, "Next Candidate is (%3d,%3d), cost %3d\n",
            bMinRow, c, bMinCost);
    *minCost = bMinCost;
    return bMinRow;
}
























































int isFeasible(int*** russelmatrix,
               int**** pos,
               int** tasks,
               int** sourcesidx,
               int** destsidx,
               int** timeidx,
               int** tpuser,
               int nsources,
               int ndest,
               int nusers) {

    int source, dest, user, time;
    int i, j;
    float k;
    float rowsum;
    float colsum;
    int flag;
    int tpu;

    flag=true;
    /* per ogni sorgente (per ogni tipo di user) la somma degli utsers      **
    ** spostati deve essere inveriore alla disponibilita'                   */
    for ( source=0; source < nsources; source++ ) {
        rowsum=0;
        user=source%nusers;
        tpu = (*tpuser)[user];
        i = (*sourcesidx)[source];
        time = (*timeidx)[source];


        for ( dest=0; dest<ndest-1; dest++ )
            if ( (*russelmatrix)[source][dest]!=-1 ) {
                k = (float) (*russelmatrix)[source][dest] / tpu;

                if ( ceil(k)!=floor(k) )
                    fprintf(stderr, "TASKS ERROR: (%3d,%3d) moved %4.2f users\n",
                            source, dest, k);

                rowsum+=k;
                }

        if ( rowsum>(*pos)[i][user][time] ) {
            fprintf(stderr, "FEASIBILITY ERROR: riga %d (%d), chiedo %f users (tipo %d) invece di %d\n",
                   i, source, rowsum, user, (*pos)[i][user][time]);
            flag=false;
            }
        }

    /* per ogni destinazione la somma dei task eseguiti deve essere pari o  **
    ** maggiore alla richiesta di tasks                                     */
    for ( dest=0; dest<ndest-1; dest++ ) {
        colsum=0;
        j = (*destsidx)[dest];
        for ( source=0; source < nsources; source++ )
            if ( (*russelmatrix)[source][dest]!=-1 )
                colsum += (*russelmatrix)[source][dest];

        if ( colsum < (*tasks)[j] ) {
            fprintf(stderr, "FEASIBILITY ERROR: colonna %d (%d), eseguo %f tasks invece di %d\n",
                   j, dest, colsum, (*tasks)[j]);
            flag=false;
            }
        }

    return flag;
}


void allocBase(int** tasks,
               int***** costs,
               int**** pos,
               int** tpuser,
               int ncelle,
               int nusers,
               int nintervalli) {

    /* //////////////////////////////////////////////////////////////////// **
    **                                                                      **
    ** Allocazione e preparazione della struttura dati                      **
    **                                                                      **
    ** //////////////////////////////////////////////////////////////////// */

    int i, j, k;        /* indici vari utili                                */

    /* tasks per ogni cella                                                 */
    *tasks=(int*) malloc(ncelle*sizeof(int));
    assert(*tasks);

    /* tasks per ogni tipo di user                                          */
    *tpuser=(int*) malloc(nusers*sizeof(int));
    assert( *tpuser );


    /* multi-matrice dei costi                                              **
        [ncelle]
            [ncelle]
                [nusers]
                    [nintervalli]
    **                                                                      */
    *costs=(int****) malloc(ncelle*sizeof(int***));
    assert(*costs!=NULL);

    for (i=0; i<ncelle; i++) {
        (*costs)[i]= (int***) malloc(ncelle*sizeof(int**));
        assert((*costs)[i]!=NULL);

        for (j=0; j<ncelle; j++) {
            (*costs)[i][j]= (int**) malloc(nusers*sizeof(int*));
            assert((*costs)[i][j]!=NULL);

            for (k=0; k<nusers; k++) {
                (*costs)[i][j][k]=(int*) malloc(nintervalli*sizeof(int));
                assert((*costs)[i][j][k]!=NULL);
                }
            }
        }


    /* posizioni degli users                                                **
        [ncelle]
            [nusers]
                [nintervalli]
    **                                                                      */
    *pos=(int***) malloc(ncelle*sizeof(int**));
    assert(*pos!=NULL);

    for (i=0; i<ncelle; i++) {
        (*pos)[i]=(int**) malloc(nusers*sizeof(int*));
        assert((*pos)[i]!=NULL);

        for (j=0; j<nusers; j++) {
            (*pos)[i][j]=(int*) malloc(nintervalli*sizeof(int));
            assert((*pos)[i][j]!=NULL);
            }
        }
}


void allocRussell(int** supply,
                  int** demand,
                  int** sourcesidx,
                  int** destsidx,
                  int** timeidx,
                  int** u,
                  int** v,
                  int*** russelcosts,
                  int*** russelmatrix,
                  int*** russelweights,
                  int nusers,
                  int ncelle,
                  int nintervalli) {

    int i, j;
    int nsources=nusers*ncelle*nintervalli; /* numero massimo di sorgenti               */
    int ndests=ncelle;                      /* numero massimo di destinazioni           */

    /* vettore supply totali per ogni riga (i.e. sorgente)                  */
    *supply=(int*) malloc(nsources*sizeof(int));
    assert( *supply );

    /* vettore domanda totale per ogni colonna (i.e. destinazione)          */
    *demand=(int*) malloc(ndests*sizeof(int));
    assert( *demand );

    /* vettore corrispondenza sorgente - cella reale di origine             */
    *sourcesidx=(int*) malloc(nsources*sizeof(int));
    assert( *sourcesidx );

    /* vettore corrispondenza destinazione - della reale di destinazione    */
    *destsidx=(int*) malloc(ndests*sizeof(int));
    assert( *destsidx );

    *timeidx=(int*) malloc(nsources*sizeof(int));
    assert( *timeidx );

    /* vettore con le posizioni dei minimi per ogni riga (i.e. sorgente)    */
    *u=(int*) malloc(nsources*sizeof(int));
    assert( *u );

    /* vettore con le posizioni dei minimi per ogni colonna                 **
    ** (i.e. destinazione)                                                  */
    *v=(int*) malloc(ndests*sizeof(int));
    assert( *v );

    /* inizializzazione delle strutture                                     */
    for ( i=0; i<nsources; i++ ) {
        (*supply)[i]=-1;
        (*sourcesidx)[i]=-1;
        (*timeidx)[i]=-1;
        (*u)[i]=INT_MIN;
        }

    for ( i=0; i<ndests; i++ ) {
        (*demand)[i]=-1;
        (*destsidx)[i]=-1;
        (*v)[i]=INT_MIN;
        }

    /* matrice dei costi per RAM                                            */
    *russelcosts=(int**) malloc(nsources*sizeof(int*));
    assert(*russelcosts);

    /* matrice delle assegnazioni (i.e. risultato del RAM)                  */
    *russelmatrix=(int**) malloc(nsources*sizeof(int*));
    assert (*russelmatrix);

    /* matrice dei pesi                                                     */
    *russelweights=(int**) malloc(nsources*sizeof(int*));
    assert (*russelweights);

    /* inizializzazione matrici costi e assegnazioni                        */
    for ( i=0; i<nsources; i++ ) {
        (*russelcosts)[i] =(int*) malloc(ndests*sizeof(int));
        assert ( (*russelcosts)[i]  );
        (*russelmatrix)[i]=(int*) malloc(ndests*sizeof(int));
        assert ( (*russelmatrix)[i] );
        (*russelweights)[i]=(int*) malloc(ndests*sizeof(int));
        assert ( (*russelweights)[i] );
        /* inizializza la nuova riga                                        */
        for ( j=0; j<ndests; j++ ) {
            (*russelcosts)[i][j] =-1;
            (*russelmatrix)[i][j]=-1;
            (*russelweights)[i][j]=-1;
            }
        }
}

void allocModi(int*** modi,
               int** pathrows,
               int** pathcols,
               int nusers,
               int ncelle,
               int nintervalli) {
    int i, j;
    /* max nr. di sorgenti      = nusers*ncelle                             **
    ** max nr. di destinazioni  = ncelle                                    */
    int len=(nusers*ncelle*nintervalli) + ncelle;

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

void readFile(int** tasks,
              int***** costs,
              int**** pos,
              int** tpuser,
              char* file) {
    /* //////////////////////////////////////////////////////////////////// **
    **                                                                      **
    ** Lettura dei dati dal file                                            **
    **                                                                      **
    ** //////////////////////////////////////////////////////////////////// */

    FILE* f;
    int nusers;         /* numero di users                                  */
    int nintervalli;    /* numero di intervalli                             */
    int ncelle;         /* numero di celle                                  */

    int user;           /* indice per l'user                                */
    int time;           /* indice per il tempo                              */
    int cella;          /* indice per la cella                              */
    int i, j, k;        /* indici vari utili                                */

    float tmp;          /* var. temporanea per lettura dei costi da file    */

    f=fopen (file, "r");
    assert (f!=NULL);

    fscanf (f, "%d", &ncelle);      /* leggo nr. celle      */
    fscanf (f, "%d", &nintervalli); /* leggo nr. intervalli */
    fscanf (f, "%d", &nusers);      /* leggo nr. genti      */

    /* ignora i 'tasks per user' (1 2 3)                                    */
    for ( i=0; i<nusers; i++ )
        fscanf (f, "%d", &((*tpuser)[i]));

    /* carica le super-matrici dei costi                                    */
    for (k=0; k<nintervalli*nusers; k++) {
        fscanf (f, "%d", &user);
        fscanf (f, "%d", &time);
        for ( i=0; i<ncelle; i++ )
            for ( j=0; j<ncelle; j++ ) {
                fscanf (f, "%f", &tmp);
                (*costs)[i][j][user][time]=(int) floor(tmp);
                }
        }

    /* carica i tasks per le celle                                          */
    for ( cella=0; cella<ncelle; cella++ )
        fscanf (f, "%d", &(*tasks)[cella]);

    /* carica le posizioni dei vari tipi di                                 **
    ** users nei diversi intervalli                                         */
    for ( k=0; k<nintervalli*nusers; k++ ) {
        fscanf (f, "%d", &user);
        fscanf (f, "%d", &time);

        for (cella=0; cella<ncelle; cella++)
            fscanf (f, "%d", &((*pos)[cella][user][time]));
        }

    fclose (f);
}






//
// int solveConflicts(int startrow,
//                    int startcol,
//                    int** pathrows,
//                    int** pathcols,
//                    int*** russelmatrix,
//                    int*** russelcosts,
//                    int*** modimatrix,
//                    int** tpuser,
//                    int nsources,
//                    int ndest,
//                    int nusers) {
//
//
//
// /* soluzione dei conflitti                                                  **
// ** prima di tutto si deve capire quanti task sono in ecceso;                **
// ** secondo, sulla base della matrice modi si cerca di spostare i task sulla **
// ** stessa colonna senza far alzare il costo troppo;                         **
// ** per spostare i task bisogna sempre trovare un path: come durante un      **
// ** normale ciclo modi, avremo una cella dove vogliamo spostare le cose e un **
// ** path da trovare;                                                         **
// ** */
//
//     int i, rs, k;
//     int user = startrow%nusers;
//     int tpu  = (*tpuser)[user];
//
//     int cuser; // current user
//     int ctpu; // current task per user
//     int cttm; // current task to move
//     int TaskstoMove = (*russelmatrix)[startrow][startcol]%tpu; //task da spostare
//
//     int ccosto; // costo corrente
//     int cbI; // current best i
//
//     int costoMin=INT_MAX; // costominimo corrente
//     int modiMin=INT_MAX; // modi minimo
//     int modiIMin;
//     int ttmMin;
//
//
//     fprintf(stderr, "\n\nRisolvo la cella (%3d,%3d)\n", startrow, startcol);
//
//
//     for ( i=0; i<nsources; i++ ) {
//         cttm = TaskstoMove;
//
//         if ( (*russelmatrix)[i][startcol]>0 ) {
//             /* se la cella e' gia' occupata, controllo se sta a posto;      **
//             ** se sta a posto la salto;                                     **
//             ** se non sta a posto controllo se aggiungendo i task che devo  **
//             ** spostare si mette a posto;                                   **
//             ** se non si mette a posto allora la considero                  */
//             cuser = i%nusers;
//             ctpu  = (*tpuser)[cuser];
//
//             if ( ((*russelmatrix)[i][startcol]+TaskstoMove)%ctpu!=0 ) {
//                 if ( ((*russelmatrix)[i][startcol]+TaskstoMove+tpu)%ctpu!=0 )
//                     continue;
//                 else
//                     cttm = TaskstoMove + tpu;
//                 }
//
//             }
//
//         cuser = i%nusers;
//         ctpu  = (*tpuser)[cuser];
//
//         if ( cttm%ctpu!=0 ) {
//             /* se sono su una riga sulla quale il numero di task che voglio **
//             ** spostare non si addice, controllo se togliendo dalla cella   **
//             ** di partenza tpu+TaskstoMove soddisfo la cella di arrivo      */
//             if ( (tpu+TaskstoMove)%ctpu!=0 )
//                 continue;
//             else
//                 cttm = TaskstoMove + tpu;
//             }
//
//
//             if ( (*modimatrix)[i][startcol]==0 ) {
//             fprintf(stderr, "Trovato costo modi '0' in posizione (%3d,%3d)\n",
//                     i, startcol);
//
//             clearPath(pathrows, pathcols, nsources, ndest);
//             initTabu();
//
//             (*pathrows)[0]=i;
//             (*pathcols)[0]=startcol;
//
//
//             rs = findPathR(i, startcol,
//                            i, startcol,
//                            pathrows, pathcols,
//                            1,  -1, russelmatrix,
//                            russelcosts, nsources, ndest);
//
//             if ( rs ) {
//                 fprintf(stderr, "\n[SC] FOUND:\n");
//                 fprintf(stderr, "Parto da (%3d,%3d)\n", i, startcol);
//                 k=0;
//                 while ( (*pathrows)[k]!=-1 ) {
//                     fprintf(stderr, "    (%3d,%3d)\n", (*pathrows)[k], (*pathcols)[k]);
//                     k++;
//                     }
//                 fprintf(stderr, "Costo %d\n", (*modimatrix)[i][startcol]);
//                 fprintf(stderr, "Tasks to move %d\n", cttm);
//                 }
//             else {
//                 fprintf(stderr, "\n[SC] [!] NO PATH FOUND\n");
//                 }
//
//
// //            fprintf(stderr, "\nTento con il nuovo metodo FAST.\n");
//
//             clearPath(pathrows, pathcols, nsources, ndest);
//             initTabu();
//
//             (*pathrows)[0]=i;
//             (*pathcols)[0]=startcol;
//
//             rs = fastFindPathR(i, startcol,
//                                i, startcol,
//                                pathrows, pathcols,
//                                1, cttm,
//                                russelmatrix, tpuser,
//                                nusers, nsources, ndest,
//                                true);
//
//             if ( rs ) {
//                 fprintf(stderr, "\n[FFPR] FOUND:\n");
//                 fprintf(stderr, "Parto da (%3d,%3d)\n", i, startcol);
//                 k=0;
//                 while ( (*pathrows)[k]!=-1 ) {
//                     fprintf(stderr, "    (%3d,%3d)\n", (*pathrows)[k], (*pathcols)[k]);
//                     k++;
//                     }
//                 fprintf(stderr, "Costo %d\n", (*modimatrix)[i][startcol]);
//                 fprintf(stderr, "Tasks to move %d\n", cttm);
//                 }
//             else {
//                 fprintf(stderr, "\n[FFPR] [!] NO PATH FOUND\n");
//                 }
//
//
//             continue;
//             }
//
//         ccosto = cttm*(*russelcosts)[i][startcol];
//
//         /* un minimo valido per tutte le celle                              */
//         if (ccosto<costoMin) {
//             costoMin = ccosto;
//             ttmMin   = cttm;
//             cbI      = i;
//             }
//         /* un minimo valido solamente per le celle che non sono state gia'  **
//         ** occupate                                                         */
//         if ( cttm==TaskstoMove )
//             if ( (*modimatrix)[i][startcol]<modiMin ) {
//                 modiMin = (*modimatrix)[i][startcol];
//                 modiIMin = i;
//                 }
//         }
//
//
//
//     /*****************************************************************************/
//     /*****************************************************************************/
//     /*****************************************************************************/
//     /*****************************************************************************/
//
//     fprintf(stderr,
//             "Provo a ricercare secondo il minimo costo con tasks diversi\n");
//     clearPath(pathrows, pathcols, nsources, ndest);
//     initTabu();
//
//     (*pathrows)[0]=cbI;
//     (*pathcols)[0]=startcol;
//
//
//     rs = findPathR(cbI, startcol,
//                    cbI, startcol,
//                    pathrows, pathcols,
//                    1,  -1, russelmatrix,
//                    russelcosts, nsources, ndest);
//
//     if ( rs ) {
//         fprintf(stderr, "\n[SC] FOUND:\n");
//         fprintf(stderr, "Parto da (%3d,%3d)\n", cbI, startcol);
//         k=0;
//         while ( (*pathrows)[k]!=-1 ) {
//             fprintf(stderr, "    (%3d,%3d)\n", (*pathrows)[k], (*pathcols)[k]);
//             k++;
//             }
//         fprintf(stderr, "Costo %d\n", (*modimatrix)[cbI][startcol]);
//         fprintf(stderr, "Tasks to move %d\n", ttmMin);
//         }
//     else {
//         fprintf(stderr, "\n[SC] [!] NO PATH FOUND\n");
//         }
//
//
//     /*****************************************************************************/
//     /*****************************************************************************/
//
// //    fprintf(stderr, "\nTento con il nuovo metodo FAST.\n");
//
//     clearPath(pathrows, pathcols, nsources, ndest);
//     initTabu();
//
//     (*pathrows)[0]=cbI;
//     (*pathcols)[0]=startcol;
//
//     rs = fastFindPathR(cbI, startcol,
//                        cbI, startcol,
//                        pathrows, pathcols,
//                        1, ttmMin,
//                        russelmatrix, tpuser,
//                        nusers, nsources, ndest,
//                        true);
//
//     if ( rs ) {
//         fprintf(stderr, "\n[FFPR] FOUND:\n");
//         fprintf(stderr, "Parto da (%3d,%3d)\n", cbI, startcol);
//         k=0;
//         while ( (*pathrows)[k]!=-1 ) {
//             fprintf(stderr, "    (%3d,%3d)\n", (*pathrows)[k], (*pathcols)[k]);
//             k++;
//             }
//         fprintf(stderr, "Costo %d\n", (*modimatrix)[cbI][startcol]);
//         fprintf(stderr, "Tasks to move %d\n", ttmMin);
//         }
//     else {
//         fprintf(stderr, "\n[FFPR] [!] NO PATH FOUND\n");
//         }
//
//
//
//     /*****************************************************************************/
//     /*****************************************************************************/
//     /*****************************************************************************/
//     /*****************************************************************************/
//
//         fprintf(stderr,
//                 "Provo a ricercare secondo il minimo costo modi\n");
//         clearPath(pathrows, pathcols, nsources, ndest);
//         initTabu();
//
//         (*pathrows)[0]=modiIMin;
//         (*pathcols)[0]=startcol;
//
//
//         rs = findPathR(modiIMin, startcol,
//                        modiIMin, startcol,
//                        pathrows, pathcols,
//                        1,  -1, russelmatrix,
//                        russelcosts, nsources, ndest);
//
//         if ( rs ) {
//             fprintf(stderr, "\n[SC] FOUND:\n");
//             fprintf(stderr, "Parto da (%3d,%3d)\n", modiIMin, startcol);
//             k=0;
//             while ( (*pathrows)[k]!=-1 ) {
//                 fprintf(stderr, "    (%3d,%3d)\n", (*pathrows)[k], (*pathcols)[k]);
//                 k++;
//                 }
//             fprintf(stderr, "Costo %d\n", (*modimatrix)[modiIMin][startcol]);
//             fprintf(stderr, "Tasks to move %d\n", TaskstoMove);
//             }
//         else {
//             fprintf(stderr, "\n[SC] [!] NO PATH FOUND\n");
//             }
//
//     /*****************************************************************************/
//     /*****************************************************************************/
//
// //    fprintf(stderr, "\nTento con il nuovo metodo FAST.\n");
//
//     clearPath(pathrows, pathcols, nsources, ndest);
//     initTabu();
//
//     (*pathrows)[0]=modiIMin;
//     (*pathcols)[0]=startcol;
//
//     rs = fastFindPathR(modiIMin, startcol,
//                        modiIMin, startcol,
//                        pathrows, pathcols,
//                        1, ttmMin,
//                        russelmatrix, tpuser, nusers,
//                        nsources, ndest, true);
//
//     if ( rs ) {
//         fprintf(stderr, "\n[FFPR] FOUND:\n");
//         fprintf(stderr, "Parto da (%3d,%3d)\n", modiIMin, startcol);
//         k=0;
//         while ( (*pathrows)[k]!=-1 ) {
//             fprintf(stderr, "    (%3d,%3d)\n", (*pathrows)[k], (*pathcols)[k]);
//             k++;
//             }
//         fprintf(stderr, "Costo %d\n", (*modimatrix)[modiIMin][startcol]);
//         fprintf(stderr, "Tasks to move %d\n", TaskstoMove);
//         }
//     else {
//         fprintf(stderr, "\n[FFPR] [!] NO PATH FOUND\n");
//         }
//
//
//     /*****************************************************************************/
//     /*****************************************************************************/
//     /*****************************************************************************/
//     /*****************************************************************************/
//
//
//
//
//
//
//
//
//
//     //
//     // for ( i=0; i<nsources; i++ ) {
//     //     if ( (*russelmatrix)[i][startcol]>0 )
//     //         continue;
//     //
//     //     cuser = i%nusers;
//     //     ctpu  = (*tpuser)[cuser];
//     //
//     //     if ( TaskstoMove%ctpu!=0 )
//     //         continue;
//     //
//     //     if ( (*modimatrix)[i][startcol]<modiMin ) {
//     //         modiMin  = (*modimatrix)[i][startcol];
//     //         bestRiga = i;
//     //         }
//     //     }
//     //
//     // if ( modiMin<(*russelcosts)[startrow][startcol] ) {
//     //     fprintf(stderr, "Ho trovato una cella con il costo min\n");
//     //     clearPath(pathrows, pathcols, nsources, ndest);
//     //     initTabu();
//     //
//     //     pathrows[0]=i;
//     //     pathcols[0]=startcol;
//     //
//     //
//     //     rs = findPathR(i, startcol,
//     //                    i, startcol,
//     //                    pathrows, pathcols,
//     //                    1,  -1, russelmatrix,
//     //                    russelcosts, nsources, ndest);
//     //
//     //     if ( rs ) {
//     //         fprintf(stderr, "SC: path found:\n");
//     //         fprintf(stderr, "Parto da (%3d,%3d)\n", i, startcol);
//     //         k=0;
//     //         while ( pathrows[k]!=-1 ) {
//     //             fprintf(stderr, "    (%3d,%3d)\n", pathrows[k], pathcols[k]);
//     //             k++;
//     //             }
//     //         fprintf(stderr, "Costo %d\n", (*modimatrix)[i][startcol]);
//     //         }
//     //     else {
//     //         fprintf(stderr, "SC: NO PATH FOUND\n");
//     //         }
//     //     }
//     // else
//     //     fprintf(stderr, "Nessuno spostamento migliore, tanto vale che abbondi\n");
//
//     return true;
// }


int fastFindPathR(int startrow,   /* starting row coordinate */
                  int startcol,   /* starting column coordinate */
                  int destrow,    /* destination row */
                  int destcol,    /* destination column */
                  int** pathrows, /* vettore indici di riga del path */
                  int** pathcols, /* vettore indici di colonna del path */
                  int cidx,       /* indice posizione corrente nel path */
                  int TasksToMove,
                  int*** russelmatrix,
                  int** tpuser,
                  int nusers,
                  int nsources,
                  int ndest,
                  int payAttention,
                  int depAllowed) {



    int i, j, rs;
    int row, column;
    int user, tpu;
    int depVisited;
    int rispettaIVincoli;


    if ( startrow==destrow && cidx>1 )
        return true;

    if ( isTabu(startrow, startcol) )
        return false;

    addTabu(startrow, startcol);

    if ( cidx%2==1 ) { // cidx dispari vado per colonna

        for ( i=searchByIndexesCol[startcol];
              searchByCol[i].column==startcol;
              i++) {



            if ( searchByCol[i].row==startrow )
                continue;

            row     = searchByCol[i].row;
            column  = searchByCol[i].column;

            user    = row%nusers;
            tpu     = (*tpuser)[user];

            if ( ((*russelmatrix)[row][column]%tpu)==0 )
                rispettaIVincoli = true;
            else
                rispettaIVincoli = false;

            #ifdef _debugFastFindPath
            sspace(cidx);
            fprintf(stderr, "c: sono a (%3d,%3d)\n", row, column);
            #endif // _debugFastFindPath


            /* dato che quando vado per colonna sottraggo, se devo fare     **
            ** attenzione, significa che sottraendo i TaskToMove dalla cella**
            ** corrente, questa deve ancora rispettare il vincolo di riga   **
            ** Questo non vale per la colonna dei depositi.                 **
            ** Non si presta attenzione per le celle che gia' prima non     **
            ** rispettavano i vincoli                                       */
            if ( payAttention && rispettaIVincoli )
                if ( (((*russelmatrix)[row][column]-TasksToMove)%tpu)!=0 )
                    if ( column<ndest-1 )
                        continue;
            /* controllo anche che dalla cella possa effettivamente spostare**
            ** TaskToMove tasks. Data la filosofia di Russel non dovrebbe   **
            ** mai accedere se non in rarissimi casi                        */
            if ( ((*russelmatrix)[row][column]-TasksToMove)<0 )
                continue;

            (*pathrows)[cidx]   = row;
            (*pathcols)[cidx]   = column;

            #ifdef _debugFastFindPath
            sspace(cidx);
            fprintf(stderr, "c: giro a (%3d,%3d)\n", row, column);
            #endif // _debugFastFindPath

            rs = fastFindPathR(row, column,
                               destrow, destcol,
                               pathrows, pathcols,
                               cidx+1, TasksToMove,
                               russelmatrix, tpuser,
                               nusers, nsources, ndest,
                               payAttention, depAllowed);
             if (rs)
                return true;


            (*pathrows)[cidx]=-1;
            (*pathcols)[cidx]=-1;
            }
        }

    else { // cidx pari vado per riga
        depVisited=false;
        for ( j=searchByIndexesRow[startrow];
              searchByRow[j].row==startrow;
              j++) {

            if ( searchByRow[j].column==startcol )
                continue;

            row     = searchByRow[j].row;
            column  = searchByRow[j].column;

            user    = row%nusers;
            tpu     = (*tpuser)[user];

            #ifdef _debugFastFindPath
            sspace(cidx);
            fprintf(stderr, "r: sono a (%3d,%3d)\n", row, column);
            #endif // _debugFastFindPath


            if ( ((*russelmatrix)[row][column]%tpu)==0 )
                rispettaIVincoli = true;
            else
                rispettaIVincoli = false;


            /* dato che quando vado per riga aggiungo, se aggiungendo i     **
            ** TaskToMove violo il vincolo di riga, allora non li posso     **
            ** aggiungere.                                                  **
            ** Tale controllo vale solamente se la cella corrente prima che **
            ** vi arrivassi io rispettava il vincolo di riga.               */
            if ( payAttention && rispettaIVincoli )
                if ( (((*russelmatrix)[row][column]+TasksToMove)%tpu)!=0 )
                    if ( column<ndest-1 )
                        continue;

            (*pathrows)[cidx]   = row;
            (*pathcols)[cidx]   = column;

            #ifdef _debugFastFindPath
            sspace(cidx);
            fprintf(stderr, "r: giro a (%3d,%3d)\n", row, column);
            #endif // _debugFastFindPath

            if ( column==ndest-1 )
                depVisited=true;

            rs = fastFindPathR(row, column,
                               destrow, destcol,
                               pathrows, pathcols,
                               cidx+1, TasksToMove,
                               russelmatrix, tpuser,
                               nusers, nsources, ndest,
                               payAttention, depAllowed);
            if (rs)
                return true;


            (*pathrows)[cidx]=-1;
            (*pathcols)[cidx]=-1;
            }


        /* Andando per riga aggiungo, quindi posso permettermi il lusso **
        ** di aggiungere al deposito senza sbattimenti di capo, in      **
        ** in alcune situazioni                                         */
        if ( !depVisited && depAllowed ) {
            row    = startrow;
            column = ndest-1;

            if ( row==destrow )
                return true;

            (*pathrows)[cidx]   = row;
            (*pathcols)[cidx]   = column;

            #ifdef _debugFastFindPath
            sspace(cidx);
            fprintf(stderr, "r: giro a (%3d,%3d)\n", row, column);
            #endif // _debugFastFindPath

            rs = fastFindPathR(row, column,
                               destrow, destcol,
                               pathrows, pathcols,
                               cidx+1, TasksToMove,
                               russelmatrix, tpuser,
                               nusers, nsources, ndest,
                               payAttention, depAllowed);
            if (rs)
                return true;


            (*pathrows)[cidx]=-1;
            (*pathcols)[cidx]=-1;
            }


        }

    return false;

}


// Fast Find Path Recursive RELAXED
int fastFindPathRR(int startrow,   /* starting row coordinate */
                   int startcol,   /* starting column coordinate */
                   int destrow,    /* destination row */
                   int destcol,    /* destination column */
                   int** pathrows, /* vettore indici di riga del path */
                   int** pathcols, /* vettore indici di colonna del path */
                   int cidx,       /* indice posizione corrente nel path */
                   int TasksToMove,
                   int currentCost, /* costo corrente di tutte le celle aggiunte */
                   int maxCost,    /* costo massimo al quale accetto una nuova cella */
                   int*** russelmatrix,
                   int*** modimatrix,
                   int** tpuser,
                   int nusers,
                   int nsources,
                   int ndest,
                   int payAttention) {



    int i, j, rs;
    int row, column;
    int user, tpu;
    int depVisited;
    int rispettaIVincoli;
    int thisCost;


    if ( startrow==destrow && cidx>1 )
        return true;

    if ( isTabu(startrow, startcol) )
        return false;

    addTabu(startrow, startcol);

    if ( cidx%2==1 ) { // cidx dispari vado per colonna

        for ( i=searchByIndexesCol[startcol];
              searchByCol[i].column==startcol;
              i++) {

            if ( searchByCol[i].row==startrow )
                continue;

            row     = searchByCol[i].row;
            column  = searchByCol[i].column;

            user    = row%nusers;
            tpu     = (*tpuser)[user];

            if ( ((*russelmatrix)[row][column]%tpu)==0 )
                rispettaIVincoli = true;
            else
                rispettaIVincoli = false;

            // Al primo giro devo beccare la CVIV che voglio aggiustare
            if ( cidx==1 && rispettaIVincoli )
                continue;

            #ifdef _debugFastFindPathR
            sspace(cidx);
            fprintf(stderr, "c: sono a (%3d,%3d)\n", row, column);
            #endif // _debugFastFindPath


            /* dato che quando vado per colonna sottraggo, se devo fare     **
            ** attenzione, significa che sottraendo i TaskToMove dalla cella**
            ** corrente, questa deve ancora rispettare il vincolo di riga   **
            ** Questo non vale per la colonna dei depositi.                 **
            ** Non si presta attenzione per le celle che gia' prima non     **
            ** rispettavano i vincoli                                       */
            if ( payAttention && rispettaIVincoli )
                if ( (((*russelmatrix)[row][column]-TasksToMove)%tpu)!=0 )
                    if ( column<ndest-1 )
                        continue;

            /* controllo anche che dalla cella possa effettivamente spostare**
            ** TaskToMove tasks. Data la filosofia di Russell non dovrebbe  **
            ** mai accedere se non in rarissimi casi                        */
            if ( ((*russelmatrix)[row][column]-TasksToMove)<0 )
                continue;

            (*pathrows)[cidx]   = row;
            (*pathcols)[cidx]   = column;

            #ifdef _debugFastFindPathR
            sspace(cidx);
            fprintf(stderr, "c: giro a (%3d,%3d)\n", row, column);
            #endif // _debugFastFindPath

            rs = fastFindPathRR(row, column,
                                destrow, destcol,
                                pathrows, pathcols,
                                cidx+1, TasksToMove,
                                currentCost, maxCost,
                                russelmatrix, modimatrix, tpuser,
                                nusers, nsources, ndest,
                                payAttention);
             if (rs)
                return true;


            (*pathrows)[cidx]=-1;
            (*pathcols)[cidx]=-1;
            }
        }

    else { // cidx pari vado per riga
        depVisited=false;
        row     = startrow;
        user    = row%nusers;
        tpu     = (*tpuser)[user];

        for ( j=0; j<ndest; j++) {
            /* salto la cella da dove provengo                              */
            if ( j==startcol )
                continue;



            column  = j;

            // if ( cidx>2 )
            //     if ( (*russelmatrix)[row][column]==-1 )
            //         continue;


            /* considero il costo della modimatrix                          **
            **    - se e' 0 probabilmetne mi trovo su una cella occupata
            **      oppure una cella che mi conviene particoalrmente
            **    - se e' maggiore di zero significa che passando per
            **      questa cella paghero' esattamente quel pedaggio per
            **      ogni task che trasferisco                               */
            thisCost = (*modimatrix)[row][column];
            if ( (currentCost+thisCost)>maxCost )
                continue;

            #ifdef _debugFastFindPathR
            sspace(cidx);
            fprintf(stderr, "r: sono a (%3d,%3d)\n", row, column);
            #endif // _debugFastFindPath

            /* se la cella e' occupata devo tener conto di non farla        **
            ** diventare una CVIV
            ** se la cella non e' occupata non aggiungo i TTM se non mi
            ** trovo su una riga giusta                                     */
            if ( (*russelmatrix)[row][column]>=0 ) {
                if ( ((*russelmatrix)[row][column]%tpu)==0 )
                    rispettaIVincoli = true;
                else
                    rispettaIVincoli = false;


                /* dato che quando vado per riga aggiungo, se aggiungendo i     **
                ** TaskToMove violo il vincolo di riga, allora non li posso     **
                ** aggiungere.                                                  **
                ** Tale controllo vale solamente se la cella corrente prima che **
                ** vi arrivassi io rispettava il vincolo di riga.               */
                if ( payAttention && rispettaIVincoli )
                    if ( (((*russelmatrix)[row][column]+TasksToMove)%tpu)!=0 )
                        if ( column<ndest-1 )
                            continue;
                }
            else {
                if ( TasksToMove%tpu!=0 )
                    continue;
                }

            (*pathrows)[cidx]   = row;
            (*pathcols)[cidx]   = column;

            #ifdef _debugFastFindPathR
            sspace(cidx);
            fprintf(stderr, "r: giro a (%3d,%3d)\n", row, column);
            #endif // _debugFastFindPath

            rs = fastFindPathRR(row, column,
                               destrow, destcol,
                               pathrows, pathcols,
                               cidx+1, TasksToMove,
                               (currentCost+thisCost), maxCost,
                               russelmatrix, modimatrix, tpuser,
                               nusers, nsources, ndest,
                               payAttention);
            if (rs)
                return true;


            (*pathrows)[cidx]=-1;
            (*pathcols)[cidx]=-1;
            }
        }

    return false;

}























































/* *********************************************************************** */
/* *********************************************************************** */
/* *********************************************************************** */

/* Direction encoding:                                                      **
**  colonna  = 0                                                            **
**  riga     = 1                                                            */
/* Sign encoding:                                                           **
**  minus    = 0                                                            **
**  plus     = 1                                                            */
int aggiustaR(int startrow,   /* starting row coordinate */
              int startcol,   /* starting column coordinate */
              int destrow,    /* destination row            */
              int destcol,    /* destination column         */
              int** pathrows, /* vettore indici di riga del path */
              int** pathcols, /* vettore indici di colonna del path */
              int cidx, /* indice posizione corrente nel path */
              int* k, /* numero di tasks da spostare */
              int* kmax,      /* massimo valore per k (fase 0) */
              int verso, /* sto andando sulle righe o sulle colonne? */
              int costo, /* costo del path fino a questo punto */
              int costomax, /* la somma del path deve fare meglio di cosi' */
              int segno,
              int*** russelmatrix,
              int*** russelcosts,
              int*** modimatrix,
              int nsources,
              int ndest,
              int nusers) {

    int i,j;
    int rs;
    int kiniziale = *k;
    int kmaxiniziale = *kmax;
    int user;
    int tpu;
    int nt;
    int min;
    int minidx;
    int futurecost;

    int way;
    int offset;


    if ( startrow==destrow && startcol==destcol && cidx>1 ) {
        #ifdef _debugAggiustaR
        fprintf(stderr, "Fine: verso='%s', segno='%s', costo=%d, costomax=%d\n",
                (verso==colonna?"colonna":"riga"),
                (segno==minus ? "minus" : "plus"),
                costo, costomax);
        i=0;
        fprintf(stderr, "--");
        while ( (*pathrows)[i]!=-1 ) {
            fprintf(stderr, " (%3d,%3d)\n", (*pathrows)[i], (*pathcols)[i]);
            i++;
            }
        fprintf(stderr, "--");
        #endif // _debugAggiustaR

        if ( verso==colonna && costo<=costomax ) {
            #ifdef _debugAggiustaR
            sspace(cidx);
            fprintf(stderr, " destination reached\n");
            #endif // _debugAggiustaR
            return true;
            }

        // FASE 0 // voglio trovare un path
        //           per aggiungere task da qualche parte
        if ( verso==colonna && segno==minus && costo<=costomax )
            return true;

        #ifdef _debugAggiustaR
        if ( verso==colonna && segno==minus && costo>costomax ) {
            sspace(cidx);
            fprintf(stderr, "Trovato path ma ha costo maggiore.\n");
            }
        #endif // _debugAggiustaR

        // FASE 4 // voglio trovare un path per togliere
        //           tasks da qualche parte
        if ( verso==colonna && segno==plus && costo<=costomax )
            return true;
        #ifdef _debugAggiustaR
        if ( verso==colonna && segno==plus && costo>costomax ) {
            sspace(cidx);
            fprintf(stderr, "Trovato path ma ha costo maggiore.\n");
            }
        #endif // _debugAggiustaR
        }


    if ( isTabuM(startrow, startcol) ) {
        #ifdef _debugAggiustaR
        sspace(cidx);
        fprintf(stderr, " ~ (%3d,%3d) e' Tabu. ~\n", startrow, startcol);
        #endif // _debugAggiustaR
        return false;
        }
    else
        addTabuM(startrow, startcol);


    if ( verso==colonna ) { // --- [[[ verso==colonna ]]] ------------------

        if ( segno==plus ) { // FASE 4 // verso==colonna && segno==plus
        // segno positivo, aggiornamento fase 4: ho tolto da qualche parte e
        // devo aggiungere da qualche altra parte (mal che vada mi trovo una
        // nuova allocazione nella colonna a costo minore)

            kiniziale=*k;

            if ( startcol==ndest-1 ) {
                futurecost  = costo;
                futurecost += (*k)*(*russelcosts)[destrow][startcol];
                futurecost -= (*k)*(*russelcosts)[destrow][destcol];

                if ( futurecost<=costomax ) {
                    (*pathrows)[cidx]=destrow;
                    (*pathcols)[cidx]=startcol;

                    #ifdef _debugAggiustaR
                    sspace(cidx);
                    fprintf(stderr, " c: giro a (%3d,%3d)\n", destrow, startcol);
                    #endif // _debugAggiustaR

                    rs = aggiustaR(destrow, startcol,
                                   destrow, destcol,
                                   pathrows, pathcols,
                                   cidx+1,
                                   k, kmax,
                                   riga,
                                   costo+(*k)*(*russelcosts)[destrow][startcol], costomax,
                                   minus,
                                   russelmatrix, russelcosts, modimatrix,
                                   nsources, ndest, nusers);

                    if ( rs==true )
                        return true;
                    }
                }


            /* trova il costo minimo della colonna                              */
            min=INT_MAX;
            for ( i=0; i<nsources; i++ ) {
                if ( i==startrow )
                    continue;

                /* per torvare il minimo considero solamente le celle non   **
                ** allocate                                                 */
                if ( (*russelmatrix)[i][startcol]!=-1 )
                    continue;

                user = i%nusers;
                tpu  = user+1;
                /* lungo il path, i tasks postati devono essere il mcm per  **
                ** non creare conflitti; nelle celle con task, la somma dei **
                ** task gia' presenti e i nuovi non devono violare il       **
                ** vincolo di riga; stesso discorso per celle che non hanno **
                ** ancora tasks; in questi casi pero', dato che so gia' i   **
                ** tasks che voglio spostare (probabilmente perche' in      **
                ** ecceso in un'allocazione ottima), non posso aumentare i  **
                ** tasks spostati se il vincolo non viene rispettato        */
                if ( (*k)>tpu ){
                    if ( (*k)%tpu!=0 )
                        continue;
                    }
                else {
                    if ( tpu%(*k)!=0 )
                        continue;
                    }


                if ( (*russelcosts)[i][startcol]<min )
                    min=(*russelcosts)[i][startcol];

                }

 //            min=INT_MIN;
            for ( offset=1; offset<nsources; offset++ )
                for ( way=-1; way<2; way+=2 ) {

                    i=startrow+way*offset;
                    if ( i<0 || i>=nsources )
                        continue;

                    /* quando sono arrivato sulla colonna, ci sono arrviato     **
                    ** togliendo tasks a una cella di questa colonna, ora non   **
                    ** posso aggiungerglieli di nuovo                           */
                    if ( i==startrow )
                        continue;

                    user = i%nusers;
                    tpu  = user+1;
                    /* lungo il path, i tasks spostati non devono creare altri  **
                    ** conflitti; nelle celle con task, la somma dei            **
                    ** task gia' presenti e i nuovi non devono violare il       **
                    ** vincolo di riga; stesso discorso per celle che non hanno **
                    ** ancora tasks; in questi casi pero', dato che so gia' i   **
                    ** tasks che voglio spostare (probabilmente perche' in      **
                    ** ecceso in un'allocazione ottima), non posso aumentare i  **
                    ** tasks spostati se il vincolo non viene rispettato        */
                    if ( (*russelmatrix)[i][startcol]==-1 )
                        nt = *k;
                    else
                        nt = (*russelmatrix)[i][startcol] + *k;

                    if ( (nt%tpu)!=0 ) {
                        #ifdef _debugAggiustaR
                            sspace(cidx);
                            fprintf(stderr, " c: (%3d,%3d) viola i vincoli\n", i, startcol);
                        #endif // _debugAggiustaR

                        if ( startcol<ndest-1 )
                            continue;
                        }


                    /* aggiungo task                                            **
                    **    a celle gia' allocate perche' so che sono ottime,     **
                    **    a celle per le quali la modi vale 0                   **
                    **    a celle con costo <= minimo costo della colonna       */
                    if ( (*russelmatrix)[i][startcol]==-1 ) {
                        /* come ultima spiaggia salvo il minimo costo       **
                        ** della colonna per poi provare a fare delle       **
                        ** allocazioni in quelle celle non considerate      */
                        if ( (*russelcosts)[i][startcol]>min )
                            continue;
                        }


                    (*pathrows)[cidx]=i;
                    (*pathcols)[cidx]=startcol;

                    #ifdef _debugAggiustaR
                    sspace(cidx);
                    fprintf(stderr, " c: giro a (%3d,%3d)\n", i, startcol);
                    #endif // _debugAggiustaR

                    rs = aggiustaR(i, startcol,
                                   destrow, destcol,
                                   pathrows, pathcols,
                                   cidx+1,
                                   k, kmax,
                                   riga,
                                   costo+(*k)*(*russelcosts)[i][startcol], costomax,
                                   minus,
                                   russelmatrix, russelcosts, modimatrix,
                                   nsources, ndest, nusers);

                    if ( rs==true )
                        return true;

                    *k = kiniziale;
                    }
            }

        if ( segno==minus ) { // FASE 0 // verso==colonna, segno==minus
            // segno negativo, aggionramento fase 0: ho aggiunto da qualche
            // parte e devo togliere i task da qualche altra, devo ancora
            // decidere quanti utenti spostare tra l'altro: il minimo e' il
            // minimo numero di task che posso fare nella cella scelta;
            // eventualmente si aggiustano, non so come.
            kiniziale=(*k);
            /* se sono arrivato nella dummy-destination, significa che ho   **
            ** accesso diretto al deposito della riga di partenza, quindi   **
            ** provo a dargli i task che mancano direttamente dal deposito  **
            ** per concludere il ciclo e controllare se il costo e' OK      */

            if ( startcol==ndest-1 ) {
                futurecost=costo;
                futurecost-=(*russelcosts)[destrow][startcol];
                futurecost+=(*russelcosts)[destrow][destcol];

                if ( costo<=costomax ) {
                    if ( (*russelmatrix)[destrow][startcol]>=*k ) {
                        /* per il deposito non m'importa che i vincoli sui task     **
                        ** vengano violati in quanto probabilmente si aggiusteranno **
                        ** poi, man mano che aggiusto altre celle                   */
                        (*pathrows)[cidx]=destrow;
                        (*pathcols)[cidx]=startcol;
                        #ifdef _debugAggiustaR
                        sspace(cidx);
                        fprintf(stderr, " c: giro a (%3d,%3d) - diretto dal deposito\n", destrow, startcol);
                        #endif // _debugAggiustaR

                        rs = aggiustaR(destrow, startcol,
                                       destrow, destcol,
                                       pathrows, pathcols,
                                       cidx+1,
                                       k, kmax,
                                       riga,
                                       costo-(*k)*(*russelcosts)[destrow][startcol], costomax,
                                       plus,
                                       russelmatrix, russelcosts, modimatrix,
                                       nsources, ndest, nusers);

                        if ( rs==true ) {
                            /* k potrebbe essere stato modificato lungo il percorso **
                            ** in tal caso non mi importa qui'                      */
                            return true;
                            }

                        /* pero' se e' stato modificato lo devo ripristinare        */
                        *k    = kiniziale;
                        *kmax = kmaxiniziale;
                        }
                    }
                }


            /* con offset invece di andare in fila dalla posizione 0 alla   **
            ** nsources, sbalzo avanti e indietro dalla riga di arrivo      */
            for ( offset=1; offset<nsources; offset++ )
                for ( way=-1; way<2; way += 2 ) {

                    i=startrow+way*offset;
                    if ( i<0 || i>=nsources )
                        continue;

                    user = i%nusers;
                    tpu  = user+1;

                    /* se sono partito da una colonna e poi ritorno su tale     **
                    ** colonna con segno negativo: non posso togliere e dare    **
                    ** stesso tempo alla cella di partenza (e destinazione)     */
                    if ( i==destrow && startcol==destcol )
                        continue;

                    /* quando son arrivato sulla colonna sono arrivato in una   **
                    ** cella della colonna dove ho aggiunto task, ora non posso **
                    ** toglierglieli di nuovo                                   */
                    if ( i==startrow )
                        continue;

                    /* dato che devo trovare una cella donatrice, se la cella   **
                    ** non ha tasks vado avanti                                 */
                    if ( (*russelmatrix)[i][startcol]==-1 )
                        continue;

                    /* dato che devo trovare una cella donatrice, se la cella   **
                    ** ha meno tasks di quelli che voglio spostare vado avanti  */
                    if ( (*russelmatrix)[i][startcol]<*k ) {
                        #ifdef _debugAggiustaR
                        sspace(cidx);
                        fprintf(stderr, " c: (%3d,%3d) non ha abbastanza task\n",
                                i, startcol);
                        #endif // _debugAggiustaR
                        continue;
                        }

                    /* se la disponibilita' e' minore del kmax, rimodulo kmax   */
                    if ( (*russelmatrix)[i][startcol]<*kmax )
                        *kmax = (*russelmatrix)[i][startcol];



                    /* se con questo k violo i vincoli lo aumento               **
                    ** finche' non li rispetto oppure ho raggiunto kmax         **
                    ** se facendo cio' sballo con i k precedenti salto la cella */
                    nt = (*russelmatrix)[i][startcol]-(*k);
                    if ( nt%tpu!=0 ) {
                        while ( (nt%tpu)!=0 && *k<*kmax ) {
                            (*k)++;
                            nt--;
                            }
                        if ( nt%tpu!=0 || *k%kiniziale!=0 ) {
                            *k=kiniziale;
                            #ifdef _debugAggiustaR
                            sspace(cidx);
                            fprintf(stderr, " c: (%3d,%3d) viola i vincoli\n",
                                    i, startcol);
                            #endif // _debugAggiustaR
                            continue;
                            }
                        }


                    #ifdef _debugAggiustaR
                    if ( *k != kiniziale ) {
                        sspace(cidx);
                        fprintf(stderr,
                                " c: considero (%3d,%3d):"
                                " ora k= %2d e kmax= %2d\n",
                                i, startcol, *k, *kmax);
                        }
                    #endif // _debugAggiustaR


                    (*pathrows)[cidx]=i;
                    (*pathcols)[cidx]=startcol;
                    #ifdef _debugAggiustaR
                    sspace(cidx);
                    fprintf(stderr, " c: giro a    (%3d,%3d)\n", i, startcol);
                    #endif // _debugAggiustaR

                    rs = aggiustaR(i, startcol,
                                   destrow, destcol,
                                   pathrows, pathcols,
                                   cidx+1,
                                   k, kmax,
                                   riga,
                                   costo-(*k)*(*russelcosts)[i][startcol], costomax,
                                   plus,
                                   russelmatrix, russelcosts, modimatrix,
                                   nsources, ndest, nusers);

                    if ( rs==true ) {
                        /* k potrebbe essere stato modificato lungo il percorso **
                        ** in tal caso bisogna ricontrollare che non sballino i **
                        ** task qui'                                            */
                        nt = ((*russelmatrix)[i][startcol]-*k);
                        if ( (nt%tpu) == 0 )
                            return true;

                        #ifdef _debugAggiustaR
                        sspace(cidx);
                        fprintf(stderr, "In posizione %d k=%d non va bene\n", cidx, *k);
                        #endif // _debugAggiustaR
                        }

                    *k    = kiniziale;
                    *kmax = kmaxiniziale;
                    }

            (*pathrows)[cidx]=-1;
            (*pathcols)[cidx]=-1;
            }
        }

    if ( verso==riga ) {
        // segno negativo, aggiornamento fase 4: ho aggiunto sulla
        // precedente iterazione di colonna e ora devo togliere qualcosa
        // sulla riga per mantenere il numero di task della sorgente
        if ( segno==minus ) { // FASE 4 // verso==riga && segno==minus
            user = startrow%nusers;
            tpu  = user+1;

            for ( offset=1; offset<ndest; offset++ )
                for ( way=-1; way<2; way+=2 ) {

                    j=startcol+way*offset;
                    if ( j<0 || j>=ndest )
                        continue;

                    /* quando sono arrivato sulla riga, sono arrivato in una    **
                    ** cella dove ho aggiunto task, ora non posso toglierglieli **
                    ** di nuovo                                                 */
                    if ( j==startcol )
                        continue;

                    /* dato che devo togliere, se sono in una cella senza       **
                    ** allocazioni, la salto                                    */
                    if ( (*russelmatrix)[startrow][j]==-1 )
                        continue;

                    /* dato che devo togliere e k e' immutabile, se la cella    **
                    ** non ha abbastanza task la salto                          */
                    if ( (*russelmatrix)[startrow][j]<(*k) ) {
                        #ifdef _debugAggiustaR
                        sspace(cidx);
                        fprintf(stderr, " r: (%3d,%3d) non ha abbastanza task\n", startrow, j);
                        #endif // _debugAggiustaR
                        continue;
                        }

                    nt = (*russelmatrix)[startrow][j]-(*k);
                    if ( (nt%tpu)!=0 ) {
                        #ifdef _debugAggiustaR
                        sspace(cidx);
                        fprintf(stderr, " r: (%3d,%3d) viola i vincoli\n", startrow, j);
                        #endif // _debugAggiustaR
                        continue;
                        }


                    (*pathrows)[cidx]=startrow;
                    (*pathcols)[cidx]=j;

                    #ifdef _debugAggiustaR
                    sspace(cidx);
                    fprintf(stderr, " r: giro a (%3d,%3d)\n", startrow, j);
                    #endif // _debugAggiustaR

                    rs = aggiustaR(startrow, j,
                                   destrow, destcol,
                                   pathrows, pathcols, cidx+1,
                                   k, kmax,
                                   colonna,
                                   costo-(*k)*(*russelcosts)[startrow][j], costomax,
                                   plus,
                                   russelmatrix, russelcosts, modimatrix,
                                   nsources, ndest, nusers);

                    if ( rs==true )
                        return true;
                    }

            (*pathrows)[cidx]=-1;
            (*pathcols)[cidx]=-1;
            } // if ( riga && minus ) // fase 4


        if ( segno==plus ) {
            // FASE 0
            // segno positivo, aggiornamento fase 0: ho tolto sulla precedente
            // iterazione di colonna e ora devo ripristinare i task tolti alla
            // sorgente

            user = startrow%nusers;
            tpu  = user+1;

            min  = INT_MAX;
            if ( (*k)>tpu ) {
                if ( (*k)%tpu!=0 )
                    min=INT_MIN;
                }
            else {
                if ( tpu%(*k)!=0 )
                    min=INT_MIN;
                }

            if ( min==INT_MAX )
                for ( j=0; j<ndest-1; j++ ) {
                    if ( (*russelmatrix)[startrow][j]!=-1 )
                        continue;

                    if ( j==startcol )
                        continue;

                    if ( (*russelcosts)[startrow][j]<min )
                        min = (*russelcosts)[startrow][j];
                    }



            //min=INT_MIN; // sono da abolire aggiunte a random nelle righe
            for ( offset=1; offset<ndest; offset++ )
                for ( way=-1; way<2; way+=2 ) {

                    j=startcol+way*offset;
                    if ( j<0 || j>=ndest )
                        continue;

                    /* quando sono arrivato sulla riga, vi sono arrivato dopo   **
                    ** aver tolto dei task nella cella di arrivo, ora non posso **
                    ** aggiungerglieli di nuovo                                 */
                    if ( j==startcol )
                        continue;

                    /* se mi trovo nel deposito della riga, non mi interessa che**
                    ** modi mi abbia detto che sia una buona cella e nemmeno che**
                    ** la cella sia gia' allocata, io sposto i task e vado      **
                    ** avanti */
                    if ( j==ndest-1 ) {
                        (*pathrows)[cidx]=startrow;
                        (*pathcols)[cidx]=j;
                        #ifdef _debugAggiustaR
                        sspace(cidx);
                        fprintf(stderr, " r: giro a (%3d,%3d) - deposito\n", startrow, j);
                        #endif // _debugAggiustaR

                        rs = aggiustaR(startrow, j,
                                       destrow, destcol,
                                       pathrows, pathcols,
                                       cidx+1,
                                       k, kmax,
                                       colonna,
                                       costo+(*k)*(*russelcosts)[startrow][j], costomax,
                                       minus,
                                       russelmatrix, russelcosts, modimatrix,
                                       nsources, ndest, nusers);

                        /* k potrebbe essere stato modificato lungo il percorso     **
                        ** devo controllare che vada ancora bene                    */
                        if ( rs==true ) {
                            if ( (*russelmatrix)[startrow][j]==-1 )
                                nt = *k;
                            else
                                nt = (*russelmatrix)[startrow][j] + *k;

                            if (  (nt%tpu) == 0 )
                                return true;
                            #ifdef _debugAggiustaR
                            sspace(cidx);
                            fprintf(stderr, "In posizione %d del path k=%d non va bene\n", cidx, *k);
                            #endif // _debugAggiustaR
                            }

                        continue;
                        }


                    /* tendenzialmente sono portato ad aggiungere task nelle    **
                    ** celle dopo aver gia' allocato qualcosa perche' so che    **
                    ** sono le allocazioni migliori; tuttavia, pur di trovare   **
                    ** un path sono disposto ad allocare task anche nelle celle **
                    ** senza precedenti allocazioni ma a minor costo            */
                    if ( (*russelmatrix)[startrow][j]==-1 ) {
                        if ( (*russelcosts)[startrow][j]>min )
                            continue;
                        }


                    /* lungo il path i task spostati devono essere il massimo   **
                    ** comune multiplo per non creare altri conflitti;          **
                    ** se con i task che sposto ora si genera un conflitto      **
                    **     aumento k finche; non risolvo il conflitto oppure ho **
                    **     ragguinto kmax                                       **
                    ** se non ho risolto il conflitto oppure il nuovo k non e'  **
                    ** un multiplo del vecchio ignoro la cella                  */
                    if ( (*russelmatrix)[startrow][j]==-1 )
                        nt = *k;
                    else
                        nt = (*russelmatrix)[startrow][j] + *k;

                    if ( nt%tpu != 0 ) {
                        while ( nt%tpu != 0 && *k<*kmax ) {
                            (*k)++;
                            nt++;
                            }

                        if ( nt%tpu!=0 || *k%kiniziale!=0 ) {
                            *k=kiniziale;
                            #ifdef _debugAggiustaR
                            sspace(cidx);
                            fprintf(stderr, " r: (%3d,%3d) viola i vincoli\n", startrow, j);
                            #endif // _debugAggiustaR
                            continue;
                            }
                        }

                    (*pathrows)[cidx]=startrow;
                    (*pathcols)[cidx]=j;
                    #ifdef _debugAggiustaR
                    sspace(cidx);
                    fprintf(stderr, " r: giro a (%3d,%3d)\n", startrow, j);
                    #endif // _debugAggiustaR

                    rs = aggiustaR(startrow, j,
                                   destrow, destcol,
                                   pathrows, pathcols,
                                   cidx+1,
                                   k, kmax,
                                   colonna,
                                   costo+(*k)*(*russelcosts)[startrow][j], costomax,
                                   minus,
                                   russelmatrix, russelcosts, modimatrix,
                                   nsources, ndest, nusers);

                    /* k potrebbe essere stato modificato lungo il percorso     **
                    ** devo controllare che vada ancora bene                    */
                    if ( rs==true ) {
                        if ( (*russelmatrix)[startrow][j]==-1 )
                            nt = *k;
                        else
                            nt = (*russelmatrix)[startrow][j] + *k;

                        if (  (nt%tpu) == 0 )
                            return true;
                        }
                    }

        (*pathrows)[cidx]=-1;
        (*pathcols)[cidx]=-1;
            }
        }

    (*pathrows)[cidx]=-1;
    (*pathcols)[cidx]=-1;
//    rmTabuM(startrow, startcol);
    return false;
}


/* *********************************************************************** */
/* *********************************************************************** */
/* *********************************************************************** */


























///* Direction encoding:                                                      **
//**  colonna  = 0                                                            **
//**  riga     = 1                                                            */
//int aggiustaR(int startrow,   /* starting row coordinate */
//              int startcol,   /* starting column coordinate */
//              int** pathrows, /* vettore indici di riga del path */
//              int** pathcols, /* vettore indici di colonna del path */
//              int cidx, /* indice posizione corrente nel path */
//              int k, /* numero di tasks da spostare */
//              int verso, /* sto andando sulle righe o sulle colonne? */
//              int costo, /* costo del path fino a questo punto */
//              int costomax, /* la somma del path deve fare meglio di cosi' */
//              //int segno,
//              int*** russelmatrix,
//              int*** russelcosts,
//              int nsources,
//              int ndest,
//              int nusers) {
//
//    int i,j;
//    int rs;
//    int min = INT_MAX;
//    int rmin =-1;
//
//
//
//
//    if ( !verso ) { /* sto andando per colonna */
//        for ( i=0; i<nsources; i++ ) {
//            if ( (*russelmatrix)[i][startcol]!=-1 ) {
//                if ( i==startrow )
//                    continue;
//
//                fprintf(stderr, "path|col: considero (%2d,%2d):",i,startcol);
//
//                if ( (((*russelmatrix)[i][startcol]+k)%(i%nusers+1))!=0 ) {
//                    fprintf(stderr, " user inadatto.\n");
//                    continue;
//                }
//
//                if ( costo+(*russelcosts)[i][startcol]<=0 ) {
//                    fprintf(stderr, "\n");
//                    (*pathrows)[cidx]=i;
//                    (*pathcols)[cidx]=startcol;
//
//                    fprintf(stderr, "Colonna - giro a (%3d,%3d)\n", i, startcol);
//
//                    rs = aggiustaR(i, startcol,
//                                   pathrows, pathcols,
//                                   cidx+1, k, 1,
//                                   costo+(*russelcosts)[i][startcol], costomax,
//                                   russelmatrix, russelcosts,
//                                   nsources, ndest, nusers);
//
//                    if ( rs==true )
//                        return true;
//
//                    }
//                else
//                    fprintf(stderr, " costo elevato\n");
//                }
//
//            if ( (*russelmatrix)[i][ndest-1]>=k )
//                if ( (((*russelmatrix)[i][startcol]+k)%(i%nusers+1))==0 )
//                    if ( (*russelcosts)[i][startcol]<min ) {
//                        min  = (*russelcosts)[i][startcol];
//                        rmin = i;
//                        }
//            }
//
//
//        if ( costo+(*russelcosts)[rmin][startcol]<=costomax ) {
//            (*pathrows)[cidx]=rmin;
//            (*pathcols)[cidx]=startcol;
//            return true;
//            }
//        else
//            return false;
//
////        (*pathrows)[cidx]=rmin;
////        (*pathcols)[cidx]=startcol;
////        return true;
//
//        }
//    else { /* sto andando per rigaaaaa ---> */
//        for ( j=0; j<ndest; j++ ) {
//            if ( (*russelmatrix)[startrow][j]!=-1 ) {
//                if ( j==startcol )
//                    continue;
//
//                fprintf(stderr, "path|rig: considero (%2d,%2d):",startrow,j);
//
//                if ( (*russelmatrix)[startrow][j]<k ) {
//                    fprintf(stderr, " non ha abbastanza task\n");
//                    continue;
//                    }
//
//                if ( (((*russelmatrix)[startrow][j]-k)%(startrow%nusers+1))!=0 ) {
//                    fprintf(stderr, " user inadatto.\n");
//                    continue;
//                    }
//
//                if ( costo-(*russelcosts)[startrow][j]<0 ) {
//                    fprintf(stderr, "\n");
//                    (*pathrows)[cidx]=startrow;
//                    (*pathcols)[cidx]=j;
//
//                    fprintf(stderr, "Riga - giro a (%3d,%3d)\n", startrow, j);
//
//                    rs = aggiustaR(startrow, j,
//                                   pathrows, pathcols, cidx+1,
//                                   k, 0,
//                                   costo-(*russelcosts)[startrow][j], costomax,
//                                   russelmatrix, russelcosts,
//                                   nsources, ndest, nusers);
//
//                    if ( rs==true )
//                        return true;
//                    }
//                }
//            }
//            return false;
//        }
//
//    return false;
//}




int getUVR(int    startrow,
           int    startcol,
           int*   nx,
           int    cidx,
           int*** russelmatrix,
           int*** russelcosts,
           int**  u,
           int**  v,
           int nsources,
           int ndest) {

    int i, j, rs;

    if ( (*nx)>=(nsources+ndest-1) )
        return true;

    if ( cidx%2==1 ) { // cidx dispari per andare per colonna
        for ( i=0; i<nsources; i++ ) {
            if ( (*russelmatrix)[i][startcol]==-1 )
                continue;

            if ( i==startrow )
                continue;

            if ( (*u)[i]!=-1 )
                continue;

            if ((*v)[startcol]==-1)
                continue;

            (*u)[i] = (*russelcosts)[i][startcol] - (*v)[startcol];
            (*nx)++;

//            fprintf(stderr, "nx %d, u[%d]=%d\n", (*nx), i, (*u)[i] );

            rs = getUVR(i, startcol,
                        nx, cidx+1,
                        russelmatrix, russelcosts,
                        u, v,
                        nsources, ndest);

            }
        }
    else { // cidx pari per andare per riga

        for ( j=0; j<ndest; j++ ) {

            if ( (*russelmatrix)[startrow][j]==-1 )
                continue;

            if ( j==startcol )
                continue;

            if ( (*v)[j]!=-1 )
                continue;

            if ( (*u)[startrow]==-1 )
                continue;

            (*v)[j] = (*russelcosts)[startrow][j] - (*u)[startrow];
            (*nx)++;

//            fprintf(stderr, "nx %d, v[%d]=%d\n", (*nx), j, (*v)[j] );

            rs = getUVR(startrow, j,
                        nx, cidx+1,
                        russelmatrix, russelcosts,
                        u, v,
                        nsources, ndest);
            }
        }

    return false;
}





int findPathR(int startrow,   /* starting row coordinate */
              int startcol,   /* starting column coordinate */
              int destrow,    /* destination row */
              int destcol,    /* destination column */
              int** pathrows, /* vettore indici di riga del path */
              int** pathcols, /* vettore indici di colonna del path */
              int cidx,       /* indice posizione corrente nel path */
              int noWay,
              int*** russelmatrix,
              int*** russelcosts,
              int nsources,
              int ndest) {




    int i, j, rs;
    int offset, way;

    if ( isTabu(startrow, startcol) )
        return false;

    addTabu(startrow, startcol);

    if ( cidx%2==1 ) { // cidx dispari sto andando per colonna

        for ( offset=1; offset<nsources; offset++ )
            for ( way=-1; way<2; way+=2 ) {
                i=startrow+(way*offset);
                if ( i<0 || i>=nsources )
                    continue;

                if ( i==destrow && startcol==destcol && noWay<=0  )
                    return true;

                if ( (*russelmatrix)[i][startcol]==-1 )
                    continue;

                (*pathrows)[cidx]=i;
                (*pathcols)[cidx]=startcol;


//                sspace(cidx);
//                fprintf(stderr, "c: giro a (%3d,%3d)\n", i, startcol);

                rs = findPathR(i, startcol,
                               destrow, destcol,
                               pathrows, pathcols,
                               cidx+1,
                               noWay-(*russelcosts)[i][startcol],
                               russelmatrix, russelcosts,
                               nsources, ndest);
                if ( rs )
                    return true;

                (*pathrows)[cidx]=-1;
                (*pathcols)[cidx]=-1;
                }
        }

    else { // cidx pari sto andando per riga
        for ( offset=1; offset<ndest; offset++ )
            for ( way=-1; way<2; way+=2 ) {
                j=startcol+(way*offset);

                if ( j<0 || j>=ndest )
                    continue;

                if ( startrow==destrow && j==destcol && noWay<=0 )
                    return true;

                if ( (*russelmatrix)[startrow][j]==-1 )
                    continue;

                (*pathrows)[cidx]=startrow;
                (*pathcols)[cidx]=j;

//                sspace(cidx);
//                fprintf(stderr, "r: giro a (%3d,%3d)\n", startrow, j);

                rs = findPathR(startrow, j,
                               destrow, destcol,
                               pathrows, pathcols,
                               cidx+1,
                               noWay+(*russelcosts)[startrow][j],
                               russelmatrix, russelcosts,
                               nsources, ndest);
                if ( rs )
                    return true;

                (*pathrows)[cidx]=-1;
                (*pathcols)[cidx]=-1;
                }
            }
//    rmTabu();
    return false;
}







//
//
///* Direction encoding:                                                      **
//**  sinistra = 0                                                            **
//**  sopra    = 1                                                            **
//**  destra   = 2                                                            **
//**  sotto    = 3                                                            **
//**                                                                          */
//int findPathR(int startrow,   /* starting row coordinate */
//              int startcol,   /* starting column coordinate */
//              int destrow,    /* destination row */
//              int destcol,    /* destination column */
//              int** pathrows, /* vettore indici di riga del path */
//              int** pathcols, /* vettore indici di colonna del path */
//              int cidx,       /* indice posizione corrente nel path */
//              int noWay,
//              int*** russelmatrix,
//              int nsources,
//              int ndest) {
//
//    int i,j;
//    int rs;
//    #ifdef _debugPrintStepByStepPrintFindPath
//    int p;
//    #endif // _debugPrintStepByStepPrintFindPath
//
//
//
//    if ( isTabu(startrow, startcol) ) {
//        #ifdef _debugPrintStepByStepPrintFindPath
//        sspace(cidx);
//        fprintf(stderr, "...isTabu\n");
//        #endif // _debugPrintStepByStepPrintFindPath
//        return false;
//    }
//    addTabu(startrow, startcol);
//
//
//    /* go left                                                              */
//    #ifdef _debugPrintStepByStepPrintFindPath
//    if ( noWay!=0 ) {
//        sspace(cidx);
//        fprintf(stderr, "I GO LEFT\n");
//        }
//    #endif // _debugPrintStepByStepPrintFindPath
//
//    i=startrow;
//    j=startcol-1;
//    rs=false;
//    if ( noWay!=0 )
//        while ( j>=0 ) {
//            if ( (*russelmatrix)[i][j]!=-1 ) {
//                (*pathrows)[cidx]=i;
//                (*pathcols)[cidx]=j;
//
//                #ifdef _debugPrintStepByStepPrintFindPath
//                sspace(cidx);
//                fprintf(stderr, " I check (%3d,%3d)\n", i, j);
//                #endif // _debugPrintStepByStepPrintFindPath
//
//                if ( i==destrow && j==destcol && cidx>1 )
//                    return true;
//
//                rs = findPathR(i, j, destrow, destcol,
//                               pathrows, pathcols, cidx+1, 2,
//                               russelmatrix, nsources, ndest);
//                if (rs)
//                    return true;
//                }
//            if ( i==destrow && j==destcol && cidx>1 ) {
//                (*pathrows)[cidx]=i;
//                (*pathcols)[cidx]=j;
//                return true;
//                }
//            j--;
//            }
//
//
//    /* go up */
//    #ifdef _debugPrintStepByStepPrintFindPath
//    if ( noWay!=1 ) {
//        for ( p=0; p<cidx; p++ )
//            fprintf(stderr, "  ");
//        fprintf(stderr, "I GO UP\n");
//        }
//    #endif // _debugPrintStepByStepPrintFindPath
//
//
//    i=startrow-1;
//    j=startcol;
//    rs=false;
//    if ( noWay!=1 )
//        while ( i>=0 ) {
//            if ( (*russelmatrix)[i][j]!=-1 ) {
//                (*pathrows)[cidx]=i;
//                (*pathcols)[cidx]=j;
//
//                #ifdef _debugPrintStepByStepPrintFindPath
//                for ( p=0; p<cidx; p++ )
//                    fprintf(stderr, "  ");
//                fprintf(stderr, "I check (%3d,%3d)\n", i, j);
//                #endif // _debugPrintStepByStepPrintFindPath
//
//
//                if ( i==destrow && j==destcol && cidx>1 )
//                    return true;
//
//                rs = findPathR(i, j, destrow, destcol,
//                               pathrows, pathcols, cidx+1, 3,
//                               russelmatrix, nsources, ndest);
//                if (rs)
//                    return true;
//                }
//            if ( i==destrow && j==destcol && cidx>1 ) {
//                (*pathrows)[cidx]=i;
//                (*pathcols)[cidx]=j;
//                return true;
//                }
//            i--;
//            }
//
//
//    /* go right */
//
//    #ifdef _debugPrintStepByStepPrintFindPath
//    if ( noWay!=2 ) {
//        for ( p=0; p<cidx; p++ )
//            fprintf(stderr, "  ");
//        fprintf(stderr, "I GO RIGHT\n");
//        }
//    #endif // _debugPrintStepByStepPrintFindPath
//
//    i=startrow;
//    j=startcol+1;
//    rs=false;
//    if ( noWay!=2 )
//        while ( j<ndest ) {
//            if ( (*russelmatrix)[i][j]!=-1 ) {
//                (*pathrows)[cidx]=i;
//                (*pathcols)[cidx]=j;
//                #ifdef _debugPrintStepByStepPrintFindPath
//                for ( p=0; p<cidx; p++ )
//                    fprintf(stderr, "  ");
//                fprintf(stderr, "I check (%3d,%3d)\n", i, j);
//                #endif // _debugPrintStepByStepPrintFindPath
//
//                if ( i==destrow && j==destcol && cidx>1 )
//                    return true;
//
//                rs = findPathR(i, j, destrow, destcol,
//                               pathrows, pathcols, cidx+1, 0,
//                               russelmatrix, nsources, ndest);
//                if (rs)
//                    return true;
//                }
//            if ( i==destrow && j==destcol && cidx>1 ) {
//                (*pathrows)[cidx]=i;
//                (*pathcols)[cidx]=j;
//                return true;
//                }
//            j++;
//            }
//
//
//    /* go down */
//    #ifdef _debugPrintStepByStepPrintFindPath
//    if ( noWay!=3 ){
//        for ( p=0; p<cidx; p++ )
//            fprintf(stderr, "  ");
//        fprintf(stderr, "I GO DOWN\n");
//        }
//    #endif // _debugPrintStepByStepPrintFindPath
//
//    i=startrow+1;
//    j=startcol;
//    rs=false;
//    if ( noWay!=3 )
//        while ( i<nsources ) {
//            if ( (*russelmatrix)[i][j]!=-1 ) {
//                (*pathrows)[cidx]=i;
//                (*pathcols)[cidx]=j;
//
//                #ifdef _debugPrintStepByStepPrintFindPath
//                for ( p=0; p<cidx; p++ )
//                    fprintf(stderr, "  ");
//                fprintf(stderr, "I check (%3d,%3d)\n", i, j);
//                #endif // _debugPrintStepByStepPrintFindPath
//
//                if ( i==destrow && j==destcol && cidx>1 )
//                    return true;
//
//                rs = findPathR(i, j, destrow, destcol,
//                               pathrows, pathcols, cidx+1, 1,
//                               russelmatrix, nsources, ndest);
//                if (rs)
//                    return true;
//                }
//            if ( i==destrow && j==destcol && cidx>1 ) {
//                (*pathrows)[cidx]=i;
//                (*pathcols)[cidx]=j;
//                return true;
//                }
//            i++;
//            }
//
//    /* backtrack */
//    (*pathrows)[cidx]=-1;
//    (*pathcols)[cidx]=-1;
////    rmTabu();
////    addTabu(startrow, startcol);
//    return false;
//}

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
