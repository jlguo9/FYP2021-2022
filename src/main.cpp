#include <iostream>
#include <fstream>
#include <string>
#include "util.h"
#include "offline_generation.h"
#include "merge_surface.h"
#include "online_generation.h"

using namespace std;

int main(int argc, char *argv[]){
    if(string(argv[1])=="-g"){  // Generate offline JND Surface
        int l_ref,h_ref;
        int l_mod,h_mod;
        double threshold;
        // arguments: -g, threshold, lowest reference, highest reference, lowest modification, highest modification
        if (argc < 8){
            cout << "too few arguments" << endl;
            cout << "Usage: ./main.exe -g [output path] [threshold] [smallest reference] [largest reference] [smallest modification] [largest modification]" << endl;
            return 0;
        }
        string output_path = string(argv[2]);
        threshold = atof(argv[3]);
        l_ref = atoi(argv[4]);
        h_ref = atoi(argv[5]);
        l_mod = atoi(argv[6]);
        h_mod = atoi(argv[7]);

        double** raw; // 2D array of storing subject test results
        raw = greedy_search(threshold,l_ref,h_ref,l_mod,h_mod);
        array2CSV("./raw_matrix.csv", h_mod-l_mod+1,h_ref-l_ref+1,raw);

        // interpolate untested points
        // this will be handled by calling a separated python script
        // the generated JND profile will be stored as csv file
        string parameter = output_path + " " + to_string(l_ref) + " " + to_string(h_ref) + " " + to_string(l_mod) + " " + to_string(h_mod);
        string cmd = "python ./interpolate.py " + parameter;
        system(cmd.c_str());   //modify this line if not match your python

        free(raw);
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

        int rows = h_mod-l_mod+1;
        int cols = h_ref-l_ref+1;

        
        double** res;
        res = merge_surface(input_path1, input_path2, rows, cols);
        array2CSV(output_path,rows,cols,res);

        string parameter = output_path + " " + to_string(l_ref) + " " + to_string(h_ref) + " " + to_string(l_mod) + " " + to_string(h_mod);
        string cmd = "python ./heatmap.py " + parameter;
        system(cmd.c_str());   //modify this line if not match your python

        free(res);
    }
    else if(string(argv[1])=="-o"){     // Generate online JND Surface according to Network Condition
        // arguments: -o, input JND Surface, output path, lowest reference, highest reference, lowest modification, highest modification, original loss rate
         if (argc < 9){
            cout << "too few arguments" << endl;
            cout << "Usage: ./main.exe -o [input filepath] [output folder] [smallest reference] [largest reference] [smallest modification] [largest modification] [original loss rate] [thread_number(optional)]" << endl;
            return 0;
        }
        string input_path = string(argv[2]);
        string output_folder = string(argv[3]);
        int l_ref = atoi(argv[4]);
        int h_ref = atoi(argv[5]);
        int l_mod = atoi(argv[6]);
        int h_mod = atoi(argv[7]);
        double pre_loss = atof(argv[8]);

        int thread_number = 0;
        if(argc == 10){
            thread_number = atoi(argv[9]);
        }

        // read in the original JND Surface
        int rows = h_mod-l_mod+1;
        int cols = h_ref-l_ref+1;
        double** origin = new double*[rows];    // the original JND Surface
        for(int i=0;i<rows;i++){
            origin[i] = new double[cols]();
        }
        CSV2array(input_path,rows,cols,origin);

        while(1){
            cout << "Please enter current loss rate (in decimal) (enter -1.0 to quit):" << endl;
            double new_loss;
            scanf("%lf", &new_loss);
            if(new_loss == -1.0)
                break;
            double** new_surface = new double*[rows];
            for(int i=0;i<rows;i++){
                new_surface[i] = new double[cols]();
            }
            new_surface = online_generation(origin, pre_loss, new_loss, l_ref, h_ref, l_mod, h_mod, thread_number);
            array2CSV(output_folder+input_path+"_"+to_string(new_loss)+".csv",rows,cols,new_surface);

            free(new_surface);
        }

        free(origin);
    }
    else{
        cout << "Usage:\n-g: Generate offline JND Surface\n-m: Merge two JND Surface\n-o: Generate online JND Surface under current network condition" << endl;
    }

    return 0;
}