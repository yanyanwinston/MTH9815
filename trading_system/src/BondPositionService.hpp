//
//  BondPositionService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/2/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondPositionService_hpp
#define BondPositionService_hpp

#include <stdio.h>
#include <map>
#include "soa_platform/positionservice.hpp"
#include "soa_platform/products.hpp"
#include "soa_platform/soa.hpp"
#include "BondTradeBookingService.hpp"


class BondPositionService: public PositionService<Bond>{
public:
    // ctor
    BondPositionService() = default;
    
    // dtor
    virtual ~BondPositionService() = default;
    
    // Add a trade to the service
    void AddTrade(const Trade<Bond>& trade) override;
    
    // Print current position
    void PrintPosition();
    
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Position<Bond> &data) override;
    
    // Add a listener to the Service for callbacks on add, remove, and update events for data to the Service.
    void AddListener(ServiceListener<Position<Bond>> *listener) override;
    
    // Get data on our service given a key
    Position<Bond>& GetData(std::string key) override;
    
    // Get all listeners on the Service.
    const std::vector< ServiceListener<Position<Bond>>* >& GetListeners() const override;

private:
    std::vector<ServiceListener<Position<Bond>>*> listeners;
    std::map<std::string, Position<Bond>> pos_pool;
};

class BondPositionListener: public ServiceListener<Trade<Bond>>{
public:
    BondPositionListener(BondTradeBookingService &tradesrv, BondPositionService &possrv);
    ~BondPositionListener()=default;
    void ProcessAdd(Trade<Bond>& data) override;
    void ProcessRemove(Trade<Bond> &data) override;
    void ProcessUpdate(Trade<Bond> &data) override;
private:
    BondPositionService* srv_ptr;
};


void BondPositionService::AddListener(ServiceListener<Position<Bond>> *listener)
{
    listeners.push_back(listener);
}
Position<Bond>& BondPositionService::GetData(std::string key)
{
    return pos_pool[key];
}
const std::vector< ServiceListener<Position<Bond>>*>& BondPositionService::GetListeners() const
{
    return listeners;
}

// Pass data into BondPositionService
void BondPositionService::OnMessage(Position<Bond> &data)
{
    std::string id = data.GetProduct().GetProductId();
    pos_pool[id] = data;
    for(auto&& lsnr : listeners)
        lsnr->ProcessAdd(data);
}

// Get trade data to update position
void BondPositionService::AddTrade(const Trade<Bond> &trade)
{
    std::cout<<"AddTrade"<<std::endl;
    std::string key = trade.GetProduct().GetProductId();//get productid key
    auto pos = pos_pool.find(key);//find position
    if(pos == pos_pool.end()){
        //if the bond is already in pos pool
        Position<Bond> initpos(makeBond(key));//Position
        initpos.AddPosition(trade);
        pos_pool.insert({key, initpos});
        pos = pos_pool.find(key);
    }
    else
        //if pos pool does not have that bond
        pos->second.AddPosition(trade);
    OnMessage(pos->second);//
}

// Print current position
void BondPositionService::PrintPosition()
{
    std::map<std::string,Position<Bond>>::iterator iter = pos_pool.begin();
    while(iter!=pos_pool.end())
    {
        std::cout<<iter->first<<" "<<iter->second.GetAggregatePosition()<<std::endl;
        iter++;
    }
}

BondPositionListener::BondPositionListener(BondTradeBookingService &trade_srv,BondPositionService &pos_srv)
{
    trade_srv.AddListener(this);
    srv_ptr = & pos_srv;
}

void BondPositionListener::ProcessAdd(Trade<Bond> &data)
{
    srv_ptr->AddTrade(data);
}

// Listener callback to process a remove event to the Service
void BondPositionListener::ProcessRemove(Trade<Bond> &data) {}

// Listener callback to process an update event to the Service
void BondPositionListener::ProcessUpdate(Trade<Bond> &data) {}

#endif /* BondPositionService_hpp */
