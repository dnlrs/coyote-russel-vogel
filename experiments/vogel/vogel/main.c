#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define min(A, B) ((A) < (B) ? A : B)
#define max(A, B) ((A) < (B) ? B : A)


int main(int argc, char* argv[])
{
    FILE* f;

    /*
    ** matrice dei costi
    **
    ** dimensioni:
        [ncelle][ncelle][nusers][nintervalli]
    */
    int**** costs;
    /*
    ** tasks per cella
    ** dimensioni:
        [ncelle]
    */
    int* tasks;
    /*
    ** posizione degli users nelle celle
    ** dimensioni:
        [ncelle][nusers][nintervalli]
    */
    int*** pos;
    /*
    ** time is $$
    */
    clock_t end;
    clock_t start;
    /*
    ** 'nusers' numero di users
    ** 'nintervalli' numero di intervalli
    ** 'ncelle' numero di celle
    */
    int nusers, nintervalli, ncelle;
    /*
    ** indici per i cicli
    */
    int user, time, cella, i, j, k;

    /* passo il nome del file come parametro */
    if ( argc<2 ) {
        fprintf(stderr, "manca il file.\n");
        return -1;
        }



    f=fopen(argv[1], "r");
    assert(f!=NULL);

    fscanf(f, "%d", &ncelle);      /* leggo nr. celle      */
    fscanf(f, "%d", &nintervalli); /* leggo nr. intervalli */
    fscanf(f, "%d", &nusers);      /* leggo nr. genti      */

    /* /////////////////////////////////////////////////////////// **
    **
    ** Allocazione e preparazione della struttura dati
    **
    ** /////////////////////////////////////////////////////////// */
    /*
    ** tasks per ogni cella
    */
    tasks=(int*) malloc(ncelle*sizeof(int));


    /*
    ** multi-matrice dei costi
    **
        [ncelle]
            [ncelle]
                [nusers]
                    [nintervalli]
    */
    costs=(int****) malloc(ncelle*sizeof(int***));
    assert(costs!=NULL);
    for (i=0; i<ncelle; i++) {
        costs[i]= (int***) malloc(ncelle*sizeof(int**));
        assert(costs[i]!=NULL);
        for (j=0; j<ncelle; j++) {
            costs[i][j]= (int**) malloc(nusers*sizeof(int*));
            assert(costs[i][j]!=NULL);
            for (k=0; k<nusers; k++) {
                costs[i][j][k]=(int*) malloc(nintervalli*sizeof(int));
                assert(costs[i][j][k]!=NULL);
                }
            }
        }

    /*
    ** posizione degli users
    **
        [ncelle]
            [nusers]
                [nintervalli]
    */
    pos=(int***) malloc(ncelle*sizeof(int**));
    assert(pos!=NULL);

    for (i=0; i<ncelle; i++) {
        pos[i]=(int**) malloc(nusers*sizeof(int*));
        assert(pos[i]!=NULL);

        for (j=0; j<nusers; j++) {
            pos[i][j]=(int*) malloc(nintervalli*sizeof(int));
            assert(pos[i][j]!=NULL);
            }
        }

//  pos=(int***) malloc(nusers*sizeof(int**));
//  assert(pos!=NULL);
//  for (i=0; i<nusers;i++) {
//      pos[i]=(int**) malloc(nintervalli*sizeof(int*));
//      assert(pos[i]!=NULL);
//      for (j=0; j<nintervalli; j++) {
//          pos[i][j]=(int*) malloc(ncelle*sizeof(int));
//          assert(pos[i][j]!=NULL);
//          }
//      }

#ifdef _debug
    printf("Numero di celle: %d\n", ncelle);
    printf("Numero di users: %d\n", nusers);
    printf("Numero di intervalli: %d\n", nintervalli);
#endif

    /* /////////////////////////////////////////////////////////// **
    **
    ** Lettura dei dati dal file
    **
    ** /////////////////////////////////////////////////////////// */
    /*
    ** ignora i 'tasks per user' (1 2 3)
    */
    for (i=0; i<3; i++)
        fscanf(f, "%d", &k);
    /*
    ** carica le super-matrici dei costi
    */
    float tmp;
    for (k=0; k<nintervalli*nusers; k++) {
        fscanf(f, "%d", &user);
        fscanf(f, "%d", &time);
        for (i=0; i<ncelle; i++)
            for (j=0; j<ncelle; j++) {
                fscanf(f, "%f", &tmp);
                costs[i][j][user][time]=(int) tmp;
                }
        }
    /*
    ** carica i tasks per le celle
    */
    for (cella=0; cella<ncelle; cella++)
        fscanf(f, "%d", &tasks[cella]);
    /*
    ** carica le posizioni dei vari tipi di
    ** users nei diversi intervalli
    */
    for (k=0; k<nintervalli*nusers; k++) {
        fscanf(f, "%d", &user);
        fscanf(f, "%d", &time);
        for (cella=0; cella<ncelle; cella++)
            fscanf(f, "%d", &pos[cella][user][time]);
        }
    fclose(f);


    /* /////////////////////////////////////////////////////////// **
    **
    ** Inizio elaborazione
    **
    ** /////////////////////////////////////////////////////////// */
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
    **  - acolumns[n]   destinazioni disponibili
    **  - supply[m]     fornitura da ogni sorgente ( s_ )
    **  - arows[m]      sorgenti disponibili
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

    int nsources;       /* numero di sorgenti                               */
    int ndest;          /* numero di destinazioni                           */
    int dest;           /* indice per le destinazioni                       */
    int source;         /* indice per le sorgenti                           */
    int totsupply;      /* fornitura totale                                 */
    int totdemand;      /* domanda totale                                   */

    int* arows;         /* differenze per le righe (sorigenti)              */
    int* acolumns;      /* differenze per le colonne                        */
    int* supply;        /* vettore delle forniture (da sorgenti)            */
    int* demand;        /* vettore delle richieste (per destinazioni)       */
    int* sourcesidx;    /* indice delle sorgenti nella matrice originale    */
    int* destsidx;      /* indice delle destinazioni nella matrice originale*/
    int** vogelscosts;  /* matrice di Vogel con i costi                     */
    int** vogelsmatrix; /* matrice di Vogel con le soluzioni                */

    int* pmr;           /* vettore con le posizioni dei minimi nelle righe  */
    int* pmc;           /* vettore con le posizioni dei minimi nelle colonne*/

    float cost;         /* variabile ausiliaria per il costo                */
    int costi;          /* variabile ausiliaria per il costo                */
    int itera;          /* variabile per le m+n-1 iterazioni del VAM        */
    int cmin;           /* variabile aus. per il costo min nella colonna    */
    int rmin;           /* variabile aus. per il costo min nella riga       */
    int maxdiff;        /* var. aus. per trovare la _differenza_ maggiore   */
    int imaxdiff;       /* indice della differenza maggiore                 */
    int flagrow;        /* se true la differenza maggiore appartiene a riga */
    int assignment;     /* var. aus. per stabilire l'assegnazione           */
    /* trova il numero di sorgenti
    **
    ** NOTA:
    ** si assume che se esiste una sorgente
    ** allora nella sorgente ci sono tutti e
    ** 3 i tipi di users!
    */
    user=0;
    time=0;
    nsources=0;
    for ( cella=0; cella<ncelle; cella++ )
        if (pos[cella][user][time])
            nsources++;

    nsources=nsources*3;
    /*
    ** trova il numero di destinazioni
    */
    ndest=0;
    for ( cella=0; cella<ncelle; cella++ )
        if (tasks[cella])
            ndest++;
    ndest++;    /* la destinazione dummy */

    /*
    ** preparazione struttura dati
    */
    arows=      (int*) malloc(nsources*sizeof(int));
    acolumns=   (int*) malloc(ndest*sizeof(int));
    assert(arows && acolumns);

    supply    = (int*) malloc(nsources*sizeof(int));
    sourcesidx= (int*) malloc(nsources*sizeof(int));
    pmr       = (int*) malloc(nsources*sizeof(int));
    demand    = (int*) malloc(ndest*sizeof(int));
    destsidx  = (int*) malloc(ndest*sizeof(int));
    pmc       = (int*) malloc(ndest*sizeof(int));
    assert(supply && demand &&       \
           sourcesidx && destsidx && \
           pmr && pmc);


    for ( i=0; i<nsources; i++ ) {
        arows[i]=0;
        supply[i]=0;
        sourcesidx[i]=-1;
        pmr[i]=-1;
        }

    for ( i=0; i<ndest; i++ ) {
        acolumns[i]=0;
        demand[i]=0;
        destsidx[i]=-1;
        pmc[i]=-1;
        }

    vogelscosts=(int**) malloc(nsources*sizeof(int*));
    vogelsmatrix=(int**) malloc(nsources*sizeof(int*));
    assert(vogelscosts && vogelsmatrix);

    for ( source=0; source<nsources; source++ ) {
        vogelscosts[source]=(int*) malloc(ndest*sizeof(int));
        vogelsmatrix[source]=(int*) malloc(ndest*sizeof(int));
        assert(vogelscosts[source] && vogelsmatrix[source]);
        }

    /*
    ** inizializzazione delle destinazioni
    */
    printf("Cerco le destinazioni... dest [tasks] -> original dest\n");
    i=0;
    totdemand=0;
    for ( dest=0; dest<ndest-1; dest++ ) {
        /* salta le destinazioni che non hanno
        ** tasks da eseguire
        */
        while (!tasks[i]) i++;
        demand[dest]=tasks[i];
        totdemand+=tasks[i];
        destsidx[dest]=i;
        printf("Nuova destinazione trovata: %d [%d] -> %d.\n",
               dest, demand[dest], destsidx[dest]);
        i++;
        }
    printf("Ho inizializzato le destinazioni.\n");
    /*
    ** inizializzazione delle sorgenti
    */
    printf("Cerco le sorgenti... sorg [tipo u] [tavail] -> original sorg\n");
    i=0;
    time=0;
    totsupply=0;
    for ( source=0; source<nsources; source++ ) {
        /* salta le sorgenti che non hanno
        ** users da regalare
        */
        while (!pos[i][source%3][time]) i++;
        /* considero i tasks non gli utenti */
        supply[source]=pos[i][source%3][time]*(source%3+1);
        totsupply+=pos[i][source%3][time]*(source%3+1);
        sourcesidx[source]=i;
        printf("Nuova sorgente trovata: %d [%d] [%d] -> %d.\n",
               source, source%3, supply[source], sourcesidx[source]);
        if ( source%3==2 )
            i++;
        }
    demand[ndest-1]=totsupply-totdemand;
    printf("Ho inizializzato le sorgenti. (supple %d e demand %d)\n",
           totsupply, totdemand);
    /*
    ** inizializza i costi e la matrice delle variabili
    */
    time=0;
    printf("Creo la matrice dei costi.\n");
    for ( source=0; source<nsources; source++ ) {
        for ( dest=0; dest<ndest-1; dest++ ) {
            i=sourcesidx[source];
            j=destsidx[dest];
            user=source%3;
            cost=(float) costs[i][j][user][time] / (user+1);

            vogelscosts[source][dest]=(int) (cost*100);
//            printf("Vecchio costo %f -> nuovo costo %d.\n",
//                   cost, vogelscosts[source][dest]);
            vogelsmatrix[source][dest]=-1;
            }
        vogelscosts[source][ndest-1]=500;
        vogelsmatrix[source][ndest-1]=-1;
        }

    itera=nsources+ndest-1;
    while ( itera>0 ) {
    printf("Iterazione %d.\n", itera);
        itera--;
        /*
        ** calcola le differenze e trova le posizioni dei
        ** minimi per ogni colonna e per ogni riga
        */
        for ( source=0; source<nsources; source++ ) {
            /* se la riga e' stata completata non si calcola
            ** piu' la differenza per quella riga
            */
            if ( supply[source]==-1 )
                continue;

            for ( dest=0; dest<ndest; dest++ ) {
                /* se la colonna e' stata completata non si calcola
                ** piu' la differenza per quella colonna
                */
                if ( demand[dest]==-1 )
                    continue;

                costi=vogelscosts[source][dest];
                //~ printf("\tSorgente %d (%d). Destinazione %d (%d) => costo %d.\n",
                       //~ source, sourcesidx[source],
                       //~ dest, destsidx[dest],
                       //~ costi);

                /* gestione del minimo e del secondo
                ** minimimo per le righe
                **
                ** se non esiste ancora un costo minimo per la
                ** riga lo si inizializza
                */
                if ( pmr[source]==-1 ){
                    pmr[source]=dest;
                    }
                else {
                    /* se esiste gia' un minimo, se il nuovo costo
                    ** e' minore del minimo allora gli si sostituisce
                    */
                    rmin=vogelscosts[source][pmr[source]];
                    if ( costi<=rmin ) {
                        arows[source]=rmin;
                        pmr[source]=dest;
                        }
                    else
                        /* se il costo corrente no e' minore del
                        ** minimo, allora potrebbe essere minore
                        ** del secondo minimo, di nuovo se questo
                        ** non fosse inizializzato lo si inizializza;
                        ** se invece questo fosse maggiore del costo
                        ** corrente lo si sostituisce
                        */
                        if ( arows[source]==-1 )
                            arows[source]=costi;
                        else
                            if ( costi<arows[source] )
                                arows[source]=costi;
                    }


                /* gestione del minimo e del secondo
                ** minimo per le colonne
                */
                if ( pmc[dest]==-1 ) {
                    pmc[dest]=source;
                    }
                else {
                    cmin=vogelscosts[pmc[dest]][dest];
                    if ( costi<=cmin ) {
                        acolumns[dest]=cmin;
                        pmc[dest]=source;
                        }
                    else
                        if ( acolumns[dest]==-1 )
                            acolumns[dest]=costi;
                        else
                            if ( costi<acolumns[dest] )
                                acolumns[dest]=costi;
                    }
                }
            }


        /*
        ** calcola la differenza per ogni riga e per
        ** ogni colonna
        **
        ** al tempo stesso cerca la differenza massima
        **
        */
        maxdiff=-1;
        imaxdiff=-1;
        flagrow=1;
        printf("Cerco la differenza massima sulle righe...\n");
        for ( source=0; source<nsources; source++ ) {
            /* ignora la riga se e' stata completata */
            if (supply[source]==-1)
                continue;
            if ( arows[source]==-1 )
                arows[source]=vogelscosts[source][pmr[source]];

            printf("Riga %d -> differenza %d - %d =",
                   source, arows[source],
                   vogelscosts[source][pmr[source]]);
            arows[source]=arows[source]-vogelscosts[source][pmr[source]];
            printf(" %d.\n", arows[source]);

            if (arows[source]>maxdiff) {
                maxdiff=arows[source];
                imaxdiff=source;
                printf("\t\tNuova massima differenza %d.\n", maxdiff);
                }
            }

        for ( dest=0; dest<ndest; dest++ ) {
            /* ignora la colonna se e' stata completata */
            if ( demand[dest]==-1 )
                continue;
            if ( acolumns[dest]==-1)
                acolumns[dest]=vogelscosts[pmc[dest]][dest];

            printf("Colonna %d -> differenza %d - %d =",
                   dest, acolumns[dest],
                   vogelscosts[pmc[dest]][dest]);
            acolumns[dest]=acolumns[dest]-vogelscosts[pmc[dest]][dest];
            printf(" %d.\n", acolumns[dest]);
            if (acolumns[dest]>maxdiff){
                maxdiff=acolumns[dest];
                imaxdiff=dest;
                flagrow=0;
                printf("\t\tNuova massima differenza %d.\n", maxdiff);
                }
            }

        printf("Eseguo l'assegnazione...\n");
        if ( flagrow ) {
            source    =imaxdiff;
            dest      =pmr[source];
            printf("Massima diff trovata sulla riga %d (%d).\n",
                   source, maxdiff);
            }
        else {
            dest    =imaxdiff;
            source  =pmc[dest];
            printf("Massima diff trovata sulla colonna %d (%d).\n",
                   dest, maxdiff);
            }

        assignment=min(supply[source], demand[dest]);
        printf("Il minimo tra %d e %d e' %d.\n",
               supply[source], demand[dest], assignment);
        vogelsmatrix[source][dest]=assignment;
        printf("Assegno %d a [%d, %d].\n",
               assignment, source, dest);
        supply[source]-=assignment;
        demand[dest]  -=assignment;

        if ( supply[source]==0 && demand[dest]==0 )
            supply[source]=-1;
        else
            if ( supply[source]==0 )
                supply[source]=-1;
            else
                if ( demand[dest]==0 )
                    demand[dest]=-1;

        for ( source=0; source<nsources; source++ ) {
            pmr[source]=-1;
            arows[source]=-1;
            }

        for ( dest=0; dest<ndest; dest++ ) {
            pmc[dest]=-1;
            acolumns[dest]=-1;
            }
        }
    end=clock();

    float obj=0;
    int overflow=0;
    for ( source=0; source<nsources; source++ )
        for ( dest=0; dest<ndest-1; dest++ )
            if ( vogelsmatrix[source][dest]!=-1 ){
                user=source%3+1;
                overflow=vogelsmatrix[source][dest]%user;
                obj+= (float) (vogelsmatrix[source][dest]+overflow)*(vogelscosts[source][dest]/100);
            }
    f=fopen("output.csv", "w");
    for ( source=0; source<nsources; source++ ) {
        for ( dest=0; dest<ndest; dest++ ) {
            if ( vogelsmatrix[source][dest]!=-1 )
                fprintf(f, "%d;", vogelsmatrix[source][dest]);
            else
                fprintf(f, "0;");
            }

        fprintf(f, ";;;");
        for ( dest=0; dest<ndest; dest++ )
                fprintf(f, "%d;", vogelscosts[source][dest]);

        fprintf(f, "\n");
        }
    fprintf(f, "\n\nTempo: %f;", ((double)end-start)/CLOCKS_PER_SEC);
    fprintf(f, "\nCosto:   %f", obj);
    fclose(f);


    /* /////////////////////////////////////////////////////////// **
    **
    ** Libera tutti
    **
    ** /////////////////////////////////////////////////////////// */

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

    free(arows);
    free(acolumns);
    free(supply);
    free(sourcesidx);
    free(pmr);
    free(demand);
    free(destsidx);
    free(pmc);

    for ( source=0; source<nsources; source++ ) {
        free(vogelscosts[source]);
        free(vogelsmatrix[source]);
        }
    free(vogelscosts);
    free(vogelsmatrix);

    return 0;
}
