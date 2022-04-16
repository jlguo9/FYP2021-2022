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

double** greedy_search(double, int, int, int, int);
void printArray(int, int, double**);