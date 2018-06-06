#include "PGM.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#define PI 3.14159265

using namespace std;

PGM::PGM()
{
}
PGM::~PGM()
{
    // destructor
}

void PGM::write_file(string filename){
    if (type != "")
        this->type = type;

    if (this->type == "P2" || this->type == "P5") {
        filename = filename.substr(0, filename.length() - 3) + "pgm";
    } else if (this->type == "P6" || this->type == "P3") {
        filename = filename.substr(0, filename.length() - 3) + "pnp";
    } else {
        cout << "There is a problem with the PGM type: " << this->type << endl;
        exit(EXIT_FAILURE);
    }

    ofstream fn(filename);
    fn << this->type << " " << this->width << " " << this->height << " " << this->max_value << endl;
    if (this->type == "P2") {
        for (int i = 0; i < this->width * this->height; i++)
            fn << this->data[i] << endl;
    } else if (this->type == "P3") {
        for (int i = 0; i < this->width * this->height; i++) {
            unsigned char red = this->color[i].red;
            unsigned char green = this->color[i].green;
            unsigned char blue = this->color[i].blue;
            fn << (int) red << " " << (int) green << " " << (int) blue << endl;
        }
    } else if (this->type == "P5" && this->max_value < 256) {
        unsigned char* bytes = new unsigned char [int(this->width * this->height)];
        for (int i = 0; i < this->width * this->height; i++)
            bytes[i] = (char) this->data[i];
        fn.write(reinterpret_cast<char *>(bytes), int (this->width * this->height));
        delete bytes;
    } else if (this->type == "P5" && this->max_value >= 256) {
        fn.write(reinterpret_cast<char *> (this->data),
                 int(this->width * this->height * sizeof(short)));
    } else if (this->type == "P6") {
        fn.write(reinterpret_cast<char *> (this->color),
                 int(this->width * this->height * sizeof(color)));
    } else {
        cout << "There is a problem with the PGM type: " << this->type << endl;
        exit(EXIT_FAILURE);
    }
    fn.close();
}

void PGM::sub_array(float top, float left, float bottom, float right){
    float latitude_scaling;
    if (top - bottom > 90)
        latitude_scaling = (float) cos((top - bottom) / 2 * PI / 180.0);
    else
        latitude_scaling = (float) cos((top + bottom) / 2 * PI / 180.0);
    int top_index = int((90 - top) / 180 * this->height);
    int bottom_index = int((90 - bottom) / 180 * this->height);
    int left_index = int(left * latitude_scaling / 360 * this->width);
    int right_index = int(right * latitude_scaling / 360 * this->width);

    PGM ret;
    this->width = float (right_index - left_index);
    this->height = float (bottom_index - top_index);
    this->max_value = this->max_value;
    this->type = this->type;

    this->data = new unsigned short int[int(this->width * this->height)];
    this->color = new COLOR[int(this->width * this->height)];
    for (int col = 0; col < this->width; col++) {
        for (int row = 0; row < this->height; row++) {
            this->data[(int) (col + row * this->width)] =
                    this->data[(int) (col + left_index + ((row + top_index) * this->width))];
            this->color[(int) (col + row * this->width)] =
                    this->color[(int) (col + left_index + ((row + top_index) * this->width))];
        }
    }
}

void PGM::read_file(string filename) {
    string word;
    ifstream fn(filename);
    if (!fn.good())
    {
        cout << "this file does not exist: " << filename << endl;
        exit(EXIT_FAILURE);
    }
    fn >> this->type;
    if (this->type != "P5" && this->type != "P2") {
        cout << "There is a problem with the this type: "
            << this->type << endl;
        exit(EXIT_FAILURE);
    }

    fn >> word;
    this->width = (float) atof(word.c_str());

    fn >> word;
    this->height = (float) atof(word.c_str());

    fn >> word;
    this->max_value = (float) atof(word.c_str());

    if (this->max_value < 0 || this->max_value > 65536) {
        cout << "There is a problem with the this max value: "
            << this->max_value << endl;
        exit(EXIT_FAILURE);
    }
    this->data = new unsigned short int[int(this->width * this->height)];
    this->color = new COLOR [int(this->width * this->height)];

    if (this->type == "P2") {
        for (int i = 0; i < this->width * this->height; i++)
            fn >> this->data[i];
    }
    else if (this->max_value < 256) {
        char tmp;
        for (int i = 0; i < this->width * this->height; i++) {
            fn.read(&tmp, 1);
            this->data[i] = (unsigned short int) tmp;
        }
    }
    else {
        fn.read(reinterpret_cast<char *>(this->data),
               int(this->width * this->height * sizeof(short int)));
    }
    fn.close();
}