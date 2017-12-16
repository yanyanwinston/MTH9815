//
//  BondAlgoExecutionService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/14/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondAlgoExecutionService_hpp
#define BondAlgoExecutionService_hpp


#include "soa_platform/executionservice.hpp"
#include "soa_platform/soa.hpp"
#include "soa_platform/products.hpp"
#include "BondMarketDataService.hpp"


class AlgoExecution : public ExecutionOrder<Bond>
{
public:
    AlgoExecution()=default;
    AlgoExecution(const string& bondid, PricingSide _side, double _price, double quantity);
private:
    static size_t cnt;
};


/*
 BondAlgoExecutionService
 should register a ServiceListener on the BondMarketDataService and aggress the top of the book,
 alternating between bid and offer (taking the opposite side of the order book to cross the spread)
 – it should send this order to the BondExecutionService via a ServiceListener and the ExecuteOrder() method.
 */
class BondAlgoExecutionService : public Service<std::string, AlgoExecution>
{
public:
    BondAlgoExecutionService()=default;
    ~BondAlgoExecutionService()=default;
    
    // Add orderbook
    void AddOrderBook(OrderBook<Bond> orderbook);
    
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(AlgoExecution &data) override;
    
    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<AlgoExecution> *listener) override;
    
    // Get data on our service given a key
    AlgoExecution& GetData(std::string key) override;
    
    // Get all listeners on the Service.
    const std::vector< ServiceListener<AlgoExecution>*>& GetListeners() const override;
private:
    std::vector<ServiceListener<AlgoExecution>*> listeners;
    std::map<std::string, AlgoExecution> algo_pool;
};

class BondAlgoExecutionListener : public ServiceListener<OrderBook<Bond>>
{
public:
    BondAlgoExecutionListener(BondMarketDataService &mktsrv, BondAlgoExecutionService &algoexsrv);
    void ProcessAdd(OrderBook<Bond> &data) override;
    void ProcessRemove(OrderBook<Bond> &data) override{};
    void ProcessUpdate(OrderBook<Bond> &data) override{};
private:
    BondAlgoExecutionService* srv_ptr;
};

size_t AlgoExecution::cnt = 0;

AlgoExecution::AlgoExecution(const string& bondid, PricingSide _side, double _price, double quantity): ExecutionOrder(makeBond(bondid), _side, "AlgoOrder #"+to_string(++cnt), LIMIT, _price, quantity/2, quantity/2, "NULL", false){}


void BondAlgoExecutionService::AddListener(ServiceListener<AlgoExecution> *listener)
{
    listeners.push_back(listener);
}
AlgoExecution& BondAlgoExecutionService::GetData(std::string key)
{
    return algo_pool[key];
}
const std::vector< ServiceListener<AlgoExecution>*>& BondAlgoExecutionService::GetListeners() const
{
    return listeners;
}

// Pass data into BondPositionService
void BondAlgoExecutionService::OnMessage(AlgoExecution &data)
{
    std::string id = data.GetProduct().GetProductId();
    algo_pool.emplace(id, data);
    for(auto&& lsnr : listeners)
        lsnr->ProcessAdd(data);
}

void BondAlgoExecutionService::AddOrderBook(OrderBook<Bond> orderbook)
{
    auto bid = AlgoExecution(orderbook.GetProduct().GetProductId(),
                             orderbook.GetBidStack().front().GetSide(),
                             orderbook.GetBidStack().front().GetPrice(),
                             orderbook.GetBidStack().front().GetQuantity());
    auto offer = AlgoExecution(orderbook.GetProduct().GetProductId(),
                               orderbook.GetOfferStack().front().GetSide(),
                               orderbook.GetOfferStack().front().GetPrice(),
                               orderbook.GetOfferStack().front().GetQuantity());
    OnMessage(bid);
    OnMessage(offer);
}

BondAlgoExecutionListener::BondAlgoExecutionListener(BondMarketDataService &mkt_srv, BondAlgoExecutionService &algo_srv)
{
    mkt_srv.AddListener(this);
    srv_ptr = &algo_srv;
}

void BondAlgoExecutionListener::ProcessAdd(OrderBook<Bond> &data)
{
    srv_ptr->AddOrderBook(data);
}

#endif /* BondAlgoExecutionService_hpp */
