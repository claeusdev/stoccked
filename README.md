# Stock Market Order Processing System

A C++ implementation of a simplified stock market order matching system. The program processes buy and sell orders for a single stock, handling both limit orders and market orders according to price-time priority rules.

## Features

- Support for both market and limit orders
- Price-time priority matching
- Partial order execution
- Real-time order book display
- Detailed execution logging
- Support for order priorities and matching rules

## Requirements

- C++11 compatible compiler (g++ recommended)
- make

## Building the Project

Clone the repository and build using make:

```bash
git clone [repository-url]
cd stock-market-system
make
```

## Running the Program

The program takes an input file as a command-line argument:

```bash
./stock_market tests/input1.txt
```

### Input File Format

The input file should be formatted as follows:
- First line: Initial trading price (floating-point number)
- Subsequent lines: Order entries in the format:
  ```
  orderID type quantity [price]
  ```
  where:
  - orderID: Unique identifier for the order
  - type: 'B' for buy, 'S' for sell
  - quantity: Number of shares
  - price: Limit price (optional - if omitted, the order is treated as a market order)

Example input file:
```
1100.00
bob1    S    1    1200.00
cathie1 B    1000
elon    S    1000  1100.00
```

### Output

The program produces two types of output:
1. Console display: Shows the current state of the order book after each order
2. Output file: Records all executions and unexecuted orders (named output*.txt corresponding to input*.txt)

## Project Structure

- `main.cpp`: Entry point and file I/O handling
- `Order.h/cpp`: Order class definition and implementation
- `OrderBook.h/cpp`: OrderBook class for order processing and matching
- `Makefile`: Build configuration
- `.gitignore`: Git ignore rules

## Cleaning up

To remove compiled files and outputs:

```bash
make clean
```

## Order Matching Rules

- Market orders have higher priority than limit orders
- For limit orders:
  - Buy orders: Higher price has higher priority
  - Sell orders: Lower price has higher priority
- When prices are equal, earlier orders have priority
- Orders can be partially executed

## License

[Your chosen license]

## Contributing

[Your contribution guidelines if applicable]
