#include <iostream>
using namespace std;

#include "Product.h"
#include "Shop.h"

// To do: Implement the constructors here!

Product::Product(int product_ID, int price, Shop* shop):
    product_ID(product_ID), price(price), origin_ID(-1)
{
    if(shop != nullptr) origin_ID = shop->get_ID();
}

Product::Product(const Product& other):
    product_ID(other.product_ID), price(other.price), origin_ID(other.origin_ID)
{}

Product* Shop::remove_product(int product_ID)
{
    Product* r = nullptr;
    for(int i = 0; i < num_of_inventory; i++)
        if(inventory[i]->get_ID() == product_ID)
        {
            r = inventory[i];
            for(int j = i; j < num_of_inventory-1; j++)
                inventory[j] = inventory[j+1];
            inventory[num_of_inventory-1] = nullptr;
            num_of_inventory--;
            return r;
        }
    return nullptr;
}

// End of to-do



// PLEASE DO NOT CHANGE!
void Product::set_origin_ID(Shop* shop) {
    if (shop != nullptr) {
        origin_ID = shop->get_ID();
    } else {
        origin_ID = -1;
    }
}

void Product::print() const {
    if (origin_ID == -1) {
        cout << " - Product ID: " << product_ID << " ($" << price << ")" << endl;
    } else {
        cout << " - Product ID: " << product_ID << " ($" << price << "; from Shop ID " << origin_ID << ")" << endl;
    }
}