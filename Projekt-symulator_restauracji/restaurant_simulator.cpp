#include "restaurant_simulator.hpp"
#include "read_preparing_time.hpp"
#include "table_id.hpp"
#include <random>
#include <map>

std::string format_price(int price);

RestaurantSimulator::RestaurantSimulator(Restaurant &&restaurant) : restaurant_(restaurant) {}

Restaurant &RestaurantSimulator::get_restaurant()
{
    return restaurant_;
}
void RestaurantSimulator::set_restaurant_attributes(std::string menu_path, std::string tables_path)
{
    Menu menu = load_menu(menu_path);
    load_tables(tables_path);
    Waiter waiter{restaurant_};
    Kitchen kitchen{restaurant_};
    restaurant_.set_waiter(waiter);
    restaurant_.set_kitchen(kitchen);
    restaurant_.set_menu(menu);
    restaurant_.get_kitchen().set_time_to_prepare(load_time_to_prepare(menu_path));
}

Menu RestaurantSimulator::load_menu(std::string path_to_file)
{
    Menu menu{};
    read_from_csv<MenuItem, Menu, Ingredient>(path_to_file, menu);
    return menu;
}

std::map<std::string, time_> RestaurantSimulator::load_time_to_prepare(std::string path_to_file)
{
    std::map<std::string, time_> time_to_prepare{};
    read_time_to_prepare(path_to_file, time_to_prepare);
    return time_to_prepare;
}

void RestaurantSimulator::load_tables(std::string path_to_file)
{
    read_tables_from_csv<Table>(path_to_file, restaurant_.get_tables());
}

void RestaurantSimulator::add_clients_to_queue(uint8_t num_of_clients)
{
    for (size_t i = 0; i < num_of_clients; i++)
    {
        std::vector<Client> clients = {};
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(1, 6);
        size_t n = dis(gen);
        for (size_t j = 0; j < n; j++)
        {
            clients.emplace_back(Client{});
        }
        queue_.emplace_back(ClientGroup{clients});
    }
}

void RestaurantSimulator::let_in_one_group_and_place()
{
    if (queue_.empty())
    {
        throw NoClientsInQueueError();
    }
    ClientGroup first_group = queue_.front();
    std::optional<Table> table = restaurant_.get_waiter().get_free_table(first_group.get_number_of_clients());
    if (table)
    {
        restaurant_.get_waiter().place_at_table(*table, first_group);
        make_table_ready(table.value());
        queue_.pop_front();
        restaurant_.remove_table(table.value().get_id());
        restaurant_.add_table(table.value());
    }
    else
        throw NoFreeTableError(first_group.get_clients().size());
}

void RestaurantSimulator::make_table_ready(Table &table)
{
    for (auto &client : table.get_clients())
    {
        client.make_order(restaurant_.get_menu());
    }
    table.switch_ready_to_order();
}

void RestaurantSimulator::clean_table(table_id id)
{
    if (restaurant_.get_table_by_id(id).get_ready_to_be_cleaned())
    {
        uint32_t seats = restaurant_.get_table_by_id(id).get_num_of_seats();
        restaurant_.remove_table(id);
        restaurant_.add_table(Table{id, seats});
    }
    else
        throw TableNotReadyToBeCleanedError(id);
}

void RestaurantSimulator::take_order_from_table(table_id table_id)
{
    if (restaurant_.get_table_by_id(table_id).get_ready_to_order())
    {
        restaurant_.get_waiter().take_order(table_id);
        restaurant_.get_table_by_id(table_id).switch_ready_to_order();
    }
    else
        throw TableNotReadyToOrderError(table_id);
}

void RestaurantSimulator::prepare_first_order()
{
    if (!restaurant_.get_kitchen().get_to_do_orders().empty())
    {
        Order &order = restaurant_.get_kitchen().get_to_do_orders().front();
        restaurant_.get_kitchen().prepare_order(order);
    }
    else
        throw NoOrdersToPrepareError();
}

void RestaurantSimulator::serve_ready_order(table_id table_id)
{
    for (auto order : get_restaurant().get_kitchen().get_ready_orders())
    {
        if (order.get_order_id() == table_id)
        {
            restaurant_.get_table_by_id(table_id).add_ready_order(order);
            restaurant_.get_kitchen().remove_ready_order(table_id);
            restaurant_.get_table_by_id(table_id).switch_ready_for_receipt();
            return;
        }
    }
    throw OrderNotFoundError(table_id);
}

void RestaurantSimulator::bring_receipt_to_table(table_id table_id)
{
    if (restaurant_.get_table_by_id(table_id).get_ready_for_receipt())
    {
        restaurant_.get_waiter().give_receipt(restaurant_.get_table_by_id(table_id));
        restaurant_.get_table_by_id(table_id).switch_ready_for_receipt();
        restaurant_.get_table_by_id(table_id).switch_ready_to_be_cleaned();
    }
    else
        throw TableNotReadyToPayError(table_id);
}

bool RestaurantSimulator::check_if_end()
{
    return (queue_.empty() && restaurant_.empty_tables());
}

table_id RestaurantSimulator::drawn_id()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<table_id> dis(0, get_restaurant().get_tables().size() - 1);
    return dis(gen);
}

std::ostream &RestaurantSimulator::show_tables_info(std::ostream &os)
{
    os << "\nStoliki: " << std::endl;
    for (const auto &[id, table] : restaurant_.get_tables())
    {
        auto free_seats = table.get_free_seats();
        os << "\t"
           << "Stolik nr: " << id << ", wolnych miejsc: "
           << free_seats << "/" << table.get_num_of_seats()
           << ", gotowy do zamówienia: " << table.get_ready_to_order()
           << ", gotowy do rachunku: " << table.get_ready_for_receipt()
           << ", gotowy do sprzątania: " << table.get_ready_to_be_cleaned()
           << '\n';
    }
    os << "\n";
    return os;
}

std::ostream &RestaurantSimulator::show_queue_info(std::ostream &os)
{
    if (queue_.empty())
    {
        os << "\nKolejka jest pusta."
           << "\n"
           << "\n";
        return os;
    }
    os << "\nKlienci w kolejce: " << std::endl;
    uint16_t counter = 1;
    for (const auto &group : queue_)
    {
        os << " " << counter << ". liczba osób: "
           << group.get_number_of_clients() << '\n';
        counter++;
    }
    os << "\n";
    return os;
}

std::ostream &RestaurantSimulator::show_menu(std::ostream &os)
{
    os << "\nMenu: " << std::endl;
    for (MenuSection menu_section : restaurant_.get_menu().get_menu_sections())
    {
        os << '\n';
        os << "  " << menu_section.get_name() << ":" << std::endl;
        for (const auto &[name, item] : menu_section.get_menu_items())
        {
            os << std::endl;
            os << "  - " << item.get_name() << "\t" << format_price(item.get_price())
               << " " << item.get_total_calories() << " kcal" << '\n';
            for (Ingredient ingredient : item.get_ingredients())
                os << "     * " << ingredient.get_name() << '\n';
        }
    }
    os << "\n";
    return os;
}

std::ostream &RestaurantSimulator::show_kitchen_info(std::ostream &os)
{
    if (!restaurant_.get_kitchen().get_ready_orders().empty())
    {
        os << "\nZamówienia gotowe: \n";
        uint16_t r_counter = 0;
        for (auto &order : restaurant_.get_kitchen().get_ready_orders())
        {
            os << "\nZamówienie nr " << r_counter << " - stolik nr " << order.get_order_id() << ": \n";
            for (auto &dish : order.get_dishes())
                os << "   - " << dish.get_name() << "\n";
            ++r_counter;
        }
    }
    else
    {
        os << "\nBrak gotowych zamówień.\n";
    }

    if (!restaurant_.get_kitchen().get_to_do_orders().empty())
    {
        std::cout << "\nZamówienia do przygotowania: \n";
        uint16_t r_counter = 0;
        for (auto &order : restaurant_.get_kitchen().get_to_do_orders())
        {
            os << "\nZamówienie nr " << r_counter << " - stolik nr " << order.get_table_id() << ": \n";
            for (auto &dish : order.get_ordered_dishes())
                os << "   - " << dish.get_name() << "\n";
            ++r_counter;
        }
    }
    else
    {
        os << "\nBrak zamówień do przygotowania.\n";
    }
    return os;
}