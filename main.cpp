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

class savedForCoin {
    public:
    double firstCash; //kolik jsi dal na začítek
    double cash; //aktuální vložená částka
    double coin; //kolik je to v coině
    double day; //kolikátý je den
    
};

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

data::data(std::string date = "", double open = 0, double high = 0, double low= 0, double close = 0, double volumeUSD = 0, double marketCapUSD = 0)
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
std::map<std::string, std::vector <savedForCoin>> NewData;
std::map<std::string, std::vector<data>> PredictedData;
double dayCountR1H;
double dayCountH1H;
double dayCountR1D;
double dayCountH1D;
double dayCountR1M;
double dayCountH1M;
double dayCount;


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

void CreateNewVectorForPrediction()
{
    for( int j = 0; j < WantedCurrency.size(); j++)
    {
        for( int i = 6; i  >= 0; i--)
        {
            PredictedData[WantedCurrency[j]].push_back(LoadedData[WantedCurrency[j]][6-i]);
        }
    }
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
                std::stringstream ss2(segment);
                std::string coin;
                while (std::getline(ss2, coin, ','))
                {
                    if(LoadedData.find(coin)!= LoadedData.end())
                    {
                        std::vector<savedForCoin> nw;
                        WantedCurrency.push_back(coin);
                        NewData.insert(std::pair<std::string, std::vector<savedForCoin>>(coin,nw));
                        std::vector<data> nd;
                        PredictedData.insert(std::pair<std::string, std::vector<data>>(coin,nd));
                    }
                    else
                    {
                        std::cerr<<"Špatně zadaná zkratka měny" << '\n';
                        return 1;
                    }
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
        else if (seglist[0] == ("-L"))
        {
            try
             {
                dayCountR1H = std::stoi(seglist[1]);
                dayCountH1H = std::stoi(seglist[1]);
                dayCount = std::stoi(seglist[1]) * 24;
                if(dayCount <= 0)
                {
                    std::cerr<<"Délka dní musí být musí být větší jak 0!" << '\n';
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
    CreateNewVectorForPrediction();
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



// //Variační koeficient
// double varKoef(int count, std::vector<data> data)
// {
//     //Trend pro variační koeficient
//     double avgTrend = trend(100, data);

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
//}

//Vrací náhodný double mezi min-max
double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

double predictOne(double RozsahMin, double RozsahMax, std::vector<data> data)
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



class NewTrent : public Event {
    void Behavior() {
        for(int i = 0 ; i < 4; i++)
        {
            double rand = Random();
            if( rand > 0.5 )
            { 
                double x = LoadedData[WantedCurrency[i]][LoadedData.size()].Close;

            }
            else
            {
                // - 
            }
        }
        
        Activate(Time + 1);
    }
};

class NewCurs : public Process {
    void Behavior() {
        for(int i = 0 ; i < WantedCurrency.size(); i++)
        {
            data predict;
            double rand = Random();
            double desRand = Random() / 10;
            double lastValueOfCoin = PredictedData[WantedCurrency[i]][PredictedData.size()-1].Close;
            double dispersion = lastValueOfCoin * desRand; // max odhylka 10% z přechoží hodnoty
            double maxP = lastValueOfCoin * (1 + dispersion); 
            double minP = lastValueOfCoin * (1 - dispersion);
            if( rand > 0.5 )
            { 
                 predict.Close = predictOne(lastValueOfCoin, maxP, PredictedData[WantedCurrency[i]]);
            }
            else
            {
                predict.Close = predictOne(minP,lastValueOfCoin, PredictedData[WantedCurrency[i]]);
            }
            PredictedData[WantedCurrency[i]].push_back(predict);
            //cout << WantedCurrency[i] << " = " << predict.Close << endl;
        }
    }
};

bool isInitRP = true;
double actualValueRP;
class RebalancPredikt : public Process {
  
public:

    void Behavior() {
        
        if(isInitRP)
        {
            isInitRP = false;
            int splt = WantedCurrency.size();
            double onEachCrypto = Deposit / splt;

            for( int i = 0; i < WantedCurrency.size(); i++)
            {
                actualValueRP = LoadedData[WantedCurrency[i]][dayCountR1H].Close;
                savedForCoin nw;
                nw.firstCash = onEachCrypto;
                nw.cash = onEachCrypto;
                nw.coin = onEachCrypto/actualValueRP;
                NewData[WantedCurrency[i]].push_back(nw);
            }
        }

      
        //Calculate
        for( int j = 0; j< WantedCurrency.size(); j++)
        {
            actualValueRP = LoadedData[WantedCurrency[j]][dayCountR1H].Close;
            double aa = NewData[WantedCurrency[j]][0].cash / NewData[WantedCurrency[j]][0].coin;
            //                9                      100                             10
            double prct = (actualValueRP / aa - 1) *100;
            if(prct > 7.0 || prct < -7.0 )
            {
                double allCash = 0;
                for( int i = 0; i < WantedCurrency.size(); i++)
                {
                    allCash += NewData[WantedCurrency[i]][0].coin * LoadedData[WantedCurrency[i]][dayCountR1H].Close;
                }
                allCash = allCash / WantedCurrency.size();
                for( int i = 0; i < WantedCurrency.size(); i++)
                {
                    NewData[WantedCurrency[i]][0].cash = allCash;
                    NewData[WantedCurrency[i]][0].coin = allCash/LoadedData[WantedCurrency[i]][dayCountR1H].Close;
                }
                break;
            }
            //cout<< WantedCurrency[i]<< "=" << onEachCrypto/LoadedData[WantedCurrency[i]][dayConut].Close << endl;
        }
        
        
        dayCountR1H--;

        if (dayCountR1H == 0)
        {
            for( int i = 0; i < WantedCurrency.size(); i++)
            {
                double nw = NewData[WantedCurrency[i]][0].coin * LoadedData[WantedCurrency[i]][dayCountR1H].Close;
                cout << "Rebalanc= " << WantedCurrency[i] << "=" << nw << " \t% = " << (nw / NewData[WantedCurrency[i]][0].firstCash - 1) * 100 << "%" <<endl;  
            }
        }
       //if rebalace

    }
};

bool isInitR = true;
double actualValueR;

class Rebalanc : public Process {
  
public:

    void Behavior() {
        
        if(isInitR)
        {
            isInitR = false;
            int splt = WantedCurrency.size();
            double onEachCrypto = Deposit / splt;

            for( int i = 0; i < WantedCurrency.size(); i++)
            {
                actualValueR = LoadedData[WantedCurrency[i]][dayCountR1H].Close;
                savedForCoin nw;
                nw.firstCash = onEachCrypto;
                nw.cash = onEachCrypto;
                nw.coin = onEachCrypto/actualValueR;
                NewData[WantedCurrency[i]].push_back(nw);
            }
        }

      
        //Calculate
        for( int j = 0; j< WantedCurrency.size(); j++)
        {
            actualValueR = LoadedData[WantedCurrency[j]][dayCountR1H].Close;
            double aa = NewData[WantedCurrency[j]][0].cash / NewData[WantedCurrency[j]][0].coin;
            //                9                      100                             10
            double prct = (actualValueR / aa - 1) *100;
            if(prct > 7.0 || prct < -7.0 )
            {
                double allCash = 0;
                for( int i = 0; i < WantedCurrency.size(); i++)
                {
                    allCash += NewData[WantedCurrency[i]][0].coin * LoadedData[WantedCurrency[i]][dayCountR1H].Close;
                }
                allCash = allCash / WantedCurrency.size();
                for( int i = 0; i < WantedCurrency.size(); i++)
                {
                    NewData[WantedCurrency[i]][0].cash = allCash;
                    NewData[WantedCurrency[i]][0].coin = allCash/LoadedData[WantedCurrency[i]][dayCountR1H].Close;
                }
                break;
            }
            //cout<< WantedCurrency[i]<< "=" << onEachCrypto/LoadedData[WantedCurrency[i]][dayConut].Close << endl;
        }
        
        
        dayCountR1H--;

        if (dayCountR1H == 0)
        {
            for( int i = 0; i < WantedCurrency.size(); i++)
            {
                double nw = NewData[WantedCurrency[i]][0].coin * LoadedData[WantedCurrency[i]][dayCountR1H].Close;
                cout << "Rebalanc= " << WantedCurrency[i] << "=" << nw << " \t% = " << (nw / NewData[WantedCurrency[i]][0].firstCash - 1) * 100 << "%" <<endl;  
            }
        }
       //if rebalace

    }
};



bool isInitH = true;
//todo dny
class Hodl : public Process {
    
public: 
    void Behavior() {
        if(isInitH)
        {
            isInitH = false;
            int splt = WantedCurrency.size();
            double onEachCrypto = Deposit / splt;

            for( int i = 0; i < WantedCurrency.size(); i++)
            {
                double actualValue = LoadedData[WantedCurrency[i]][dayCountH1H].Close;
                savedForCoin nw;
                nw.firstCash = onEachCrypto;
                nw.cash = onEachCrypto;
                nw.coin = onEachCrypto/actualValue;
                NewData[WantedCurrency[i]].push_back(nw);
                
                //cout<< WantedCurrency[i]<< "=" << onEachCrypto/LoadedData[WantedCurrency[i]][dayConut].Close << endl;
            }
        }

        dayCountH1H--;
        //cout << dayCountH1H << endl;

        if(dayCountH1H == 0)
        {
            for( int i = 0; i < WantedCurrency.size(); i++)
            {
                //cout<< "Crypto: " <<onEachCrypto << endl;
                double nw = NewData[WantedCurrency[i]][1].coin * LoadedData[WantedCurrency[i]][dayCountH1H].Close;
                cout << "HODL= " << WantedCurrency[i] << "=" << nw << "\t % = " << (nw / NewData[WantedCurrency[i]][1].firstCash - 1) * 100 << "%" <<endl;
            }
        }
    }
};

class Hour : public Event {
    void Behavior() {
        (new NewCurs)->Activate();
        Activate( Time + 1);
    }
};

class Day : public Event {
    void Behavior() {
        (new Rebalanc)->Activate();
        (new Hodl)->Activate();
        Activate( Time + 24);
    }
};

class Week : public Event {
    void Behavior() {

        Activate( Time + 24 * 7);
    }
};

class Month : public Event {
    void Behavior() {

        Activate( Time + 24 * 7 * 31);
    }
};

//todo check jestli den je v dané měně ještě abys nepřesáhl pole;
int main(int argc, char *argv[]){
    LoadData();
    if (ArgvParse(argc, argv) != 0)
        return -1;

    SetOutput("crypto.dat");
    Init(0, dayCount);

    (new Hour)->Activate();
    (new Day)->Activate();

    Run();

    //double rozsah = varKoef(10, LoadedData["ATO"]);

    SIMLIB_statistics.Output();   // print run statistics

}
