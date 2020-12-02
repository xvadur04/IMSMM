#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream
#include <stdio.h>
#include <string>
#include <iostream>



std::vector<std::pair<std::string, std::vector<int>>> read_csv(std::string filename){
    // Reads a CSV file into a vector of <string, vector<int>> pairs where
    // each pair represents <column name, column values>

    // Create a vector of <string, int vector> pairs to store the result
    std::vector<std::pair<std::string, std::vector<int>>> result;

    // Create an input filestream
    std::ifstream myFile(filename);

    // Make sure the file is open
    if(!myFile.is_open()) throw std::runtime_error("Could not open file");

    // Helper vars
    std::string line, colname;
    int val;

    // Read the column names
    if(myFile.good())
    {
        // Extract the first line in the file
        std::getline(myFile, line);

        // Create a stringstream from line
        std::stringstream ss(line);

        // Extract each column name
        while(std::getline(ss, colname, ',')){

            // Initialize and add <colname, int vector> pairs to result
            result.push_back({colname, std::vector<int> {}});
        }
    }

    // Read data, line by line
    while(std::getline(myFile, line))
    {
        // Create a stringstream of the current line
        std::stringstream ss(line);

        // Keep track of the current column index
        int colIdx = 0;

        // Extract each integer
        while(ss >> val){

            // Add the current integer to the 'colIdx' column's values vector
            result.at(colIdx).second.push_back(val);

            // If the next token is a comma, ignore it and move on
            if(ss.peek() == ',') ss.ignore();

            // Increment the column index
            colIdx++;
        }
    }

    // Close file
    myFile.close();

    return result;
}

#include "simlib.h"

const double g = 9.81;          // gravity acceleration

class Ball : ConditionDown {    // ball model description
    Integrator v,y;               // state variables
    unsigned count;               // bounce event count
    void Action()  {              // state event description
        Print("# Odraz %u:\n", ++count);
        Out();                    // print state
        v = -0.8 * v.Value();     // the energy loss
        y = 0;            // this is needed for detection when small energy!
        if(count>=20)             // after 20 bounces:
            Stop();                 //   end simulation run
    }
public:
    Ball(double initialposition) :
            ConditionDown(y),           // bounce condition: (y>=0) from TRUE to FALSE
            v(-g),                      // y' = INTG( - m * g )
            y(v, initialposition),      // y  = INTG( y' )
            count(0) {}                 // init bounce count
    void Out() {
        Print("%-9.3f  % -9.3g  % -9.3g\n",
              T.Value(), y.Value(), v.Value());
    }
};

Ball m1(1.0);                   // model of system

void Sample() { m1.Out(); }     // output the ball state periodically
Sampler S(Sample,0.01);

int main() {                    // experiment description
    SetOutput("ball.dat");
    Print("# Ball --- model of bouncing ball\n");
    Print("# Time y v \n");
    Init(0);                      // initialize experiment
    SetStep(1e-10,0.5);           // bisection needs small minstep
    SetAccuracy(1e-5,0.001);      // set numerical error tolerance
    Run();                        // run simulation, print results
    SIMLIB_statistics.Output();   // print run statistics
}
