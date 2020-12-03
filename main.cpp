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
                    WantedCurrency.push_back(segment);
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
double trend(std::vector<data> data)
{
    //int  test[4] = {4,1,2,0};
    double xi = 0, yi = 0, xi2 = 0, xiyi = 0, a = 0, b = 0; 
    int iter = 0;
    for(int i = Trend-1; i >= 0 ; i--)
    {
        iter = iter + 1; //hodnota x v každém běhu
        xi = xi + iter;
        yi = yi + data[i].Close;
        //yi = yi + test[j-i];
        xi2 = xi2 + (iter * iter);
        xiyi = xiyi + iter * data[i].Close;
    }
    b = (xi2*yi - xiyi * xi)/(xi2*Trend - xi*xi);
    a = (xiyi*Trend - xi*yi)/(xi2*Trend - xi*xi);
    return a * (Trend + 1) + b;
}

double prediktMedian(std::vector<data> data)
{
    double sum;
    for(int i = 0; i<30; i++)
    {
        sum += data[i].Close;
    }

    printf("%8.6f  %8.6f\n", sum/30,sum);
    printf("%8.6f  %8.6f\n", sum/31,sum);
    printf("%8.6f  %8.6f\n", sum/32,sum);
    
    return sum/30;
}

double monteCarlo()
{
    double prediction = trend(LoadedData["ATO"]); //5.7, 5,3
    double longTrend = prediktMedian(LoadedData["ATO"]);

    double rozdilPrediktLast = prediction - 5.3;
    double rozdilPredikLngTrnd = prediction - longTrend;


    double max = prediction * 1.05;
    double min = prediction * 0.95;
    
    double cntUp = 0;
    double cntDown = 0;
    int a = 500;

    double forCount = 10000;
    for( int i = 0; i <= forCount; i++)
    {
        double nrm = Normal(1,0.4);
        double rand = prediction + prediction * nrm;
        
        if(max > rand && min < rand)
        {
            cntUp += 1;
            cntDown += rand;
        
        }
        if( i == a )
        {
            //printf( "MC = %8.6f  %d\n", rand,a);
            //printf("%f \n", nrm);
            a += 500;
        }

    }
    //printf( "Up = %8.6f\n", cntDown/cntUp);
    
    return(cntDown/cntUp);

}

void test()
{ 
    int sizeLen = LoadedData["ATO"].size();
    std::vector<double> Open;
    std::vector<double> Change;
    std::vector<int> RandomA;
    std::vector<double> RandomChange;
    std::vector<double> Price;
    int a = sizeLen;
    int b = sizeLen;
    int c = sizeLen;
    int e = sizeLen;
    int f = sizeLen;
    for( int i = 0; i < 50; i++)
    {
        printf("%d \n",i);
        Open.push_back(LoadedData["ATO"][i].Close);
    }
    for( int i = 1; i < 20 ; i++)
    {
        printf("%d \n",i);
        Change.push_back(log(Open[i]/Open[i-1]));
        RandomA.push_back(rand() % 10);
    }
    for( int i = 0; i < 10; i++)
    {
        printf("%d \n",i);
        printf("%f \n", Change[RandomA[i]]);
        RandomChange.push_back(Change[RandomA[i]]);
    }
    Price.push_back(LoadedData["ATO"][0].Close);
    for( int i = 1; i < 10; i++)
    {
        printf("%d \n",i);
        Price.push_back(Price[i-1]*exp(RandomChange[i]));
        printf("%8.6f \n", Price[i] );
    }
    
    printf("test");

}


int main(int argc, char *argv[]){
    LoadData();
    if (ArgvParse(argc, argv) != 0)
        return -1;

    test();
    return 1;

    double a = 0;
    for(int i = 0; i<=100 ; i++)
    {
        a += monteCarlo();
    }

    printf("%8.6f \n", a/100);
    printf("5,7\n");
    

    //printf("value = %8.6f\n", );
    return 1;


    
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
