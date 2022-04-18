#include "online_generation.h"
#include "util.h"

int f1(int r, double pre_loss, double new_loss){
    int res;
    res = r;    // for debugging
    
    // TO BE DEFINED ACCORDING TO ERROR-CONCEALMENT STRATEGY

    return res;
}

int f2(int m, double pre_loss, double new_loss){
    int res;
    res = m;    // for debugging
    
    // TO BE DEFINED ACCORDING TO ERROR-CONCEALMENT STRATEGY

    return res;
}

double** online_generation(double** origin, double pre_loss, double new_loss, int l_ref, int h_ref, int l_mod, int h_mod, int thread_number){
    int rows = h_mod-l_mod+1;
    int cols = h_ref-l_ref+1;
    double** new_surface = new double*[rows];
    for(int i=0;i<rows;i++){
        new_surface[i] = new double[cols]();
    }

    // loop on all grid to calculate new values
    if(thread_number > 0){
        omp_set_num_threads(thread_number);

        #pragma omp parallel for
        for(int i = 0;i < rows;i++){
            for(int j = 0;j < cols;j++){
                int r = l_ref + j;
                int m = h_mod - i;
                int dist_r = f1(r, pre_loss, new_loss);
                int dist_m = f2(m, pre_loss, new_loss);
                new_surface[i][j] = origin[h_mod-dist_m][dist_r-l_ref];
            }
        }
    }
    else{
        for(int i = 0;i < rows;i++){
            for(int j = 0;j < cols;j++){
                int r = l_ref + j;
                int m = h_mod - i;
                int dist_r = f1(r, pre_loss, new_loss);
                int dist_m = f2(m, pre_loss, new_loss);
                new_surface[i][j] = origin[h_mod-dist_m][dist_r-l_ref];
            }
        }
    }

    printArray(rows,cols,new_surface);
    return new_surface;
}