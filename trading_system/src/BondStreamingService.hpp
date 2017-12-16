//
//  BondStreamingService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/14/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondStreamingService_hpp
#define BondStreamingService_hpp

#include "soa_platform/streamingservice.hpp"
#include "BondAlgoStreamingService.hpp"
#include "utils.hpp"

class BondStreamingConnector: public Connector<PriceStream<Bond>>, private FileWriter
{
public:
    BondStreamingConnector(std::string _path);
    void Publish(const PriceStream<Bond> &data)override{};
};

BondStreamingConnector::BondStreamingConnector(std::string _path):FileWriter(_path){}

class BondStreamingService : StreamingService<Bond>
{
public:
    BondStreamingService(){}
    
    void PublishPrice(const PriceStream<Bond>& priceStream)override{}
    
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PriceStream<Bond> &data) override;
    
    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<PriceStream<Bond>> *listener) override;
    
    // Get data on our service given a key
    PriceStream<Bond>& GetData(std::string key) override;
    
    // Get all listeners on the Service.
    const std::vector<ServiceListener<PriceStream<Bond>>* >& GetListeners() const override;
private:
    std::vector<ServiceListener<PriceStream<Bond>>*> listeners;
    std::map<std::string, PriceStream<Bond>> data_pool;
    
};

class BondStreamingListener : public ServiceListener<AlgoStream>
{
public:
    BondStreamingListener(BondAlgoStreamingService &algo_srv, BondStreamingService &str_srv);
    void ProcessAdd(AlgoStream& data)override;
    void ProcessRemove(AlgoStream& data)override {};
    void ProcessUpdate(AlgoStream& data)override {};
private:
    BondStreamingService *srv_ptr;
};

void BondStreamingService::AddListener(ServiceListener<PriceStream<Bond>> *listener)
{
    listeners.push_back(listener);
}
PriceStream<Bond>& BondStreamingService::GetData(std::string key)
{
    return data_pool[key];
}
const std::vector< ServiceListener<PriceStream<Bond>>*>& BondStreamingService::GetListeners() const
{
    return listeners;
}

// Pass data into BondStreamingService
void BondStreamingService::OnMessage(PriceStream<Bond> &data)
{
    std::string id = data.GetProduct().GetProductId();
    data_pool[id] = data;
    for(auto&& lsnr : listeners)
        lsnr->ProcessAdd(data);
}

BondStreamingListener::BondStreamingListener(BondAlgoStreamingService &algo_srv, BondStreamingService &str_srv)
{
    algo_srv.AddListener(this);
    srv_ptr = &str_srv;
}

void BondStreamingListener::ProcessAdd(AlgoStream& data)
{
    srv_ptr->PublishPrice(data.GetPriceStream());
}


#endif /* BondStreamingService_hpp */
