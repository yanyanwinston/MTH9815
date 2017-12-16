//
//  BondPricingService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/2/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondPricingService_hpp
#define BondPricingService_hpp


#include <string>
#include <map>
#include "soa_platform/pricingservice.hpp"
#include "soa_platform/products.hpp"
#include "soa_platform/soa.hpp"
#include "utils.hpp"


class BondPricingService: public PricingService<Bond>{
public:
    
    // Get data on our service given a key
    Price<Bond>& GetData(string key)override;
    
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(Price<Bond>& data) override;
    
    // Get the id given price
    std::string GetID(Price<Bond>& data);
    
    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Price<Bond>> *listener) override;
    
    // Get all listeners on the Service.
    const std::vector< ServiceListener<Price<Bond>>* >& GetListeners() const override;
private:
    std::map<string, Price<Bond>> prices;
    std::vector<ServiceListener<Price<Bond>>*> listeners;
};

class BondPricingConnector: public Connector<Price<Bond>>, public FileReader{
public:
    BondPricingConnector(std::string file):FileReader(file),ptr(nullptr){};
    ~BondPricingConnector() = default;
    void start(BondPricingService &srv);
    void Publish(const Price<Bond> &data) override{};
private:
    BondPricingService* ptr;
};

void BondPricingService::AddListener(ServiceListener<Price<Bond>> *listener)
{
    listeners.push_back(listener);
}
Price<Bond>& BondPricingService::GetData(std::string key)
{
    return prices[key];
}
const std::vector<ServiceListener<Price<Bond>>*>& BondPricingService::GetListeners() const
{
    return listeners;
}

std::string BondPricingService::GetID(Price<Bond>& data)
{
    return data.GetProduct().GetProductId();
}

void BondPricingService::OnMessage(Price<Bond>& data)
{
    prices.emplace(GetID(data),data);
    for(auto&& lsnr : listeners)
        lsnr->ProcessAdd(data);
}

void BondPricingConnector::start(BondPricingService &srv)
{
    ptr = &srv;
    std::vector<std::string> record;
    if(ptr != nullptr){
        while(ReadLine(record)){
            Price<Bond> price(makeBond(record[0]), BondPrice_string2double(record[1]), BondPrice_string2double(record[2]));
            ptr->OnMessage(price);
        }
    }
}




#endif /* BondPricingService_hpp */
