#include "EGM.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#define PI 3.14159265
using namespace std;

EGM::EGM()
{
}

EGM::~EGM()
{
    // destructor
}

void EGM::add_color() {
    int reds[8] = {0, 0, 255, 100, 100, 255, 200, 200};
    int greens[8] = {200, 200, 255, 100, 100, 255, 50, 50};
    int blues[8] = {0, 0, 255, 10, 10, 255, 50, 50};

    for (int i = 0; i < this->width * this->height; i++) {
        int egm_col = i % this->width;
        int egm_row = (i - egm_col) / this->width;
        int pgm_col = int(egm_col * (this->width / pgm.width));
        int pgm_row = int(egm_row * (this->height / pgm.height));
        int j = int(pgm_col + pgm_row * pgm.width);
        pgm.color[j].red = unsigned char(reds[this->data[i]] * pgm.data[i]);
        pgm.color[j].green = unsigned char(greens[this->data[i]] * pgm.data[i]);
        pgm.color[j].blue = unsigned char(blues[this->data[i]] * pgm.data[i]);
    }
}

void EGM::add_lines(){
    for (int i = 0; i < this->width * this->height; i++) {
        int egm_col = i % this->width;
        int egm_row = (i - egm_col) / this->width;
        if (this->higher_neighbor(egm_col, egm_row)) {
            int pgm_col = int(egm_col * (this->width / pgm.width));
            int pgm_row = int(egm_row * (this->height / pgm.height));
            int j = (int) (pgm_col + pgm_row * pgm.width);
            pgm.color[j].red = 0;
            pgm.color[j].green = 0;
            pgm.color[j].blue = 0;
            pgm.data[j] = 0;
        }
    }
}

bool EGM::higher_neighbor(int col, int row)
{
    short int value = this->data[col + row * this->width];

    if (col != 0 && this->data[col - 1 + row * this->width] < value)
        return true; // higher then left
    if (col != this->width - 1 && this->data[col + 1 + row * this->width] < value)
        return true; // higher then right
    if (row != 0 && this->data[col + (row - 1) * this->width] < value)
        return true; // higher then top
    if ((row != this->height - 1) && this->data[col + this->width*(row + 1)] < value)
        return true; // higher then bottom
    return false;
}

void EGM::read_file(string filename){
    ifstream fn(filename, ios::binary);
    int header_info[8];
    char *typ = (char *) header_info;

    fn.read(reinterpret_cast<char *>(header_info), 32);

    this->width = header_info[2];
    this->height = header_info[3];
    this->min_val = header_info[4];
    this->max_value = header_info[5];
    this->x_scale = header_info[6];
    this->y_scale = header_info[7];
    if (*typ != 'E' || *(typ +1) != '4') {
        cout << "There is a problem with the EGM image: "
            << filename << endl;
        cout << *typ << " " << *(typ +1);
        exit(EXIT_FAILURE);
    }

    this->data = new unsigned short int[this->width * this->height];
    fn.read(reinterpret_cast<char *>(this->data),
            this->width * this->height * sizeof(short int));

    // bin the data to eight different elevations
    float bin = float (this->max_value - this->min_val / 8);
    for (int i = 0; i < this->width * this->height; i++)
        this->data[i] = unsigned short(((this->data[i]) - this->min_val) / bin);

    for(int i = 0; i < 8; i++)
        cout << *(typ + i) << endl;

    fn.close();
}