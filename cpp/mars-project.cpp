#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#define PI 3.14159265

using namespace std;

class COLOR {
public:
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

class EGM {
public:
    int width;
    int height;
    int min_val;
    int max_value;
    int x_scale;
    int y_scale;
    unsigned short int *data;
    
    EGM::EGM()
    {
    }
    
    EGM::~EGM()
    {
        // destructor
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
        float bin = this->max_value - this->min_val / 8;
        for (int i = 0; i < this->width * this->height; i++)
            this->data[i] = ((this->data[i]) - this->min_val) / bin;

        fn.close();
    }
};

class PGM {
public:
    string type;
    float width;
    float height;
    float max_value;
    unsigned short int *data;
    COLOR *color;

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
            fn.write(reinterpret_cast<char *>(bytes), this->width * this->height);
            delete bytes;
        } else if (this->type == "P5" && this->max_value >= 256) {
            fn.write(reinterpret_cast<char *> (this->data),
                     int(this->width * this->height * sizeof(short)));
        } else if (this->type == "P6") {
            fn.write(reinterpret_cast<char *> (this->color),
                     int(this->width * this->height * sizeof(COLOR)));
        } else {
            cout << "There is a problem with the PGM type: " << this->type << endl;
            exit(EXIT_FAILURE);
        }
        fn.close();
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
        this->color = new COLOR[int(this->width * this->height)];

        if (this->type == "P2") {
            for (int i = 0; i < this->width * this->height; i++)
                fn >> this->data[i];
        }
        else if (this->max_value < 256) {
            char tmp;
            for (int i = 0; i < this->width * this->height; i++) {
                fn.read(&tmp, 1);
                this->data[i] = (short int) tmp;
            }
        }
        else {
            fn.read(reinterpret_cast<char *>(this->data),
                    int(this->width * this->height * sizeof(short int)));
        }
        fn.close();
    }
};


PGM extract_sub_array(PGM &pgm, float top, float left, float bottom, float right);

void add_contour_lines(EGM &egm, PGM &pgm);

void get_mars_location(string egm_file, string pgm_file, string output_file,
                       float top, float left, float bottom, float right);

bool is_higher_than_a_neighbor(EGM &egm, int col, int row);

void add_elevation_color(EGM &egm, PGM &pgm);

int main() {
    float top, bottom, left, right;

    cout << "What is the top left corner coordinates (x,y) and the bottom right corner coordinate"<< endl;
    cin >> left >> top >> right >> bottom;
    get_mars_location("mola_016_globe.egm",
                      "mola_016_globe_illum_input.pgm",
                      "mola_016_globe_illum_output.pgm",
                      top, left, bottom, right);
    return 0;
}

void get_mars_location(string egm_file, string pgm_file, string output_file,
                       float top, float left, float bottom, float right) {
    EGM egm = EGM();
    egm.read_file(egm_file);
    PGM pgm = PGM();
    pgm.read_file(pgm_file);

    add_elevation_color(egm, pgm);
    add_contour_lines(egm, pgm);
    PGM out = extract_sub_array(pgm, top, left, bottom, right);
    pgm.write_file(output_file);

    delete[] egm.data;
    delete[] pgm.data;
    delete[] out.data;
    delete[] out.color;
    delete[] pgm.color;
}

bool is_higher_than_a_neighbor(EGM &egm, int col, int row) {
    short int value = egm.data[col + row * egm.width];

    if (col != 0 && egm.data[col - 1 + row * egm.width] < value)
        return true; // higher then left
    if (col != egm.width - 1 && egm.data[col + 1 + row * egm.width] < value)
        return true; // higher then right
    if (row != 0 && egm.data[col + (row - 1) * egm.width] < value)
        return true; // higher then top
    if (row != egm.height - 1 && egm.data[col + (row + 1) * egm.width] < value)
        return true; // higher then bottom
    return false;
}

void add_contour_lines(EGM &egm, PGM &pgm) {
    for (int i = 0; i < egm.width * egm.height; i++) {
        int egm_col = i % egm.width;
        int egm_row = (i - egm_col) / egm.width;
        if (is_higher_than_a_neighbor(egm, egm_col, egm_row)) {
            int pgm_col = int(egm_col * (egm.width / pgm.width));
            int pgm_row = int(egm_row * (egm.height / pgm.height));
            int j = (int) (pgm_col + pgm_row * pgm.width);
            pgm.color[j].red = 0;
            pgm.color[j].green = 0;
            pgm.color[j].blue = 0;
            pgm.data[j] = 0;
        }
    }
}

void add_elevation_color(EGM &egm, PGM &pgm) {
    int reds[8] = {0, 0, 255, 100, 100, 255, 200, 200};
    int greens[8] = {200, 200, 255, 100, 100, 255, 50, 50};
    int blues[8] = {0, 0, 255, 10, 10, 255, 50, 50};

    for (int i = 0; i < egm.width * egm.height; i++) {
        int egm_col = i % egm.width;
        int egm_row = (i - egm_col) / egm.width;
        int pgm_col = int(egm_col * (egm.width / pgm.width));
        int pgm_row = int(egm_row * (egm.height / pgm.height));
        int j = int(pgm_col + pgm_row * pgm.width);
        pgm.color[j].red = unsigned char(reds[egm.data[i]] * pgm.data[i]);
        pgm.color[j].green = unsigned char(greens[egm.data[i]] * pgm.data[i]);
        pgm.color[j].blue = unsigned char(blues[egm.data[i]] * pgm.data[i]);
    }
}

PGM extract_sub_array(PGM &pgm, float top, float left, float bottom, float right) {
    // convert degree lat / long into int index into PGM data
    float latitude_scaling;
    if (top - bottom > 90)
        latitude_scaling = (float) cos((top - bottom) / 2 * PI / 180.0);
    else
        latitude_scaling = (float) cos((top + bottom) / 2 * PI / 180.0);
    int top_index = int((90 - top) / 180 * pgm.height);
    int bottom_index = int((90 - bottom) / 180 * pgm.height);
    int left_index = int(left * latitude_scaling / 360 * pgm.width);
    int right_index = int(right * latitude_scaling / 360 * pgm.width);

    PGM ret;
    ret.width = right_index - left_index;
    ret.height = bottom_index - top_index;
    ret.max_value = pgm.max_value;
    ret.type = pgm.type;

    ret.data = new unsigned short int[int(ret.width * ret.height)];
    ret.color = new COLOR[int(ret.width * ret.height)];
    for (int col = 0; col < ret.width; col++) {
        for (int row = 0; row < ret.height; row++) {
            ret.data[(int) (col + row * ret.width)] =
                    pgm.data[(int) (col + left_index + ((row + top_index) * pgm.width))];
            ret.color[(int) (col + row * ret.width)] =
                    pgm.color[(int) (col + left_index + ((row + top_index) * pgm.width))];
        }
    }
    return ret;
}