#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <tuple>
#include <math.h>

using namespace std;

struct node
{
    int x;  //row in the array
    int y;  //col in the array

    bool operator< (const node &i) const
    {
        if (x == i.x)
            return y < i.y;
        else
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

void array2CSV(int rows, int cols, double** array){
    ofstream outfile;
    outfile.open("./raw_matrix.csv");

    for(int i = 0;i<rows;i++){
        outfile << array[i][0];
        for(int j = 1;j<cols;j++){
            outfile << "," << array[i][j];
        }
        outfile << endl;
    }
    outfile.close();
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
    double init_vol = (pow(rows,2)+pow(cols,2)) * fabs(raw[0][0]-raw[h_mod-l_mod][h_ref-l_ref]); //l^2|pi-pj|
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
        double largest_vol;
        if((largest_vol=volume.top()) < threshold){
            cout << "largest volume now: " << largest_vol << " < " << threshold << endl;
            break;
        }
        volume.pop();
        map<pair<node,node>,region>::iterator iter = regs.begin();
        node upper_left,lower_right;
        region target;
        while(iter != regs.end()){
            if((iter->second).volume == largest_vol){
                upper_left.x = (iter->first).first.x;
                upper_left.y = (iter->first).first.y;
                lower_right.x = (iter->first).second.x;
                lower_right.y = (iter->first).second.y;
                target.lower_bound = (iter->second).lower_bound;
                target.upper_bound = (iter->second).upper_bound;
                target.volume = (iter->second).volume;
                break;
            }
            iter ++;
        }
        regs.erase(iter);   //delete the region in map (because it's going to be divided)

        // [DEBUG]
        // cout << "upper left: " << upper_left.x << "," << upper_left.y << endl;
        // cout << "lower right: " << lower_right.x << "," << lower_right.y << endl;
        // cout << "region volume: " << target.volume << endl;
        
        node mid = {(upper_left.x+lower_right.x)/2,(upper_left.y+lower_right.y)/2};
        cout << "Please enter the subject test result at (" << l_ref+mid.y << "," << h_mod-mid.x << ")" << endl;
        cin >> raw[mid.x][mid.y];

        // divide the region into 4 sub-regions
        // upper-left sub
        double vol1 = (pow(mid.x-upper_left.x,2)+pow(mid.y-upper_left.y,2)) * 
            fabs(raw[upper_left.x][upper_left.y]-raw[mid.x][mid.y]);    //l^2|pi-pj|
        region sub1 = {target.upper_bound,raw[mid.x][mid.y],vol1};
        regs.insert(pair<pair<node,node>,region>(pair<node,node>(upper_left,mid),sub1));
        volume.push(vol1);

        //upper-right sub
        double vol2 = (pow(mid.x-upper_left.x,2)+pow(lower_right.y-mid.y,2)) * 
            fabs(raw[upper_left.x][upper_left.y]-raw[lower_right.x][lower_right.y]);    //l^2|pi-pj|
        region sub2 = {target.upper_bound,target.lower_bound,vol2};
        node sub2_up = {upper_left.x,mid.y};
        node sub2_low = {mid.x,lower_right.y};
        regs.insert(pair<pair<node,node>,region>(pair<node,node>(sub2_up,sub2_low),sub2));
        volume.push(vol2);

        //lower-left sub
        double vol3 = (pow(lower_right.x-mid.x,2)+pow(mid.y-upper_left.y,2)) * 
            fabs(raw[upper_left.x][upper_left.y]-raw[lower_right.x][lower_right.y]);    //l^2|pi-pj|
        region sub3 = {target.upper_bound,target.lower_bound,vol3};
        node sub3_up = {mid.x,upper_left.y};
        node sub3_low = {lower_right.x,mid.y};
        regs.insert(pair<pair<node,node>,region>(pair<node,node>(sub3_up,sub3_low),sub3));
        volume.push(vol3);

        //lower-right sub
        double vol4 = (pow(lower_right.x-mid.x,2)+pow(lower_right.y-mid.y,2)) * 
            fabs(raw[mid.x][mid.y]-raw[lower_right.x][lower_right.y]);    //l^2|pi-pj|
        region sub4 = {raw[mid.x][mid.y],target.lower_bound,vol4};
        regs.insert(pair<pair<node,node>,region>(pair<node,node>(mid,lower_right),sub4));
        volume.push(vol4);

        printArray(rows,cols,raw);

        // [DEBUG]
        // cout << "map size: " << regs.size() << endl;
        // for(iter = regs.begin(); iter != regs.end(); iter++){
        //     cout << iter->first.first.x << " " << iter->first.first.y << iter->first.second.x << " " << iter->first.second.y << endl;
        // }
        // break;
    }
    
    return raw;
}

int main(int argc, char *argv[]){
    int l_ref,h_ref;
    int l_mod,h_mod;
    double threshold;
    // arguments: lowest reference, highest reference, lowest modification, highest modification
    if (argc < 6){
        cout << "too few arguments" << endl;
        cout << "Usage: ./main.exe [threshold] [smallest reference] [largest reference] [smallest modification] [largest modification]" << endl;
        return 0;
    }
    threshold = atof(argv[1]);
    l_ref = atoi(argv[2]);
    h_ref = atoi(argv[3]);
    l_mod = atoi(argv[4]);
    h_mod = atoi(argv[5]);

    double** raw; // 2D array of storing subject test results
    raw = greedy_search(threshold,l_ref,h_ref,l_mod,h_mod);
    array2CSV(h_mod-l_mod+1,h_ref-l_ref+1,raw);

    // interpolate untested points
    // this will be handled by calling a separated python script
    // the generated JND profile will be stored as csv file
    string parameter = " " + to_string(l_ref) + " " + to_string(h_ref) + " " + to_string(l_mod) + " " + to_string(h_mod);
    system("python ./interpolate.py"+parameter);   //modify this line if not match your python

    return 0;
}