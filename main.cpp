/*Program to compute FFT of 3 acceleration axes, made for TRACTIAN hardware challenge*/
/*Marlon J. de Almeida*/
/*Built with standard C++ libraries and ftfw3*/
/*The program parses csv data onto 3 double float vectors, zero pads uneven data up*/
/*to a power of 2 for use with the radix-2 FFT algorithm*/
/*zero-padding without previous windowing causes large artifact to be filtered on 0 frequency*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include "fftw3.h"

void csv_parse(std::ifstream &file_stream, std::vector<double> &x, std::vector<double> &y, std::vector<double> &z);

int main()
{
    std::ifstream input("1602245833-2715-NAO7856.txt");
    std::ofstream output("output.txt");
    std::vector<double> x, y, z;
    fftw_complex x_mag[4225], y_mag[4225], z_mag[4225];    //fftw complex array type declaration (fixed size for convenience)

    float freq_res = (4139/2.715)/4139;                    //frequency resolution is freq. of sampling over number of samples (4139 original samples over 2.7sec)

    csv_parse(input, x, y, z);                             //apply parsing function

    for (int i = 4139; i<4225; i++) {                      //zero padding up to next power of 2, 65^2
        x[i] = y[i] = z[i] = 0.0;
        }

    /*fftw plans*/
    fftw_plan plan_x = fftw_plan_dft_r2c_1d(4225, x.data(), x_mag, FFTW_PATIENT);
    fftw_plan plan_y = fftw_plan_dft_r2c_1d(4225, y.data(), y_mag, FFTW_PATIENT);
    fftw_plan plan_z = fftw_plan_dft_r2c_1d(4225, z.data(), z_mag, FFTW_PATIENT);
    /*fftw application*/
    fftw_execute(plan_x);
    fftw_execute(plan_y);
    fftw_execute(plan_z);

    output << "x,y,z,freq\n";                              //write csv header to file

    for(int i = 0; i<4225; i++) {                          //write absolute value of magnitude(s) and respective frequency to file
        output << fabs(x_mag[i][0]) << ',' << fabs(y_mag[i][0]) << ','<< fabs(z_mag[i][0]) << ',' << freq_res*i << '\n';
        }

    output.close();

return 0;
}

void csv_parse(std::ifstream &file_stream, std::vector<double> &x, std::vector<double> &y, std::vector<double> &z)
{
    std::string string, val;
    getline(file_stream, string);                   //read and skip csv header line

    while (getline (file_stream, string)) {         //read each line
        std::stringstream s(string);                //string stream to parse
        getline(s, val,',');                        //individual getline() will read column of line up to comma, to be pushed onto vector(s)
        x.push_back(stof(val));
        getline(s, val,',');
        y.push_back(stof(val));
        getline(s, val,',');
        z.push_back(stof(val));
    }
}
