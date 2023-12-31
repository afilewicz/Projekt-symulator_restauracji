#include <stdexcept>

class DishAlreadyExistsError : public std::invalid_argument
{
public:
    DishAlreadyExistsError(std::string dish_name) : std::invalid_argument("Dish with following name already exists: " + dish_name) {}
};

class DishDoesNotExistError : public std::invalid_argument
{
public:
    DishDoesNotExistError(std::string dish_name) : std::invalid_argument("Dish with following name does not exist: " + dish_name) {}
};

class EmptyNameError : public std::invalid_argument
{
public:
    EmptyNameError() : std::invalid_argument("Name cannot be empty") {}
};

class NegativePriceError : public std::invalid_argument
{
public:
    NegativePriceError() : std::invalid_argument("Price cannot be negative") {}
};

class IngredientDoesNotExistError : public std::invalid_argument
{
public:
    IngredientDoesNotExistError(std::string ingredient_name) : std::invalid_argument("Ingredient with following name does not exist: " + ingredient_name) {}
};

class RemovingFromEmptyListError : public std::invalid_argument
{
public:
    RemovingFromEmptyListError() : std::invalid_argument("Can not remove from empty list") {}
};

class NegativeCaloriesError : public std::invalid_argument
{
public:
    NegativeCaloriesError() : std::invalid_argument("Calories cannot be negative") {}
};

class MenuSectionDoesNotExistError : public std::invalid_argument
{
public:
    MenuSectionDoesNotExistError(std::string menu_section_name) : std::invalid_argument("Menu section with following name does not exist: " + menu_section_name) {}
};

class NoFreeTableError : public std::invalid_argument
{
public:
    NoFreeTableError(size_t num_of_people) : std::invalid_argument("No free table for group with following number of people: " + num_of_people) {}
};

class EmptyQueueError : public std::invalid_argument
{
public:
    EmptyQueueError() : std::invalid_argument("Queue is empty") {}
};

class OrderNotFoundError : public std::invalid_argument
{
public:
    OrderNotFoundError(uint32_t id) : std::invalid_argument("Order with given id not found: " + id) {}
};

class TableNotReadyToOrderError : public std::invalid_argument
{
public:
    TableNotReadyToOrderError(uint32_t id) : std::invalid_argument("Table with given id is not ready to order: " + id) {}
};

class TableNotReadyToPayError : public std::invalid_argument
{
public:
    TableNotReadyToPayError(uint32_t id) : std::invalid_argument("Table with given id is not ready to pay: " + id) {}
};

class TableNotReadyToBeCleanedError : public std::invalid_argument
{
public:
    TableNotReadyToBeCleanedError(uint32_t id) : std::invalid_argument("Table with given id is not ready to be cleaned: " + id) {}
};

class NoOrdersToPrepareError : public std::invalid_argument
{
public:
    NoOrdersToPrepareError() : std::invalid_argument("No orders to prepare") {}
};

class NoClientsInQueueError : public std::invalid_argument
{
public:
    NoClientsInQueueError() : std::invalid_argument("No clients in queue") {}
};