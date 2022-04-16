#include <iostream>
#include <fstream>
#include <string>
#include "util.h"
#include "offline_generation.h"

using namespace std;

int main(int argc, char *argv[]){
    if(string(argv[1])=="-g"){  // Generate offline JND Surface
        int l_ref,h_ref;
        int l_mod,h_mod;
        double threshold;
        // arguments: -g, threshold, lowest reference, highest reference, lowest modification, highest modification
        if (argc < 7){
            cout << "too few arguments" << endl;
            cout << "Usage: ./main.exe -g [threshold] [smallest reference] [largest reference] [smallest modification] [largest modification]" << endl;
            return 0;
        }
        threshold = atof(argv[2]);
        l_ref = atoi(argv[3]);
        h_ref = atoi(argv[4]);
        l_mod = atoi(argv[5]);
        h_mod = atoi(argv[6]);

        double** raw; // 2D array of storing subject test results
        raw = greedy_search(threshold,l_ref,h_ref,l_mod,h_mod);
        array2CSV("./raw_matrix.csv", h_mod-l_mod+1,h_ref-l_ref+1,raw);

        // interpolate untested points
        // this will be handled by calling a separated python script
        // the generated JND profile will be stored as csv file
        string parameter = to_string(l_ref) + " " + to_string(h_ref) + " " + to_string(l_mod) + " " + to_string(h_mod);
        string cmd = "python ./interpolate.py " + parameter;
        system(cmd.c_str());   //modify this line if not match your python
    }
    else if(string(argv[1])=="-m"){     // merge two JND Surface
        // arguments: -m, input JND Surface 1, input JND Surface 1, output path, lowest reference, highest reference, lowest modification, highest modification
         if (argc < 9){
            cout << "too few arguments" << endl;
            cout << "Usage: ./main.exe -m [input filepath 1] [input filepath 2] [output path] [smallest reference] [largest reference] [smallest modification] [largest modification]" << endl;
            return 0;
        }
        string input_path1 = string(argv[2]);
        string input_path2 = string(argv[3]);
        string output_path = string(argv[4]);
        int l_ref = atoi(argv[5]);
        int h_ref = atoi(argv[6]);
        int l_mod = atoi(argv[7]);
        int h_mod = atoi(argv[8]);

        double** surface1;
        double** surface2;
        int rows = h_mod-l_mod+1;
        int cols = h_ref-l_ref+1;
        for(int i=0;i<rows;i++){
            surface1[i] = new double[cols]();
            surface2[i] = new double[cols]();
        }
        
        

    }
    else if(string(argv[1])=="-o"){     // Generate online JND Surface according to Network Condition
        // TODO
    }
    else{
        cout << "Usage:\n-g: Generate offline JND Surface\n-m: Merge two JND Surface\n-o: Generate online JND Surface under current network condition" << endl;
    }

    return 0;
}