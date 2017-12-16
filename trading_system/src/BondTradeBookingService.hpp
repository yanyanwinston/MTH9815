//
//  BondTradeBookingService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/2/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondTradeBookingService_hpp
#define BondTradeBookingService_hpp

#include <stdio.h>
#include "soa_platform/tradebookingservice.hpp"
#include "soa_platform/products.hpp"
#include "utils.hpp"

class BondTradeBookingService: public TradeBookingService<Bond>{
public:
    BondTradeBookingService() = default;
    ~BondTradeBookingService() = default;
    
    // Book the trade
    void BookTrade(Trade<Bond>& trade) override;
    
    // Get data on our service given a key
    Trade<Bond>& GetData(std::string key) override;
    
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Trade<Bond> &data) override;
    
    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Trade<Bond>> *listener) override;
    
    // Get all listeners on the Service.
    const std::vector<ServiceListener<Trade<Bond>>*>& GetListeners() const override;
    
private:
    std::vector<ServiceListener<Trade<Bond>>*> listeners;
    std::map<string, Trade<Bond>*> trades;
};

class BondTradeBookingConnector:public Connector<Trade<Bond>>, public FileReader
{
public:
    BondTradeBookingConnector(std::string file): FileReader(file), srv_ptr(nullptr){}
    virtual ~BondTradeBookingConnector() = default;
    void start(BondTradeBookingService &_srv);
    void Publish(const Trade<Bond> &data) override{};
private:
    BondTradeBookingService* srv_ptr;
};

void BondTradeBookingService::BookTrade(Trade<Bond>& trade)
{
    for(auto&& l: listeners)
        l->ProcessAdd(trade);
}

Trade<Bond>& BondTradeBookingService::GetData(std::string key)
{
    return *trades[key];
}

void BondTradeBookingService::OnMessage(Trade<Bond> &data)
{
    std::cout<<"onmessage"<<std::endl;
    trades[data.GetTradeId()] = &data;
    BookTrade(data);// pass data to listerners
}

void BondTradeBookingService::AddListener(ServiceListener<Trade<Bond>> *listener)
{
    listeners.push_back(listener);
}

const std::vector<ServiceListener<Trade<Bond>>*>& BondTradeBookingService::GetListeners() const
{
    return listeners;
}

void BondTradeBookingConnector::start(BondTradeBookingService &_srv)
{
    //bind service
    srv_ptr = &_srv;
    std::vector<std::string> record;
    if(srv_ptr != nullptr)
    {
        while(ReadLine(record))
        {
            Side side;
            if(record[4]=="BUY")
                side = BUY;
            else
                side = SELL;
            //(const T &_product, string _tradeId, string _book, long _quantity, Side _side, double _price)
            Trade<Bond> trade(makeBond(record[1]), record[0], record[2], stoi(record[3]), side);
            std::cout<<"test"<<std::endl;
            srv_ptr->OnMessage(trade);// pass trade data to tradebookingservice
            std::cout<<"test"<<std::endl;
        }
    }
}

#endif /* BondTradeBookingService_hpp */
