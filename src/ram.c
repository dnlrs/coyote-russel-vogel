/*
** Russell's Approximation Method
** ------------------------------
**
** Per ogni riga "i" si trova il costo massimo e lo si salva in u[i].
** Per ogni colonna "j" si trova il costo massimo e lo si salva in v[j].
**
** Per ogni varibile non ancora inizializzata, si calcola
**                  d[i][j] = c[i][j]-u[i]-v[j]
**
** Si esegue l'assegnazione per la variabile che ha il d[i][j] massimo.
**
** Assegnazione
** ------------
**
** Si assegna il massimo tra la domanda e la fornitura disponibile.
**
*/

#include "coyote.h"


struct s_ram{


    int** russelcosts;  /* matrice di Russell con i costi                     */
    int** russelmatrix; /* matrice di Russell con le soluzioni                */
    int** russelweights; /* matrice di Russell con i pesi di ogni variabile   */


    int* u;             /* largest unit costs for row                       */
    int* v;             /* largest unit costs for columns                   */
};
static struct s_ram* ramData;



void initRAM(nusers, ncelle) {

    ramData = (struct s_ram*) malloc(sizeof(struct s_ram));
    assert( ramData );

    /* //////////////////////////////////////////////////////////////////// **
    ** Allocazione e preparazione della struttura dati per Russel           **
    ** //////////////////////////////////////////////////////////////////// */
    allocRussell(&(ramData->supply),
                 &(ramData->demand),
                 &(ramData->sourcesidx),
                 &(ramData->destsidx),
                 &(ramData->u),
                 &(ramData->v),
                 &(ramData->russelcosts),
                 &(ramData->russelmatrix),
                 &(ramData->russelweights),
                 nusers,
                 ncelle);
}

void ram() {

    /* //////////////////////////////////////////////////////////////////// **
    ** Strutture dati                                                       **
    ** //////////////////////////////////////////////////////////////////// */


    int nsources;       /* numero di sorgenti                               */
    int ndest;          /* numero di destinazioni                           */
    int totsupply;      /* fornitura totale                                 */
    int totdemand;      /* domanda totale                                   */

    int dest;           /* indice per le destinazioni                       */
    int source;         /* indice per le sorgenti                           */


    int* supply;        /* vettore delle forniture (da sorgenti)            */
    int* demand;        /* vettore delle richieste (per destinazioni)       */

    int* sourcesidx;    /* indice delle sorgenti nella matrice originale    */
    int* destsidx;      /* indice delle destinazioni nella matrice originale*/

    int** russelcosts;  /* matrice di Russell con i costi                     */
    int** russelmatrix; /* matrice di Russell con le soluzioni                */
    int** russelweights; /* matrice di Russell con i pesi di ogni variabile   */


    int* u;             /* largest unit costs for row                       */
    int* v;             /* largest unit costs for columns                   */


    float cost;         /* variabile ausiliaria per il costo                */
//    int costi;          /* variabile ausiliaria per il costo                */
    int ccost;          /* variabile ausiliaria per il costo                */

    int itera;          /* variabile per le ~(m+n-1) iterazioni del RAM     */
//    int cmin;           /* variabile aus. per il costo min nella colonna    */
//    int rmin;           /* variabile aus. per il costo min nella riga       */
    int cmax;           /* variabile aus. pos. max. weight colonna          */
    int rmax;           /* variabile aus. pos. max. weight riga             */

    int maxdiff;        /* var. aus. per trovare la _differenza_ maggiore   */

    int assignment;     /* var. aus. per stabilire l'assegnazione           */


    int dummyCost;
    dummyCost=1000;



    /* ==================================================================== */
    /* ==================================================================== */

    time=0; // TODO: togliere il time=0 quando si hanno piu' intervalli
            //       di tempo
    /* NOTE:
    **   si imposta il tempo a 0 ma la procedura
    **   va ripetuta per tutti gli istanti di tempo                         */

    /* ==================================================================== */
    /* ==================================================================== */


    /* ==================================================================== */
    /*  1. INIZIALIZZA SORGENTI E DI DESTINAZIONI                           */
    /* ==================================================================== */

    /* -------------------------------------------------------------------- */
    /* DESTINAZIONI                                                         */
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

    ndest=dest+1; /* la dummy-destination                                        */
    #ifdef _debugVerbose
    fprintf(stderr, "Ho inizializzato le destinazioni.\n");
    #endif // _debugVerbose


    /* -------------------------------------------------------------------- */
    /* SORGENTI                                                             */
    /* -------------------------------------------------------------------- */
    #ifdef _debugVerbose
    fprintf(stderr, "Cerco le sorgenti... sorg [tipo user] [n. users] [tasks] -> sorg orig.\n");
    #endif // _debugVerbose
    totsupply=0;
    source=0;
    /* in questo contesto "flag" e' TRUE se la cella corrente contiene      **
    ** almeno un tipo di user diverso da zero                               */
    for ( cella=0; cella<ncelle; cella++ ) {
        flag=false;
        for ( user=0; user<nusers; user++ )
            if ( pos[cella][user][time]!=0 ) {
                flag=true;
                break;
            }

        if ( flag ) {
            /* Riempi le prossime nusers sources con i task disponibili.    **
            ** NB: considero i tasks non gli utenti                         */
            for ( user=0; user<nusers; user++ ) {
                supply[source+user]     = pos[cella][user][time]*(user+1);
                totsupply              += pos[cella][user][time]*(user+1);
                sourcesidx[source+user] = cella;
                #ifdef _debugVerbose
                fprintf(stderr, "Nuova sorgente: %4d [%4d] [%4d] [%4d] -> %4d.\n",
                       source+user, user, pos[cella][user][time],
                       supply[source+user], sourcesidx[source]);
                #endif // _debugVerbose
            }
            /* avanza di nusers sorgenti                                    */
            source += nusers;
        }
    }
    nsources=source;
    #ifdef _debugVerbose
    fprintf(stderr, "Ho inizializzato le sorgenti. (supply %6d e demand %6d)\n",
           totsupply, totdemand);
    #endif // _debugVerbose

    /* calcola la differenza tra domanda e fornitura e assegnala alla       **
    ** dummy-destination                                                    */
    if ( totsupply>totdemand )
        demand[ndest-1]=totsupply-totdemand;
    else
        demand[ndest-1]=0;


    /* -------------------------------------------------------------------- */
    /*  2. COSTI E MATRICI DELLE VARIABILI                                  */
    /* -------------------------------------------------------------------- */
    for ( source=0; source<nsources; source++ ) {
        i=sourcesidx[source];
        user=source%3;
        k=user+1;

        for ( dest=0; dest<ndest-1; dest++ ) {
            j=destsidx[dest];

            cost=(float) costs[i][j][user][time] / k;
            russelcosts[source][dest]=(int) (cost*100);

            russelmatrix[source][dest]=-1;
            }

        /* inizializza anche la colonna della dummy-destination             */
        russelcosts[source][ndest-1]=kkk;
        russelmatrix[source][ndest-1]=-1;
        }


    #ifdef _debugPrintCostsMatrix
    /* stampa matrice dei costi per debug                                   */
    printf("COSTI;");
    for (j=0; j<ndest;j++)
        printf("%4d;", j);
    printf("\n");

    for (i=0; i<nsources; i++) {
        printf("%4d;", i);
        for (j=0; j<ndest; j++)
            printf("%4d;", russelcosts[i][j]);
        printf("\n");
    }
    printf("\n");
    printf("\n");
    #endif // _debugPrintCostsMatrix


    /* ==================================================================== */
    /*  4. RUSSELL'S APPROXIMATION METHOD                                   */
    /* ==================================================================== */
    #ifdef _debugVerbose
    fprintf(stderr, "Inizio elaborazione RAM::\n");
    #endif // _debugVerbose


    /* inizializzo vettori "u" e "v"                                        */
    for ( source=0; source<nsources; source++ )
        u[source]=-1;
    for ( dest=0; dest<ndest; dest++ )
        v[dest]=-1;

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
    /* calcola i pesi per ogni cella d[i][j] = c[i][j]-u[i]-v[j]            */
    for ( source=0; source<nsources; source++ )
        for ( dest=0; dest<ndest; dest++ ) {
            /* non serve perche' e' la prima inizializzazione che viene     **
            ** fatta...
            if ( russelmatrix[source][dest]!=-1 )
                continue;
            **                                                              */

            ccost=russelcosts[source][dest];
            russelweights[source][dest]=ccost-u[source]-v[dest];

            if ( russelweights[source][dest]<maxdiff ) {
                maxdiff=russelweights[source][dest];
                rmax=source;
                cmax=dest;
                }
            }



    /* ==================================================================== */
    /* ==================================================================== */
    /* CICLO RUSSELL                                                        */
    /* ==================================================================== */
    /* ==================================================================== */

//    itera=ndest;
    itera=nsources+ndest-1;
    #ifdef _debugPrintStepByStep
    fprintf(stderr, "Iterazioni (e quindi variabili): %4d\n", itera);
    #endif // _debugPrintStepByStep

    while ( itera>0 ) {
        itera--;
        /* stampa mosse iterazione corrente per debug                       */
        #ifdef _debugPrintStepByStep
        fprintf(stderr,
                "==============================================================================================================\n");
        fprintf(stderr,
                "==============================================================================================================\n");
        fprintf(stderr, " PRIMA DELL'ASSEGNAZIONE\n");
        fprintf(stderr,
                "====================================================================================================\n");

        fprintf(stderr, " [%2d] ;", itera);
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", j);
        fprintf(stderr, " supply;");
        fprintf(stderr, "   u\n");

        for (i=0; i<nsources; i++) {
            fprintf(stderr, "  %4d;", i);
            for (j=0; j<ndest; j++) {
                if (russelmatrix[i][j]!=-1)
                    fprintf(stderr, "%4d;", russelmatrix[i][j]);
                else
                    fprintf(stderr, "    ;");
            }
            fprintf(stderr, "   %4d;", supply[i]);
            fprintf(stderr, "%4d;", u[i]);
            fprintf(stderr, "\n");
        }

        fprintf(stderr, "demand;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", demand[j]);
        fprintf(stderr, "\n");
        fprintf(stderr, "   u  ;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", v[j]);
        fprintf(stderr, "\n");
        fprintf(stderr, "\n");
        #endif // _debugPrintStepByStep

        /* ---------------------------------------------------------------- */
        /* cerca la variabile piu' negativa di tutte                        */
        /* ---------------------------------------------------------------- */
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
                    #ifdef _debugVerbose
                    fprintf(stderr, "Nuova maxdiff (%4d) riga %4d colonna %4d\n",
                           maxdiff, source, dest);
                    #endif // _debugVerbose
                    }
                }
            }

        if ( rmax==-1 && cmax==-1 )
            continue;

        /* ---------------------------------------------------------------- */
        /* ASSEGNAZIONE                                                     */
        /* ---------------------------------------------------------------- */
        #ifdef _debugVerbose
        fprintf(stderr, "Eseguo l'assegnazione (destinazioni rimaste: %4d):\n", itera);
        #endif // _debugVerbose


        /* dopo aver fatto uno spostamento di task che non corrisponde alla **
        ** totale fornitura possibile (per i motivi che puoi leggere nel    **
        ** commento sotto) devo fare in modo che alle prossime iterazioni   **
        ** questa casella non venga piu' presa in considerazione: il flag   **
        ** indica proprio questo e se il flag e' attivo viene incrementato  **
        ** il costo della cella                                             */
        flag=false;


        /* se la domanda < della fornitura posso fornire al massimo un      **
        ** numero intero di utenti quindi un multiplo del numero di task    **
        ** che puo' svolgere l'utente corrente                              */
        source  =rmax;
        dest    =cmax;

        if ( demand[dest] < supply[source] ) {
//            user = (source % nusers);
//            k = demand[dest] % (user+1);
//
//            if ( k != 0 ) {
//                assignment=demand[dest]-k;
//                flag=true;
//                #ifdef _debugVerbose
//                fprintf(stderr, "NOTA ASSEGNAZIONE: eseguiro' aggiustamenti\n");
//                #endif // _debugVerbose
//                }
//            else
                assignment=demand[dest];
            }

        else
            /* se la domanda >= della fornitura, posso tranquillamente      **
            ** spostare tutta la fornitura rimasta                          */
            assignment=supply[source];

        #ifdef _debugPrintAssignments
        fprintf(stderr, "Assegno %d:\n", assignment);
        fprintf(stderr, "  sorgente     %4d (user type:%2d - supply:%4d)\n",
               source, user, supply[source]);
        fprintf(stderr, "  destinazione %4d (demand:   %4d)\n",
               dest, demand[dest]);
        #endif // _debugPrintAssignments

        if ( russelmatrix[source][dest]==-1 )
            russelmatrix[source][dest]=assignment;
        else
            russelmatrix[source][dest]+=assignment;

        supply[source]-=assignment;
        demand[dest]  -=assignment;


        /* se mi trovo nella situazione del flag                                    */
/* ======================================================================== */
/* GESTIONE MOLTEPLICITA' UTENTE TASKS                                      */
/* ======================================================================== */
//        int cbc;    /* current best cost                                    */
//        if ( flag ) {
//
//            /* CURIOSITA': se mi trovo nell'ultima colonna puo'     **
//            ** capitare che mi ritrovi a spostare tasks da una      **
//            ** cella a se stessa, essendo che la riga corrente e    **
//            ** la riga a costo minore per l'ultima destinazione     **
//            ** sono la stessa.                                      **
//            ** Per risolvere, semplicemente gli do i task che       **
//            ** mancano senza nemmeno scomodare le sorgenti          */
//            if ( dest==ndest-1 ) {
////                user = source % nusers;
////
////                russelmatrix[source][dest]  += user+1;
////                supply[source]              -= user+1;
//                demand[dest]                -= k;
//
//                #ifdef _debugVerbose
//                fprintf(stderr, "(%4d,%4d) Aggiusto peggio: assegno %4d invece di %2d (perche' dummy-destination)\n",
//                        source, dest, user+1, k);
//                fprintf(stderr, "[assegnamento virtuale]\n");
//                #endif // _debugVerbose
//                }
//
//            else {
//                /* cerco di trovare una sorgente che abbia ancora           **
//                ** fornitura e tra quelle disponibili scelgo quella a costo **
//                ** minore                                                   */
//                cmax=-1;
//                for ( i=0; i<nsources; i++ ) {
//                    if ( supply[i]==-1 )
//                        continue;
//
//                    if ( supply[i]>=k ) {
//                        user  = (i%nusers);
//
//                        if ( (user+1)<=k ) {
//                            flag=k%(user+1);
//
//                            if ( flag==0 ) {
//                                if ( cmax==-1 )
//                                    cmax=i;
//                                else {
//                                    ccost = russelcosts[i][dest]*(user+1);
//                                    cbc   = russelcosts[cmax][dest]*((cmax%nusers)+1);
//                                    if ( ccost<=cbc )
//                                        cmax=i;
//                                    }
//                                }
//                            }
//                        }
//                    }
//
//                /* se non ho trovato nessuna sorgente disponibile con un        **
//                ** adeguato numero di tasks                                     */
//                if ( cmax==-1 ) {
//                    /* dato che i task sono contati e assegnando                **
//                    ** in modo peggiorativo (piu' task del dovuto) poi ottengo  **
//                    ** soluzioni non fattibili perche' mancano i task per le    **
//                    ** altre destinazioni, non mi resta che togliere task dalla **
//                    ** dummy destination e aggiungerli qui' che c'e' piu'       **
//                    ** bisogno.                                                 */
//                    for ( i=0; i<nsources; i++ ) {
//                        /* se nell'ultima colonna ci sono meno di k tasks       **
//                        ** disponibili la zompo                                 */
//                        if ( russelmatrix[i][ndest-1]<k )
//                            continue;
//
//                        user= (i%nusers);
//                        if ( (user+1)<=k ) {
//                            flag=k%(user+1);
//
//                            if ( flag==0 ) {
//                                if ( cmax==-1 )
//                                    cmax=i;
//                                else {
//                                    ccost=russelcosts[i][dest]*(user+1);
//                                    cbc  =russelcosts[cmax][dest]*((cmax%nusers)+1);
//                                    if ( cost<cbc )
//                                        cmax=i;
//                                    }
//                                }
//                            }
//                        }
//                    user=cmax%nusers;
//                    assignment=k;
//
//                    if ( russelmatrix[cmax][dest]==-1 )
//                        russelmatrix[cmax][dest]  = assignment;
//                    else
//                        russelmatrix[cmax][dest] += assignment;
//
//                    russelmatrix[cmax][ndest-1]  -= assignment;
//
//                    /* non c'e' bisogno che modifico la supply della riga       **
//                    ** ho solamente spostato i task dalla dummy-destination     */
//    //                supply[cmax] -= assignment;
//                    demand[dest] -= assignment;
//                    if ( demand[ndest-1]==-1 ) {
//                        demand[ndest-1]  =assignment;
//                        itera++;
//                        }
//                    else
//                        demand[ndest-1] +=assignment;
//
//                    #ifdef _debugVerbose
//                    fprintf(stderr,
//                            "(%4d,%4d) Aggiusto meglio: assegno %4d tasks [user type %2d] (tolto all'ultima colonna)\n",
//                            cmax, dest, assignment, cmax%nusers+1);
//                    #endif // _debugVerbose
//                    }
//                else {
//                    user = cmax % nusers;
//                    assignment=k;
//
//                    if ( russelmatrix[cmax][dest]==-1 )
//                        russelmatrix[cmax][dest]  = assignment;
//                    else
//                        russelmatrix[cmax][dest] += assignment;
//
//                    supply[cmax] -= assignment;
//                    demand[dest] -= assignment;
//
//                    #ifdef _debugVerbose
//                    fprintf(stderr,
//                            "(%4d,%4d) Aggiusto meglio: assegno %4d tasks [user type %2d]\n",
//                            cmax, dest, assignment, cmax%nusers+1);
//                    #endif // _debugVerbose
//                    }
//                }
//            }


/* ======================================================================== */
/* ======================================================================== */

        /* AGGIORNAMENTO FORNITURE / DOMANDA                                */
        /* situazione degenere                                              */
        if ( (supply[source]==0 && demand[dest]==0) || supply[source]==0 ) {
            supply[source]  =-1;
            u[source]     =-1;

            if ( itera==1 && demand[dest]==0 ) {
                demand[dest]    =-1;
                v[dest]       =-1;
//                itera--;
                }
            }
        else
            if ( demand[dest]==0 ) {
                demand[dest]    =-1;
                v[dest]       =-1;
//                itera--;
                }

        /* stampa mosse iterazione corrente per debug                       */
        #ifdef _debugPrintStepByStep
        fprintf(stderr,
                "====================================================================================================\n");
        fprintf(stderr, " DOPO DELL'ASSEGNAZIONE\n");
        fprintf(stderr,
                "====================================================================================================\n");

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
//        fprintf(stderr, "\n");
        fprintf(stderr,
                "====================================================================================================\n");
        fprintf(stderr,
                "====================================================================================================\n");
        fprintf(stderr,
                "====================================================================================================\n");

        #endif // _debugPrintStepByStep

//        /* AGGIORNAMENTO ================================================== */
//        if ( supply[source]==-1 ) {
//            for ( j=0; j<ndest; j++ ) {
//                if ( demand[j]==-1 ) {
//                    continue;
//                    }
//
//                v[j]=-1;
//                for ( i=0; i<nsources; i++ ) {
//                    if ( supply[i]==-1 )
//                        continue;
//
//                    if ( russelcosts[i][j]>v[j] )
//                        v[j]=russelcosts[i][j];
//                    }
//                }
//            }
//        else
//            if ( demand[dest]==-1 ) {
//                for ( i=0; i<nsources; i++ ) {
//                    if ( supply[i]==-1 )
//                        continue;
//
//                    u[i]=-1;
//                    for ( j=0; j<ndest; j++ ) {
//                        if ( demand[j]==-1 )
//                            continue;
//
//                        if ( russelcosts[i][j]>u[i] )
//                            u[i]=russelcosts[i][j];
//
//                        }
//                    }
//                }
//
//        maxdiff=INT_MAX;
//        rmax=-1;
//        cmax=-1;
//        /* calcola i pesi per ogni cella d[i][j] = c[i][j]-u[i]-v[j]            */
//        for ( source=0; source<nsources; source++ ) {
//            if ( supply[source]==-1 )
//                continue;
//
//            for ( dest=0; dest<ndest; dest++ ) {
//                if ( demand[dest]==-1 )
//                    continue;
//
////                if ( russelmatrix[source][dest]!=-1 )
////                    continue;
//
//                ccost=russelcosts[source][dest];
//                russelweights[source][dest]=ccost-u[source]-v[dest];
//
//                if ( russelweights[source][dest]<maxdiff ) {
//                    maxdiff=russelweights[source][dest];
//                    rmax=source;
//                    cmax=dest;
//                    }
//                }
//            }
//        /* FINE AGGIORNAMENTO ============================================= */

        /* NO AGGIORNAMENTO SOLO RICERCA DEL MASSIMO ====================== */
        maxdiff=INT_MAX;
        rmax=-1;
        cmax=-1;
        for ( source=0; source<nsources; source++ ) {
            if ( supply[source]==-1 )
                continue;

            for ( dest=0; dest<ndest; dest++ ) {
                if ( demand[dest]==-1 )
                    continue;

//                if ( russelmatrix[source][dest]!=-1 )
//                    continue;

//                ccost=russelcosts[source][dest];
//                russelweights[source][dest]=ccost-u[source]-v[dest];

                if ( russelweights[source][dest]<maxdiff ) {
                    maxdiff=russelweights[source][dest];
                    rmax=source;
                    cmax=dest;
                    }
                }
            }
        /* FINE AGGIORNAMENTO ============================================= */
        }













        }





















}
