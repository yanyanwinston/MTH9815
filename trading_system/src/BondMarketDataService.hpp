//
//  BondMarketDataService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/11/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondMarketDataService_hpp
#define BondMarketDataService_hpp

#include <stdio.h>
#include "soa_platform/marketdataservice.hpp"
#include "soa_platform/products.hpp"

class BondMarketDataService : public MarketDataService<Bond>
{
private:
    std::map<string, OrderBook<Bond>> orderbook_pool;
    std::vector<ServiceListener<OrderBook<Bond>>*> listeners;
public:
    //default constructor
    BondMarketDataService() {};
    
    //get the best bidoffer for a specific product
    BidOffer GetBestBidOffer(const std::string &productId) override;
    
    // Get data on our service given a key
    OrderBook<Bond>& GetData(std::string key) override;
    
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(OrderBook<Bond>& data) override;
    
    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<OrderBook<Bond>>* _listener) override;
    
    // Get all listeners on the Service.
    const std::vector<ServiceListener<OrderBook<Bond>>*>& GetListeners() const override;
};


class BondMarketDataConnector : public Connector<OrderBook<Bond>>, private FileReader
{
public:
    BondMarketDataConnector(std::string file);
    
    // Generate price update
    void start(BondMarketDataService &mktsrv);
    void Publish(const OrderBook<Bond> &data) override{};
    void AddOrderBook(OrderBook<Bond> orderbook);
private:
    BondMarketDataService *srv_ptr;
};

BondMarketDataConnector::BondMarketDataConnector(std::string file):FileReader(file),srv_ptr(nullptr){};

void BondMarketDataConnector::start(BondMarketDataService &mkt_srv)
{
    // Bind service
    srv_ptr = &mkt_srv;
    
    std::vector<std::string> record;
    if(srv_ptr != nullptr){
        while(ReadLine(record)){
            std::deque<string> q(record.begin(), record.end());
            std::vector<Order> bid, offer;
            auto bond = makeBond(q.front());
            q.pop_front();
            for(int i=0; i<5; i++)
            {
                auto price = BondPrice_string2double(q.front());
                q.pop_front();
                auto volume = stoi(q.front());
                bid.push_back(Order(price, volume, BID));
                q.pop_front();
            }
            for(int i=0; i<5; i++)
            {
                auto price = BondPrice_string2double(q.front());
                q.pop_front();
                auto volume = stoi(q.front());
                offer.push_back(Order(price, volume, OFFER));
                q.pop_front();
            }
            auto book = OrderBook<Bond>(bond, bid, offer);
            srv_ptr->OnMessage(book);
        }
    }
}




void BondMarketDataService::OnMessage(OrderBook<Bond> &data)
{
    std::string id = data.GetProduct().GetProductId();
    orderbook_pool.emplace(id,data);
    for(auto&& lsnr : listeners)
        lsnr->ProcessAdd(data);
}

BidOffer BondMarketDataService::GetBestBidOffer(const std::string &productId)
{
    auto orderbook = GetData(productId);
    return BidOffer(orderbook.GetBidStack().front(), orderbook.GetOfferStack().front());
}

OrderBook<Bond>& BondMarketDataService::GetData(std::string key)
{
    return orderbook_pool[key];
};

void BondMarketDataService::AddListener(ServiceListener<OrderBook<Bond>> *_listener)
{
    listeners.push_back(_listener);
}

const std::vector< ServiceListener<OrderBook<Bond>>*>& BondMarketDataService::GetListeners() const
{
    return listeners;
};

#endif /* BondMarketDataService_hpp */
