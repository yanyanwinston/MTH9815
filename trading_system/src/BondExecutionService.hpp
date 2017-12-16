//
//  BondExecutionService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/14/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondExecutionService_hpp
#define BondExecutionService_hpp

#include "soa_platform/executionservice.hpp"
#include "soa_platform/soa.hpp"
#include "BondAlgoExecutionService.hpp"

class BondExecutionService : public ExecutionService<Bond>
{
public:
    BondExecutionService(){};
    void ExecuteOrder(const ExecutionOrder<Bond>& order, Market market) override;
    void OnMessage(ExecutionOrder<Bond> &data) override;
    void AddListener(ServiceListener<ExecutionOrder<Bond>> *listener) override;
    ExecutionOrder<Bond>& GetData(std::string key) override;
    const std::vector<ServiceListener<ExecutionOrder<Bond>>*>& GetListeners() const override;
private:
    std::vector<ServiceListener<ExecutionOrder<Bond>>*> listeners;
    std::map<std::string, ExecutionOrder<Bond>> pos_pool;
};

class BondExecutionListener : public ServiceListener<AlgoExecution>
{
public:
    BondExecutionListener(BondAlgoExecutionService &algosrv, BondExecutionService &exesrv);
    void ProcessAdd(AlgoExecution &data) override;
    void ProcessRemove(AlgoExecution &data) override{};
    void ProcessUpdate(AlgoExecution &data) override{};
private:
    BondExecutionService* srv_ptr;
};


void BondExecutionService::AddListener(ServiceListener<ExecutionOrder<Bond>> *listener)
{
    listeners.push_back(listener);
}
ExecutionOrder<Bond>& BondExecutionService::GetData(std::string key)
{
    return pos_pool[key];
}
const std::vector< ServiceListener<ExecutionOrder<Bond>>*>& BondExecutionService::GetListeners() const
{
    return listeners;
}

// Pass data into BondPositionService
void BondExecutionService::OnMessage(ExecutionOrder<Bond> &data)
{
    std::string id = data.GetProduct().GetProductId();
    pos_pool.emplace(id,data);
    for(auto&& lsnr : listeners){
        lsnr->ProcessAdd(data);
    }
}

void BondExecutionService::ExecuteOrder(const ExecutionOrder<Bond>& order, Market market)
{
    auto tmp = order;
    OnMessage(tmp);
}

BondExecutionListener::BondExecutionListener(BondAlgoExecutionService &algo_srv,BondExecutionService &exe_srv)
{
    algo_srv.AddListener(this);
    srv_ptr = &exe_srv;
}

void BondExecutionListener::ProcessAdd(AlgoExecution &data)
{
    srv_ptr->ExecuteOrder(static_cast<ExecutionOrder<Bond>>(data), Market(rand()%3));
}

#endif /* BondExecutionService_hpp */
