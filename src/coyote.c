#include "coyote.h"

//#define _debugVerbose

struct s_coiote {
    int**** costs;          /* Costi
                                    [ncelle][ncelle][nusers][nintervalli]   */
    int*    tasks;          /* Tasks            [ncelle]                    */
    int*    tpu;            /* Tasks Per User   [nusers]                    */
    int***  pos;            /* Supply   [ncelle][nusers][nintervalli]       */

    int     nusers;         /* numero di users                              */
    int     nintervalli;    /* numero di intervalli                         */
    int     ncelle;         /* numero di celle                              */

    //TODO: assignment matrix una per ram e una per vam (anche piu' di una in realta'
    int***  assm;           /* Assignment Matrix
                                [ncelle*nusers+1][ncelle+1][nintervalli]    */

    int***  assc;           /* Assignment Costs Matrix
                                [ncelle*nusers+1][ncelle+1][nintervalli]    */
    int**   sourcesidx;     /* indici sorgenti nella matrice originale
                                [ncelle*nusers+1][nintervalli]              */
    int**   destsidx;       /* indici destinazioni nella matrice originale
                                [ncelle+1][nintervalli]                     */
    int     totdemand;      /* Richieste totali                             */
    int*    totsupply;      /* Forniture totali [nintervalli]               */
};

static struct s_coiote* coioteData;




void initCoiote(char* file) {
    FILE* f;

    #ifdef _debugVerbose
    fprintf(stderr, "Inizializzo CoIoTe...\n");
    #endif // _debugVerbose

    coioteData=(struct s_coiote*) malloc(sizeof(struct s_coiote));
    assert( coioteData );

    /* leggo il numero di celle, users e intervalli per un'allocazione      **
    ** della memoria consapevole.                                           */
    f=fopen(file, "r");
    assert(f!=NULL);
    fscanf(f, "%d", &(coioteData->ncelle));         /* leggo nr. celle      */
    fscanf(f, "%d", &(coioteData->nintervalli));    /* leggo nr. intervalli */
    fscanf(f, "%d", &(coioteData->nusers));         /* leggo nr. genti      */
    fclose(f);

    #ifdef _debugVerbose
    fprintf(stderr, "Numero di celle:      %4d\n", coioteData->ncelle);
    fprintf(stderr, "Numero di users:      %4d\n", coioteData->nusers);
    fprintf(stderr, "Numero di intervalli: %4d\n", coioteData->nintervalli);
    #endif // _debugVerbose

    /* -------------------------------------------------------------------- **
    ** Allocazione e preparazione della struttura dati di base              **
    ** -------------------------------------------------------------------- */
    #ifdef _debugVerbose
    fprintf(stderr,
            "Allocazione e inizializzazione della struttra dati...");
    #endif // _debugVerbose

    allocBase();

    #ifdef _debugVerbose
    fprintf(stderr, " OK\n");
    #endif // _debugVerbose


    /* -------------------------------------------------------------------- **
    ** Lettura dei dati dal file                                            **
    ** -------------------------------------------------------------------- */
    readFile(file);

    /* -------------------------------------------------------------------- **
    ** Allocazione e preparazione della struttura dati per Russel           **
    ** -------------------------------------------------------------------- */
    #ifdef _debugVerbose
    fprintf(stderr,
            "Allocazione e inizializzazione della struttra dati RAM...");
    #endif // _debugVerbose
    initRAM(coioteData->nusers,
            coioteData->ncelle);
    #ifdef _debugVerbose
    fprintf(stderr, " OK\n");
    #endif // _debugVerbose

    /* -------------------------------------------------------------------- **
    ** Allocazione e preparazione della struttura dati per Vogel            **
    ** -------------------------------------------------------------------- */
    #ifdef _debugVerbose
    fprintf(stderr,
            "Allocazione e inizializzazione della struttra dati VAM...");
    #endif // _debugVerbose
    initVAM(coioteData->nusers,
            coioteData->ncelle);
    #ifdef _debugVerbose
    fprintf(stderr, " OK\n");
    #endif // _debugVerbose

    /* -------------------------------------------------------------------- **
    ** Allocazione e preparazione della struttura dati per MODI             **
    ** -------------------------------------------------------------------- */
    #ifdef _debugVerbose
    fprintf(stderr,
            "Allocazione e inizializzazione della struttra dati MODI...");
    #endif // _debugVerbose
    initMODI(coioteData->nusers,
             coioteData->ncelle);
    #ifdef _debugVerbose
    fprintf(stderr, " OK\n");
    fprintf(stderr, "Inizializzazione completata.\n");
    #endif // _debugVerbose
}


int coioteProcess() {
    /* -------------------------------------------------------------------- **
    ** Strutture dati                                                       **
    ** -------------------------------------------------------------------- */
    FILE *f;

    int**** costs;
    int* tasks;
    int*** pos;

    int nusers;         /* numero di users                                  */
    int nintervalli;    /* numero di intervalli                             */
    int ncelle;         /* numero di celle                                  */

    /* time is $$                                                           */
    clock_t end;
    clock_t start;

    int user;           /* indice per l'user                                */
    int time;           /* indice per il tempo                              */
    int cella;          /* indice per la cella                              */
    int i, j, k;        /* indici vari utili                                */

    int flag;           /* useful Status                                    */

    float obj=0.0;


    /* //////////////////////////////////////////////////////////////////// **
    ** Elaborazione                                                         **
    ** //////////////////////////////////////////////////////////////////// */
    start=clock();























    #ifdef _debugPrintStepByStepPrintFinal
    fprintf(stderr,
            "====================================================================================================\n");
    fprintf(stderr, " FINAL MATRIX\n");
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
    fprintf(stderr,
            "====================================================================================================\n");
    fprintf(stderr,
            "====================================================================================================\n");
    fprintf(stderr,
            "====================================================================================================\n");
    #endif // _debugPrintStepByStepPrintFinal

    /* ==================================================================== */
    /* -------------------------------------------------------------------- */
    /* ==================================================================== */


    end=clock();

    if ( isFeasible(&russelmatrix, &pos, &tasks,
                   &sourcesidx, &destsidx,
                   nsources, ndest, nusers, time) ) {
        obj=0;
        for ( source=0; source < nsources; source++ )
            for ( dest=0; dest<ndest-1; dest++ )
                if ( russelmatrix[source][dest]!=-1 ) {
                    user=source%nusers;
                    i = sourcesidx[source];
                    j = destsidx[dest];
    //                    k =(int) ceil((float)  russelmatrix[source][dest] / (user+1));
                    k =(int) floor((float)  russelmatrix[source][dest] / (user+1));
                    obj += k * costs[i][j][user][time];
                    }

    //        printf("Istanza %s.\n", argv[1]);
    //        printf("Tempo: %f\n", ((double)end-start)/CLOCKS_PER_SEC);
    //        printf("Costo: %f\n", obj);
    //        printf("ITERAZIONE;Istanza;OF;Time\n");

        fprintf(stdout, "%s;%d;%d;%f\n",basename(argv[1]), kkk,
               (int) obj,(((double)end-start)/CLOCKS_PER_SEC));
        fprintf(stderr, "%s;%d;%d;%f\n",basename(argv[1]), kkk,
               (int) obj, (((double)end-start)/CLOCKS_PER_SEC));
        }
    else {
        fprintf(stdout, "%s;%d;-;-;SOLUTION UNFEASIBLE\n",
                basename(argv[1]), kkk);
        fprintf(stderr, "%s;%d;-;-;SOLUTION UNFEASIBLE\n",
                basename(argv[1]), kkk);

        }
    #ifdef _debugVerbose
    fprintf(stderr, "\n\n");
    #endif // _debugVerbose





    return 0;

}



/* ------------------------------------------------------------------------ **
** Libera tutti                                                             **
** ------------------------------------------------------------------------ */
void freeCoiote() {

    int i, j, k;

    int*     tasks      = coioteData->tasks;
    int*     tpu        = coioteData->tpu;
    int****  costs      = coioteData->costs;
    int***   pos        = coioteData->pos;

    int***   assc       = coioteData->assc;
    int**    sourcesidx = coioteData->sourcesidx;
    int**    destsidx   = coioteData->destsidx;

    int ncelle          = coioteData->ncelle;
    int nusers          = coioteData->nusers;
    int nintervalli     = coioteData->nintervalli;

    int nsources        = ncelle*nusers+1;
    int ndests          = ncelle+1;


    freeMODI();
    freeVAM();
    freeRAM();

    free( tasks );
    free( tpu   );

    for ( i=0; i<nsources; i++ )
        free( sourcesidx[i] );
    free( sourcesidx    );

    for ( i=0; i<ndests  ; i++ )
        free( destsidx[i]   );
    free( destsidx      );


    for ( i=0; i<ncelle; i++ )
        for ( j=0; j<ncelle; j++)  {
            for ( k=0; k<nusers; k++ )
                free( costs[i][j][k] );
            free( costs[i][j] );
            }
        free( costs[i] );

    free( costs );

    for ( i=0; i<ncelle; i++ ) {
        for ( j=0; j<nusers; j++ )
            free( pos[i][j] );
        free( pos[i] );
        }

    free( pos );

    for ( i=0; i<nsources; i++ )
        free( assc[i] );

    free( assc );
}



/* ------------------------------------------------------------------------ **
** Lettura dei dati dal file                                                **
** ------------------------------------------------------------------------ */
void readFile(char* file) {
    FILE* f;
    int nusers;         /* numero di users                                  */
    int nintervalli;    /* numero di intervalli                             */
    int ncelle;         /* numero di celle                                  */

    int**     tasks = &(coioteData->tasks);
    int**     tpu   = &(coioteData->tpu);
    int*****  costs = &(coioteData->costs);
    int****   pos   = &(coioteData->pos);

    int*  totdemand = &(coioteData->totdemand);
    int** totsupply = &(coioteData->totsupply);


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

    #ifdef _debugVerbose
    fprintf(stderr, "Inizializzo i 'tasks per user'...");
    #endif // _debugVerbose

    /* leggi i 'tasks per user'                                             */
    for ( i=0; i<nusers; i++ )
        fscanf (f, "%d", &((*tpu)[i]));

    #ifdef _debugVerbose
    fprintf(stderr, " OK\n");
    fprintf(stderr, "Carico la matrice dei costi...");
    #endif // _debugVerbose

    /* carica le super-matrici dei costi                                    */
    for ( k=0; k<nintervalli*nusers; k++ ) {
        fscanf (f, "%d", &user);
        fscanf (f, "%d", &time);

        for ( i=0; i<ncelle; i++ )
            for ( j=0; j<ncelle; j++ ) {
                fscanf (f, "%f", &tmp);
                (*costs)[i][j][user][time]=(int) floor(tmp);
                }
        }

    #ifdef _debugVerbose
    fprintf(stderr, " OK\n");
    fprintf(stderr, "Carico i tasks per le celle destinazione...");
    #endif // _debugVerbose

    /* carica i tasks per le celle                                          */
    *totdemand=0;
    for ( cella=0; cella<ncelle; cella++ ) {
        fscanf (f, "%d", &((*tasks)[cella]));
        *totdemand += (*tasks)[cella];
        }

    #ifdef _debugVerbose
    fprintf(stderr, " OK\n");
    fprintf(stderr, "Carico le posizioni degli utenti...");
    #endif // _debugVerbose

    /* carica le posizioni dei vari tipi di                                 **
    ** users nei diversi intervalli                                         */
    for ( k=0; k<nintervalli*nusers; k++ ) {
        fscanf (f, "%d", &user);
        fscanf (f, "%d", &time);

        for (cella=0; cella<ncelle; cella++)
            fscanf (f, "%d", &((*pos)[cella][user][time]));
        }

    #ifdef _debugVerbose
    fprintf(stderr, " OK\n");
    #endif // _debugVerbose

    fclose (f);
}

/* ------------------------------------------------------------------------ **
** Allocazione e inizializzazione della struttura dati                      **
** ------------------------------------------------------------------------ */
void allocBase() {
    // TODO: inserire inizializzazioni
    int i, j, k;

    int**    tasks      = &(coioteData->tasks);
    int**    tpu        = &(coioteData->tpu);
    int***** costs      = &(coioteData->costs);
    int****  pos        = &(coioteData->pos);

    int****  assc       = &(coioteData->assc);
    int***   sourcesidx = &(coioteData->sourcesidx);
    int***   destsidx   = &(coioteData->destsidx);

    int**    totsupply  = &(coioteData->totsupply);

    int ncelle          = coioteData->ncelle;
    int nusers          = coioteData->nusers;
    int nintervalli     = coioteData->nintervalli;

    int nsources        = ncelle*nusers+1;
    int ndests          = ncelle+1;


    // [ncelle]
    *tasks = (int*) malloc(ncelle*sizeof(int));
    assert( *tasks );

    *tpu   = (int*) malloc(nusers*sizeof(int));
    assert( *tpu );

    *totsupply = (int*) malloc((nintervalli*sizeof(int)));
    assert( *totsupply );
    // [ncelle][ncelle][nusers][nintervalli]
    *costs = (int****) malloc(ncelle*sizeof(int***));
    assert( *costs );
    for ( i=0; i<ncelle; i++ ) {
        (*costs)[i] = (int***) malloc(ncelle*sizeof(int**));
        assert( (*costs)[i] );

        for ( j=0; j<ncelle; j++ ) {
            (*costs)[i][j] = (int**) malloc(nusers*sizeof(int*));
            assert( (*costs)[i][j] );

            for ( k=0; k<nusers; k++ ) {
                (*costs)[i][j][k] = (int*) malloc(nintervalli*sizeof(int));
                assert( (*costs)[i][j][k] );
                }
            }
        }

    // [ncelle][nusers][nintervalli]
    *pos = (int***) malloc(ncelle*sizeof(int**));
    assert( *pos );
    for ( i=0; i<ncelle; i++ ) {
        (*pos)[i] = (int**) malloc(nusers*sizeof(int*));
        assert( (*pos)[i] );

        for ( j=0; j<nusers; j++ ) {
            (*pos)[i][j] = (int*) malloc(nintervalli*sizeof(int));
            assert( (*pos)[i][j] );
            }
        }


    // [ncelle*nusers+1]
    *sourcesidx = (int**) malloc(nsources*sizeof(int*));
    assert( *sourcesidx );
    for ( i=0; i<nsources; i++ ) {
        (*destsidx)[i] = (int*) malloc(nintervalli*sizeof(int));
        assert( (*destsidx)[i] );
        }

    // [ncelle+1]
    *destsidx   = (int**) malloc(ndests*sizeof(int*));
    assert( *destsidx );
    for ( i=0; i<ndests; i++ ) {
        (*destsidx)[i] =(int*) malloc(nintervalli*sizeof(int));
        assert( (*destsidx)[i] );
        }

    // [ncelle*nusers+1][ncelle+1][nintervalli]
    // [sources]        [dests]   [intervals]
    *assc   = (int***) malloc(nsources*sizeof(int**));
    assert( *assc );
    for ( i=0; i<nsources; i++ ) {
        (*assc)[i] = (int**) malloc(ndests*sizeof(int*));
        assert( (*assc)[i] );

        for ( j=0; j<ndests; j++ ) {
            (*assc)[i][j] = (int*) malloc(nintervalli*sizeof(int));
            assert* (*assc)[i][j] );
            }
        }
}
