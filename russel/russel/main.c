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


//#define _debug

//#define _debugVerbose             /* stampa info cu cio' che sta facendo  */
//#define _debugPrintCostsMatrix    /* stampa la matrice dei costi per VAM  */
//#define _debugPrintStepByStep     /* stampa matrice assegnazioni each time*/
//#define _debugPrintStepByStepModi     /* stampa matrice MODI each time*/
//#define _debugPrintAssignments    /* verbose info sulle assegnazioni      */


#define min(A, B) ((A) < (B) ? A : B)
#define max(A, B) ((A) < (B) ? B : A)

void allocBase(int **tasks,
               int *****costs,
               int ****pos,
               int ncelle,
               int nusers,
               int nintervalli);

void readFile(int **tasks,
              int *****costs,
              int ****pos,
              char* file);

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

int isFeasible(int*** russelmatrix,
               int**** pos,
               int** tasks,
               int** sourcesidx,
               int** destsidx,
               int nsources,
               int ndest,
               int nusers,
               int time);



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
    int time;           /* indice per il tempo                              */
    int cella;          /* indice per la cella                              */
    int i, j, k;        /* indici vari utili                                */

    int flag;           /* useful boolean flag                              */

    float obj=0;

    /*                                                                      **
    ** ... per AVM                                                          **
    **                                                                      */
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
//    int** modi;

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
    allocBase(&tasks, &costs, &pos,
              ncelle, nusers, nintervalli);

#ifdef _debugVerbose
    fprintf(stderr, "Numero di celle:      %4d\n", ncelle);
    fprintf(stderr, "Numero di users:      %4d\n", nusers);
    fprintf(stderr, "Numero di intervalli: %4d\n", nintervalli);
#endif // _debugVerbose

    /* //////////////////////////////////////////////////////////////////// **
    ** Allocazione e preparazione della struttura dati per Russel           **
    ** //////////////////////////////////////////////////////////////////// */
    allocRussell(&supply, &demand, &sourcesidx, &destsidx, &u, &v,
              &russelcosts, &russelmatrix, &russelweights,
              nusers, ncelle);

    /* //////////////////////////////////////////////////////////////////// **
    ** Lettura dei dati dal file                                            **
    ** //////////////////////////////////////////////////////////////////// */
    readFile(&tasks, &costs, &pos, argv[1]);

#ifdef _debugVerbose
    fprintf(stderr, "Caricamento dei dati completato.\n");
#endif // _debugVerbose

    int kkk=0;
    for ( kkk=10000; kkk<1000000; kkk+=1000 ) {
    /* //////////////////////////////////////////////////////////////////// **
    ** Inizio elaborazione                                                  **
    ** //////////////////////////////////////////////////////////////////// */
    start=clock();

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
            if ( russelmatrix[source][dest]!=-1 )
                continue;

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

    itera=ndest;
    while ( itera>0 ) {
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
            user = (source % nusers);
            k = demand[dest] % (user+1);

            if ( k != 0 ) {
                assignment=demand[dest]-k;
                flag=true;
                #ifdef _debugVerbose
                fprintf(stderr, "NOTA ASSEGNAZIONE: eseguiro' aggiustamenti\n");
                #endif // _debugVerbose
                }
            else
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
        int cbc;    /* current best cost                                    */
        if ( flag ) {

            /* CURIOSITA': se mi trovo nell'ultima colonna puo'     **
            ** capitare che mi ritrovi a spostare tasks da una      **
            ** cella a se stessa, essendo che la riga corrente e    **
            ** la riga a costo minore per l'ultima destinazione     **
            ** sono la stessa.                                      **
            ** Per risolvere, semplicemente gli do i task che       **
            ** mancano senza nemmeno scomodare le sorgenti          */
            if ( dest==ndest-1 ) {
//                user = source % nusers;
//
//                russelmatrix[source][dest]  += user+1;
//                supply[source]              -= user+1;
                demand[dest]                -= k;

                #ifdef _debugVerbose
                fprintf(stderr, "(%4d,%4d) Aggiusto peggio: assegno %4d invece di %2d (perche' dummy-destination)\n",
                        source, dest, user+1, k);
                fprintf(stderr, "[assegnamento virtuale]\n");
                #endif // _debugVerbose
                }

            else {
                /* cerco di trovare una sorgente che abbia ancora           **
                ** fornitura e tra quelle disponibili scelgo quella a costo **
                ** minore                                                   */
                cmax=-1;
                for ( i=0; i<nsources; i++ ) {
                    if ( supply[i]==-1 )
                        continue;

                    if ( supply[i]>=k ) {
                        user  = (i%nusers);

                        if ( (user+1)<=k ) {
                            flag=k%(user+1);

                            if ( flag==0 ) {
                                if ( cmax==-1 )
                                    cmax=i;
                                else {
                                    ccost = russelcosts[i][dest]*(user+1);
                                    cbc   = russelcosts[cmax][dest]*((cmax%nusers)+1);
                                    if ( ccost<=cbc )
                                        cmax=i;
                                    }
                                }
                            }
                        }
                    }

                /* se non ho trovato nessuna sorgente disponibile con un        **
                ** adeguato numero di tasks                                     */
                if ( cmax==-1 ) {
                    /* dato che i task sono contati e assegnando                **
                    ** in modo peggiorativo (piu' task del dovuto) poi ottengo  **
                    ** soluzioni non fattibili perche' mancano i task per le    **
                    ** altre destinazioni, non mi resta che togliere task dalla **
                    ** dummy destination e aggiungerli qui' che c'e' piu'       **
                    ** bisogno.                                                 */
                    for ( i=0; i<nsources; i++ ) {
                        /* se nell'ultima colonna ci sono meno di k tasks       **
                        ** disponibili la zompo                                 */
                        if ( russelmatrix[i][ndest-1]<k )
                            continue;

                        user= (i%nusers);
                        if ( (user+1)<=k ) {
                            flag=k%(user+1);

                            if ( flag==0 ) {
                                if ( cmax==-1 )
                                    cmax=i;
                                else {
                                    ccost=russelcosts[i][dest]*(user+1);
                                    cbc  =russelcosts[cmax][dest]*((cmax%nusers)+1);
                                    if ( cost<cbc )
                                        cmax=i;
                                    }
                                }
                            }
                        }
                    user=cmax%nusers;
                    assignment=k;

                    if ( russelmatrix[cmax][dest]==-1 )
                        russelmatrix[cmax][dest]  = assignment;
                    else
                        russelmatrix[cmax][dest] += assignment;

                    russelmatrix[cmax][ndest-1]  -= assignment;

                    /* non c'e' bisogno che modifico la supply della riga       **
                    ** ho solamente spostato i task dalla dummy-destination     */
    //                supply[cmax] -= assignment;
                    demand[dest] -= assignment;
                    if ( demand[ndest-1]==-1 ) {
                        demand[ndest-1]  =assignment;
                        itera++;
                        }
                    else
                        demand[ndest-1] +=assignment;

                    #ifdef _debugVerbose
                    fprintf(stderr,
                            "(%4d,%4d) Aggiusto meglio: assegno %4d tasks [user type %2d] (tolto all'ultima colonna)\n",
                            cmax, dest, assignment, cmax%nusers+1);
                    #endif // _debugVerbose
                    }
                else {
                    user = cmax % nusers;
                    assignment=k;

                    if ( russelmatrix[cmax][dest]==-1 )
                        russelmatrix[cmax][dest]  = assignment;
                    else
                        russelmatrix[cmax][dest] += assignment;

                    supply[cmax] -= assignment;
                    demand[dest] -= assignment;

                    #ifdef _debugVerbose
                    fprintf(stderr,
                            "(%4d,%4d) Aggiusto meglio: assegno %4d tasks [user type %2d]\n",
                            cmax, dest, assignment, cmax%nusers+1);
                    #endif // _debugVerbose
                    }
                }
            }


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
                itera--;
                }
            }
        else
            if ( demand[dest]==0 ) {
                demand[dest]    =-1;
                v[dest]       =-1;
                itera--;
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

    /* ==================================================================== */
    /* -------------------------------------------------------------------- */
    /* ==================================================================== */
    /*                              MODI                                    */
    /* ==================================================================== */
    /* ==================================================================== */

//    /* matrice dei pesi                                                     */
//    modi=(int**) malloc(nsources*sizeof(int*));
//    assert (modi);
//
//    /* inizializzazione matrici costi e assegnazioni                        */
//    for ( i=0; i<nsources; i++ ) {
//        modi[i] =(int*) malloc(ndest*sizeof(int));
//        assert ( modi[i] );
//        for ( j=0; j<ndest; j++ )
//            modi[i][j]=-1;
//        }
//
//    int nx=0;
//    for ( source=0; source<nsources; source++ )
//        for ( dest=0; dest<ndest; dest++ )
//            if ( russelmatrix[source][dest]!=-1 ) {
//                nx++;
//                modi[source][dest]=0;
//                }
//
//
//    dest=ndest-1;
//    v[dest]=0;
//    for ( source=0; source<nsources; source++ )
//        if ( russelmatrix[source][dest]!=-1 ) {
//            u[source]=russelcosts[source][dest]-v[dest];
//            nx--;
//            }
//
//
//
//    while ( nx>0 ) {
////        fprintf(stderr, "nx: %d\n", nx);
//        for ( source=0; source<nsources; source++ )
//            for ( dest=0; dest<ndest; dest++ )
//                if ( russelmatrix[source][dest]!=-1 ) {
//                    if ( v[dest]==-1 && u[source]!=-1 ) {
//                        v[dest]=russelcosts[source][dest]-u[source];
//                        nx--;
//                        }
//                    else
//                        if ( v[dest]!=-1 && u[source]==-1 ) {
//                            u[source]=russelcosts[source][dest]-v[source];
//                            nx--;
//                            }
//
//                    if ( v[dest]!=-1 && u[source]!=-1 )
//                        nx--;
//                    }



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
        //}
//
//    rmax=-1;
//    cmax=-1;
//    maxdiff=INT_MAX;
//    for ( source=0; source<nsources; source++ )
//        for ( dest=0; dest<ndest; dest++ )
//            if ( u[source]+v[dest]!=russelcosts[source][dest] ) {
//                modi[source][dest]=russelcosts[source][dest]-u[source]-v[dest];
//
//                if ( modi[source][dest]<maxdiff ) {
//                    maxdiff=modi[source][dest];
//                    rmax=source;
//                    cmax=dest;
//                    }
//                }

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

//    fprintf(stderr, "BEST IMPROVEMENT %d, row %4d column %4d\n\n", maxdiff, rmax, cmax);



    #ifdef _debugPrintStepByStep
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
    #endif // _debugPrintStepByStep

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
                    k =(int) ceil((float)  russelmatrix[source][dest] / (user+1));
                    obj += k * costs[i][j][user][time];
                    }

//        printf("Istanza %s.\n", argv[1]);
//        printf("Tempo: %f\n", ((double)end-start)/CLOCKS_PER_SEC);
//        printf("Costo: %f\n", obj);
//        printf("ITERAZIONE;Istanza;OF;Time\n");

        printf("%s;%d;%d;%f\n",basename(argv[1]), kkk,
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
    }   // for ( kkk ... )








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
    free(v);

    for ( source=0; source<nsources; source++ ) {
        free(russelcosts[source]);
        free(russelmatrix[source]);
        free(russelweights[source]);
//        free(modi[source]);
        }
    free(russelcosts);
    free(russelmatrix);
    free(russelweights);
//    free(modi);

    return 0;

}

int isFeasible(int*** russelmatrix,
               int**** pos,
               int** tasks,
               int** sourcesidx,
               int** destsidx,
               int nsources,
               int ndest,
               int nusers,
               int time) {

    int source, dest, user;
    int i, j;
    float k;
    float rowsum;
    int colsum;
    int flag;

    flag=true;
    /* per ogni sorgente (per ogni tipo di user) la somma degli utsers      **
    ** spostati deve essere inveriore alla disponibilita'                   */
    for ( source=0; source < nsources; source++ ) {
        rowsum=0;
        user=source%nusers;
        i = (*sourcesidx)[source];

        for ( dest=0; dest<ndest-1; dest++ )
            if ( (*russelmatrix)[source][dest]!=-1 ) {
                k = (float) (*russelmatrix)[source][dest] / (user+1);

                if ( ceil(k)!=floor(k) )
                    fprintf(stderr, "TASKS ERROR: (%3d,%3d) moved %4.2f users\n",
                            i, (*destsidx)[dest], k);

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
            fprintf(stderr, "FEASIBILITY ERROR: colonna %d (%d), eseguo %d tasks invece di %d\n",
                   j, dest, colsum, (*tasks)[j]);
            flag=false;
            }
        }

    return flag;
}


void allocBase(int **tasks,
               int *****costs,
               int ****pos,
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
                  int** u,
                  int** v,
                  int*** russelcosts,
                  int*** russelmatrix,
                  int*** russelweights,
                  int nusers,
                  int ncelle) {

    int i, j;
    int nsources=nusers*ncelle; /* numero massimo di sorgenti               */
    int ndests=ncelle;          /* numero massimo di destinazioni           */

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
        (*u)[i]=-1;
        }

    for ( i=0; i<ndests; i++ ) {
        (*demand)[i]=-1;
        (*destsidx)[i]=-1;
        (*v)[i]=-1;
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

void readFile(int **tasks,
              int *****costs,
              int ****pos,
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
        fscanf (f, "%d", &k);

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
