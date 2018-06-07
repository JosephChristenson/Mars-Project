#include <iostream>
#include <string>
#include "COLOR.h"
#include "EGM.h"
#include "PGM.h"
using namespace std;
void get_mars_location(string egm_file, string pgm_file, string output_file,
                       float top, float left, float bottom, float right);
int main() {
    float top, bottom, left, right;

    //cout << "What is the top left corner coordinates (x,y) and the bottom right corner coordinate"<< endl;
    //cin >> left >> top >> right >> bottom;
    left = 0; top = 0; right = 99; bottom = 99;
    get_mars_location("MolaInput.egm",
                      "MolaInput.pgm",
                      "MolaOut.pgm",
                      top, left, bottom, right);
    return 0;
}

void get_mars_location(string egm_file, string pgm_file, string output_file,
                       float top, float left, float bottom, float right) {
    EGM egm = EGM();
    egm.read_file(egm_file);
    PGM pgm = PGM();
    pgm.read_file(pgm_file);
    egm.add_color();
    egm.add_lines();
    pgm.sub_array(top, left, bottom, right);
    pgm.write_file(output_file);

    delete[] egm.data;
    delete[] pgm.data;
    //delete[] out.data;
    //delete[] out.color;
    delete[] pgm.color;
}