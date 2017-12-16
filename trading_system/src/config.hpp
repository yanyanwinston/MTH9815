//
//  config.hpp
//  trading_system
//
//  Created by Yan Yan on 12/1/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//  Configuration file

#ifndef config_h
#define config_h

#include <string>
#include <vector>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "soa_platform/products.hpp"

using boost::gregorian::date;

//CUSIPS
const std::string BOND_YEAR_2  = "123456U22";
const std::string BOND_YEAR_3  = "123456U33";
const std::string BOND_YEAR_5  = "123456U55";
const std::string BOND_YEAR_7  = "123456U77";
const std::string BOND_YEAR_10 = "123456U10";
const std::string BOND_YEAR_30 = "123456R30";

const std::vector<std::string> CUSIPS_LIST = {
    BOND_YEAR_2,
    BOND_YEAR_3,
    BOND_YEAR_5,
    BOND_YEAR_7,
    BOND_YEAR_10,
    BOND_YEAR_30
};

//Some constant number
const int NUM_PRICE = 100;//1000000
const int NUM_TRADE = 10;
const int NUM_MARKETDATA = 100;//1000000
const int NUM_INQUIRY = 10;
const int NUM_BONDS = 6;

const std::vector<BondIdType> BOND_ID_TYPE_LIST{CUSIP, CUSIP, CUSIP,  CUSIP,  CUSIP,  CUSIP};
const std::vector<std::string> TICKER_LIST{"T", "T", "T", "T", "T", "T"};
const std::vector<float> COUPON_LIST{0.05f, 0.05f, 0.05f, 0.05f, 0.05f, 0.05f};
const std::vector<date> MATURITY_LIST
{
    date(2018, boost::date_time::Nov, 1),
    date(2018, boost::date_time::Nov, 1),
    date(2018, boost::date_time::Nov, 1),
    date(2018, boost::date_time::Nov, 1),
    date(2018, boost::date_time::Nov, 1),
    date(2018, boost::date_time::Nov, 1)
};

//PV01
const std::vector<double> PV01_LIST
{
    0.01972332,
    0.03747381,
    0.01738524,
    0.02347587,
    0.07873954,
    0.01558592
};

//Orderbook
const std::vector<std::string> POSITION_BOOK_LIST
{
    "TRSY1",
    "TRSY2",
    "TRSY3"
};

#endif /* config_h */
