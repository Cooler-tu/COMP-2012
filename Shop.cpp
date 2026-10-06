#include <iostream>
using namespace std;

#include "Customer.h"
#include "Shop.h"
#include "Platform.h"
#include "Product.h"
#include "Rating.h"

// To do: Implement the member functions here!

Shop::Shop(int shop_ID, Platform* platform, double cut_rate):
    shop_ID(shop_ID), platform(platform), cut_rate(cut_rate), balance(0), 
    inventory(new Product* [2]), num_of_inventory(0), inventory_capacity(2), ratings(nullptr)
{
    if(platform != nullptr) platform->add_shop(this);
}

Shop::~Shop()
{
    for(int i = 0; i < num_of_inventory; i++)
        delete inventory[i];
    delete [] inventory;
    Rating* now = ratings;
    while(now != nullptr)
    {
        Rating* next = now->shop_next;
        now->shop = nullptr;
        now->shop_next = nullptr;
        if(now->customer == nullptr)
            delete now;
        now = next;
    }
}

bool Shop::stock_product(Product* product)
{
    if(product == nullptr) return false;
    for(int i = 0; i < num_of_inventory; i++)
        if(inventory[i] == product) return false;
    product->set_origin_ID(this);
    if(num_of_inventory == inventory_capacity)
    {
        inventory_capacity*=2;
        Product** inventory_new = new Product* [inventory_capacity];
        for(int j = 0; j < num_of_inventory; j++)  
            inventory_new[j] = inventory[j];
        delete [] inventory;
        inventory = inventory_new;
    }
    for(int i = num_of_inventory; i > 0; i--)
    {
        if(inventory[i-1]->get_ID() < product->get_ID())
        {
            inventory[i] = product;
            break;
        }
        inventory[i] = inventory[i-1];
        inventory[i-1] = nullptr;
    }
    if(inventory[0] == nullptr) inventory[0] = product;
    num_of_inventory++;
    return true;
}  

// Only write in the to-do section, PLEASE DO NOT CHANGE ANYTHING ELSE!
void Shop::print_ratings() const {
    cout << "----------------------- SHOP RATING DATA ----------------------" << endl;
    cout << "Ratings for Shop ID: " << shop_ID << endl;
    if (ratings == nullptr) {
        cout << "- No reviews available." << endl;
    } else {
        // To-do: Implement the Rating printing here!
        Rating* now = ratings;
        while(now != nullptr)
        {
            now->print(true, false);
            now = now->shop_next;
        }
        // End of to-do
    }
    cout << "----------------------------- END -----------------------------" << endl << endl;
}

void Shop::set_ratings_head(Rating* r)
{
    if(r == nullptr) return;
    if(r->shop_ID != shop_ID) return;
    
    if(ratings == nullptr)
    {
        ratings = r;
        return;
    }
    if (r == ratings) return;
    Rating* now = ratings->shop_next, *pre = ratings;
    while(now != nullptr)
    {
        if(now == r)
        {
            pre->shop_next = now->shop_next;
            now->shop_next = ratings;
            ratings = now;
            return;
        }
        now = now->shop_next;
        pre = pre->shop_next;
    }
    r->shop_next = ratings;
    ratings = r;
}

const Rating* Shop::get_i_th_rating(int i)
{
    if(i < 0) return nullptr;
    Rating* now = ratings, *pre = nullptr;
    while(now != nullptr && i != 0)
    {
        pre = now;
        now = now->shop_next;
        i--;
    }
    if(i == 0)
        return now;
    else return nullptr;
}

int Shop::get_average_score() const
{
    if(ratings == nullptr) return -1;
    int tot = 0, n = 0;
    Rating* now = ratings;
    while(now != nullptr)
    {
        tot += now->score;
        n++;
        now = now->shop_next;
    }
    return tot/n;
}

bool Shop::remove_rating(int customer_ID)
{
    Rating* now = ratings, *pre = nullptr;
    while(now != nullptr)
    {
        if(now->customer_ID == customer_ID)
        {
            if(pre) pre->shop_next = now->shop_next;
            else ratings = now->shop_next;
            now->shop = nullptr;
            now->shop_next = nullptr;
            if(now->customer == nullptr) delete now;
            return true;
        }
        pre = now;
        now = now->shop_next;
    }
    return false;
}

bool Shop::handle_order(const int* order, int order_size, Customer* customer)
{
    if(order == nullptr || order_size <= 0 || customer == nullptr)
        return false;
        
    int pos = 0, tot = 0, i;
    for(i = 0; i < order_size;)
    {
        while(pos < num_of_inventory && order[i] > inventory[pos]->get_ID()){pos++;}
        while(pos < num_of_inventory && i < order_size && order[i] == inventory[pos]->get_ID())
        {
            tot += inventory[pos]->get_price();
            i++; pos++;
        }
        if(pos < num_of_inventory && i < order_size && order[i] < inventory[pos]->get_ID())
            return false;
        if(pos == num_of_inventory) return false;
    }
    if(i != order_size) return false;

    int rev = 0;
    if(platform == nullptr) rev = tot;
    else if(this->platform->check_if_VIP(customer->get_ID()))
    {
        tot *= platform->get_VIP_discount();
        rev = tot * cut_rate / 2;
    }
    else rev = tot * cut_rate;
    if(tot > customer->get_balance()) return false;

    pos = 0;
    for(int i = 0; i < order_size;)
    {
        while(pos < num_of_inventory && order[i] > inventory[pos]->get_ID()){pos++;}
        while(pos < num_of_inventory && i < order_size && order[i] == inventory[pos]->get_ID())
        {
            customer->add_product(inventory[pos]);
            remove_product(inventory[pos]->get_ID());
            i++;
        }
    }

    customer->change_balance(-1*tot);
    this->change_balance(rev);
    platform->change_balance(tot-rev);
    return true;

}

// End of to-do



// PLEASE DO NOT CHANGE!
int Shop::get_platform_ID() const {
    if (platform == nullptr) {
        return -1;
    } else {
        return platform->get_ID();
    }
}

void Shop::print() const {
    cout << "-------------------------- SHOP DATA --------------------------" << endl;
    cout << "Shop ID: " << shop_ID;
    if (platform != nullptr) {
        cout << "; Associated platform ID: " << platform->get_ID() << " (Cut Rate: " << cut_rate*100 << "%)" << endl;
    } else {
        cout << "; No associated platform."  << endl;
    }
    cout << "- Balance: $" << balance << endl;
    cout << "- Number of Products in Inventory: " << num_of_inventory << endl;
    cout << "----------------------------- END -----------------------------" << endl << endl;
}

void Shop::print_inventory() const {
    cout << "------------------------ INVENTORY DATA -----------------------" << endl;
    cout << "Inventory of Shop ID: " << shop_ID << endl;
    if (num_of_inventory == 0) cout << "- Out of stock." << endl;
    for (int i = 0; i < num_of_inventory; i++) {
        inventory[i]->print();
    }
    cout << "----------------------------- END -----------------------------" << endl << endl;
}