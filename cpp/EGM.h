#ifndef MARS_PROJECT_EGM_H
#define MARS_PROJECT_EGM_H
#include "PGM.h"
#include <string>

using namespace std;


class EGM {
    PGM pgm = PGM();
public:
    int width;
    int height;
    int min_val;
    int max_value;
    int x_scale;
    int y_scale;
    unsigned short int *data;

    EGM();
    ~EGM();
    void add_color();
    void add_lines();
    bool higher_neighbor(int col, int row);
    void read_file(string filename);
};


#endif //MARS_PROJECT_EGM_H
