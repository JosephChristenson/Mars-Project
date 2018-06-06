//
// Created by jechr on 6/6/2018.
//

#ifndef MARS_PROJECT_PGM_H
#define MARS_PROJECT_PGM_H
#include "COLOR.h"
#include <string>


using namespace std;

class PGM {
public:
    string type;
    float width;
    float height;
    float max_value;
    unsigned short int *data;
    COLOR* color;

    PGM();
    ~PGM();
    void read_file(string filename);
    void write_file(string filename);
    void sub_array(float top, float left, float bottom, float right);
};


#endif //MARS_PROJECT_PGM_H
