#include <iostream>
#include <queue>
#include <map>
#include <tuple>
#include <math.h>
using namespace std;

struct node
{
    int x;
    int y;

    bool operator< (const node &i) const
    {
        return x < i.x;
    }

     bool operator== (const node &i) const
    {
        return (x == i.x && y == i.y);
    }
};


struct region{
    double upper_bound;
    double lower_bound;
    double volume;
};

void printArray(int rows,int cols,double** array){
    for(int i=0;i<rows;i++){
        for(int j=0;j<cols;j++){
            cout << array[i][j] << " ";
        }
        printf("\n");
    }
}

double** greedy_search(double threshold, int l_ref, int h_ref, int l_mod, int h_mod){
    int rows = h_mod-l_mod+1;
    int cols = h_ref-l_ref+1;
    double** raw = new double*[rows];
    for(int i=0;i<rows;i++){
        raw[i] = new double[cols]();
    }

    // initialize
    cout << "Please enter the subject test result at (" << l_ref << "," << h_mod << ")" << endl;
    cin >> raw[0][0];
    cout << "Please enter the subject test result at (" << h_ref << "," << l_mod << ")" << endl;
    cin >> raw[h_mod-l_mod][h_ref-l_ref];
    double init_vol = (pow(rows,2)+pow(cols,2)) * (raw[0][0]-raw[h_mod-l_mod][h_ref-l_ref]); //l^2|pi-pj|
    //priority queue, to store the volume of regions and pop the largest one
    priority_queue<double> volume;
    volume.push(init_vol);
    //key is a pair(upper-left node, lower-right node); value is a region structure (upper-bound,lower-bound,volume)
    map<pair<node,node>,region> regs;
    node left_top = {0, 0}; //upper left cornor
    node right_down = {h_mod-l_mod, h_ref-l_ref}; //lower right cornor
    region init = {raw[0][0], raw[h_mod-l_mod][h_ref-l_ref], init_vol}; //the initial region(whole region)
    regs.insert(pair<pair<node,node>,region>(pair<node,node>(left_top,right_down),init));

    //loop
    while(true){
        int largest_vol;
        if((largest_vol=volume.top()) < threshold){
            break;
        }
        volume.pop();
    }

    // printArray(rows,cols,raw);
    
    return raw;
}

int main(int argc, char *argv[]){
    int l_ref,h_ref;
    int l_mod,h_mod;
    double threshold;
    // arguments: lowest reference, highest reference, lowest modification, highest modification
    if (argc < 6){
        cout << "too few arguments" << endl;
        return 0;
    }
    threshold = atof(argv[1]);
    l_ref = atoi(argv[2]);
    h_ref = atoi(argv[3]);
    l_mod = atoi(argv[4]);
    h_mod = atoi(argv[5]);

    double** raw; // 2D array of storing subject test results
    raw = greedy_search(threshold,l_ref,h_ref,l_mod,h_mod);
    
    return 0;
}