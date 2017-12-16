//
//  BondAlgoStreammingService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/15/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondAlgoStreammingService_hpp
#define BondAlgoStreammingService_hpp

#include "soa_platform/streamingservice.hpp"
#include "soa_platform/products.hpp"
#include "soa_platform/soa.hpp"
#include "utils.hpp"
#include "BondPricingService.hpp"

class AlgoStream
{
public:
    AlgoStream():ptr(nullptr){}
    AlgoStream(const PriceStream<Bond>& _pstr);
    const PriceStream<Bond>& GetPriceStream() const;
    const Bond& GetProduct() const;
private:
    const PriceStream<Bond>* ptr;
    Bond bond;
};

class BondAlgoStreamingService: public StreamingService<AlgoStream>
{
public:
    void PublishPrice(const PriceStream<Bond>& priceStream);
    
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


class BondAlgoStreamingListener: public ServiceListener<Price<Bond>>
{
public:
    BondAlgoStreamingListener(BondAlgoStreamingService &algo_srv, BondPricingService &prc_srv);
    void ProcessAdd(Price<Bond> &data)override;
    void ProcessRemove(Price<Bond> &data)override;
    void ProcessUpdate(Price<Bond> &data)override;
private:
    BondStreamingService *srv_ptr;
};

BondAlgoStreamingListener::BondAlgoStreamingListener(BondAlgoStreamingService &algo_srv, BondPricingService &prc_srv)
{
    prc_srv.AddListener(this);
    srv_ptr = &algo_srv;
}

void BondAlgoStreamingListener::ProcessAdd(Price<Bond> &data)
{
    PriceStreamOrder bidorder(data.GetMid() - data.GetBidOfferSpread() * 0.5, 0, 0, BID);
    PriceStreamOrder offerorder(data.GetMid() + data.GetBidOfferSpread() * 0.5, 0, 0, OFFER);
    srv_ptr->PublishPrice({data.GetProduct(), bidorder, offerorder});
}

void BondAlgoStreamingListener::ProcessRemove(Price<Bond> &data){}

void BondAlgoStreamingListener::ProcessUpdate(Price<Bond> &data){}

AlgoStream::AlgoStream(const PriceStream<Bond>& _ptr):ptr(&_ptr){}

const Bond& AlgoStream::GetProduct() const
{
    return bond;
}

const PriceStream<Bond>& AlgoStream::GetPriceStream() const
{
    return *ptr;
}

void BondAlgoStreamingService::AddListener(ServiceListener<PriceStream<Bond>> *listener)
{
    listeners.push_back(listener);
}

PriceStream<Bond>& BondAlgoStreamingService::GetData(std::string key)
{
    return data_pool[key];
}

const std::vector<ServiceListener<PriceStream<Bond>>*>& BondAlgoStreamingService::GetListeners() const
{
    return listeners;
}

void BondAlgoStreamingService::OnMessage(PriceStream<Bond> &data)
{
    std::string id = data.GetProduct().GetProductId();
    data_pool[id] = data;
    for(auto&& lsnr : listeners)
        lsnr->ProcessAdd(data);
}


#endif /* BondAlgoStreammingService_hpp */
