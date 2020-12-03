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
#include <getopt.h>
#include <unistd.h>
#include <math.h>

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

class Currency {      
  public:            
    std::string name; 
    double balance;  
};

std::vector<std::vector<std::pair<std::string, std::vector<int>>>> entryData;
std::map<std::string, std::vector<data>> LoadedData;
std::vector<Currency> ActualData;
std::vector<std::string> WantedCurrency; //všechny chtěné měny
double Deposit; // Vložený vklad
int Trend; //počítaný trend
std::map<std::string, std::vector<data>> NewData;


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

bool is_double(const std::string& s)
{
    std::istringstream iss(s);
    double d;
    return iss >> d >> std::ws && iss.eof();
}


int ArgvParse(int argc, char *argv[])
{
    if ( argc <= 2)
    {
        std::cerr<<"Málo požadovaných argumentů" << '\n';
        return 1;
    }
    for(int i = 1; i < argc ; i++)
    {
        std::string segment;
        std::vector<std::string> seglist;
        std::stringstream ss(argv[i]);

        while(std::getline(ss, segment, '='))
        {
            seglist.push_back(segment);
        }
        
        if(seglist[0] == ("-C"))
        {
            segment;
            std::vector<std::string> seglist1;
            std::stringstream ss1(seglist[1]);

            while(std::getline(ss1, segment, '='))
            {
                if(LoadedData.find(segment)!=LoadedData.end())
                {
                    NewData.insert(std::pair<std::string, std::vector<data>>(seglist,all))
                }
                else
                {
                    std::cerr<<"Špatně zadaná zkratka měny" << '\n';
                    return 1;
                }
            }
        }
        else if (seglist[0] == ("-D"))
        {
            try
            {
                Deposit = std::stod(seglist[1]);
                if(Deposit <= 100)
                {
                    std::cerr<<"Vklad musí být větší 100!" << '\n';
                    return 1;
                }
            }
            catch(std::exception& e)
            {
                std::cerr<<"Špatně zadaný vklad!" << '\n';
                return 1;
            }
            
        }
        else if (seglist[0] == ("-T"))
        {
            try
            {
                Trend = std::stoi(seglist[1]);
                if(Trend <= 0)
                {
                    std::cerr<<"Trend musí být větší jak 0!" << '\n';
                    return 1;
                }
            }
            catch(std::exception& e)
            {
                std::cerr<<"Špatně zadaný trend!" << '\n';
                return 1;
            } 
        }
        else
        {
            std::cerr<<"Neznámí agument!" << '\n';
            return 1;
        
        }
    }
    return 0;
}

//výpočet trendu metodou nejmenších čtverců
double trend(int count, std::vector<data> data)
{
    double xi = 0, yi = 0, xi2 = 0, xiyi = 0, a = 0, b = 0; 
    int iter = 0;
    for(int i = count-1; i >= 0 ; i--)
    {
        iter = iter + 1; //hodnota x v každém běhu
        xi = xi + iter;
        yi = yi + data[i].Close;
        xi2 = xi2 + (iter * iter);
        xiyi = xiyi + iter * data[i].Close;
    }
    b = (xi2*yi - xiyi * xi)/(xi2*count - xi*xi);
    a = (xiyi*count - xi*yi)/(xi2*count - xi*xi);
    return a * (count + 1) + b;
}

class NewTrent : public Event {
    void Behavior() {
        double rand = Random();
        if( rand > 0.5 )
        { 
            //+
        }
        else
        {
            // - 
        }
        Activate(Time + 1);
    }
};
// //Variační koeficient
// double varKoef(int count, std::vector<data> data)
// {
//     //Trend pro variační koeficient
//     double avgTrend = trend(100, data);



class Rebalanc : public Process {
    bool isInit = true;
    void Behavior() {
       if(isInit)
       {
           //buy
           isInit = false;
       }
       //Calculate
       //if rebalace

    }
};

class Hodl : public Process {
    bool isInit = true;
    void Behavior() {
        if(isInit)
        {
            //buy
            isInit = false;
        }
        //Buy(();
        Wait(364);
        //Sold();
    }
};


//     std::vector<double> container;
//     //rozptyl
//     for(int i = 0; i < count; i++)
//     {
//         double val = data[i].Close - avgTrend;
//         val = val * val;
//         container.push_back(val);
//     }
//     double result = 0;
//     for(int j = 0; j < container.size(); j++)
//     {
//         result = result + container[j]; 
//     }
//     result = result/container.size();

//     //odchylka
//     double sx = sqrt(result);

//     //var koef
//     double vx = (sx / avgTrend);

//     return vx;
}

//Vrací náhodný double mezi min-max
double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

double predictOne(double RozsahMin, double RozsahMax, std::vector<data> data)
{
    for (int days = 0; days < 100; days++)
    {
        double trendToPrint = trend(7, data);
        double a = RozsahMin;
        double b = RozsahMax;
        int N = 1000;
        
        std::vector<double> xrand;

        for( int i = 0; i < N; i++)
        {
            double randNum = fRand(a, b);
            xrand.push_back(randNum);
        }

        double resultRand = 0.0;
        double narust = 0.0;

        for( int j = 0; j < N; j++)
        {
            resultRand = resultRand + xrand[j];
        }

        return resultRand/double(N);
    }
}


Histogram Predict("Predict",0,25,20);

int main(int argc, char *argv[]){
    LoadData();
    if (ArgvParse(argc, argv) != 0)
        return -1;

    Init(0, 365);





    
    //double rozsah = varKoef(10, LoadedData["ATO"]);
    
    
    

    SetOutput("crypto.dat");
    Init(0,365);                      // initialize experiment
    SetStep(0,1);           // bisection needs small minstep
    Run();                        // run simulation, print results
    SIMLIB_statistics.Output();   // print run statistics

}
