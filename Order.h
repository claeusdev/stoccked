#ifndef ORDER_H
#define ORDER_H

#include <string>

class Order {
public:
    Order(std::string id, char t, int qty, double price, bool market, int index);
    
    bool operator<(const Order& other) const;

    std::string orderId;
    char type;  // 'B' for buy, 'S' for sell
    int quantity;
    double limitPrice;
    bool isMarketOrder;
    int originalIndex;  // To track arrival order
};

#endif // ORDER_H
