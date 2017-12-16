//
//  BondInquiryService.hpp
//  trading_system
//
//  Created by Yan Yan on 12/10/17.
//  Copyright © 2017 Yan Yan. All rights reserved.
//

#ifndef BondInquiryService_hpp
#define BondInquiryService_hpp

#include <stdio.h>
#include "soa_platform/inquiryservice.hpp"
#include "soa_platform/products.hpp"
#include "utils.hpp"
#include <map>

typedef Inquiry<Bond> BondInquiry;

class BondInquiryService: public InquiryService<Bond>
{
public:
    // ctor for an BondInquiryService
    BondInquiryService(const Connector<BondInquiry>& _conn): conn(&const_cast<Connector<BondInquiry>&>(_conn)){}
    
    // Send a quote back to the client
    void SendQuote(const string &inquiryId, double price) override;
    
    // Reject an inquiry from the client
    void RejectInquiry(const string &inquiryId) override;
    
    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(BondInquiry &data) override;
    
    // Get data on BondInquirService given a key
    BondInquiry& GetData(std::string key) override;
    
    // Add a listener to the Service for callbacks on add, remove, and update events for data to the Service.
    void AddListener(ServiceListener<BondInquiry> *listener) override;
    
    // Get all listeners on the Service.
    const vector< ServiceListener<BondInquiry>* >& GetListeners() const override;
    
    // Get inquiryid
    std::string GetID(const BondInquiry& v);
private:
    Connector<BondInquiry> *conn;
    std::vector<ServiceListener<BondInquiry>*> listeners;
    std::map<std::string, BondInquiry> data_pool;
};


class BondInquiryConnector : public Connector<BondInquiry>, private FileReader
{
public:
    // ctor
    BondInquiryConnector(std::string file);
    
    // Start load file
    void start(BondInquiryService &inqsrv);
    
    // Publish data to the Connector
    void Publish(const BondInquiry& data) override;
private:
    BondInquiryService* srv_ptr;
};



BondInquiry& BondInquiryService::GetData(std::string key)
{
    return data_pool[key];
}

void BondInquiryService::AddListener(ServiceListener<BondInquiry> *listener)
{
    listeners.push_back(listener);
}
const std::vector< ServiceListener<BondInquiry>* >& BondInquiryService::GetListeners() const
{
    return listeners;
}
std::string BondInquiryService::GetID(const BondInquiry& v)
{
    return v.GetProduct().GetProductId();
}

void BondInquiryService::OnMessage(BondInquiry &data)
{
    switch(data.GetState()){
        case RECEIVED :
            data_pool.emplace(data.GetInquiryId(), data);
            for(auto&& lsnr : listeners)
                lsnr->ProcessAdd(data);
            SendQuote(data.GetInquiryId(), 100.00);
            break;
        case QUOTED :
            data_pool.emplace(data.GetInquiryId(), data);
            for(auto&& lsnr : listeners)
                lsnr->ProcessAdd(data);
            break;
        case DONE :
            data_pool.emplace(data.GetInquiryId(), data);
            for(auto&& lsnr : listeners)
                lsnr->ProcessAdd(data);
            break;
        default :
            break;
    }
}

void BondInquiryService::RejectInquiry(const string &inquiryId)
{
    std::cout<<"Reject Inquiry!"<<std::endl;
}

void BondInquiryService::SendQuote(const string &inquiryId, double price)
{
    auto inq = GetData(inquiryId);
    inq.SetPrice(price);
    conn->Publish(inq);
}

BondInquiryConnector::BondInquiryConnector(std::string file): FileReader(file){srv_ptr=nullptr;}

void BondInquiryConnector::Publish(const BondInquiry& data)
{
    // Set state to QUOTED
    auto quoted = data;
    quoted.SetState(QUOTED);
    srv_ptr->OnMessage(quoted);
    // Set state to DONE
    quoted.SetState(DONE);
    srv_ptr->OnMessage(quoted);
}

void BondInquiryConnector::start(BondInquiryService &inqsrv)
{
    srv_ptr = &inqsrv;
    std::vector<std::string> record;
    if(srv_ptr != nullptr){
        while(ReadLine(record)){
            BondInquiry inq(record[0], makeBond(record[1]), Side(stoi(record[2])), stoi(record[3]), BondPrice_string2double(record[4]), InquiryState(stoi(record[5])));
            srv_ptr->OnMessage(inq);
        }
    }
}


#endif /* BondInquiryService_hpp */
