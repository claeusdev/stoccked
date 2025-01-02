#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <sstream>

class Order {
public:
    std::string orderId;
    char type;  // 'B' for buy, 'S' for sell
    int quantity;
    double limitPrice;
    bool isMarketOrder;
    int originalIndex;  // To track arrival order

    Order(std::string id, char t, int qty, double price, bool market, int index)
        : orderId(id), type(t), quantity(qty), limitPrice(price),
          isMarketOrder(market), originalIndex(index) {}

    // For priority comparison
    bool operator<(const Order& other) const {
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
};

class OrderBook {
private:
    std::vector<Order> buyOrders;
    std::vector<Order> sellOrders;
    double lastTradedPrice;
    std::ofstream outputFile;
    int orderIndex;

public:
    OrderBook(const std::string& outputFileName, double initialPrice) 
        : lastTradedPrice(initialPrice), orderIndex(0) {
        outputFile.open(outputFileName);
        outputFile << std::fixed << std::setprecision(2);
        std::cout << std::fixed << std::setprecision(2);
    }

    ~OrderBook() {
        writeUnexecutedOrders();
        outputFile.close();
    }

    void processOrder(const std::string& id, char type, int quantity, 
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

private:
    void matchOrder(Order& order, std::vector<Order>& counterOrders,
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

            // Determine execution price based on market order rules
            double execPrice;
            if (order.isMarketOrder && topOrder.isMarketOrder) {
                execPrice = lastTradedPrice;
            } else if (order.isMarketOrder && !topOrder.isMarketOrder) {
                execPrice = topOrder.limitPrice;
            } else if (!order.isMarketOrder && topOrder.isMarketOrder) {
                execPrice = order.limitPrice;
            } else {
                execPrice = (topOrder.originalIndex < order.originalIndex) ?
                           topOrder.limitPrice : order.limitPrice;
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

            // Update quantities and maintain residual orders
            order.quantity -= execQty;
            topOrder.quantity -= execQty;
            
            std::cout << "DEBUG: Executed " << execQty << " shares at " << execPrice 
                      << ", Remaining: order=" << order.quantity 
                      << ", top=" << topOrder.quantity << std::endl;

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

    void writeExecution(const std::string& orderId, int quantity,
                       double price, bool isBuy) {
        outputFile << "order " << orderId << " " << quantity << " shares "
                  << (isBuy ? "purchased" : "sold") << " at price "
                  << price << std::endl;
    }

    void writeUnexecutedOrders() {
        // Sort orders by original index before writing
        std::vector<Order> allUnexecuted;
        
        // Collect all unexecuted orders
        for (const Order& order : buyOrders) {
            allUnexecuted.push_back(order);
        }
        for (const Order& order : sellOrders) {
            allUnexecuted.push_back(order);
        }
        
        // Sort by original arrival time
        std::sort(allUnexecuted.begin(), allUnexecuted.end(),
                 [](const Order& a, const Order& b) {
                     return a.originalIndex < b.originalIndex;
                 });
                 
        // Write in arrival order
        for (const Order& order : allUnexecuted) {
            outputFile << "order " << order.orderId << " "
                      << order.quantity << " shares unexecuted" << std::endl;
        }
    }

    void displayOrderBook() {
        std::cout << "\nLast trading price: " << lastTradedPrice << std::endl;
        std::cout << "Buy                       Sell" << std::endl;
        std::cout << "------------------------------------------" << std::endl;

        // Display orders in arrival order (no sorting)
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
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string inputFileName = argv[1];
    std::string outputFileName = "output" + inputFileName.substr(5);
    
    std::ifstream inputFile(inputFileName);
    if (!inputFile) {
        std::cerr << "Error opening input file" << std::endl;
        return 1;
    }

    // Read initial trading price
    double initialPrice;
    inputFile >> initialPrice;
    
    OrderBook orderBook(outputFileName, initialPrice);

    // Process orders
    std::string orderId;
    char type;
    int quantity;
    double price;
    std::string line;
    
    // Skip the first line (already read initial price)
    std::getline(inputFile, line);
    
    while (std::getline(inputFile, line)) {
        std::replace(line.begin(), line.end(), '\t', ' ');
        std::istringstream iss(line);
        iss >> orderId >> type >> quantity;
        
        bool isMarket = !(iss >> price);
        orderBook.processOrder(orderId, type, quantity, price, isMarket);
    }

    inputFile.close();
    return 0;
}
