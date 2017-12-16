//
//  main.cpp
//  trading_system
//
//  Created by Yan Yan on 12/1/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//  Test the trading system

#include "BondTradeBookingService.hpp"
#include "BondPositionService.hpp"
#include "BondRiskService.hpp"
#include "BondMarketDataService.hpp"
#include "BondInquiryService.hpp"
#include "BondAlgoExecutionService.hpp"
#include "BondAlgoStreamingService.hpp"
#include "BondStreamingService.hpp"
#include "BondPricingService.hpp"
#include "BondHistoricalDataService.hpp"
#include "BondExecutionService.hpp"


void testInquiry();
void testBooking_Position_Risk();
void testMarketdata();
void testPricing_Stream();

int main()
{
    //testBooking_Position_Risk();
    //testMarketdata();
    //testPricing_Stream();
    //testInquiry();
    std::cout << "Test done!";
    return 0;
}


void testMarketdata()
{
    //Test marketdata->algoexecution->execution->historicaldata
    SimMarketData("marketdata.txt");
    BondMarketDataService mkt_srv;
    BondMarketDataConnector mkt_conn("marketdata.txt");
    BondAlgoExecutionService algo_srv;
    BondAlgoExecutionListener algo_lsnr(mkt_srv, algo_srv);
    BondExecutionService exe_srv;
    BondExecutionListener exe_lsnr(algo_srv, exe_srv);
    BondHistoricalExecutionService hist_srv;
    BondHistoricalExecutionConnector hist_conn;
    BondHistoricalExecutionServiceListener hist_lsnr(hist_srv, exe_srv);
    mkt_conn.start(mkt_srv);
}

void testBooking_Position_Risk()
{
    //Test tradebooking->position->risk->historicaldata
    SimTrade("trades.txt");
    BondTradeBookingService trade_srv;
    BondPositionService position_srv;
    BondPositionListener position_lsnr(trade_srv, position_srv);
    BondRiskService risk_srv;
    BondRiskListener risk_lsnr(position_srv, risk_srv);
    BondHistoricalPV01Service hist_srv;
    BondHistoricalPV01Connector hist_conn;
    BondHistoricalPV01ServiceListener hist_lsnr(hist_srv, risk_srv);
    BondTradeBookingConnector tradebooking_conn("trades.txt");
    tradebooking_conn.start(trade_srv);
}

void testInquiry()
{
    //Test inquiry->historicaldata
    SimInquiry("inquiries.txt");
    BondInquiryConnector iq_conn("inquiries.txt");
    BondInquiryService iq_srv(iq_conn);
    BondHistoricalInquiryService hist_srv;
    BondHistoricalInquiryConnector hist_conn;
    BondHistoricalInquiryServiceListener hist_lsnr(hist_srv, iq_srv);
    iq_conn.start(iq_srv);
}



void testPricing_Stream()
 {
    //Test pricing->algostreaming->streaming
    //uncompleted part
    SimPrice("prices.txt");
    BondPricingService prc_srv;
    BondPricingConnector conn("prices.txt");
    
    BondAlgoStreamingService algo_srv;
    BondStreamingService str_srv;//'streaming.txt'
    //BondAlgoStreamingListener algo_lsnr(algo_srv, prc_srv);
    BondStreamingListener str_lsnr(algo_srv, str_srv);
    conn.start(prc_srv);
}

