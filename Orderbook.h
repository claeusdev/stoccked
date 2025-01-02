#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <vector>
#include <fstream>
#include <string>
#include "Order.h"

class OrderBook {
public:
    OrderBook(const std::string& outputFileName, double initialPrice);
    ~OrderBook();

    void processOrder(const std::string& id, char type, int quantity, 
                     double price, bool isMarket);

private:
    void matchOrder(Order& order, std::vector<Order>& counterOrders,
                   std::vector<Order>& sameTypeOrders);
    void writeExecution(const std::string& orderId, int quantity,
                       double price, bool isBuy);
    void writeUnexecutedOrders();
    void displayOrderBook();

    std::vector<Order> buyOrders;
    std::vector<Order> sellOrders;
    double lastTradedPrice;
    std::ofstream outputFile;
    int orderIndex;
};

#endif // ORDERBOOK_H
