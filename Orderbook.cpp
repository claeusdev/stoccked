#include "Orderbook.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

OrderBook::OrderBook(const std::string& outputFileName, double initialPrice) 
    : lastTradedPrice(initialPrice), orderIndex(0) {
    outputFile.open(outputFileName);
    outputFile << std::fixed << std::setprecision(2);
    std::cout << std::fixed << std::setprecision(2);
}

OrderBook::~OrderBook() {
    writeUnexecutedOrders();
    outputFile.close();
}

void OrderBook::processOrder(const std::string& id, char type, int quantity, 
                         double price, bool isMarket) {
    Order newOrder(id, type, quantity, price, isMarket, orderIndex++);

    // Try to match and execute the order
    if (type == 'B') {
        matchOrder(newOrder, sellOrders, buyOrders);
    } else {
        matchOrder(newOrder, buyOrders, sellOrders);
    }

    displayOrderBook();
}

void OrderBook::matchOrder(Order& order, std::vector<Order>& counterOrders,
                       std::vector<Order>& sameTypeOrders) {
    while (order.quantity > 0 && !counterOrders.empty()) {
        // Sort counter orders by priority
        std::sort(counterOrders.begin(), counterOrders.end());
        Order& topOrder = counterOrders[0];

        // Check if orders can match
        bool canMatch = order.isMarketOrder || topOrder.isMarketOrder ||
                      (order.type == 'B' ? 
                       order.limitPrice >= topOrder.limitPrice :
                       order.limitPrice <= topOrder.limitPrice);

        if (!canMatch) break;

        // Determine execution price
        double execPrice;
        if (order.isMarketOrder && topOrder.isMarketOrder) {
            execPrice = lastTradedPrice;
        } else if (order.isMarketOrder) {
            execPrice = topOrder.limitPrice;
        } else if (topOrder.isMarketOrder) {
            execPrice = order.limitPrice;
        } else {
            execPrice = (order.originalIndex < topOrder.originalIndex) ?
                       order.limitPrice : topOrder.limitPrice;
        }

        // Determine execution quantity
        int execQty = std::min(order.quantity, topOrder.quantity);
        
        // Record the execution
        if (order.type == 'B') {
            writeExecution(order.orderId, execQty, execPrice, true);
            writeExecution(topOrder.orderId, execQty, execPrice, false);
        } else {
            writeExecution(topOrder.orderId, execQty, execPrice, true);
            writeExecution(order.orderId, execQty, execPrice, false);
        }

        lastTradedPrice = execPrice;

        // Update quantities
        order.quantity -= execQty;
        topOrder.quantity -= execQty;

        // Remove fully executed counter order
        if (topOrder.quantity == 0) {
            counterOrders.erase(counterOrders.begin());
        }
    }

    // Add remaining order to appropriate list if not fully executed
    if (order.quantity > 0) {
        sameTypeOrders.push_back(order);
    }
}

void OrderBook::writeExecution(const std::string& orderId, int quantity,
                           double price, bool isBuy) {
    outputFile << "order " << orderId << " " << quantity << " shares "
              << (isBuy ? "purchased" : "sold") << " at price "
              << price << std::endl;
}

void OrderBook::writeUnexecutedOrders() {
    std::vector<Order> allUnexecuted;
    
    for (const Order& order : buyOrders) {
        allUnexecuted.push_back(order);
    }
    for (const Order& order : sellOrders) {
        allUnexecuted.push_back(order);
    }
    
    std::sort(allUnexecuted.begin(), allUnexecuted.end(),
             [](const Order& a, const Order& b) {
                 return a.originalIndex < b.originalIndex;
             });
             
    for (const Order& order : allUnexecuted) {
        outputFile << "order " << order.orderId << " "
                  << order.quantity << " shares unexecuted" << std::endl;
    }
}

void OrderBook::displayOrderBook() {
    std::cout << "\nLast trading price: " << lastTradedPrice << std::endl;
    std::cout << "Buy                       Sell" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    size_t maxLines = std::max(buyOrders.size(), sellOrders.size());
    for (size_t i = 0; i < maxLines; i++) {
        // Print buy order if exists
        if (i < buyOrders.size()) {
            const Order& buy = buyOrders[i];
            std::cout << std::left << std::setw(12) << buy.orderId;
            if (buy.isMarketOrder)
                std::cout << "M     ";
            else
                std::cout << buy.limitPrice << " ";
            std::cout << std::setw(8) << buy.quantity;
        } else {
            std::cout << std::setw(25) << " ";
        }

        // Print sell order if exists
        if (i < sellOrders.size()) {
            const Order& sell = sellOrders[i];
            std::cout << std::left << std::setw(12) << sell.orderId;
            if (sell.isMarketOrder)
                std::cout << "M     ";
            else
                std::cout << sell.limitPrice << " ";
            std::cout << sell.quantity;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
