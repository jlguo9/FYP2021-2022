#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// void array2CSV(string, int, int, double**);
// void CSV2array(string, int, int, double**);
// void printArray(int, int, double**);

void array2CSV(string output_path, int rows, int cols, double** array){
    ofstream outfile;
    outfile.open(output_path);

    for(int i = 0;i<rows;i++){
        outfile << array[i][0];
        for(int j = 1;j<cols;j++){
            outfile << "," << array[i][j];
        }
        outfile << endl;
    }
    outfile.close();
}

void CSV2array(string input_path, int rows, int cols, double** array){
    ifstream infile;
    infile.open(input_path);

    for(int i = 0;i<rows;i++){
        string line_str;
        getline(infile, line_str);
        int first_comma = 0, last_comma = 0;
        for(int j = 0;j<cols;j++){
            last_comma = line_str.find(',',first_comma);
            array[i][j] = atof(line_str.substr(first_comma,last_comma-first_comma).c_str());
            first_comma = last_comma + 1;
        }
    }
    infile.close();
}

void printArray(int rows,int cols,double** array){
    for(int i=0;i<rows;i++){
        for(int j=0;j<cols;j++){
            cout << array[i][j] << " ";
        }
        printf("\n");
    }
}