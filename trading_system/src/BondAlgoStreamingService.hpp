//
//  BondAlgoStreamingService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/14/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondAlgoStreamingService_hpp
#define BondAlgoStreamingService_hpp

#include "soa_platform/streamingservice.hpp"
#include "soa_platform/soa.hpp"
#include "BondPricingService.hpp"

class AlgoStream
{
private:
    PriceStream<Bond>* ptr;
    Bond bond;
public:
    AlgoStream()=default;
    AlgoStream(PriceStream<Bond>& stream):ptr(&stream){};
    PriceStream<Bond>& GetPriceStream(){return *ptr;};
    Bond& GetProduct(){return bond;};
};

class BondAlgoStreamingService: public Service<std::string,AlgoStream>
{
private:
    std::map<std::string, AlgoStream> datapool;
    vector<ServiceListener<AlgoStream>*> listeners;
public:
    BondAlgoStreamingService()=default;
    AlgoStream& GetData(std::string key) override{
        return datapool[key];
    }
    std::string GetID(AlgoStream& data)
    {
        return data.GetProduct().GetProductId();
    }
    void OnMessage(AlgoStream &data) override{
        datapool.emplace(GetID(data),data);
        for(auto&& lsnr : listeners){
            lsnr->ProcessAdd(data);
        }
    }
    void AddListener(ServiceListener<AlgoStream> *listener) override{
        listeners.push_back(listener);
    }
    const std::vector<ServiceListener<AlgoStream>*>& GetListeners() const override{
        return listeners;
    }
    void PublishPrice(PriceStream<Bond>& stream);
};

class BondAlgoStreamingListener: public ServiceListener<Price<Bond>>
{
public:
    BondAlgoStreamingListener(BondPricingService &pricing_srv, BondAlgoStreamingService &streaming_srv)
    {
        pricing_srv.AddListener(this);
        ptr = &streaming_srv;
    }
    void ProcessAdd(Price<Bond> &data) override;
private:
    BondAlgoStreamingService *ptr;
};

void BondAlgoStreamingService::PublishPrice(PriceStream<Bond>& stream)
{
    AlgoStream algostream(stream);
    OnMessage(algostream);
}

void BondAlgoStreamingListener::ProcessAdd(Price<Bond> &data)
{
    PriceStreamOrder bidorder(data.GetMid() - data.GetBidOfferSpread() * 0.5, 0, 0, BID);
    PriceStreamOrder offerorder(data.GetMid() + data.GetBidOfferSpread() * 0.5, 0, 0, OFFER);
    //ptr->PublishPrice({data.GetProduct(), bidorder, offerorder});
}

#endif /* BondAlgoStreamingService_hpp */
