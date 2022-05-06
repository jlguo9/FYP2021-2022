#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>

using namespace std;

int f1(int, double, double);
int f2(int, double, double);
double** online_generation(double**, double, double, int, int, int, int, int);