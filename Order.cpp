#include "Order.h"

Order::Order(std::string id, char t, int qty, double price, bool market, int index)
    : orderId(id), type(t), quantity(qty), limitPrice(price),
      isMarketOrder(market), originalIndex(index) {}

bool Order::operator<(const Order& other) const {
    if (type == 'B') {  // Buy orders
        if (isMarketOrder != other.isMarketOrder)
            return isMarketOrder;  // Market orders have higher priority
        if (!isMarketOrder && limitPrice != other.limitPrice)
            return limitPrice > other.limitPrice;  // Higher price = higher priority
        return originalIndex < other.originalIndex;  // Earlier = higher priority
    } else {  // Sell orders
        if (isMarketOrder != other.isMarketOrder)
            return isMarketOrder;  // Market orders have higher priority
        if (!isMarketOrder && limitPrice != other.limitPrice)
            return limitPrice < other.limitPrice;  // Lower price = higher priority
        return originalIndex < other.originalIndex;  // Earlier = higher priority
    }
}
