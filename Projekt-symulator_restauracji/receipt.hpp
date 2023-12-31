#pragma once
#include "order.hpp"
#include <ostream>

class Receipt
{
private:
    Order order_;

public:
    Receipt(const Order order);
    Order &get_order();
    int get_total_price();
    std::ostream &show_receipt(std::ostream &os);
};