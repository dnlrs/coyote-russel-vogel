#include "coyote.h"

#include <assert.h>
#include <math.h>
#include <tgmath.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libgen.h>
#include <limits.h>



int main(int argc, char* argv[])
{
    /* passo il nome del file come parametro */
    if ( argc<2 ) {
        fprintf(stderr, "manca il file.\n");
        return -1;
        }


    initCoiote(argv[2]);

    freeCoiote();

}


