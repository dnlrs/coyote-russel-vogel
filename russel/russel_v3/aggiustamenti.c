// RICERCA DELLA MINIMA NUOVA DESTINAZIONE DOVE Allocare


for ( source=0; source<nsources; source++ ) {
    /* se ritorno sulla cella che viola la salto                */
    if ( source==rmax )
        continue;

    cuser = source%nusers;
    ctpu  = tpuser[cuser];

    /* Se la cella e' occupata gli aggiungo i TaskToMove;       **
    **    se non rispetta i vincoli                             **
    **        vado avanti;                                       **
    **    altrimenti                                            **
    **        contronto il costo con quello minimo;              */
    if ( russelmatrix[source][j]>0 ) {

        if ( ((russelmatrix[source][j]+ttm)%ctpu)==0 ) {
            // ccost = ttm * russelcosts[source][j];

            if ( russelmatrix[source][ndest-1]>=ttm )
                ccost = ttm * modi[source][j];
            else
                ccost = ttm * modi[source][ndest-1];

            if ( ccost<cmin ) {
                cmin = ccost;
                rmin = source;
                }

            }
        else {
            if ( ((russelmatrix[source][j]+ttm+tpu)%ctpu)==0 ) {
                // ccost = (ttm+tpu) * russelcosts[source][j];

                if ( russelmatrix[source][ndest-1]>=(ttm+tpu) )
                    ccost = (ttm+tpu) * modi[source][j];
                else
                    ccost = (ttm+tpu) * modi[source][ndest-1];

                if ( ccost<cmin ) {
                    cmin = ccost;
                    rmin = source;
                    }

                }
            }
        }

    else { // russelmatrix[source][j] == -1
        if ( modi[source][j]==0 ) {
            fprintf(stderr, "Cella modi costo 0! (%3d,%3d)\n", source, j);

            /* Se la matrice modi e' 0, significa che ci posso  **
            ** spostare tutti i task che voglio senza che il    **
            ** costo della mia obj. func. cresca                **
            ** quindi se il modulo tra i tpu della cella        **
            ** corrente e i tpu della cella che viola i vincoli **
            ** da come resto esattamente i TaskToMove, oppure   **
            ** i TaskToMove gia' soddisfano i task per user     **
            ** della cella, siamo a cavallo                     **
            ** Inoltre, per la proprieta' della matrice MODI,   **
            ** data una cella a costo 0, c'e' sempre un path    **
            ** per spostarci i task; lungo il path rispettero'  **
            ** i vincoli?                                       */
            if ( (ttm%ctpu)==0) {
                if ( russelmatrix[source][ndest-1]>=ttm )
                    ccost = ttm*modi[source][j];
                else
                    ccost = ttm*modi[source][ndest-1];

                if ( ccost<cmin ) {
                    cmin = ccost;
                    rmin = source;
                    }
                }
            else {
                if (((ttm+tpu)%ctpu)==0 ) {

                    // ccost = 0;
                    if ( russelmatrix[source][ndest-1]>=(ttm+tpu) )
                        ccost = (ttm+tpu) * modi[source][j];
                    else
                        ccost = (ttm+tpu) * modi[source][ndest-1];

                    if ( ccost<cmin ) {
                        cmin = ccost;
                        rmin = source;
                        }

                    }
                }
            }

        else { // modi[source][j] > 0
            // fprintf(stderr, "Cella modi costo maggiore di zero (%3d,%3d)\n", source, j);

            /* dato che la cella non e' a costo 0, ci muovo lo  **
            ** stetto necessario                                **
            **   - i ttm se non violano i vincoli               **
            **   - i tpu+ttm se non violano i vincoli e         **
            **     comunque hanno un costo inferiore            **
            **     all'attuale costo minimo                     **
            **     (piu' unico che raro)                        */

            if ( (ttm%ctpu)==0 ) {

                // ccost = ttm*russelcosts[source][j];
                ccost = ttm*modi[source][j];

                // fprintf(stderr, "ttm compatibili, costo %d\n", ccost);
                if ( ccost<cmin ) {
                    cmin = ccost;
                    rmin = source;
                    }

                }
            else {
                if ( ((ttm+tpu)%ctpu)==0 ) {

                    // ccost = (tpu+ttm)*russelcosts[source][j];
                    ccost = (tpu+ttm)*modi[source][j];
                    // fprintf(stderr,
                    //         "ttm compatibili come resto, costo %d\n",
                    //         ccost);
                    if ( ccost<cmin ) {
                        cmin = ccost;
                        rmin = source;
                        }

                    }
                }
            }
        }
    } // for each source



// [C2S2]

cmin = INT_MAX;
int pt;
for ( i=0; i<nsources; i++ ) {
    fprintf(stderr, "sdfsd\n");
    if ( i==source || i==rmax )
        continue;

    if ( russelmatrix[i][ndest-1]<ttm )
        continue;

    if ( russelmatrix[i][j] != -1 )
        pt = russelmatrix[i][j];
    else
        pt = 0;

    if ( ((pt+ttm)%(tpuser[i%nusers]))!=0 )
        continue;

    if ( russelcosts[i][j]<cmin ) {
        cmin = russelcosts[i][j];
        source = i;
        }
    }

if ( (ttm*russelcosts[source][j])<tpu*russelcosts[rmax][j] ) {
    if ( russelmatrix[source][j]==-1 )
        russelmatrix[source][j] = ttm;
    else
        russelmatrix[source][j] += ttm;

    russelmatrix[source][ndest-1] -= ttm;
    if ( russelmatrix[source][ndest-1]==0 )
        russelmatrix[source][ndest-1] =-1;

    russelmatrix[rmax][j] -= ttm;
    if ( russelmatrix[rmax][j]==0 )
        russelmatrix[rmax][j]=-1;

    if ( russelmatrix[rmax][ndest-1]==-1 )
        russelmatrix[rmax][ndest-1] = ttm;
    else
        russelmatrix[rmax][ndest-1] += ttm;

    }
else
if ( russelmatrix[rmax][ndest-1]>=ttm ) {
    russelmatrix[rmax][j] += ttm;
    russelmatrix[rmax][ndest-1] -= ttm;
    if ( russelmatrix[rmax][ndest-1]==0 )
        russelmatrix[rmax][ndest-1]=-1;
    }





















































/* Aggiustamento 1  */


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


    for ( dest=0; dest<ndest; dest++ )
        fprintf(stderr, "%d ", v[dest]);
    fprintf(stderr, "\n");
    for ( dest=0; dest<ndest; dest++ )
        fprintf(stderr, "%d ", u[dest]);
    fprintf(stderr, "\n");


    /* FASE 1                                                               **
    ** Per tutte quelle celle che violano il vincolo ma che sono da sole    **
    ** sulla colonna, non resta che:                                        **
    **   1. Allocare una nuova cella, che non violi i vincoli e che abbia   **
    **      il prezzo piu' basso possibile                                  **
    **   2. Aggiungere task fino a far rispettare i vincoli se cio' che     **
    **      pago sovrabbondando con i task e' inferiore a cio' che pagherei **
    **      spostando quelli in eccesso (a patto di avere disponibilita')   */

    int ttm;    // TaskToMove
    int tta;    // TaskToAdd
    cost  =-1;  // costo cella con violazione
    int costA =-1;  // costo abbondando
    rmax  =-1;  // riga cella con violazione
    user  =-1;  // user cella con violazione
    tpu   =-1;  // TaskPerUser cella con violazione

    int cuser =-1;      // current user
    int ctpu  =-1;      // current TaskPerUser
    int rmin  =-1;      // current riga con costo minimo
    cmin  =INT_MAX; // costo minimo
    ccost =-1;      // current costo
    int canA  = false;
    int r, c;
    int mcm;



    for ( j=0; j<ndest; j++ ) {
        if ( v[j]==1 ) {

            cmin  =INT_MAX; // costo minimo
            rmax  = u[j]; // indice di riga CVIV
            user  = rmax%nusers; // tipo user CVIC
            tpu   = tpuser[user]; // TPU CVIV
            cost  = russelcosts[rmax][j]; // costo CVIV
            ttm   = russelmatrix[rmax][j]%tpu; // Task To Move
            tta   = tpu - ttm; // Task To Add


            /* Se la cella si e' messa a posto grazie a poteri              **
            ** sovrannaturali la ignoro                                     */
            if ( ttm==0 )
                continue;


            /* Per avere un confronto reale tra il costo di spostare i task
            ** in eccesso e quello di sovrabbondare per far rispettare il
            ** TPU, dovremmo sottrarre al primo costo il guagno dal
            ** togliere i task in ecceso dalla CVIV. Per evitare cio'
            ** facci ola cosa contraria e considero il secondo costo come
            ** il costo di spostare TPU tasks nella CVIV.
            **
            ** Ricordo che
            **                 tta + ttm = tpu
            **
            ** In un primo momento inoltre considero il secondo costo
            ** solamente se ho disponibilita' di TTA task nel deposito
            ** della CVIV.                                                  */
            if ( russelmatrix[rmax][ndest-1]>=tta )
                costA = cost*tpu;
            else
                costA = INT_MAX;

            fprintf(stderr, "Cella (%3d,%3d), unica sulla colonna\n", rmax, j);

            for ( source=0; source<nsources; source++ ) {
                /* se ritorno sulla cella che viola la salto                */
                if ( source==rmax )
                    continue;

                cuser = source%nusers;
                ctpu  = tpuser[cuser];

                /* Se la cella e' occupata gli aggiungo i TaskToMove;       **
                **    se non rispetta i vincoli                             **
                **        vado avanti;                                       **
                **    altrimenti                                            **
                **        contronto il costo con quello minimo;              */
                if ( russelmatrix[source][j]>0 ) {
                    // fprintf(stderr, "tovata cella occupata (%3d,%3d)\n", source, j);

                    if ( ((russelmatrix[source][j]+ttm)%ctpu)==0 ) {
                        ccost = ttm * russelcosts[source][j];

                        // fprintf(stderr, "E' compatibile, costo %d\n",ccost);
                        if ( ccost<cmin ) {
                            cmin = ccost;
                            rmin = source;
                            }

                        }
                    else {
                        if ( ((russelmatrix[source][j]+ttm+tpu)%ctpu)==0 ) {
                            ccost = (ttm+tpu) * russelcosts[source][j];

                            if ( ccost<cmin ) {
                                cmin = ccost;
                                rmin = source;
                                }

                            }
                        }
                    }

                else { // russelmatrix[source][j] == -1
                    if ( modi[source][j]==0 ) {
                        fprintf(stderr, "Cella modi costo 0! (%3d,%3d)\n", source, j);

                        /* Se la matrice modi e' 0, significa che ci posso  **
                        ** spostare tutti i task che voglio senza che il    **
                        ** costo della mia obj. func. cresca                **
                        ** quindi se il modulo tra i tpu della cella        **
                        ** corrente e i tpu della cella che viola i vincoli **
                        ** da come resto esattamente i TaskToMove, oppure   **
                        ** i TaskToMove gia' soddisfano i task per user     **
                        ** della cella, siamo a cavallo                     **
                        ** Inoltre, per la proprieta' della matrice MODI,   **
                        ** data una cella a costo 0, c'e' sempre un path    **
                        ** per spostarci i task; lungo il path rispettero'  **
                        ** i vincoli?                                       */
                        if ( (ttm%ctpu)==0 || (ctpu%tpu)==ttm ) {

                            ccost = 0;
                            if ( ccost<cmin ) {
                                cmin = ccost;
                                rmin = source;
                                }

                            }
                        }

                    else { // modi[source][j] > 0
                        // fprintf(stderr, "Cella modi costo maggiore di zero (%3d,%3d)\n", source, j);

                        /* dato che la cella non e' a costo 0, ci muovo lo  **
                        ** stetto necessario                                **
                        **   - i ttm se non violano i vincoli               **
                        **   - i tpu+ttm se non violano i vincoli e         **
                        **     comunque hanno un costo inferiore            **
                        **     all'attuale costo minimo                     **
                        **     (piu' unico che raro)                        */

                        if ( (ttm%ctpu)==0 ) {

                            ccost = ttm*russelcosts[source][j];
                            // fprintf(stderr, "ttm compatibili, costo %d\n", ccost);
                            if ( ccost<cmin ) {
                                cmin = ccost;
                                rmin = source;
                                }

                            }
                        else {
                            if ( (ctpu%tpu)==ttm ) {

                                ccost = (tpu+ttm)*russelcosts[source][j];
                                // fprintf(stderr,
                                //         "ttm compatibili come resto, costo %d\n",
                                //         ccost);
                                if ( ccost<cmin ) {
                                    cmin = ccost;
                                    rmin = source;
                                    }

                                }
                            }
                        }
                    }
                } // for each source

            /* NON dimentichiamoci che, se abbondare conviene piu' di       **
            ** togliere i task in eccesso, allora con fare DECISO           **
            ** abbondiamo                                                   **
            **        "Melius abundare quam deficere".                      **
            **                            Kebabbaro con cipolla in mano.    */
            if ( costA<cmin ) {
                cmin = costA;
                rmin = rmax;
                fprintf(stderr, "Melius abundare quam deficere.\n");
                }

            source = rmin;
            cuser  = source%nusers;
            ctpu   = tpuser[cuser];


            fprintf(stderr, "ha come minimo (%3d,%3d), costo %d (ttm %d)\n\n",
                    source, j, cmin, ttm);


            /* Prima di procedere devo capire quanti task ho deciso di      **
            ** spostare.                                                    */
            if ( russelmatrix[source][dest]==-1 ) {
                if ( (ttm%ctpu)!=0 ) { // controesempio ttm 2 su una CACM con tpu=1
                    if ( ((ttm+tpu)%ctpu)==0 )
                        ttm += tpu;
                    else
                        fprintf(stderr, "WHAT THE TPU TTM HACK?!\n");
                    }
                }
            else {
                if ( russelmatrix[source][dest]>=0 ) {
                    if ( ((russelmatrix[source][j]+ttm)%ctpu)!=0 ) {
                        if ( ((russelmatrix[source][j]+ttm+tpu)%ctpu)==0 )
                            ttm += tpu;
                        else
                            fprintf(stderr, "WHAT THE TPU TTM HACK SECOND?!\n");
                        }
                    }
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
            **   la cella era una CVIV e risolvo la vuolazione, oppure,
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
            ** Soluzione 0 [C2S0]
            ** Dato che il deposito della CACM e' vuoto, faccio il giro
            ** nell'altro senso, spostando i TTA.
            ** Le caratteristiche di questo path sono le stesse del path di
            ** [C1S1]:
            **   - e' vietato passare dal deposito se non ha task
            **   - nei depositi non si controllano le violazioni
            **   - tutte le celle all'interno del path devono essere CNVIV
            **     o diventarlo dopo l'attraversamento
            **   - i TTA sono fissi
            **
            ** [WARN] Il path e' valido SSE la CVIV originaria vi e'
            **        compresa
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

                if ( rs ) { // Soluzione 1 [C1S1]
                    fprintf(stderr, "[C1S1] Path found\n");
                    for ( i=0; pathrows[i]!=-1; i++ ) {
                        r = pathrows[i];
                        c = pathcols[i];

                        if ( (i%2)==0 ) { // indice pari -> +
                            fprintf(stderr, "Aggiungo a (%3d,%3d)\n", r, c);
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
                    }
                else { // Soluzione 2 [C1S2]
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
                    }



                // EMERGENZA CASO 1?

                }
            else {  // CASO 2

                clearPath(&pathrows, &pathcols, nsources, ndest);
                initTabu();

                pathrows[0]=source;
                pathcols[0]=ndest-1;

                rs = fastFindPathR(source, ndest-1,
                                   source, ndest-1,
                                   &pathrows, &pathcols,
                                   1, tta,
                                   &russelmatrix, &tpuser,
                                   nusers, nsources, ndest,
                                   true,   // payAttention
                                   false); // depAllowed


                flag = false;
                rs = false;
                if ( rs ) {
                    for ( i=0; pathrows[i]!=-1; i++ ) {
                        if ( rmax==pathrows[i] && j==pathcols[i] ) {
                            flag = true;
                            break;
                            }
                        }
                    }

                if ( flag ) { // Soluzione 0 [C2S0]
                    fprintf(stderr, "[C2S0] Path found\n");

                    for ( i=0; pathrows[i]!=-1; i++ ) {

                        r = pathrows[i];
                        c = pathcols[i];

                        if ( (i%2)==0 ) { // indice pari -> +
                            fprintf(stderr, "Aggiungo  %2d a (%3d,%3d)\n", tta, r, c);
                            if ( russelmatrix[r][c]==-1 )
                                russelmatrix[r][c]  = tta;
                            else
                                russelmatrix[r][c] += tta;

                            }
                        else { // indice dispari -> -
                            fprintf(stderr, "Sottraggo %2d a (%3d,%3d)\n", tta, r, c);
                            russelmatrix[r][c] -= tta;
                            }

                        }
                    }

                else { // Soluzioni 1/2
                    clearPath(&pathrows, &pathcols, nsources, ndest);
                    initTabu();

                    pathrows[0]=source;
                    pathcols[0]=j;

                    pathrows[1]=rmax;
                    pathcols[1]=j;

                    rs = fastFindPathR(rmax, j,
                                       source, j,
                                       &pathrows, &pathcols,
                                       2, ttm,
                                       &russelmatrix, &tpuser,
                                       nusers, nsources, ndest,
                                       true, // payAttention
                                       true); // depAllowed

                    if ( rs ) { // Soluzione 1 [C2S2]
                        fprintf(stderr, "[C2S1] Path found\n");
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
                        }
                    else { // Soluzione 2 [C2S2]
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


                            }
                        else
                            fprintf(stderr, "[C2S2] No Path found\n");

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




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////










    //
    // for ( source=0; source<nsources; source++ )
    //     for ( dest=0; dest<ndest-1; dest++ ) {
    //         if ( russelmatrix[source][dest]==-1 )
    //             continue;
    //
    //         user = source%nusers;
    //         tpu=tpuser[user];
    //
    //         if ( russelmatrix[source][dest]%tpu!=0 )
    //             solveConflicts(source, dest,
    //                            &pathrows, &pathcols,
    //                            &russelmatrix, &russelcosts,
    //                            &modi, &tpuser, nsources,
    //                            ndest, nusers);
    //     }




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////




















///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////






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


    for ( dest=0; dest<ndest; dest++ )
        fprintf(stderr, "%d ", v[dest]);
    fprintf(stderr, "\n");
    for ( dest=0; dest<ndest; dest++ )
        fprintf(stderr, "%d ", u[dest]);
    fprintf(stderr, "\n");


    /* FASE 1                                                               **
    ** Per tutte quelle celle che violano il vincolo ma che sono da sole    **
    ** sulla colonna, non resta che:                                        **
    **   1. Allocare una nuova cella, che non violi i vincoli e che abbia   **
    **      il prezzo piu' basso possibile                                  **
    **   2. Aggiungere task fino a far rispettare i vincoli se cio' che     **
    **      pago sovrabbondando con i task e' inferiore a cio' che pagherei **
    **      spostando quelli in eccesso (a patto di avere disponibilita')   */

    int ttm;    // TaskToMove
    int tta;    // TaskToAdd
    cost  =-1;  // costo cella con violazione
    int costA =-1;  // costo abbondando
    rmax  =-1;  // riga cella con violazione
    user  =-1;  // user cella con violazione
    tpu   =-1;  // TaskPerUser cella con violazione

    int cuser =-1;      // current user
    int ctpu  =-1;      // current TaskPerUser
    int rmin  =-1;      // current riga con costo minimo
    cmin  =INT_MAX; // costo minimo
    ccost =-1;      // current costo
    int canA  = false;
    int r, c;
    int mcm;



    for ( j=0; j<ndest; j++ ) {
        if ( v[j]==1 ) {

            cmin  =INT_MAX; // costo minimo
            rmax  = u[j]; // indice di riga CVIV
            user  = rmax%nusers; // tipo user CVIC
            tpu   = tpuser[user]; // TPU CVIV
            cost  = russelcosts[rmax][j]; // costo CVIV
            ttm   = russelmatrix[rmax][j]%tpu; // Task To Move
            tta   = tpu - ttm; // Task To Add


            /* Se la cella si e' messa a posto grazie a poteri              **
            ** sovrannaturali la ignoro                                     */
            if ( ttm==0 )
                continue;


            /* Per avere un confronto reale tra il costo di spostare i task
            ** in eccesso e quello di sovrabbondare per far rispettare il
            ** TPU, dovremmo sottrarre al primo costo il guagno dal
            ** togliere i task in ecceso dalla CVIV. Per evitare cio'
            ** facci ola cosa contraria e considero il secondo costo come
            ** il costo di spostare TPU tasks nella CVIV.
            **
            ** Ricordo che
            **                 tta + ttm = tpu
            **
            ** In un primo momento inoltre considero il secondo costo
            ** solamente se ho disponibilita' di TTA task nel deposito
            ** della CVIV.                                                  */
            if ( russelmatrix[rmax][ndest-1]>=tta )
                costA = cost*tpu;
            else
                costA = INT_MAX;

            fprintf(stderr, "Cella (%3d,%3d), unica sulla colonna\n", rmax, j);

            for ( source=0; source<nsources; source++ ) {
                /* se ritorno sulla cella che viola la salto                */
                if ( source==rmax )
                    continue;

                cuser = source%nusers;
                ctpu  = tpuser[cuser];

                /* Se la cella e' occupata gli aggiungo i TaskToMove;       **
                **    se non rispetta i vincoli                             **
                **        vado avanti;                                       **
                **    altrimenti                                            **
                **        contronto il costo con quello minimo;              */
                if ( russelmatrix[source][j]>0 ) {
                    // fprintf(stderr, "tovata cella occupata (%3d,%3d)\n", source, j);

                    if ( ((russelmatrix[source][j]+ttm)%ctpu)==0 ) {
                        ccost = ttm * russelcosts[source][j];

                        // fprintf(stderr, "E' compatibile, costo %d\n",ccost);
                        if ( ccost<cmin ) {
                            cmin = ccost;
                            rmin = source;
                            }

                        }
                    else {
                        if ( ((russelmatrix[source][j]+ttm+tpu)%ctpu)==0 ) {
                            ccost = (ttm+tpu) * russelcosts[source][j];

                            if ( ccost<cmin ) {
                                cmin = ccost;
                                rmin = source;
                                }

                            }
                        }
                    }

                else { // russelmatrix[source][j] == -1
                    if ( modi[source][j]==0 ) {
                        fprintf(stderr, "Cella modi costo 0! (%3d,%3d)\n", source, j);

                        /* Se la matrice modi e' 0, significa che ci posso  **
                        ** spostare tutti i task che voglio senza che il    **
                        ** costo della mia obj. func. cresca                **
                        ** quindi se il modulo tra i tpu della cella        **
                        ** corrente e i tpu della cella che viola i vincoli **
                        ** da come resto esattamente i TaskToMove, oppure   **
                        ** i TaskToMove gia' soddisfano i task per user     **
                        ** della cella, siamo a cavallo                     **
                        ** Inoltre, per la proprieta' della matrice MODI,   **
                        ** data una cella a costo 0, c'e' sempre un path    **
                        ** per spostarci i task; lungo il path rispettero'  **
                        ** i vincoli?                                       */
                        if ( (ttm%ctpu)==0 || (ctpu%tpu)==ttm ) {

                            ccost = 0;
                            if ( ccost<cmin ) {
                                cmin = ccost;
                                rmin = source;
                                }

                            }
                        }

                    else { // modi[source][j] > 0
                        // fprintf(stderr, "Cella modi costo maggiore di zero (%3d,%3d)\n", source, j);

                        /* dato che la cella non e' a costo 0, ci muovo lo  **
                        ** stetto necessario                                **
                        **   - i ttm se non violano i vincoli               **
                        **   - i tpu+ttm se non violano i vincoli e         **
                        **     comunque hanno un costo inferiore            **
                        **     all'attuale costo minimo                     **
                        **     (piu' unico che raro)                        */

                        if ( (ttm%ctpu)==0 ) {

                            ccost = ttm*russelcosts[source][j];
                            // fprintf(stderr, "ttm compatibili, costo %d\n", ccost);
                            if ( ccost<cmin ) {
                                cmin = ccost;
                                rmin = source;
                                }

                            }
                        else {
                            if ( (ctpu%tpu)==ttm ) {

                                ccost = (tpu+ttm)*russelcosts[source][j];
                                // fprintf(stderr,
                                //         "ttm compatibili come resto, costo %d\n",
                                //         ccost);
                                if ( ccost<cmin ) {
                                    cmin = ccost;
                                    rmin = source;
                                    }

                                }
                            }
                        }
                    }
                } // for each source

            /* NON dimentichiamoci che, se abbondare conviene piu' di       **
            ** togliere i task in eccesso, allora con fare DECISO           **
            ** abbondiamo                                                   **
            **        "Melius abundare quam deficere".                      **
            **                            Kebabbaro con cipolla in mano.    */
            if ( costA<cmin ) {
                cmin = costA;
                rmin = rmax;
                fprintf(stderr, "Melius abundare quam deficere.\n");
                }

            source = rmin;
            cuser  = source%nusers;
            ctpu   = tpuser[cuser];


            fprintf(stderr, "ha come minimo (%3d,%3d), costo %d (ttm %d)\n\n",
                    source, j, cmin, ttm);


            /* Prima di procedere devo capire quanti task ho deciso di      **
            ** spostare.                                                    */
            if ( russelmatrix[source][dest]==-1 ) {
                if ( (ttm%ctpu)!=0 ) { // controesempio ttm 2 su una CACM con tpu=1
                    if ( ((ttm+tpu)%ctpu)==0 )
                        ttm += tpu;
                    else
                        fprintf(stderr, "WHAT THE TPU TTM HACK?!\n");
                    }
                }
            else {
                if ( russelmatrix[source][dest]>=0 ) {
                    if ( ((russelmatrix[source][j]+ttm)%ctpu)!=0 ) {
                        if ( ((russelmatrix[source][j]+ttm+tpu)%ctpu)==0 )
                            ttm += tpu;
                        else
                            fprintf(stderr, "WHAT THE TPU TTM HACK SECOND?!\n");
                        }
                    }
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
            **   la cella era una CVIV e risolvo la vuolazione, oppure,
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
            ** Soluzione 0 [C2S0]
            ** Dato che il deposito della CACM e' vuoto, faccio il giro
            ** nell'altro senso, spostando i TTA.
            ** Le caratteristiche di questo path sono le stesse del path di
            ** [C1S1]:
            **   - e' vietato passare dal deposito se non ha task
            **   - nei depositi non si controllano le violazioni
            **   - tutte le celle all'interno del path devono essere CNVIV
            **     o diventarlo dopo l'attraversamento
            **   - i TTA sono fissi
            **
            ** [WARN] Il path e' valido SSE la CVIV originaria vi e'
            **        compresa
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

                if ( rs ) { // Soluzione 1 [C1S1]
                    fprintf(stderr, "[C1S1] Path found\n");
                    for ( i=0; pathrows[i]!=-1; i++ ) {
                        r = pathrows[i];
                        c = pathcols[i];

                        if ( (i%2)==0 ) { // indice pari -> +
                            fprintf(stderr, "Aggiungo a (%3d,%3d)\n", r, c);
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
                    }
                else { // Soluzione 2 [C1S2]
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
                    }



                // EMERGENZA CASO 1?

                }
            else {  // CASO 2

                clearPath(&pathrows, &pathcols, nsources, ndest);
                initTabu();

                pathrows[0]=source;
                pathcols[0]=ndest-1;

                rs = fastFindPathR(source, ndest-1,
                                   source, ndest-1,
                                   &pathrows, &pathcols,
                                   1, tta,
                                   &russelmatrix, &tpuser,
                                   nusers, nsources, ndest,
                                   true,   // payAttention
                                   false); // depAllowed


                flag = false;
                rs = false;
                if ( rs ) {
                    for ( i=0; pathrows[i]!=-1; i++ ) {
                        if ( rmax==pathrows[i] && j==pathcols[i] ) {
                            flag = true;
                            break;
                            }
                        }
                    }

                if ( flag ) { // Soluzione 0 [C2S0]
                    fprintf(stderr, "[C2S0] Path found\n");

                    for ( i=0; pathrows[i]!=-1; i++ ) {

                        r = pathrows[i];
                        c = pathcols[i];

                        if ( (i%2)==0 ) { // indice pari -> +
                            fprintf(stderr, "Aggiungo  %2d a (%3d,%3d)\n", tta, r, c);
                            if ( russelmatrix[r][c]==-1 )
                                russelmatrix[r][c]  = tta;
                            else
                                russelmatrix[r][c] += tta;

                            }
                        else { // indice dispari -> -
                            fprintf(stderr, "Sottraggo %2d a (%3d,%3d)\n", tta, r, c);
                            russelmatrix[r][c] -= tta;
                            }

                        }
                    }

                else { // Soluzioni 1/2
                    clearPath(&pathrows, &pathcols, nsources, ndest);
                    initTabu();

                    pathrows[0]=source;
                    pathcols[0]=j;

                    pathrows[1]=rmax;
                    pathcols[1]=j;

                    rs = fastFindPathR(rmax, j,
                                       source, j,
                                       &pathrows, &pathcols,
                                       2, ttm,
                                       &russelmatrix, &tpuser,
                                       nusers, nsources, ndest,
                                       true, // payAttention
                                       true); // depAllowed

                    if ( rs ) { // Soluzione 1 [C2S2]
                        fprintf(stderr, "[C2S1] Path found\n");
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
                        }
                    else { // Soluzione 2 [C2S2]
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


                            }
                        else
                            fprintf(stderr, "[C2S2] No Path found\n");

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



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////





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


for ( dest=0; dest<ndest; dest++ )
    fprintf(stderr, "%d ", v[dest]);
fprintf(stderr, "\n");
for ( dest=0; dest<ndest; dest++ )
    fprintf(stderr, "%d ", u[dest]);
fprintf(stderr, "\n");


/* FASE 1                                                               **
** Per tutte quelle celle che violano il vincolo ma che sono da sole    **
** sulla colonna, non resta che:                                        **
**   1. Allocare una nuova cella, che non violi i vincoli e che abbia   **
**      il prezzo piu' basso possibile                                  **
**   2. Aggiungere task fino a far rispettare i vincoli se cio' che     **
**      pago sovrabbondando con i task e' inferiore a cio' che pagherei **
**      spostando quelli in eccesso (a patto di avere disponibilita')   */

int ttm;    // TaskToMove
int tta;    // TaskToAdd
cost  =-1;  // costo cella con violazione
int costA =-1;  // costo abbondando
rmax  =-1;  // riga cella con violazione
user  =-1;  // user cella con violazione
tpu   =-1;  // TaskPerUser cella con violazione

int cuser =-1;      // current user
int ctpu  =-1;      // current TaskPerUser
int rmin  =-1;      // current riga con costo minimo
cmin  =INT_MAX; // costo minimo
ccost =-1;      // current costo
int canA  = false;
int r, c;
int mcm;



for ( j=0; j<ndest; j++ ) {
    if ( v[j]==1 ) {

        cmin  =INT_MAX; // costo minimo
        rmax  = u[j]; // indice di riga CVIV
        user  = rmax%nusers; // tipo user CVIC
        tpu   = tpuser[user]; // TPU CVIV
        cost  = russelcosts[rmax][j]; // costo CVIV
        ttm   = russelmatrix[rmax][j]%tpu; // Task To Move
        tta   = tpu - ttm; // Task To Add


        /* Se la cella si e' messa a posto grazie a poteri              **
        ** sovrannaturali la ignoro                                     */
        if ( ttm==0 )
            continue;


        /* Per avere un confronto reale tra il costo di spostare i task
        ** in eccesso e quello di sovrabbondare per far rispettare il
        ** TPU, dovremmo sottrarre al primo costo il guagno dal
        ** togliere i task in ecceso dalla CVIV. Per evitare cio'
        ** facci ola cosa contraria e considero il secondo costo come
        ** il costo di spostare TPU tasks nella CVIV.
        **
        ** Ricordo che
        **                 tta + ttm = tpu
        **
        ** In un primo momento inoltre considero il secondo costo
        ** solamente se ho disponibilita' di TTA task nel deposito
        ** della CVIV.                                                  */
        if ( russelmatrix[rmax][ndest-1]>=tta )
            costA = cost*tpu;
        else
            costA = INT_MAX;

        fprintf(stderr, "Cella (%3d,%3d), unica sulla colonna\n", rmax, j);

        for ( source=0; source<nsources; source++ ) {
            /* se ritorno sulla cella che viola la salto                */
            if ( source==rmax )
                continue;

            cuser = source%nusers;
            ctpu  = tpuser[cuser];

            /* Se la cella e' occupata gli aggiungo i TaskToMove;       **
            **    se non rispetta i vincoli                             **
            **        vado avanti;                                       **
            **    altrimenti                                            **
            **        contronto il costo con quello minimo;              */
            if ( russelmatrix[source][j]>0 ) {
                // fprintf(stderr, "tovata cella occupata (%3d,%3d)\n", source, j);

                if ( ((russelmatrix[source][j]+ttm)%ctpu)==0 ) {
                    ccost = ttm * russelcosts[source][j];

                    // fprintf(stderr, "E' compatibile, costo %d\n",ccost);
                    if ( ccost<cmin ) {
                        cmin = ccost;
                        rmin = source;
                        }

                    }
                else {
                    if ( ((russelmatrix[source][j]+ttm+tpu)%ctpu)==0 ) {
                        ccost = (ttm+tpu) * russelcosts[source][j];

                        if ( ccost<cmin ) {
                            cmin = ccost;
                            rmin = source;
                            }

                        }
                    }
                }

            else { // russelmatrix[source][j] == -1
                if ( modi[source][j]==0 ) {
                    fprintf(stderr, "Cella modi costo 0! (%3d,%3d)\n", source, j);

                    /* Se la matrice modi e' 0, significa che ci posso  **
                    ** spostare tutti i task che voglio senza che il    **
                    ** costo della mia obj. func. cresca                **
                    ** quindi se il modulo tra i tpu della cella        **
                    ** corrente e i tpu della cella che viola i vincoli **
                    ** da come resto esattamente i TaskToMove, oppure   **
                    ** i TaskToMove gia' soddisfano i task per user     **
                    ** della cella, siamo a cavallo                     **
                    ** Inoltre, per la proprieta' della matrice MODI,   **
                    ** data una cella a costo 0, c'e' sempre un path    **
                    ** per spostarci i task; lungo il path rispettero'  **
                    ** i vincoli?                                       */
                    if ( (ttm%ctpu)==0 || (ctpu%tpu)==ttm ) {

                        ccost = 0;
                        if ( ccost<cmin ) {
                            cmin = ccost;
                            rmin = source;
                            }

                        }
                    }

                else { // modi[source][j] > 0
                    // fprintf(stderr, "Cella modi costo maggiore di zero (%3d,%3d)\n", source, j);

                    /* dato che la cella non e' a costo 0, ci muovo lo  **
                    ** stetto necessario                                **
                    **   - i ttm se non violano i vincoli               **
                    **   - i tpu+ttm se non violano i vincoli e         **
                    **     comunque hanno un costo inferiore            **
                    **     all'attuale costo minimo                     **
                    **     (piu' unico che raro)                        */

                    if ( (ttm%ctpu)==0 ) {

                        ccost = ttm*russelcosts[source][j];
                        // fprintf(stderr, "ttm compatibili, costo %d\n", ccost);
                        if ( ccost<cmin ) {
                            cmin = ccost;
                            rmin = source;
                            }

                        }
                    else {
                        if ( (ctpu%tpu)==ttm ) {

                            ccost = (tpu+ttm)*russelcosts[source][j];
                            // fprintf(stderr,
                            //         "ttm compatibili come resto, costo %d\n",
                            //         ccost);
                            if ( ccost<cmin ) {
                                cmin = ccost;
                                rmin = source;
                                }

                            }
                        }
                    }
                }
            } // for each source

        /* NON dimentichiamoci che, se abbondare conviene piu' di       **
        ** togliere i task in eccesso, allora con fare DECISO           **
        ** abbondiamo                                                   **
        **        "Melius abundare quam deficere".                      **
        **                            Kebabbaro con cipolla in mano.    */
        if ( costA<cmin ) {
            cmin = costA;
            rmin = rmax;
            fprintf(stderr, "Melius abundare quam deficere.\n");
            }

        source = rmin;
        cuser  = source%nusers;
        ctpu   = tpuser[cuser];


        fprintf(stderr, "ha come minimo (%3d,%3d), costo %d (ttm %d)\n\n",
                source, j, cmin, ttm);


        /* Prima di procedere devo capire quanti task ho deciso di      **
        ** spostare.                                                    */
        if ( russelmatrix[source][dest]==-1 ) {
            if ( (ttm%ctpu)!=0 ) { // controesempio ttm 2 su una CACM con tpu=1
                if ( ((ttm+tpu)%ctpu)==0 )
                    ttm += tpu;
                else
                    fprintf(stderr, "WHAT THE TPU TTM HACK?!\n");
                }
            }
        else {
            if ( russelmatrix[source][dest]>=0 ) {
                if ( ((russelmatrix[source][j]+ttm)%ctpu)!=0 ) {
                    if ( ((russelmatrix[source][j]+ttm+tpu)%ctpu)==0 )
                        ttm += tpu;
                    else
                        fprintf(stderr, "WHAT THE TPU TTM HACK SECOND?!\n");
                    }
                }
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
        **   la cella era una CVIV e risolvo la vuolazione, oppure,
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
        ** Soluzione 0 [C2S0]
        ** Dato che il deposito della CACM e' vuoto, faccio il giro
        ** nell'altro senso, spostando i TTA.
        ** Le caratteristiche di questo path sono le stesse del path di
        ** [C1S1]:
        **   - e' vietato passare dal deposito se non ha task
        **   - nei depositi non si controllano le violazioni
        **   - tutte le celle all'interno del path devono essere CNVIV
        **     o diventarlo dopo l'attraversamento
        **   - i TTA sono fissi
        **
        ** [WARN] Il path e' valido SSE la CVIV originaria vi e'
        **        compresa
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

            if ( rs ) { // Soluzione 1 [C1S1]
                fprintf(stderr, "[C1S1] Path found\n");
                for ( i=0; pathrows[i]!=-1; i++ ) {
                    r = pathrows[i];
                    c = pathcols[i];

                    if ( (i%2)==0 ) { // indice pari -> +
                        fprintf(stderr, "Aggiungo a (%3d,%3d)\n", r, c);
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
                }
            else { // Soluzione 2 [C1S2]
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
                }



            // EMERGENZA CASO 1?

            }
        else {  // CASO 2

            clearPath(&pathrows, &pathcols, nsources, ndest);
            initTabu();

            pathrows[0]=source;
            pathcols[0]=ndest-1;

            rs = fastFindPathR(source, ndest-1,
                               source, ndest-1,
                               &pathrows, &pathcols,
                               1, tta,
                               &russelmatrix, &tpuser,
                               nusers, nsources, ndest,
                               true,   // payAttention
                               false); // depAllowed


            flag = false;
            rs = false;
            if ( rs ) {
                for ( i=0; pathrows[i]!=-1; i++ ) {
                    if ( rmax==pathrows[i] && j==pathcols[i] ) {
                        flag = true;
                        break;
                        }
                    }
                }

            if ( flag ) { // Soluzione 0 [C2S0]
                fprintf(stderr, "[C2S0] Path found\n");

                for ( i=0; pathrows[i]!=-1; i++ ) {

                    r = pathrows[i];
                    c = pathcols[i];

                    if ( (i%2)==0 ) { // indice pari -> +
                        fprintf(stderr, "Aggiungo  %2d a (%3d,%3d)\n", tta, r, c);
                        if ( russelmatrix[r][c]==-1 )
                            russelmatrix[r][c]  = tta;
                        else
                            russelmatrix[r][c] += tta;

                        }
                    else { // indice dispari -> -
                        fprintf(stderr, "Sottraggo %2d a (%3d,%3d)\n", tta, r, c);
                        russelmatrix[r][c] -= tta;
                        }

                    }
                }

            else { // Soluzioni 1/2
                clearPath(&pathrows, &pathcols, nsources, ndest);
                initTabu();

                pathrows[0]=source;
                pathcols[0]=j;

                pathrows[1]=rmax;
                pathcols[1]=j;

                rs = fastFindPathR(rmax, j,
                                   source, j,
                                   &pathrows, &pathcols,
                                   2, ttm,
                                   &russelmatrix, &tpuser,
                                   nusers, nsources, ndest,
                                   true, // payAttention
                                   true); // depAllowed

                if ( rs ) { // Soluzione 1 [C2S2]
                    fprintf(stderr, "[C2S1] Path found\n");
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
                    }
                else { // Soluzione 2 [C2S2]
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


                        }
                    else
                        fprintf(stderr, "[C2S2] No Path found\n");

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












/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////







    //
    // for ( source=0; source<nsources; source++ )
    //     for ( dest=0; dest<ndest-1; dest++ ) {
    //         if ( russelmatrix[source][dest]==-1 )
    //             continue;
    //
    //         user = source%nusers;
    //         tpu=tpuser[user];
    //
    //         if ( russelmatrix[source][dest]%tpu!=0 )
    //             solveConflicts(source, dest,
    //                            &pathrows, &pathcols,
    //                            &russelmatrix, &russelcosts,
    //                            &modi, &tpuser, nsources,
    //                            ndest, nusers);
    //     }



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


// VARI TIPI DI AGGIUSTAMENTI




   /* se ci sono variabili a 0 nella modi matrix, significa che spostare   **
   ** dei task in queste posizioni non costa nulla dal punto di vista      **
   ** della obj.func.; pero' farlo in modo attento potrebbe significare    **
   ** risolvere problemi con il numero di task allocati lungo il path      */
   int kmax=INT_MAX;
   #ifdef _debugPrintStepByStepModi
   fprintf(stderr, "AGGIORNAMENTO 0 ------------------------------------\n");
   fprintf(stderr, "Sfruttare tutti gli zeri della modi matrix.\n");
   #endif // _debugPrintStepByStepModi
   for ( source=0; source<nsources; source++ )
           for ( dest=0; dest<ndest-1; dest++ ) {
               /* ignoro le variabili gia' presenti nella base             */
               if ( russelmatrix[source][dest]!=-1 )
                   continue;
               /* ignoro quelle posizioni nelle quali, se dovessi spostare **
               ** task avrei un aumento del costo complessivo              */
               if ( modi[source][dest]>0 )
                   continue;


               ccost = russelcosts[source][dest];
               k = (source%nusers)+1;
               kmax=INT_MAX;

               clearPath(&pathrows, &pathcols, nsources, ndest);
               initTabuM();
               initTabu();

               pathrows[0]=source;
               pathcols[0]=dest;

               fprintf(stderr, "Parto da (%3d,%3d)\n", source, dest);

               rs = findPathR(source, /* starting row coordinate                     */
                              dest, /* starting column coordinate                  */
                              source, /* destination row coordinate                  */
                              dest, /* destination column coordinate               */
                              &pathrows, /* vettore indici di riga del path        */
                              &pathcols, /* vettore indici di colonna del path     */
                              1,  /* la prima posizione e' la starting cell        */
                              -1, /* inizialmente nessuna direzione e' vietata     */
                              &russelmatrix,
                              &russelcosts,
                              nsources,
                              ndest);

               if ( rs ) {
                   fprintf(stderr, "A0: path found:\n");
                   i=0;
                   while ( pathrows[i]!=-1 ) {
                       fprintf(stderr, "    (%3d,%3d)\n", pathrows[i], pathcols[i]);
                       i++;
                       }
                   }
               else {
                   fprintf(stderr, "A0: NO PATH FOUND\n");
                   }

               rs = aggiustaR(source, dest,
                              source, dest,
                              &pathrows, &pathcols, 1,
                              &k, &kmax,
                              colonna, /* verso */
                              0, 0,
                              minus,
                              &russelmatrix, &russelcosts, &modi,
                              nsources, ndest, nusers);

               /* se ho trovao un path, controllo se lungo la strada ho    **
               ** incongruenze di task, se cosi fosse effettuo lo          **
               ** spostamento dei task                                     */
               if (rs) {
                   i=0;
                   fprintf(stderr, "PATH:\n");
                   while ( pathrows[i]!=-1 ) {
                       fprintf(stderr, "(%3d,%3d)\n", pathrows[i], pathcols[i]);
                       i++;
                       }

                   nx=i;
                   if ( pathrows[i-1]==pathrows[0] && pathcols[i-1]==pathcols[0] )
                       nx--;

                   ccost=0;
                   for ( i=0; i<nx; i++ ) {
                       if ( i%2==0 )
                           ccost += russelcosts[pathrows[i]][pathcols[i]];
                       else
                           ccost -= russelcosts[pathrows[i]][pathcols[i]];
                       }
                   fprintf(stderr, "Guadagno: %4d, Task spostati %d\n", ccost, k);

                   flag=false;
                   for ( i=1; i<nx; i++ ) {
                       if ( russelmatrix[pathrows[i]][pathcols[i]]%(pathrows[i]%nusers+1) != 0 ) {
                           fprintf(stderr, "la (%3d,%3d) ha task sballati\n", pathrows[i], pathcols[i]);
                           flag=true;
                           break;
                           }
                       }

                   /* override per dire che se modi mi dice che conviene,  **
                   ** task sballati o meno, lo faccio                      */
                   if ( modi[source][dest]<0 ) {
                       fprintf(stderr, "I task stavano a posto ma modi dice che conviene. Path seguito.\n");
                       flag=true;
                       }

                   if ( flag )
                       for ( i=0; i<nx; i++ ) {
                           if ( i%2==0 ) {
                               if ( russelmatrix[pathrows[i]][pathcols[i]]==-1 )
                                   russelmatrix[pathrows[i]][pathcols[i]]  = k;
                               else
                                   russelmatrix[pathrows[i]][pathcols[i]] += k;
                               }
                           else {
                               russelmatrix[pathrows[i]][pathcols[i]] -= k;

                               if ( russelmatrix[pathrows[i]][pathcols[i]]==0 )
                                   russelmatrix[pathrows[i]][pathcols[i]] =-1;
                               }
                           }
                   else
                       fprintf(stderr, "Path non seguito, i task stavano a posto\n");
                   }
               else
                       fprintf(stderr, "NO PATH!\n");
               }

   fprintf(stderr, "\n\n");
   fprintf(stderr, "========================================================\n");
   fprintf(stderr, "    AGGIORNAMENTO 1 ------------------------------------\n");
   fprintf(stderr, "Varie strategie...\n");
   /* per le variabili gia' esistenti le quali pero' presentano            **
   ** un'incongruenza:                                                     **
   **  1. provo a togliere i task in eccesso seguendo un path,
   **     eventualmente aggiungendo un'allocazione lungo il path;          **
   **     tale path deve avere un costo complessivo inferiore al costo di  **
   **     aggiungere task alla cella di partenza per rendere i task        **
   **     congruenti                                                       **
   **     CASO EXTREMIS: tolgo i task e li alloco per la stessa            **
   **                    destinazione da un'altra sorgente con task nel    **
   **                    deposito                                          **
   **  2. provo ad aggiungere i task mancanti seguendo un path             **
   **     CASO EXTREMIS: aggiungo i task dal deposito della sorgente       **
   **                    stessa, se ne ha disponibilita'                   **
   **  3. confronto i costi delle varie soluzioni fattibili, scelto il     **
   **     costo inferiore                                                  */

   int costoUno=INT_MAX; /* 1.                                             */
   int costoDue=INT_MAX; /* 1. caso extremis                               */
   int costoTre=INT_MAX; /* 2.                                             */
   int costoQua=INT_MAX; /* 2. caso extremis                               */
   int zmin;
   int pt;
   int usertasks;
   int lowcost;
   for ( source=0; source<nsources; source++ )
           for ( dest=0; dest<ndest-1; dest++ ) {
               if ( russelmatrix[source][dest]==-1 )
                   continue;

               user  = source%nusers;
               tpu   = user+1;
               ccost = russelcosts[source][dest];
               k     = russelmatrix[source][dest]%tpu;


               if ( russelmatrix[source][dest]%tpu==0 )
                   continue;

               fprintf(stderr, "\n\n"
                       "---------------------------------------------------\n"
                       "--------------- |  cella (%3d,%3d)  | -------------\n"
                       "---------------------------------------------------\n",
                       source, dest);

               costoUno=INT_MAX; /* 0.                                     */
               costoDue=INT_MAX; /* 1.                                     */
               costoTre=INT_MAX; /* 2.                                     */
               costoQua=INT_MAX; /* 2. caso extremis                       */



               /* per calcolare il costo peggiore, invece di seguire un    **
               ** path considero la semplice allocazione di altri utenti   **
               ** compatibili con il numero di task in eccesso             **
               ** se il costo totale lungo il path e' inferiore al costo   **
               ** allocare a costo minimo altri utenti allora potrebbe     **
               ** convenire il path                                        */

               /* STRATEGIA 0 -------------------------------------------- */
               fprintf(stderr, "\nSTRATEGIA 0: nuova allocazione per dest.\n");
               cmin = INT_MAX;

               for ( i=0; i<nsources; i++ ) {
                   /* non allocare nella stessa cella                      */
                   if ( i==source )
                       continue;


                   /* ci sono task neldeposito?                            */
                   if ( russelmatrix[i][ndest-1]<k )
                       continue;

                   /* ci osno gia' allocati altri task?                    */
                   if ( russelmatrix[i][dest] !=-1 )
                       pt = russelmatrix[i][dest];
                   else
                       pt = 0;
                   /* rovino la festa anche a questa cella?                */
                   if ( (k+pt)%((i%nusers)+1)!=0 )
                       continue;

                   if ( k*russelcosts[i][dest]<cmin ) {
                       cmin = k*russelcosts[i][dest];
                       fprintf(stderr, "new b.allocation (%3d,%3d), $$ %d\n",
                               i, dest, russelcosts[i][dest]);
                       }
                   }

               if ( cmin != INT_MAX ) {
                   costoDue = cmin - k*russelcosts[source][dest];

                   fprintf(stderr, "COSTO: %4d, Task da spostare %d\n", costoDue, k);
                   }
               else
                   fprintf(stderr, "Nessuna altra sorgente disponibile\n");

               // ========================================================
               // ========================================================
               // STRATEGIA 0 bis
               int kk=k;
               int daSpostare;
               int costoCin=INT_MAX;
               int gooddest=-1;
               for ( i=0; i<nsources; i++ ) {

                   usertasks=(i%nusers)+1;
                   /* non allocare nella stessa cella                      */
                   if ( i==source )
                       continue;

                   if ( russelmatrix[i][dest] !=-1 )
                       pt = russelmatrix[i][dest];
                   else
                       pt = 0;

                   while ( (pt+kk)%usertasks!=0 )
                       kk++;

                   /* se la modifica va bene alla cella sorgente, siamo a  **
                   ** cavallo                                              */
                   if ( (russelmatrix[source][dest]-kk)%((source%nusers)+1)!=0 ) {
                       kk=k;
                       continue;
                       }

                   if ( russelmatrix[i][ndest-1]<kk ) {
                       kk=k;
                       continue;
                       }

                   if ( (kk*russelcosts[i][dest]-kk*russelcosts[source][dest])<costoCin ) {
                       costoCin = kk*russelcosts[i][dest]-kk*russelcosts[source][dest];
                       daSpostare=kk;
                       gooddest=i;
                       fprintf(stderr, "Cella (%3d,%3d), kk %d, dest %d, CostoCin %d\n",
                               i, dest, kk, gooddest, costoCin);
                       }

                   kk=k;
                   }
               fprintf(stderr, "COSTO 0-Bis: %4d, Task da spostare %d in %d\n",
                       costoCin, daSpostare, gooddest);


               // ========================================================
               // ========================================================
               // ========================================================

               /* STRATEGIA 1 -------------------------------------------- */
               fprintf(stderr, "\nSTRATEGIA 1: tolgo task in eccesso\n");

               clearPath(&pathrows, &pathcols, nsources, ndest);
               initTabuM();
               pathrows[0]=source;
               pathcols[0]=dest;

               fprintf(stderr, "Parto da (%3d,%3d)\n", source, dest);

               rs = aggiustaR(source, dest,
                              source, dest,
                              &pathrows, &pathcols, 1,
                              &k, &kmax,
                              colonna,
//                               0, cmin,
                              0, costoDue, // la sottrazione dei task la fa alla fine
                              plus,
                              &russelmatrix, &russelcosts, &modi,
                              nsources, ndest, nusers);

               if ( rs ) {
                   i=0;
                   fprintf(stderr, "PATH:\n");
                   while ( pathrows[i]!=-1 ) {
                       fprintf(stderr, " (%3d,%3d)\n", pathrows[i], pathcols[i]);
                       i++;
                       }

                   nx=i;
                   if ( pathrows[i-1]==pathrows[0] && pathcols[i-1]==pathcols[0] )
                       nx--;

                   costoUno=0;
                   for ( i=0; i<nx; i++ ) {
                       if ( i%2==0 )
                           costoUno -= k*russelcosts[pathrows[i]][pathcols[i]];
                       else
                           costoUno += k*russelcosts[pathrows[i]][pathcols[i]];
                       }
                   fprintf(stderr, "COSTO: %4d, Task da spostare %d\n", costoUno, k);

                   }
               else
                   fprintf(stderr, "NO PATH!\n");

               // =========================================================
               // =========================================================
               // =========================================================
               // =========================================================
               k = tpu - k; /* K cambia!                                   */
               // =========================================================
               // =========================================================

               /* STRATEGIA 2: CASO EXTREMIS                               */
               fprintf(stderr, "\nSTRATEGIA 2: CASO EXTREMIS --- (aggiungo dal deposito, abbondo)\n");
               if ( russelmatrix[source][ndest-1]>=k ) {
                   costoQua = k*ccost;
                   costoQua = k*ccost;
                   fprintf(stderr, "COSTO: %4d, Task da spostare %d\n", costoQua, k);
                   }
               else
                   fprintf(stderr, "Nessun task nel deposito\n");


             /* STRATEGIA 2 --------------------------------------------   */
               fprintf(stderr,"\nSTRATEGIA 2: aggiungo task per abbondare (path)\n");
               fprintf(stderr,"min cost finora %d\n", min((min(costoUno, costoDue)), costoTre));

               /* provo ad aggiungere i task                               */
               clearPath(&pathrows, &pathcols, nsources, ndest);
               pathrows[0]=source;
               pathcols[0]=dest;

               kmax=k;
               initTabuM();

               fprintf(stderr, "Parto da (%3d,%3d)\n", source, dest);
               rs = aggiustaR(source, dest,
                              source, dest,
                              &pathrows, &pathcols, 1,
                              &k, &kmax,
                              colonna, /* verso */
                              0, min((min(costoUno, costoDue)), costoTre),
                              minus,
                              &russelmatrix, &russelcosts, &modi,
                              nsources, ndest, nusers);
               if (rs) {

                   i=0;
                   fprintf(stderr, "PATH:\n");
                   while ( pathrows[i]!=-1 ) {
                       fprintf(stderr, "(%3d,%3d)\n", pathrows[i], pathcols[i]);
                       i++;
                       }

                   nx=i;
                   if ( pathrows[i-1]==pathrows[0] && pathcols[i-1]==pathcols[0] )
                       nx--;

                   costoTre=0;
                   for ( i=0; i<nx; i++ ) {
                       if ( i%2==0 )
                           costoTre += k* russelcosts[pathrows[i]][pathcols[i]];
                       else
                           costoTre -= k* russelcosts[pathrows[i]][pathcols[i]];
                       }
                   fprintf(stderr, "COSTO: %4d, Task da spostare %d\n", costoTre, k);
                   }
               else
                   fprintf(stderr, "NO PATH!\n");





               /* -------------------------------------------------------- */
               /*                          ACTION                          */
               /* -------------------------------------------------------- */
               user = source%nusers;
               tpu  = user+1;

               k     = russelmatrix[source][dest]%tpu;

               int flag0=false;
               int flag1=false; // nuova allocazione
               int flag2=false;
               int flag3=false;
               int flag4=false;


               zmin=INT_MAX;

               if ( costoCin<zmin ) { // riallocazione intelligente
                   zmin=costoCin;
                   flag0=true;
                   }

               if ( costoUno<zmin ) { // riallocazione path
                   zmin=costoUno;
                   flag1=true;

                   flag0=false;
                   }

               if ( costoDue<zmin ) { // riallocazione spontanea
                   zmin=costoDue;
                   flag2=true;

                   flag0=false;
                   flag1=false;
                   }

               if ( costoTre<zmin ) { // sovradosaggio tasks path
                   zmin=costoTre;
                   flag3=true;

                   flag0=false;
                   flag1=false;
                   flag2=false;
                   }

               if ( costoQua<zmin ) { // sovradosaggio spontaneo
                   zmin=costoQua;
                   flag4=true;

                   flag0=false;
                   flag1=false;
                   flag2=false;
                   flag3=false;
                   }



               if ( flag0 ) {
                   fprintf(stderr, "Eseguo strategia 0 bis per (%3d,%3d)\n", source, dest);

                   k=daSpostare;
                   i=gooddest;

                   /* alloco i task nella nuova cella                      */
                   if ( russelmatrix[i][dest]==-1 )
                       russelmatrix[i][dest]  = k;
                   else
                       russelmatrix[i][dest] += k;
                   /* tolgo i task dal deposito della nuova cella          */
                   russelmatrix[i][ndest-1] -=k;
                   if ( russelmatrix[i][ndest-1]==0 )
                       russelmatrix[i][ndest-1]=-1;

                   /* tolgo i task dalla cella sorgente                    */
                   russelmatrix[source][dest] -= k;
                   if ( russelmatrix[source][dest]==0 )
                       russelmatrix[source][dest]=-1;
                   /* aggiungo i task al deposito della cella sorgente     */
                   if ( russelmatrix[source][ndest-1]==-1 )
                       russelmatrix[source][ndest-1] = k;
                   else
                       russelmatrix[source][ndest-1] += k;

                   }



               if ( flag1 ) {
                   fprintf(stderr, "Eseguo strategia 1 per (%3d,%3d)\n", source, dest);
                   k     = russelmatrix[source][dest]%tpu;
                   /* provo a togliere i task                                  */
                   clearPath(&pathrows, &pathcols, nsources, ndest);
                   initTabuM();
                   pathrows[0]=source;
                   pathcols[0]=dest;

                   rs = aggiustaR(source, dest,
                                  source, dest,
                                  &pathrows, &pathcols, 1,
                                  &k, &kmax,
                                  colonna,
                                  0, costoDue,
                                  plus,
                                  &russelmatrix, &russelcosts, &modi,
                                  nsources, ndest, nusers);
                   i=0;
                   while ( pathrows[i]!=-1 )
                       i++;

                   nx=i;
                   if ( pathrows[i-1]==pathrows[0] && pathcols[i-1]==pathcols[0] )
                       nx--;

                   for ( i=0; i<nx; i++ ) {
                       if ( i%2 ==0 ) {
                           russelmatrix[pathrows[i]][pathcols[i]] -= k;
                           if ( russelmatrix[pathrows[i]][pathcols[i]]==0 )
                               russelmatrix[pathrows[i]][pathcols[i]] =-1;
                           }
                       else {
                           if ( russelmatrix[pathrows[i]][pathcols[i]]==-1 )
                               russelmatrix[pathrows[i]][pathcols[i]]  = k;
                           else
                               russelmatrix[pathrows[i]][pathcols[i]] += k;
                           }
                       }
                   }


               if ( flag2 ) {
                   k     = russelmatrix[source][dest]%tpu;
                   cmin = INT_MAX;
                   int pt;
                   for ( i=0; i<nsources; i++ ) {
                       /* non allocare nella stessa cella                      */
                       if ( i==source )
                           continue;
                       /* ci sono task neldeposito?                            */
                       if ( russelmatrix[i][ndest-1]<k )
                           continue;
                       /* ci osno gia' allocati altri task?                    */
                       if ( russelmatrix[i][dest] !=-1 )
                           pt = russelmatrix[i][dest];
                       else
                           pt = 0;
                       /* rovino la festa anche a questa cella?                */
                       if ( (k+pt)%((i%nusers)+1)!=0 )
                           continue;

                       if ( k*russelcosts[i][dest]<cmin ) {
                           cmin = k*russelcosts[i][dest];
                           rmax = i;
                           }
                       }
                   fprintf(stderr, "Eseguo strategia 0: alloco nuovi task in (%3d,%3d)\n", rmax, dest);

                   if ( russelmatrix[rmax][dest]==-1 )
                       russelmatrix[rmax][dest]  = k;
                   else
                       russelmatrix[rmax][dest] += k;

                   russelmatrix[rmax][ndest-1] -= k;
                   if ( russelmatrix[rmax][ndest-1]==0 )
                       russelmatrix[rmax][ndest-1]=-1;

                   russelmatrix[source][dest] -= k;
                   if ( russelmatrix[source][dest]==0 )
                       russelmatrix[source][dest]=-1;

                   if ( russelmatrix[source][ndest-1]==-1 )
                       russelmatrix[source][ndest-1]  = k;
                   else
                       russelmatrix[source][ndest-1] += k;
                   }


               if ( flag3 ) {
                   fprintf(stderr, "Eseguo strategia 2 per (%3d,%3d)\n", source, dest);
                   k     = russelmatrix[source][dest]%tpu;
                   k     = tpu - k;
                   clearPath(&pathrows, &pathcols, nsources, ndest);
                   initTabuM();
                   pathrows[0]=source;
                   pathcols[0]=dest;
                   kmax=k;

                   rs = aggiustaR(source, dest,
                                  source, dest,
                                  &pathrows, &pathcols, 1,
                                  &k, &kmax,
                                  colonna, /* verso */
                                  0, min((min(costoUno, costoDue)), costoTre),
                                  minus,
                                  &russelmatrix, &russelcosts, &modi,
                                  nsources, ndest, nusers);

                   i=0;
                   while ( pathrows[i]!=-1 )
                       i++;

                   nx=i-1;
                   for ( i=0; i<nx; i++ ) {
                       if ( i%2==0 ) {
                           if ( russelmatrix[pathrows[i]][pathcols[i]]==-1 )
                                   russelmatrix[pathrows[i]][pathcols[i]]  = k;
                           else
                               russelmatrix[pathrows[i]][pathcols[i]] += k;
                           }
                       else {
                           russelmatrix[pathrows[i]][pathcols[i]] -= k;

                           if ( russelmatrix[pathrows[i]][pathcols[i]]==0 )
                               russelmatrix[pathrows[i]][pathcols[i]] =-1;
                           }
                       }
                   }



               if ( flag4 ) {
                   fprintf(stderr, "Eseguo strategia '2:CE': aggiungo dal deposito per (%3d,%3d)\n", source, dest);
                   k     = russelmatrix[source][dest]%tpu;
                   k     = tpu - k;
                   russelmatrix[source][ndest-1] -= k;
                   if ( russelmatrix[source][ndest-1]==0 )
                       russelmatrix[source][ndest-1] =-1;

                   russelmatrix[source][dest] += k;
                   }




               #ifdef _debugPrintStepByStepPrintFinal
               fprintf(stderr,
                       "----------------------------------------------------------------------------------------------------\n");
               fprintf(stderr, " Aggiustato (%3d,%3d)\n", source, dest);
               fprintf(stderr,
                       "----------------------------------------------------------------------------------------------------\n");

               fprintf(stderr, "  tpu ;");
               fprintf(stderr, " [%2d] ;", itera);
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



               }


    /* raggruppo le allocazioni in quante meno sorgenti possibile, magari   **
    ** trovando anche qualche percorso che mi porta il bilancio in negativo **
    ** in ogni caso raggruppo solamente se il risultato e' inferiore o      **
    ** uguale alla situazione attuale                                       */
    /*                                      ...    ndest-1
                    |>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> [i-1]
                    |                                [i]
                    |<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< [i+1]
    **                                                                      */

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// NUOVA ITERAZIONE RUSSEL

    /* Inizializzazione 'u', 'v' e 'supply' e 'demand'                      */
    for ( source=0; source<nsources; source++ ) {
        u[source]=INT_MIN;
        supply[source]=-1;
        }

    for ( dest=0; dest<ndest; dest++ ) {
        v[dest]=INT_MIN;
        demand[dest]=-1;
        }

    /* ricerca del massimo per riga e colonna e reset supply e demand       */
    int TaskToMove;
    for ( source=0; source<nsources; source++ ) {
        user = source%nusers;
        tpu  = tpuser[user];

        for ( dest=0; dest<ndest; dest++ ) {

            if ( russelmatrix[source][dest]>0 ) {
                TaskToMove = russelmatrix[source][dest]%tpu;
                if ( TaskToMove>0 ) {
                    // reset supply e demand
                    if ( supply[source]==-1 )
                        supply[source]  = TaskToMove;
                    else
                        supply[source] += TaskToMove;

                    if ( demand[dest]==-1 )
                        demand[dest]  = TaskToMove;
                    else
                        demand[dest] += TaskToMove;

                    // rimozione eccesso
                    russelmatrix[source][dest] -= TaskToMove;
                    }

                if ( dest==ndest-1 ) {
                    if ( supply[source]==-1 )
                        supply[source]  = russelmatrix[source][dest];
                    else
                        supply[source] += russelmatrix[source][dest];
                    }
                }


            ccost=russelcosts[source][dest];
            /* controllo se ho trovato un massimo per la riga "source"      */
            if ( ccost>u[source] )
                u[source]=ccost;

            /* controllo se ho trovato un massimo per la colonna "dest"     */
            if ( ccost>v[dest] )
                v[dest]=ccost;

            }
        }

    maxdiff=INT_MAX;
    rmax=-1;
    cmax=-1;
    /* calcola le delta per ogni cella d[i][j] = c[i][j]-u[i]-v[j]          */
    for ( source=0; source<nsources; source++ ) {
        user = source%nusers;
        tpu  = tpuser[user];


        for ( dest=0; dest<ndest; dest++ ) {

            ccost=russelcosts[source][dest];
            russelweights[source][dest]=ccost-u[source]-v[dest];

            if ( demand[dest]%tpu==0 &&
                 supply[source]>=demand[dest] &&
                 demand[dest]>0 ) {



                if ( russelweights[source][dest]<maxdiff ) {
                    maxdiff=russelweights[source][dest];
                    rmax=source;
                    cmax=dest;
                    }

                }
            }
        }



    fprintf(stderr, "Trovato minimo (%3d,%3d) demand %3d supply %3d\n",
            rmax, cmax, demand[cmax], supply[rmax]);


    itera = 0;
    for ( dest=0; dest<ndest; dest++ ) {
        if ( demand[dest]!=-1)
            itera++;
        }

    while (itera>1) {
        fprintf(stderr, " PRIMA DELL'ASSEGNAZIONE\n");
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



        /* RICERCA DEL MASSIMO MINIMO ===================================== */
        maxdiff=INT_MAX;
        rmax=-1;
        cmax=-1;
        for ( source=0; source<nsources; source++ ) {
            if ( supply[source]==-1 )
                continue;

            user = source%nusers;
            tpu  = tpuser[user];


            for ( dest=0; dest<ndest; dest++ ) {
                if ( demand[dest]==-1 )
                    continue;

                if ( demand[dest]%tpu==0 &&
                     supply[source]>=demand[dest] &&
                     demand[dest]>0) {

                    if ( russelweights[source][dest]<maxdiff ) {
                        maxdiff=russelweights[source][dest];
                        rmax=source;
                        cmax=dest;
                        }

                    }
                }
            }

        fprintf(stderr, "Trovato minimo (%3d,%3d) demand %3d supply %3d\n",
                rmax, cmax, demand[cmax], supply[rmax]);

        source  = rmax;
        dest    = cmax;

        if ( demand[dest] < supply[source] )
            assignment=demand[dest];
        else
            assignment=supply[source];
            // assignment=tpuser[source%nusers];

        fprintf(stderr, "RAM: assegno %d tasks in (%3d,%3d)\n",
                assignment, source, dest);


        if ( russelmatrix[source][dest]==-1 )
            russelmatrix[source][dest]  = assignment;
        else
            russelmatrix[source][dest] += assignment;

        supply[source] -= assignment;
        demand[dest]   -= assignment;



        if ( supply[source]==0 && demand[dest]==0 ) {
            supply[source]  = -1;
            u[source]       = -1;

        //    if ( itera==1 ) {
        //        demand[dest]    =-1;
        //        v[dest]       =-1;
        //        }
            }

        else {
            if ( supply[source]==0 ) {
                supply[source]  = -1;
                u[source]       = -1;
                }

            else if ( demand[dest]==0 ) {
                demand[dest]    =-1;
                v[dest]         =-1;

                itera--;
                }
            }

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
        }
