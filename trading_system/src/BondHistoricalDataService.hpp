//
//  BondHistoricalDataService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/14/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondHistoricalDataService_hpp
#define BondHistoricalDataService_hpp

#include "soa_platform/historicaldataservice.hpp"
#include "soa_platform/soa.hpp"
#include "BondExecutionService.hpp"
#include <memory>


/**************************************** Risk.txt ****************************************/
class BondHistoricalPV01Connector: public Connector<PV01<Bond>>
{
public:
    BondHistoricalPV01Connector(){}
    // function overloading
    void Publish(const PV01<Bond>& data)override;
};

void BondHistoricalPV01Connector::Publish(const PV01<Bond>& data)
{
    ofstream os("risk.txt", ios_base::app);
    string msg = "PV01 " + std::to_string(data.GetPV01());
    os << msg << std::endl;
}


class BondHistoricalPV01Service : public HistoricalDataService<PV01<Bond> >
{
public:
    BondHistoricalPV01Service()
    {
        conn_ptr = new BondHistoricalPV01Connector();
    }
    
    PV01<Bond> & GetData(string persistKey) override;
    void OnMessage(PV01<Bond> &b) override;
    void AddListener(ServiceListener<PV01<Bond>>* listener) override;
    const std::vector<ServiceListener<PV01<Bond>>*>& GetListeners() const override;
    void PersistData(std::string persistKey, const PV01<Bond>& data) override;
    
private:
    std::vector<ServiceListener<PV01<Bond>>*> listeners;
    std::map<std::string, PV01<Bond>> risk_pool;
    BondHistoricalPV01Connector* conn_ptr;
};


class BondHistoricalPV01ServiceListener: public ServiceListener<PV01<Bond>>
{
public:
    BondHistoricalPV01ServiceListener(BondHistoricalPV01Service &hist_srv, BondRiskService &risk_srv);
    void ProcessAdd(PV01<Bond> &data) override;
    void ProcessRemove(PV01<Bond> &data) override{}
    void ProcessUpdate(PV01<Bond> &data) override{}
private:
    BondHistoricalPV01Service * srv_ptr;
};



PV01<Bond> & BondHistoricalPV01Service::GetData(string persistKey)
{
    return risk_pool.at(persistKey);
}

void BondHistoricalPV01Service::AddListener(ServiceListener<PV01<Bond> > *listener)
{
    listeners.push_back(listener);
}

const std::vector<ServiceListener<PV01<Bond>>*>& BondHistoricalPV01Service::GetListeners() const
{
    return listeners;
}

void BondHistoricalPV01Service::PersistData(std::string persistKey, const PV01<Bond>& data)
{
    conn_ptr->Publish(data);
}

void BondHistoricalPV01Service::OnMessage(PV01<Bond> &data)
{
    auto persistKey = data.GetProduct().GetProductId();
    risk_pool.emplace(persistKey, data);
    for (auto& lsnr : listeners)
        lsnr->ProcessAdd(data); // notify listeners
}

BondHistoricalPV01ServiceListener::BondHistoricalPV01ServiceListener(BondHistoricalPV01Service &hist_srv,
                                                                     BondRiskService &risk_srv)
{
    risk_srv.AddListener(this);
    srv_ptr = &hist_srv;
}
void BondHistoricalPV01ServiceListener::ProcessAdd(PV01<Bond> &data)
{
    srv_ptr->OnMessage(data);
    srv_ptr->PersistData(data.GetProduct().GetProductId(), data);
}

/**************************************** Execution.txt ****************************************/


class BondHistoricalExecutionConnector: public Connector<ExecutionOrder<Bond>>
{
public:
    BondHistoricalExecutionConnector(){}
    void Publish(const ExecutionOrder<Bond>& data)override;
};

void BondHistoricalExecutionConnector::Publish(const ExecutionOrder<Bond>& data)
{
    ofstream os("executions.txt", ios_base::app);
    std::string msg = "executing order. orderid:" + data.GetOrderId()
    + " CUSID: " + data.GetProduct().GetProductId();
    os << msg << endl;
}


class BondHistoricalExecutionService: public HistoricalDataService<ExecutionOrder<Bond>>
{
public:
    BondHistoricalExecutionService()
    {
        conn_ptr = new BondHistoricalExecutionConnector();
    }
    
    ExecutionOrder<Bond> & GetData(string persistKey) override;
    void OnMessage(ExecutionOrder<Bond> &b) override;
    void AddListener(ServiceListener<ExecutionOrder<Bond>>* listener) override;
    const std::vector<ServiceListener<ExecutionOrder<Bond>>*>& GetListeners() const override;
    void PersistData(std::string persistKey, const ExecutionOrder<Bond>& data) override;
    
private:
    std::vector<ServiceListener<ExecutionOrder<Bond>>*> listeners;
    std::map<std::string, ExecutionOrder<Bond>> exe_pool;
    BondHistoricalExecutionConnector* conn_ptr;
};


class BondHistoricalExecutionServiceListener: public ServiceListener<ExecutionOrder<Bond>>
{
public:
    BondHistoricalExecutionServiceListener(BondHistoricalExecutionService &hist_srv, BondExecutionService &exe_srv);
    void ProcessAdd(ExecutionOrder<Bond> &data) override;
    void ProcessRemove(ExecutionOrder<Bond> &data) override{}
    void ProcessUpdate(ExecutionOrder<Bond> &data) override{}
private:
    BondHistoricalExecutionService* srv_ptr;
};


ExecutionOrder<Bond> & BondHistoricalExecutionService::GetData(string persistKey)
{
    return exe_pool.at(persistKey);
}

void BondHistoricalExecutionService::AddListener(ServiceListener<ExecutionOrder<Bond>> *listener)
{
    listeners.push_back(listener);
}

const std::vector<ServiceListener<ExecutionOrder<Bond>>*>& BondHistoricalExecutionService::GetListeners() const
{
    return listeners;
}

void BondHistoricalExecutionService::PersistData(std::string persistKey, const ExecutionOrder<Bond>& data)
{
    conn_ptr->Publish(data);
}

void BondHistoricalExecutionService::OnMessage(ExecutionOrder<Bond> &data)
{
    auto persistKey = data.GetProduct().GetProductId();
    exe_pool.emplace(persistKey, data);
    for (auto& lsnr : listeners)
        lsnr->ProcessAdd(data); // notify listeners
}

BondHistoricalExecutionServiceListener::BondHistoricalExecutionServiceListener(BondHistoricalExecutionService &hist_srv, BondExecutionService &exe_srv)
{
    exe_srv.AddListener(this);
    srv_ptr = &hist_srv;
}
void BondHistoricalExecutionServiceListener::ProcessAdd(ExecutionOrder<Bond> &data)
{
    srv_ptr->OnMessage(data);
    srv_ptr->PersistData(data.GetProduct().GetProductId(), data);
}



/**************************************** Inquiries.txt ****************************************/


class BondHistoricalInquiryConnector: public Connector<Inquiry<Bond>>
{
public:
    BondHistoricalInquiryConnector(){}
    void Publish(const Inquiry<Bond>& data)override;
};

void BondHistoricalInquiryConnector::Publish(const Inquiry<Bond>& data)
{
    ofstream os("allinquiries.txt", ios_base::app);
    std::string msg;
    msg += "inquiry id is: " + data.GetInquiryId();
    data.GetSide() == Side::BUY ? msg += "; BUY " : msg += "; SELL ";
    msg += data.GetProduct().GetProductId() + " for ";
    msg += std::to_string(data.GetQuantity()) + " quantity, at ";
    msg += std::to_string(data.GetPrice()) + " price.";
    os << msg << endl;
}


class BondHistoricalInquiryService: public HistoricalDataService<Inquiry<Bond>>
{
public:
    BondHistoricalInquiryService()
    {
        conn_ptr = new BondHistoricalInquiryConnector();
    }
    
    Inquiry<Bond> & GetData(string persistKey) override;
    void OnMessage(Inquiry<Bond> &b) override;
    void AddListener(ServiceListener<Inquiry<Bond>>* listener) override;
    const std::vector<ServiceListener<Inquiry<Bond>>*>& GetListeners() const override;
    void PersistData(std::string persistKey, const Inquiry<Bond>& data) override;
    
private:
    std::vector<ServiceListener<Inquiry<Bond>>*> listeners;
    std::map<std::string, Inquiry<Bond>> iq_pool;
    BondHistoricalInquiryConnector* conn_ptr;
};


class BondHistoricalInquiryServiceListener: public ServiceListener<Inquiry<Bond>>
{
public:
    BondHistoricalInquiryServiceListener(BondHistoricalInquiryService &hist_srv, BondInquiryService &exe_srv);
    void ProcessAdd(Inquiry<Bond> &data) override;
    void ProcessRemove(Inquiry<Bond> &data) override{}
    void ProcessUpdate(Inquiry<Bond> &data) override{}
private:
    BondHistoricalInquiryService* srv_ptr;
};


Inquiry<Bond>& BondHistoricalInquiryService::GetData(string persistKey)
{
    return iq_pool.at(persistKey);
}

void BondHistoricalInquiryService::AddListener(ServiceListener<Inquiry<Bond>> *listener)
{
    listeners.push_back(listener);
}

const std::vector<ServiceListener<Inquiry<Bond>>*>& BondHistoricalInquiryService::GetListeners() const
{
    return listeners;
}

void BondHistoricalInquiryService::PersistData(std::string persistKey, const Inquiry<Bond>& data)
{
    conn_ptr->Publish(data);
}

void BondHistoricalInquiryService::OnMessage(Inquiry<Bond> &data)
{
    auto persistKey = data.GetProduct().GetProductId();
    iq_pool.emplace(persistKey, data);
    for (auto& lsnr : listeners)
        lsnr->ProcessAdd(data); // notify listeners
}

BondHistoricalInquiryServiceListener::BondHistoricalInquiryServiceListener(BondHistoricalInquiryService &hist_srv, BondInquiryService &iq_srv)
{
    iq_srv.AddListener(this);
    srv_ptr = &hist_srv;
}
void BondHistoricalInquiryServiceListener::ProcessAdd(Inquiry<Bond> &data)
{
    srv_ptr->OnMessage(data);
    srv_ptr->PersistData(data.GetProduct().GetProductId(), data);
}


#endif /* BondHistoricalDataService_hpp */
