//
//  BondRiskService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/11/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondRiskService_hpp
#define BondRiskService_hpp

#include "soa_platform/riskservice.hpp"
#include "soa_platform/products.hpp"
#include "BondPositionService.hpp"
#include "utils.hpp"

class BondRiskService : public RiskService<Bond>
{
public:
    // ctor
    BondRiskService() = default;
    
    // Add a position that the service will risk
    void AddPosition(Position<Bond> &position) override;
    
    // Get the bucketed risk for the bucket sector
    double GetBucketedRisk(const BucketedSector<Bond> &sector) override;
    
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(PV01<Bond> &data) override;
    
    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<PV01<Bond>> *listener) override;
    
    // Get data on our service given a key
    PV01<Bond>& GetData(std::string key) override;
    
    // Get all listeners on the Service.
    const std::vector< ServiceListener<PV01<Bond>>* >& GetListeners() const override;
private:
    std::vector<ServiceListener<PV01<Bond>>*> listeners;
    std::map<std::string, PV01<Bond>> risk_pool;
};

class BondRiskListener : public ServiceListener<Position<Bond>>
{
public:
    BondRiskListener(BondPositionService &position_srv, BondRiskService &risk_srv);
    void ProcessAdd(Position<Bond> &data) override;
    void ProcessRemove(Position<Bond> &data) override{};
    void ProcessUpdate(Position<Bond> &data) override{};
private:
    BondRiskService* risksrv_ptr;
};

void BondRiskService::AddListener(ServiceListener<PV01<Bond>> *listener)
{
    listeners.push_back(listener);
}

PV01<Bond>& BondRiskService::GetData(std::string key)
{
    return risk_pool[key];
}

const std::vector< ServiceListener<PV01<Bond>>*>& BondRiskService::GetListeners() const
{
    return listeners;
}

void BondRiskService::AddPosition(Position<Bond> &position)
{
    auto risk = PV01<Bond>(position.GetProduct(), BondPV01(position.GetProduct().GetProductId()), position.GetAggregatePosition());
    OnMessage(risk);
}

void BondRiskService::OnMessage(PV01<Bond> &data)
{
    std::string id = data.GetProduct().GetProductId();
    risk_pool.emplace(id, data);
    for(auto&& lsnr : listeners)
        lsnr->ProcessAdd(data);
}

double BondRiskService::GetBucketedRisk(const BucketedSector<Bond> &sector)
{
    double bucketedrisk = 0;
    auto bond_list = sector.GetProducts();
    for(auto& i: bond_list){
        auto& pv01 = GetData(i.GetProductId());
        bucketedrisk += pv01.GetPV01() * pv01.GetQuantity();
    }
    return bucketedrisk;
}

BondRiskListener::BondRiskListener(BondPositionService &position_srv, BondRiskService &risk_srv)
{
    position_srv.AddListener(this);
    risksrv_ptr = &risk_srv;
}

void BondRiskListener::ProcessAdd(Position<Bond> &data)
{
    risksrv_ptr->AddPosition(data);
}

#endif /* BondRiskService_hpp */
