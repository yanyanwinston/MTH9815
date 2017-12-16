/**
 * tradebookingservice.hpp
 * Defines the data types and Service for trade booking.
 *
 * @author Breman Thuraisingham
 */
#ifndef TRADE_BOOKING_SERVICE_HPP
#define TRADE_BOOKING_SERVICE_HPP

#include <string>
#include <vector>
#include "soa.hpp"

// Trade sides
enum Side { BUY, SELL };

/**
 * Trade object with a price, side, and quantity on a particular book.
 * Type T is the product type.
 */
template<typename T>
class Trade
{

public:
  Trade(){}
    
  // ctor for a trade
  Trade(const T &_product, string _tradeId, string _book, long _quantity, Side _side);//, double _price

  // Get the product
  const T& GetProduct() const;

  // Get the trade ID
  const string& GetTradeId() const;

  // Get the book
  const string& GetBook() const;

  // Get the quantity
  long GetQuantity() const;

  // Get the side
  Side GetSide() const;
  
  //Get the price
  double GetPrice() const;

private:
  T product;
  string tradeId;
  string book;
  long quantity;
  Side side;
  //double price;

};

/**
 * Trade Booking Service to book trades to a particular book.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class TradeBookingService : public Service<string,Trade <T> >
{

public:

  // Book the trade
  virtual void BookTrade(Trade<T> &trade) = 0;
  // trades.txt
  // 2y, TID_1, TRSY1, 1000.000, 100.0, BUY
  // 3y, TID_2, TRSY1, 5000.000, 45, SELL
};

template<typename T>
Trade<T>::Trade(const T &_product, string _tradeId, string _book, long _quantity, Side _side) ://, double _price
  product(_product)
{
  tradeId = _tradeId;
  book = _book;
  quantity = _quantity;
  side = _side;
  //price = _price;
}

template<typename T>
const T& Trade<T>::GetProduct() const
{
  return product;
}

template<typename T>
const string& Trade<T>::GetTradeId() const
{
  return tradeId;
}

template<typename T>
const string& Trade<T>::GetBook() const
{
  return book;
}

template<typename T>
long Trade<T>::GetQuantity() const
{
  return quantity;
}

template<typename T>
Side Trade<T>::GetSide() const
{
  return side;
}
/*
template<typename T>
double Trade<T>::GetPrice() const
{
  return price;
}*/

template<typename T>
void TradeBookingService<T>::BookTrade(Trade<T> &trade)
{
}

#endif
