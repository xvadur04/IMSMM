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
#include "simlib.h"
#include <dirent.h>
#include <ctime>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <bits/stdc++.h>

using std::cout; using std::cin;
using std::endl; using std::vector;



class data {
    public:
        std::string Date;
        double Open; //1
        double High; //2
        double Low; //3
        double Close; //4
        double VolumeUSD; //5
        double MarketCapUSD; //6
        data(std::string date, double open, double high, double low, double close, double volumeUSD, double marketCapUSD);

};

data::data(std::string date, double open, double high, double low, double close, double volumeUSD, double marketCapUSD)
{
    Date = date;
    Open = open;
    High = high;
    Low = low;
    Close = close;
    VolumeUSD = volumeUSD;
    MarketCapUSD = marketCapUSD;
}

std::vector<std::vector<std::pair<std::string, std::vector<int>>>> entryData;

std::map<std::string, std::vector<data>> LoadedData;


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

    // // Read the column names
    // if(myFile.good())
    // {
    //     // Extract the first line in the file
    //     std::getline(myFile, line);

    //     // Create a stringstream from line
    //     std::stringstream ss(line);

    //     // Extract each column name
    //     while(std::getline(ss, colname, ',')){

    //         // Initialize and add <colname, int vector> pairs to result
    //         result.push_back({colname, std::vector<int> {}});
    //     }
    // }

    std::getline(myFile, line); //First row
    // Read data, line by line
    std::string segment;
    std::vector<std::string> seglist;
    std::vector<data> all;
    while(std::getline(myFile, line))
    {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> splittedStrings;
        while (std::getline(ss, item, ','))
        {
            splittedStrings.push_back(item);
        }

        

        data datas(splittedStrings[0], std::stod(splittedStrings[1]),std::stod(splittedStrings[2]),
                    std::stod(splittedStrings[3]),std::stod(splittedStrings[4]),
                    std::stod(splittedStrings[5]),std::stod(splittedStrings[6]));

        all.push_back(datas);
        
            

        // // Keep track of the current column index
        

        // // Extract each integer
        // while(ss >> val){

        //     // Add the current integer to the 'colIdx' column's values vector
        //     result.at(colIdx).second.push_back(val);

        //     // If the next token is a comma, ignore it and move on
        //     if(ss.peek() == ',') ss.ignore();

        //     // Increment the column index
        //     colIdx++;
        // }
    }
    std::string name = filename.substr(6,3);
    LoadedData.insert(std::pair<std::string, std::vector<data>>(name,all));
    // Close file
    myFile.close();

    return result;
}

void LoadData()
{
    DIR *dir; struct dirent *diread;
    vector<std::string> files;

    if ((dir = opendir("./day")) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            files.push_back(diread->d_name);
        }
        closedir (dir);
    } else {
        perror ("opendir");
    }

    for(std::string file: files)
    {
        if(file != "." && file != "..")
        {
            entryData.push_back(read_csv("./day/" + file));
        }
    }
}

//Třída pro měnu, název a množství měny
class Currency {      
  public:            
    std::string name; 
    double balance;  
};

vector<Currency> ActualData;

// class day : public Process {   // ball model description
//   double Prichod;                 // atribute of each customer
//   void behavoir() {               // --- behavoir specification ---
//     Prichod = Time;               // konkrétní den
//     for()
//     Seize(Box);                   // zůstatek pro den...
//     Wait(10);                     // time of service
//     Release(Box);                 // end of service
//     Table(Time-Prichod);          // waiting and service time
//   }
// };



int main() {
    //LoadData();
    int pocetMen = 10;
    for (int i =0; i < pocetMen; i++ )
    { 
        Currency curr;
        curr.name = "some name";
        curr.balance = 9 + Random();
        ActualData.insert(ActualData.begin() + i , curr);
    }

    SetOutput("crypto.dat");
    Init(0,365);                      // initialize experiment
    SetStep(1e-10,0.5);           // bisection needs small minstep
    Run();                        // run simulation, print results
    SIMLIB_statistics.Output();   // print run statistics

}
