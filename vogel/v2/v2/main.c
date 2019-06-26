#include <assert.h>
#include <math.h>
#include <tgmath.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>


#define true  1
#define false 0


//#define _debug

//#define _debugVerbose             /* stampa info cu cio' che sta facendo  */
//#define _debugPrintCostsMatrix    /* stampa la matrice dei costi per VAM  */
//#define _debugPrintStepByStep     /* stampa matrice assegnazioni each time*/
//#define _debugPrintAssignments    /* verbose info sulle assegnazioni      */


//#define tabuMax 10000

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

void allocVogel(int** rowdiff,
                int** coldiff,
                int** supply,
                int** demand,
                int** sourcesidx,
                int** destsidx,
                int** pmr,
                int** psmr,
                int** pmc,
                int** psmc,
                int*** vogelscosts,
                int*** vogelsmatrix,
                int nusers,
                int ncelle);

int isFeasible(int*** vogelsmatrix,
               int**** pos,
               int** tasks,
               int** sourcesidx,
               int** destsidx,
               int nsources,
               int ndest,
               int nusers,
               int time);


//void addTabu(int source, int dest);
//int isTabu(int source, int dest);
//
//struct s_move{
//    int s;
//    int d;
//};
//
//struct s_move moves[tabuMax];
//int nTabus=0;

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

    int* rowdiff;       /* differenze per le righe (sorgenti)               */
    int* coldiff;       /* differenze per le colonne                        */

    int* supply;        /* vettore delle forniture (da sorgenti)            */
    int* demand;        /* vettore delle richieste (per destinazioni)       */

    int* sourcesidx;    /* indice delle sorgenti nella matrice originale    */
    int* destsidx;      /* indice delle destinazioni nella matrice originale*/

    int** vogelscosts;  /* matrice di Vogel con i costi                     */
    int** vogelsmatrix; /* matrice di Vogel con le soluzioni                */

    int* pmr;           /* vettore con le posizioni dei minimi nelle righe  */
    int* psmr;          /* vettore con le pos. dei secondi minimi per righe */
    int* pmc;           /* vettore con le posizioni dei minimi nelle colonne*/
    int* psmc;          /* vettore con le pos. secondi minimi per colonne   */

    float cost;         /* variabile ausiliaria per il costo                */
    int costi;          /* variabile ausiliaria per il costo                */

    int itera;          /* variabile per le m+n-1 iterazioni del VAM        */
    int cmin;           /* variabile aus. per il costo min nella colonna    */
    int rmin;           /* variabile aus. per il costo min nella riga       */

    int maxdiff;        /* var. aus. per trovare la _differenza_ maggiore   */
    int imaxdiff;       /* indice della differenza maggiore                 */

    int flagrow;        /* se true la differenza maggiore appartiene a riga */
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
    ** Allocazione e preparazione della struttura dati per Vogel            **
    ** //////////////////////////////////////////////////////////////////// */
    allocVogel(&rowdiff, &coldiff,
               &supply, &demand,
               &sourcesidx, &destsidx,
               &pmr, &psmr, &pmc, &psmc,
               &vogelscosts, &vogelsmatrix,
               nusers, ncelle);


    /* //////////////////////////////////////////////////////////////////// **
    ** Lettura dei dati dal file                                            **
    ** //////////////////////////////////////////////////////////////////// */
    readFile(&tasks, &costs, &pos, argv[1]);

#ifdef _debugVerbose
    fprintf(stderr, "Caricamento dei dati completato.\n");
#endif // _debugVerbose


    int kkk=0;
    for ( kkk=0; kkk<5000; kkk++ ) {
    /* //////////////////////////////////////////////////////////////////// **
    ** Inizio elaborazione                                                  **
    ** //////////////////////////////////////////////////////////////////// */
    start=clock();

    /*
    ** Vogel's approximation method
    ** ----------------------------
    **
    ** Per ogni colonna (e per ogni riga) si calcola la
    ** _differenza_ intesa come la differenza aritmetica tra i
    ** due costi piu' piccoli per tale colonna (riga).
    **
    ** Per la colonna (o la riga) con la differenza maggiore
    ** si seleziona come variabile di base la cella con il
    ** costo minore per quella colonna (o riga).
    **
    ** In quella variabile si allocano le risorse disponibili
    ** in base ai constraints di riga o di colonna*.
    **
    ** Se dopo aver definito una varibile i constraints di
    ** riga sono a 0, si elimina la riga, se i constraints di
    ** colonna sono a zero, si elimina la colona. Quando rimane
    ** una sola cella disponibile si allocano 'tutte cose'.
    **
    ** *i constraints di riga indicano la fornitura totale
    **  possibile per la sorgente i-esima; i constriants di
    **  colonna indicano la domanda totale per la sink j-esima
    **
    **
    ** Senza badare ad ottimizzazioni si usano le seguenti
    ** strutture:
    **  - demand[n]     richieste per ogni destinazione ( d_ )
//    **  - acolumns[n]   destinazioni disponibili
    **  - supply[m]     fornitura da ogni sorgente ( s_ )
//    **  - arows[m]      sorgenti disponibili
    ** con m righe e n colonne.
    **
    **     i\j  1   2   3   ...   n  |
    **                               |
    **      1                        | s1
    **      2                        | s2
    **      3                        | s3
    **     ...                       | ...
    **      m                        | sm
    **     -------------------------------
    **          d1  d2  d3        dn |
    **
    **
    ** acolumns e arows sono i vettori che definiscono se
    ** le colonne o le righe sono ancora disponibili:
    **  - se il valore e' -1 la colona/riga e' completa
    **  - altrimenti il valore corrispondere alla
    **    differenza per quella colonna/riga
    */

    /* ==================================================================== */
    /* ==================================================================== */

    time=0; // TODO: togliere il time=0 quando si hanno piu' intervalli
            //       di tempo
    /* NOTE:
    **   si imposta il tempo a 0 ma la procedura
    **   va ripetuta per tutti gli istanti di tempo                         */

    /* ==================================================================== */    /* ==================================================================== */


    /*                                                                      **
    **  1. INIZIALIZZA SORGENTI E DI DESTINAZIONI                           **
    **                                                                      */
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







//    /* -------------------------------------------------------------------- */
//    /* SORGENTI                                                             */
//    /* -------------------------------------------------------------------- */

//
//
//    nsources=0;
//    for ( cella=0; cella<ncelle; cella++ )
//        for ( user=0; user<nusers; user++ )
//            if (pos[cella][user][time]) {
//                /* basta che uno solo dei vari tipi di users per la cella   **
//                ** sia diverso da zero                                      */
//                nsources++;
//                break;
//                }
//    nsources=nsources*nusers;
//
//    /* DESTINAZIONI                                                         */
//    ndest=0;
//    for ( cella=0; cella<ncelle; cella++ )
//        if (tasks[cella])
//            ndest++;
//    ndest++;    /* la destinazione dummy                                    */
//    /*                                                                      **
//    **  2. INIZIALIZZA LE DESTINAZIONI E LE SORGENTI                        **
//    **                                                                      */
//
//    /* -------------------------------------------------------------------- */
//    /* DESTINAZIONI                                                         */
//    /* -------------------------------------------------------------------- */
//
//
//
//    i=0;
//    totdemand=0;
//    for ( dest=0; dest<ndest-1; dest++ ) {
//        /* salta le destinazioni che non hanno tasks da eseguire            */
//        while ( tasks[i]==0 ) {
//            i++;
//            if ( i>=ncelle ) break;
//            }
//
//        if ( i>=ncelle ) continue;
//
//        demand[dest]=tasks[i]; /* imposta la domanda                        */
//        totdemand += tasks[i]; /* incrementa il numero totale di tasks      */
//        destsidx[dest]=i;      /* salva la corrispondenza per la dest.      */
//        i++;
//        #ifdef _debug
//        printf("Nuova destinazione trovata: %4d [%4d] -> %4d.\n",
//               dest, demand[dest], destsidx[dest]);
//        #endif // _debug
//        }
//
//    #ifdef _debug
//    printf("Ho inizializzato le destinazioni.\n");
//    #endif // _debug
//
//    /* -------------------------------------------------------------------- */
//    /* SORGENTI                                                             */
//    /* -------------------------------------------------------------------- */
//
//    #ifdef _debug
//    printf("Cerco le sorgenti... sorg [tipo user] [n. users] [tasks] -> sorg orig.\n");
//    #endif // _debug
//
//    time=0; // TODO: togliere il time=0 quando si hanno piu' intervalli di tempo
//
//
//    i=0;
//    totsupply=0;
//    source=0;
//    /* in questo contesto il flag e' true se nella cella corrente almeno un **
//    ** tipo di users e' diverso da 0                                        */
//    flag=0;
//    for ( cella=0; cella<ncelle; cella++ ) {
//        flag=0;
//        for ( user=0; user<nusers; user++ )
//            if ( pos[cella][user][time] != 0 ) {
//                flag=1;
//                break;
//            }
//
//        if ( flag ) {
//            /* Riempi le prossime nusers sources con i task disponibili.    **
//            ** NB: considero i tasks non gli utenti                         */
//            for ( user=0; user<nusers; user++ ) {
//                supply[source+user]=pos[cella][user][time]*(user+1);
//                totsupply += pos[cella][user][time]*(user+1);
//                sourcesidx[source+user] = cella;
//                #ifdef _debug
//                printf("Nuova sorgente trovata: %4d [%4d] [%4d] [%4d] -> %4d.\n",
//                       source+user, user, pos[cella][user][time],
//                       supply[source+user], sourcesidx[source]);
//                #endif // _debug
//            }
//            /* avanza di nusers sorgenti                                    */
//            source += nusers;
//        }
//    }
//
//    /* calcola la differenza tra domanda e fornitura e assegnala alla       **
//    ** dummy-destination                                                    */
//    if ( totsupply-totdemand > 0 )
//        demand[ndest-1]=totsupply-totdemand;
//    else
//        demand[ndest-1]=0;
//
//    #ifdef _debug
//    printf("Ho inizializzato le sorgenti. (supply %6d e demand %6d)\n",
//           totsupply, totdemand);
//    #endif // _debug
//
//    /* WARNING
//    **   qui' ancora si considera che se in una cella c'e' un tipo di utente
//    **   allora automaticamente quella cella contiene tutti e 3 i tipi di
//    **   utenti
//    */
//    for ( source=0; source<nsources; source++ ) {
//        /* salta le sorgenti che non hanno users da regalare                */
//        while ( pos[i][source%3][time]==0 ) i++;
//
//        /* considero i tasks non gli utenti */
//        supply[source]=pos[i][source%3][time]*(source%3+1);
//        totsupply+=pos[i][source%3][time]*(source%3+1);
//        sourcesidx[source]=i;
//
//
//
//        if ( source%3==2 )
//            i++;
//        }



    /*                                                                      **
    **  2. INIZIALIZZA COSTI E MATRICI DELLE VARIABILI                      **
    **                                                                      */
    for ( source=0; source<nsources; source++ ) {
        i=sourcesidx[source];
        user=source%3;
        k=user+1;

        for ( dest=0; dest<ndest-1; dest++ ) {
            j=destsidx[dest];

            cost=(float) costs[i][j][user][time] / k;
            vogelscosts[source][dest]=(int) (cost*100);

            vogelsmatrix[source][dest]=-1;
            }

        /* inizializza anche la colonna della dummy-destination             */
        vogelscosts[source][ndest-1]=kkk;
        vogelsmatrix[source][ndest-1]=-1;
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
            printf("%4d;", vogelscosts[i][j]);
        printf("\n");
    }
    printf("\n");
    printf("\n");
    #endif // _debugPrintCostsMatrix


    /* ==================================================================== **
    **  4. VOGEL'S APPROXIMATION METHOD                                     **
    ** ==================================================================== */
    #ifdef _debugVerbose
    fprintf(stderr, "Inizio elaborazione VAM::\n");
    #endif // _debugVerbose

    /* ---------------------------------------------------------------- **
    ** calcola le differenze e trova le posizioni dei minimi per ogni   **
    ** colonna e per ogni riga                                          **
    ** ---------------------------------------------------------------- */
    for ( source=0; source<nsources; source++ ) {

//  NON DOVREBBE ACCADERE PERCHE' E' LA PRIMA VOLTA CHE INIZIALIZZO
//        /* se la sorgente corrente non ha piu' forniture, la riga       **
//        ** corrispondente viene ignorata e non si calcola               **
//        ** piu' la differenza per tale riga                             */
//        if ( supply[source]==-1 )
//            continue;


//        /* non considero il costo della destinazione dummy              */
//        for ( dest=0; dest<ndest-1; dest++ ) {

        for ( dest=0; dest<ndest; dest++ ) {
//  NON DOVREBBE ACCADERE PERCHE' E' LA PRIMA VOLTA CHE INIZIALIZZO
//            /* se la destinazione corrente non ha piu' task da essere   **
//            ** eseguiti, viene ignorata e per colonna corrispondente    **
//            ** non si calcola piu' la differenza                        */
//            if ( demand[dest]==-1 )
//                continue;

            costi=vogelscosts[source][dest];
            #ifdef _debugVerbose
            fprintf(stderr, "Sorgente %4d - destinazione %4d => costo %4d\n",
                   source, dest, costi);
            #endif // _debugVerbose

            /* gestione del MINIMO e del SECONDO MINIMO per le RIGHE    */
            /* -------------------------------------------------------- */
            /* se non esiste ancora un costo minimo per la RIGA lo si   **
            ** inizializza                                              */
            if ( pmr[source]==-1 )
                pmr[source]=dest;
            else {
                /* se esiste gia' un minimo e il nuovo costo e' minore  **
                ** del minimo allora gli si sostituisce e l'attuale     **
                ** minimo diventa secondo minimo                        */
                rmin=vogelscosts[source][pmr[source]];
                if ( costi<=rmin ) {
//                    /* nel vettore delle differenze, durante le         **
//                    ** iterazioni di inizializzazione viene salvato la  **
//                    ** posizione del secondo minimo nella riga          */
//                    rowdiff[source]=pmr[source];
                    psmr[source]=pmr[source];
                    pmr[source]=dest;
                    }
                else
                    /* se il costo non e' minore del minimo,            **
                    ** allora potrebbe essere minore del secondo        **
                    ** minimo:                                          **
                    **  - se questo non fosse inizializzato lo si       **
                    **    inizializza;                                  **
                    **  - se invece questo fosse maggiore del costo     **
                    **    corrente lo si sostituisce;                   */
                    if ( psmr[source]==-1 )
                        psmr[source]=dest;
//                    if ( rowdiff[source]==-1 )
//                        rowdiff[source]=dest;
                    else {
//                        rmin=vogelscosts[source][rowdiff[source]];
                        rmin=vogelscosts[source][psmr[source]];
                        if ( costi<rmin )
                            psmr[source]=dest;
//                            rowdiff[source]=dest;
                        }
                }


            /* gestione del MINIMO e del SECONDO MINIMO per le COLONNE  */
            /* -------------------------------------------------------- */
            /* se non esiste ancora un costo minimo per la COLONNA lo   **
            ** si inizializza                                           */
            if ( pmc[dest]==-1 )
                pmc[dest]=source;
            else {
                /* se esiste gia' un minimo e il nuovo costo e' minore  **
                ** del minimo allora gli si sostituisce                 */
                cmin=vogelscosts[pmc[dest]][dest];
                if ( costi<=cmin ) {
//                    /* nel vettore delle differenze, durante le         **
//                    ** iterazioni di inizializzazione viene salvata la  **
//                    ** poszione del secondo minimo nella colonna        */
//                    coldiff[dest]=pmc[dest];
                    psmc[dest]=pmc[dest];
                    pmc[dest]=source;
                    }
                else
                    if ( psmc[dest]==-1 )
                        psmc[dest]=source;
//                    if ( coldiff[dest]==-1 )
//                        coldiff[dest]=source;
                    else {
//                        cmin=vogelscosts[coldiff[dest]][dest];
                        cmin=vogelscosts[psmc[dest]][dest];
                        if ( costi<cmin )
                            psmc[dest]=source;
//                            coldiff[dest]=source;
                        }
                }
            }
        }



    /* calcola le differenze per ogni colonna e per ogni riga               */
    #ifdef _debugVerbose
    fprintf(stderr, "Calcolo le differenze per le righe:\n");
    #endif // _debugVerbose
    for ( source=0; source<nsources; source++ ) {
        cmin = vogelscosts[source][pmr[source]];
        rmin = vogelscosts[source][psmr[source]];
        rowdiff[source] = rmin - cmin;
        #ifdef _debugVerbose
        fprintf(stderr, "Riga %4d: differenza %4d\n",
               source, rowdiff[source]);
        #endif // _debugVerbose
        }

    #ifdef _debugVerbose
    fprintf(stderr, "Calcolo le differenze per le colonne:\n");
    #endif // _debugVerbose
    for ( dest=0; dest<ndest; dest++ ) {
        cmin = vogelscosts[pmc[dest]][dest];
        rmin = vogelscosts[psmc[dest]][dest];
        coldiff[dest] = rmin - cmin;
        #ifdef _debugVerbose
        fprintf(stderr, "Colonna %4d: differenza %4d\n",
               dest, coldiff[dest]);
        #endif // _debugVerbose
        }



//    itera=nsources+ndest-1;
    itera=ndest;
    while ( itera>0 ) {
//        itera--;

         /* stampa mosse iterazione corrente per debug                       */
        #ifdef _debugPrintStepByStep
        fprintf(stderr, "=================================================================================================================\n");
        fprintf(stderr, "=================================================================================================================\n");
        fprintf(stderr, " PRIMA DELL'ASSEGNAZIONE\n");
        fprintf(stderr, "=================================================================================================================\n");

        fprintf(stderr, " [%2d] ;", itera);
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", j);
        fprintf(stderr, " supply;");
        fprintf(stderr, " diff;");
        fprintf(stderr, " min\n");

        for (i=0; i<nsources; i++) {
            fprintf(stderr, "  %4d;", i);
            for (j=0; j<ndest; j++) {
                if (vogelsmatrix[i][j]!=-1)
                    fprintf(stderr, "%4d;", vogelsmatrix[i][j]);
                else
                    fprintf(stderr, "    ;");
            }
            fprintf(stderr, "   %4d;", supply[i]);
            fprintf(stderr, " %4d;", rowdiff[i]);
            fprintf(stderr, "%4d;", pmr[i]);
            fprintf(stderr, "\n");
        }

        fprintf(stderr, "demand;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", demand[j]);
        fprintf(stderr, "\n");
        fprintf(stderr, "diff  ;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", coldiff[j]);
        fprintf(stderr, "\n");
        fprintf(stderr, "min   ;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", pmc[j]);
        fprintf(stderr, "\n");
        fprintf(stderr, "\n");
        #endif // _debugPrintStepByStep



        /* ---------------------------------------------------------------- */
        /* cerca la differenza massima per ogni riga e per ogni colonna     */
        /* ---------------------------------------------------------------- */
        flagrow=true;
        maxdiff=-1;
        imaxdiff=-1;
        for ( source=0; source<nsources; source++ ) {
            /* ignora la riga se e' stata completata */
            if ( supply[source]==-1 )
                continue;

            if ( rowdiff[source]>maxdiff ) {
                maxdiff=rowdiff[source];
                imaxdiff=source;
                #ifdef _debugVerbose
                fprintf(stderr, "Nuova maxdiff per la riga %4d: %4d\n",
                       source, maxdiff);
                #endif // _debugVerbose
                }

            if ( rowdiff[source]==maxdiff &&
                 rowdiff[source]!=-1         ) {
                /* costo minimo per l'attuale differenza massima            */
                cmin=vogelscosts[imaxdiff][pmr[imaxdiff]];
                /* costo minimo per l'attuale riga con la stessa differenza **
                ** massima                                                  */
                rmin=vogelscosts[source][pmr[source]];

                if ( rmin<cmin ) {
                    maxdiff=rowdiff[source];
                    imaxdiff=source;
                    #ifdef _debugVerbose
                    fprintf(stderr, "Nuova maxdiff per la riga %4d: %4d (perche' la riga ha un minimo minore)\n",
                           source, maxdiff);
                    #endif // _debugVerbose
                    }
                }
            }

        /* niente piu' sorgenti, cosa vado avanti a fare?                   */
        if ( maxdiff==-1 ) {
            itera--;
            continue;
            }


        for ( dest=0; dest<ndest; dest++ ) {
            /* ignora la colonna se e' stata completata */
            if ( demand[dest]==-1 )
                continue;

            if ( coldiff[dest]==-1 )
                continue;

            if ( coldiff[dest]>maxdiff ){
                maxdiff=coldiff[dest];
                imaxdiff=dest;
                flagrow=false;
                #ifdef _debugVerbose
                fprintf(stderr, "Nuova maxdiff per la colonna %4d: %4d.\n",
                       dest, maxdiff);
                #endif // _debugVerbose
                }

            if ( coldiff[dest]==maxdiff ){
                if ( flagrow ) {
                    cmin=vogelscosts[imaxdiff][pmr[imaxdiff]];
                    rmin=vogelscosts[pmc[dest]][dest];
                    if ( rmin<cmin ) {
                        maxdiff=coldiff[dest];
                        imaxdiff=dest;
                        flagrow=false;
                        #ifdef _debugVerbose
                        fprintf(stderr, "Nuova maxdiff per la colonna %4d: %4d (perche' la colonna ha un minimo minore)\n",
                               dest, maxdiff);
                        #endif // _debugVerbose
                        }
                    }
                else {
                    cmin=vogelscosts[pmc[imaxdiff]][imaxdiff];
                    rmin=vogelscosts[pmc[dest]][dest];

                    if ( rmin<cmin ) {
                        maxdiff=coldiff[dest];
                        imaxdiff=dest;
                        flagrow=false;
                        #ifdef _debugVerbose
                        fprintf(stderr, "Nuova maxdiff per la colonna %4d: %4d (perche' la colonna ha un minimo minore)\n",
                               dest, maxdiff);
                        #endif // _debugVerbose
                        }
                    }
                }
            }


        /* ---------------------------------------------------------------- */
        /* ASSEGNAZIONE                                                     */
        /* ---------------------------------------------------------------- */
        #ifdef _debugVerbose
        fprintf(stderr, "Eseguo l'assegnazione (destinazioni rimaste: %4d):\n", itera);
        #endif // _debugVerbose



        if ( flagrow ) {
            source    =imaxdiff;
            dest      =pmr[source];

            #ifdef _debugVerbose
            fprintf(stderr, "maxdiff di riga\n");
            fprintf(stderr, "riga %4d - colonna %4d\n",
                   source, dest);
            #endif // _debugVerbose
            }
        else {
            dest    =imaxdiff;
            source  =pmc[dest];

            #ifdef _debugVerbose
            fprintf(stderr, "maxdiff di colonna\n");
            fprintf(stderr, "riga %4d - colonna %4d\n",
                   source, dest);
            #endif // _debugVerbose
            }


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
        if ( demand[dest] < supply[source] ) {
            user = (source % nusers);
            k = demand[dest] % (user+1);

            if ( k != 0 ) {
                assignment=demand[dest]-k;
                flag=true;
                #ifdef _debugVerbose
                fprintf(stderr, "NOTA ASSEGNAZIONE: eseguiro' aggiustamenti\n");
                #endif
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

        if ( vogelsmatrix[source][dest]==-1 )
            vogelsmatrix[source][dest]=assignment;
        else
            vogelsmatrix[source][dest]+=assignment;

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
//                vogelsmatrix[source][dest]  += user+1;
//                supply[source]              -= user+1;
                demand[dest]                -= k;
                #ifdef _debugVerbose
                fprintf(stderr, "(%4d,%4d) Aggiusto peggio: assegno %4d invece di %2d (perche' dummy-destination)\n",
                        source, dest, user+1, k);
                fprintf(stderr, "[assegnamento virtuale]\n");
                #endif
                }

            else {
                cmin=-1;
                for ( i=0; i<nsources; i++ ) {
                    if ( supply[i]==-1 )
                        continue;

                    if ( supply[i]>=k ) {
                        user  = (i%nusers);

                        if ( (user+1)<=k ) {
                            flag=k%(user+1);

                            if ( flag==0 ) {
                                if ( cmin==-1 )
                                    cmin=i;
                                else {
                                    costi = vogelscosts[i][dest]*(user+1);
                                    cbc   = vogelscosts[cmin][dest]*((cmin%nusers)+1);
                                    if ( costi<=cbc )
                                        cmin=i;
                                    }
                                }
                            }

    //                    if ( (user+1)>k )
    //                        flag=(user+1)%k;
    //                    else
    //                        flag=k%(user+1);
    //
    //                    if ( flag==0 ) {
    //                        if ( cmin==-1 )
    //                            cmin=i;
    //                        else    {
    //                            costi = vogelscosts[i][dest]*(user+1);
    //                            cbc   = vogelscosts[cmin][dest]*((cmin%nusers)+1);
    //                            if ( costi<=cbc )
    //                                cmin=i;
    //                            }
    //                        }
                        }
                    }

                if ( cmin==-1 ) {
                    /* dato che non ho trovato una sorgente adatta tra quelle   **
                    ** disponibili, dato che i task sono contati e assegnando   **
                    ** in modo peggiorativo (piu' task del dovuto) poi ottengo  **
                    ** soluzioni non fattibili perche' mancano i task per le    **
                    ** altre destinazioni, non mi resta che togliere task dalla **
                    ** dummy destination e aggiungerli qui' che c'e' piu'       **
                    ** bisogno.                                                 */
                    for ( i=0; i<nsources; i++ ) {
                        /* se nell'ultima colonna ci sono meno di k tasks       **
                        ** disponibili la zompo                                 */
                        if ( vogelsmatrix[i][ndest-1]<k )
                            continue;

                        user= (i%nusers);
                        if ( (user+1)<=k ) {
                            flag=k%(user+1);

                            if ( flag==0 ) {
                                if ( cmin==-1 )
                                    cmin=i;
                                else {
                                    costi=vogelscosts[i][dest]*(user+1);
                                    cbc  =vogelscosts[cmin][dest]*((cmin%nusers)+1);
                                    if ( cost<cbc )
                                        cmin=i;
                                    }
                                }
                            }
                        }
                    user=cmin%nusers;
                    assignment=k;

                    if ( vogelsmatrix[cmin][dest]==-1 )
                        vogelsmatrix[cmin][dest]  = assignment;
                    else
                        vogelsmatrix[cmin][dest] += assignment;

                    vogelsmatrix[cmin][ndest-1]  -= assignment;

                    /* non c'e' bisogno che modifico la supply della riga       **
                    ** ho solamente spostato i task dalla dummy-destination     */
    //                supply[cmin] -= assignment;
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
                            cmin, dest, assignment, cmin%nusers+1);
                    #endif
                    }
                else {
                    user = cmin % nusers;
                    assignment=k;

                    if ( vogelsmatrix[cmin][dest]==-1 )
                        vogelsmatrix[cmin][dest] = assignment;
                    else
                        vogelsmatrix[cmin][dest] += assignment;

                    supply[cmin] -= assignment;
                    demand[dest] -= assignment;

                    #ifdef _debugVerbose
                    fprintf(stderr,
                            "(%4d,%4d) Aggiusto meglio: assegno %4d tasks [user type %2d]\n",
                            cmin, dest, assignment, cmin%nusers+1);
                    #endif
                    }
                }
            }


/* ======================================================================== *//* ======================================================================== */






        /* situazione degenere */
        if ( (supply[source]==0 && demand[dest]==0) || supply[source]==0 ) {
            supply[source]  =-1;
            rowdiff[source] =-1;
            pmr[source]     =-1;
            psmr[source]    =-1;

            if ( itera==1 && demand[dest]==0 ) {
                demand[dest]    =-1;
                coldiff[dest]   =-1;
                pmc[dest]       =-1;
                psmc[dest]      =-1;
                itera--;
                }
            }
        else
            if ( demand[dest]==0 ) {
                demand[dest]    =-1;
                coldiff[dest]   =-1;
                pmc[dest]       =-1;
                psmc[dest]      =-1;
                itera--;
                }
//            if ( supply[source]==0 )
//                supply[source]=-1;
//            else
//                if ( demand[dest]==0 )
//                    demand[dest]=-1;


        /* stampa mosse iterazione corrente per debug                       */
         #ifdef _debugPrintStepByStep
        fprintf(stderr, "=================================================================================================================\n");
        fprintf(stderr, " DOPO DELL'ASSEGNAZIONE\n");
        fprintf(stderr, "=================================================================================================================\n");

        fprintf(stderr, " [%2d] ;", itera);
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", j);
        fprintf(stderr, "supply;");
        fprintf(stderr, "diff;");
        fprintf(stderr, "min\n");

        for (i=0; i<nsources; i++) {
            fprintf(stderr, "  %4d;", i);
            for (j=0; j<ndest; j++) {
                if (vogelsmatrix[i][j]!=-1)
                    fprintf(stderr, "%4d;", vogelsmatrix[i][j]);
                else
                    fprintf(stderr, "    ;");
            }
            fprintf(stderr, "  %4d;", supply[i]);
            fprintf(stderr, "%4d;", rowdiff[i]);
            fprintf(stderr, "%4d;", pmr[i]);
            fprintf(stderr, "\n");
        }

        fprintf(stderr, "demand;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", demand[j]);
        fprintf(stderr, "\n");
        fprintf(stderr, "diff  ;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", coldiff[j]);
        fprintf(stderr, "\n");
        fprintf(stderr, "min   ;");
        for (j=0; j<ndest;j++)
            fprintf(stderr, "%4d;", pmc[j]);
        fprintf(stderr, "\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "=================================================================================================================\n");
        fprintf(stderr, "=================================================================================================================\n");
        fprintf(stderr, "=================================================================================================================\n");

        #endif // _debugPrintStepByStep


/* NUOVO AGGIORNAMENTO ==================================================== */


        /* se e' stata eliminata un riga devo aggiornare tutte le colonne   **
        ** per le quali il minimo o il secondo minimo capitava proprio      **
        ** sulal riga elimnata                                              */
        if ( supply[source]==-1 ) {
            for ( j=0; j<ndest; j++ ) { /* per ogni colonna                 */
                if ( demand[j]==-1 ) {
                    pmc[j]=-1;
                    psmc[j]=-1;
                    continue;
                    }

                if ( pmc[j]==source || psmc[j]==source ) {/* se capita male */
                    pmc[j]=-1;
                    psmc[j]=-1;

                    for ( i=0; i<nsources; i++ ) { /* per ogni riga         */
                        if ( supply[i]==-1 )
                            continue;

                        if ( pmc[j]==-1 )  /* se non c'e' il minimo        */
                            pmc[j]=i;
                        else {
                            cmin=vogelscosts[i][j];
                            /* se ho trovato un minimo migliore             */
                            if ( cmin<=vogelscosts[pmc[j]][j] ) {
                                psmc[j]=pmc[j];
                                pmc[j]=i;
                                }
                            else
                                /* se ho trovato un secondo minimo          */
                                if ( psmc[j]==-1  )
                                    psmc[j]=i;
                                else
                                    /* se ho trovato un secondo minimo      **
                                    ** migliore                             */
                                    if ( cmin<vogelscosts[psmc[j]][j] )
                                        psmc[j]=i;
                            }
                        }
                    /* aggiorna la diferenza per la colonna                 */
                    /* un minimo esiste per forza altrimenti la colonna     **
                    ** sarebbe stata eliminata                              */
                    if (pmc[j]!=-1) {
                        cmin=vogelscosts[pmc[j]][j];
                        if ( psmc[j]!=-1 ) { /* se esiste un secondo minimo     */
                            rmin = vogelscosts[psmc[j]][j];
                            coldiff[j]=rmin-cmin;
                            }
                        else
                            coldiff[j]=cmin;
                        }
                    else
                        coldiff[j]=-1;
                    }
                }
            }
        else if ( demand[dest]==-1 ) { /* demand[dest]==-1 */
            for ( i=0; i<nsources; i++ ) {
                if ( supply[i]==-1 ) {
                    pmr[i]=-1;
                    psmr[i]=-1;
                    continue;
                    }

                if ( pmr[i]==dest || psmr[i]==dest ) {
                    pmr[i]=-1;
                    psmr[i]=-1;

                    for ( j=0; j<ndest; j++ ) {
                        if ( demand[j]==-1 )
                            continue;

                        if ( pmr[i]==-1 )
                            pmr[i]=j;
                        else {
                            rmin=vogelscosts[i][j];
                            if ( rmin<=vogelscosts[i][pmr[i]] ) {
                                psmr[i]=pmr[i];
                                pmr[i]=j;
                                }
                            else
                                if ( psmr[i]==-1 )
                                    psmr[i]=j;
                                else
                                    if ( rmin<vogelscosts[i][psmr[i]] )
                                        psmr[i]=j;
                            }
                        }
                    if ( pmr[i]!=-1 ) {
                        cmin=vogelscosts[i][pmr[i]];
                        if ( psmr[i]!=-1 ) {
                            rmin=vogelscosts[i][psmr[i]];
                            rowdiff[i]=rmin-cmin;
                            }
                        else
                            rowdiff[i]=cmin;
                        }
                    else
                        rowdiff[i]=-1;
                    }
                }
            }



/* ======================================================================== */



//
//
//        for ( source=0; source<nsources; source++ ) {
//            pmr[source]=-1;
//            rowdiff[source]=-1;
//            }
//
//        for ( dest=0; dest<ndest; dest++ ) {
//            pmc[dest]=-1;
//            coldiff[dest]=-1;
//            }
//
//
//
//        /* aggiorna struttura */
//        for ( source=0; source<nsources; source++ ) {
//            if ( supply[source]==-1 )
//                    continue;
//
//            for ( dest=0; dest<ndest; dest++ ) {
//                if ( demand[dest]==-1 )
//                    continue;
//
////                if ( isTabu(source, dest) )
////                    continue;
//
//                costi=vogelscosts[source][dest];
//
//                if ( pmr[source]==-1 )
//                        pmr[source]=dest;
//                else {
//                    rmin=vogelscosts[source][pmr[source]];
//                    if ( costi<=rmin ) {
//                        rowdiff[source]=pmr[source];
//                        pmr[source]=dest;
//                        }
//                    else
//                        if ( rowdiff[source]==-1 )
//                            rowdiff[source]=dest;
//                        else {
//                            rmin=vogelscosts[source][rowdiff[source]];
//                            if ( costi<rmin )
//                                rowdiff[source]=dest;
//                            }
//                    }
//
//                if ( pmc[dest]==-1 )
//                        pmc[dest]=source;
//                else {
//                    cmin=vogelscosts[pmc[dest]][dest];
//                    if ( costi<=cmin ) {
//                        coldiff[dest]=pmc[dest];
//                        pmc[dest]=source;
//                        }
//                    else
//                        if ( coldiff[dest]==-1 )
//                            coldiff[dest]=source;
//                        else {
//                            cmin=vogelscosts[coldiff[dest]][dest];
//                            if ( costi<cmin )
//                                coldiff[dest]=source;
//                            }
//                    }
//                }
//            }
//
//
//        for ( source=0; source<nsources; source++ ) {
//            if ( supply[source]==-1 || pmr[source]==-1 )
//                continue;
//
//            if ( rowdiff[source]==-1 )
//                rowdiff[source]=vogelscosts[source][pmr[source]];
//            else {
//                cmin = vogelscosts[source][pmr[source]];
//                rmin = vogelscosts[source][rowdiff[source]];
//                rowdiff[source] = rmin - cmin;
//                }
//            }
//
//        for ( dest=0; dest<ndest; dest++ ) {
//            if ( demand[dest]==-1 || pmc[dest]==-1 )
//                continue;
//
//            if (coldiff[dest]==-1)
//                coldiff[dest]=vogelscosts[pmc[dest]][dest];
//            else {
//                cmin = vogelscosts[pmc[dest]][dest];
//                rmin = vogelscosts[coldiff[dest]][dest];
//                coldiff[dest] = rmin - cmin;
//                }
//            }
        }
    end=clock();

//    /* check feaseability                                                   */
//    flag=true;
//    float rowsum;
//    int colsum;
//    float ktm;
//    /* per ogni sorgente (per ogni tipo di user) la somma degli utsers      **
//    ** spostati deve essere inveriore alla disponibilita'                   */
//    for ( source=0; source < nsources; source++ ) {
//        rowsum=0;
//        user=source%nusers;
//        i = sourcesidx[source];
//        for ( dest=0; dest<ndest-1; dest++ )
//            if ( vogelsmatrix[source][dest]!=-1 ) {
////                k =(int) ceil((float)  vogelsmatrix[source][dest] / (user+1));
//                ktm =(float) vogelsmatrix[source][dest] / (user+1);
//
//                if ( ceil(ktm)!=floor(ktm) )
//                    fprintf(stderr, "Row %4d Column %4d K is %4f\n",
//                            i, destsidx[dest], ktm);
//
//                rowsum+=ktm;
//                }
//
//        if ( rowsum>pos[i][user][time] ) {
//            printf("Riga %d, chiedo %f users (tipo %d) invece di %d\n",
//                   i, rowsum, user, pos[i][user][time]);
//            flag=false;
//            }
//        }
//    /* per ogni destinazione la somma dei task eseguiti deve essere pari o  **
//    ** maggiore alla richiesta di tasks                                     */
//    for ( dest=0; dest<ndest-1; dest++ ) {
//        colsum=0;
//        j = destsidx[dest];
//        for ( source=0; source < nsources; source++ )
//            if ( vogelsmatrix[source][dest]!=-1 )
//                colsum += vogelsmatrix[source][dest];
//
//        if ( colsum < tasks[j] ) {
//            printf("Colonna %d, tasks %d invece di %d\n", j, colsum, tasks[j]);
//            flag=false;
//            }
//        }



    if ( isFeasible(&vogelsmatrix, &pos, &tasks,
                    &sourcesidx, &destsidx,
                    nsources, ndest, nusers, time) ) {
        obj=0;
        for ( source=0; source < nsources; source++ )
            for ( dest=0; dest<ndest-1; dest++ )
                if ( vogelsmatrix[source][dest]!=-1 ) {
                    user=source%nusers;
                    i = sourcesidx[source];
                    j = destsidx[dest];
                    k =(int) ceil((float)  vogelsmatrix[source][dest] / (user+1));
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
    fprintf(stderr, "\n\n");
    }   // for ( kkk .... )
//    printf("\n\n\n");


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

    free(rowdiff);
    free(coldiff);
    free(supply);
    free(sourcesidx);
    free(pmr);
    free(psmr);
    free(demand);
    free(destsidx);
    free(pmc);
    free(psmc);

    for ( source=0; source<nsources; source++ ) {
        free(vogelscosts[source]);
        free(vogelsmatrix[source]);
        }
    free(vogelscosts);
    free(vogelsmatrix);

    return 0;
}


//void addTabu(int source, int dest) {
//    if (nTabus>tabuMax)
//        exit(-11);
//
//    moves[nTabus].s=source;
//    moves[nTabus].d=dest;
//    nTabus++;
//
//    printf("Aggiungo Tabu %4d - %4d\n", source, dest);
//}
//
//
//int isTabu(int source, int dest) {
//    int i;
//
//    for (i=0; i<nTabus; i++)
//        if (moves[nTabus].s==source && moves[nTabus].d==dest)
//            return 1;
//
//    return 0;
//}

int isFeasible(int*** vogelsmatrix,
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
            if ( (*vogelsmatrix)[source][dest]!=-1 ) {
                k = (float) (*vogelsmatrix)[source][dest] / (user+1);

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
            if ( (*vogelsmatrix)[source][dest]!=-1 )
                colsum += (*vogelsmatrix)[source][dest];

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


void allocVogel(int** rowdiff,
                int** coldiff,
                int** supply,
                int** demand,
                int** sourcesidx,
                int** destsidx,
                int** pmr,
                int** psmr,
                int** pmc,
                int** psmc,
                int*** vogelscosts,
                int*** vogelsmatrix,
                int nusers,
                int ncelle) {

    int i, j;
    int nsources=nusers*ncelle; /* numero massimo di sorgenti               */
    int ndests=ncelle;          /* numero massimo di destinazioni           */


    /* vettore per le differenze per riga (i.e. sorgenti)                   */
    *rowdiff=(int*) malloc(nsources*sizeof(int));
    assert( *rowdiff );

    /* vettore per le differenze per colonna (i.e. destinazioni)            */
    *coldiff=(int*) malloc(ndests*sizeof(int));
    assert( *coldiff );

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
    *pmr=(int*) malloc(nsources*sizeof(int));
    assert( *pmr );

    /* vettore con le posizioni dei secondi minimi per ogni riga            **
    ** (i.e. sorgente)                                                      */
    *psmr=(int*) malloc(nsources*sizeof(int));
    assert( *psmr );

    /* vettore con le posizioni dei minimi per ogni colonna                 **
    ** (i.e. destinazione)                                                  */
    *pmc=(int*) malloc(ndests*sizeof(int));
    assert( *pmc );

    /* vettore con le posizioni dei secondi minimi per ogni colonna         **
    ** (i.e. destinazione)                                                  */
    *psmc=(int*) malloc(ndests*sizeof(int));
    assert( *psmc );

    /* inizializzazione delle strutture                                     */
    for ( i=0; i<nsources; i++ ) {
        (*rowdiff)[i]=-1;
        (*supply)[i]=-1;
        (*sourcesidx)[i]=-1;
        (*pmr)[i]=-1;
        (*psmr)[i]=-1;
        }

    for ( i=0; i<ndests; i++ ) {
        (*coldiff)[i]=-1;
        (*demand)[i]=-1;
        (*destsidx)[i]=-1;
        (*pmc)[i]=-1;
        (*psmc)[i]=-1;
        }

    /* matrice dei costi per VAM                                            */
    *vogelscosts=(int**) malloc(nsources*sizeof(int*));
    assert(*vogelscosts);

    /* matrice delle assegnazioni (i.e. risultato del VAM)                  */
    *vogelsmatrix=(int**) malloc(nsources*sizeof(int*));
    assert (*vogelsmatrix);

    /* inizializzazione matrici costi e assegnazioni                        */
    for ( i=0; i<nsources; i++ ) {
        (*vogelscosts)[i] =(int*) malloc(ndests*sizeof(int));
        assert ( (*vogelscosts)[i]  );
        (*vogelsmatrix)[i]=(int*) malloc(ndests*sizeof(int));
        assert ( (*vogelsmatrix)[i] );
        /* inizializza la nuova riga                                        */
        for ( j=0; j<ndests; j++ ) {
            (*vogelscosts)[i][j] =-1;
            (*vogelsmatrix)[i][j]=-1;
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
