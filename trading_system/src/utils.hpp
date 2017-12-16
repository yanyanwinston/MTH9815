//
//  utils.hpp
//  trading_system
//
//  Created by Yan Yan on 12/1/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//  Implement some generic tools for different part of trading system

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <string>
#include <fstream>
#include "soa_platform/products.hpp"
#include "soa_platform/inquiryservice.hpp"
#include "config.hpp"
#include <iostream>
#include <vector>

// Convert fraction notation to decimal
double BondPrice_string2double(std::string strprice){
    double dprice;
    // exclude -xyz part from 100-xyz
    dprice = stoi(strprice.substr(0, strprice.length()-4));
    dprice += stoi(strprice.substr(strprice.length()-3, 2)) / 32.0;
    if(strprice.back() == '+')
        strprice.back() = '4';
    dprice += stoi(strprice.substr(strprice.length()-1)) / 256.0;
    return dprice;
}

// Convert decimal notation to fraction
std::string BondPrice_double2string(double dprice){
    std::string strprice;
    int int_price = static_cast<int>(dprice);//integer part
    dprice -= int_price;//decimal part
    strprice = std::to_string(int_price);
    strprice += '-';
    int iprice = static_cast<int>(dprice * 256 + 0.5);// round to 1/256
    int r = iprice % 8;
    strprice += std::to_string(iprice / 80);
    strprice += std::to_string(iprice / 8 % 10);
    if(r == 4)
        strprice += '+';
    else
        strprice += std::to_string(r);
    return strprice;
}

// Simulate Prices.txt
void SimPrice(std::string file){
    std::ofstream myfile(file);
    if (myfile.is_open())
    {
        for(auto&& bondid : CUSIPS_LIST)
        {
            for(int i=1; i<=NUM_PRICE;i++)
            {
                double mid = 100 + (rand()%512-256)/256.0;
                double spread = (rand()%3+2)/256.0;
                std::vector<std::string> record = {bondid, BondPrice_double2string(mid), BondPrice_double2string(spread)};
                std::copy(record.begin(), record.end(), std::ostream_iterator<std::string>(myfile, ","));
                myfile << std::endl;
            }
        }
        myfile.close();
    }
    else std::cout << "Unable to open file";
}

// Simulate trades.txt
void SimTrade(std::string file){
    std::ofstream myfile(file);
    std::vector<std::string> TRSY_LIST = {"TRSY1", "TRSY2", "TRSY3"};
    std::vector<std::string> DIRECTION = {"BUY", "SELL"};
    int cnt=0;
    for(auto&& bondid : CUSIPS_LIST){
        for(int i=1; i<=NUM_TRADE; i++){
            std::vector<std::string> record = {
                "T"+std::to_string(++cnt), // trade id
                bondid, // bond id
                TRSY_LIST[rand()%3], // TRSY
                BondPrice_double2string(100 + (rand()%512 - 256) / 256.0), // price
                std::to_string((rand()%10)*10000), // volume
                DIRECTION[rand()%2] // direction
            };
            copy(record.begin(), record.end(), std::ostream_iterator<std::string>(myfile, ","));
            myfile << std::endl;
        }
    }
    myfile.close();
}


// Simulate inquiries.txt
void SimInquiry(std::string file){
    ofstream myfile (file);
    size_t inq_id = 0;
    std::vector<std::string> DIRECTION = {"BUY", "SELL"};
    for(auto&& bondid : CUSIPS_LIST){
        for(int i=1; i<=NUM_INQUIRY; i++){
            vector<string> record;
            record.push_back("Inquiry #" + to_string(++inq_id));
            record.push_back(bondid);
            record.push_back(to_string(rand()%2));//Side
            record.push_back(to_string(rand()%1000000));//quantity
            record.push_back(BondPrice_double2string(100 + (rand()%512 - 256) / 256.0));//price
            record.push_back(to_string(InquiryState::RECEIVED));
            copy(record.begin(), record.end(), ostream_iterator<string>(myfile, ","));
            myfile << std::endl;
        }
    }
    myfile.close();
}

void SimMarketData(std::string file){
    ofstream myfile(file);
    const double incre = 1.0/ 256.0;
    for(auto&& bondid : CUSIPS_LIST){
        for(int i=1; i<=NUM_MARKETDATA; i++){
            std::vector<std::string> record;
            record.push_back(bondid);
            double mid = 100 + (rand()%512 - 256) / 256.0;
            double bid = mid - incre;
            double ask = mid + incre;
            for(int j=0; j<5; j++){
                record.push_back(BondPrice_double2string(bid - j*incre));
                record.push_back(to_string((j+1) * 10 * 1000000));
            }
            for(int j=0; j<5; j++){
                record.push_back(BondPrice_double2string(ask + j*incre));
                record.push_back(to_string((j+1) * 10 * 1000000));
            }
            copy(record.begin(), record.end(), ostream_iterator<string>(myfile, ","));
            myfile << std::endl;
        }
    }
    myfile.close();
}

//File Read Tool
class FileReader
{
public:
    FileReader(const std::string& _path):path(_path){file.open(path);}
    ~FileReader(){file.close();}
    bool ReadLine(std::vector<std::string>& _line)
    {
        _line.clear();
        std::string line;
        if(getline(file, line))
        {
            stringstream lineStream(line);
            string cell;
            while(getline(lineStream, cell, ',')){
                _line.push_back(cell);
            }
            return true;
        }
        return false;
    }
private:
    std::string path;
    std::ifstream file;
};

//File Write Tool
class FileWriter
{
public:
    FileWriter(const std::string& _path):path(_path){file.open(_path);}
    ~FileWriter(){file.close();}
    void WriteLine(const std::vector<std::string>& _line)
    {
        copy(_line.begin(), _line.end(),
             ostream_iterator<string>(file, ","));
        file << endl;
    }

private:
    std::string path;
    std::ofstream file;
};

//Make a bond object
Bond makeBond(std::string _bondid)
{
    size_t i;
    for(i=0;i<NUM_BONDS;i++)
    {
        if(CUSIPS_LIST[i]==_bondid)
        {
            return Bond(_bondid,BOND_ID_TYPE_LIST[i],TICKER_LIST[i],COUPON_LIST[i],MATURITY_LIST[i]);
        }
    }
    return Bond();
}

//Return Bond PV01
double BondPV01(const std::string cusip){
    size_t i;
    for(i=0; i<NUM_BONDS; i++){
        if(CUSIPS_LIST[i] == cusip){
            return PV01_LIST[i];
        }
    }
    return 0;
}

#endif /* utils_hpp */



