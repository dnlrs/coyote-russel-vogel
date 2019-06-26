#ifndef _vam_h_
#define _vam_h_

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


#endif // _vam_h_
