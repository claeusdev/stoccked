#include <iostream>
#include <fstream>
#include <sstream>
#include "Orderbook.h"

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
        std::istringstream iss(line);
        iss >> orderId >> type >> quantity;
        
        bool isMarket = !(iss >> price);
        orderBook.processOrder(orderId, type, quantity, price, isMarket);
    }

    inputFile.close();
    return 0;
}
