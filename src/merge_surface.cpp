#include "merge_surface.h"
#include "util.h"

double** merge_surface(string input_path1, string input_path2, int rows, int cols){
    
    double** surface1 = new double*[rows];
    double** surface2 = new double*[rows];
    double** res = new double*[rows];
    for(int i=0;i<rows;i++){
        surface1[i] = new double[cols]();
        surface2[i] = new double[cols]();
        res[i] = new double[cols]();
    }
    
    CSV2array(input_path1, rows, cols, surface1);
    CSV2array(input_path2, rows, cols, surface2);
    

    for(int i=0;i<rows;i++){
        for(int j=0;j<cols;j++){
            if(surface1[i][j]>=0 && surface2[i][j]>=0){     // situation when both increasing
                res[i][j] = min(surface1[i][j] + surface2[i][j] - 0.5, 1.0);
            }
            else if(surface1[i][j]<0 && surface2[i][j]<0){   // situation when both negative
                res[i][j] = max(surface1[i][j] + surface2[i][j] + 0.5, -1.0);
            }
            else{       // situation when one positive one negative
                res[i][j] = 0.5 + surface1[i][j] + surface2[i][j];
            }
        }
    }
    printArray(rows, cols, res);
    free(surface1);
    free(surface2);
    return res;
}