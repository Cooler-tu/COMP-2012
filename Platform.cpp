#include <iostream>
using namespace std;

#include "Platform.h"
#include "Shop.h"
#include "Customer.h"
#include "CustomerData.h"

// To do: Implement the member functions here!

Platform::Platform(int platform_ID, int VIP_threshold, double VIP_discount, int subscription_fee):
    platform_ID(platform_ID), VIP_threshold(VIP_threshold), VIP_discount(VIP_discount), subscription_fee(subscription_fee),
    balance(0), shops(new Shop* [2]), num_of_shops(0), shops_capacity(2), customer_data(nullptr)
{}

Platform::~Platform()
{
    for(int i = 0; i < num_of_shops; i++)
        delete shops[i]; 
    num_of_shops = 0;
    delete [] shops;
}

bool Platform::add_shop(Shop* shop)
{
    if(shop == nullptr || shop->get_platform_ID() != this->platform_ID) return false;
    for(int i = 0; i < num_of_shops; i++)
        if(shops[i] == shop) return false;
    if(num_of_shops == shops_capacity)
    {
        shops_capacity *= 2;
        Shop** shops_new = new Shop* [shops_capacity];
        for(int j = 0; j < num_of_shops; j++)
            shops_new[j] = shops[j];
        delete [] shops;
        shops = shops_new;
    }
    shops[num_of_shops++] = shop;
    return true;
}

bool Platform::delete_shop(int shop_ID)
{
    for(int i = 0; i < this->num_of_shops; i++)
        if(shops[i]->get_ID() == shop_ID)
        {
            delete shops[i];
            for(int j = i; j < num_of_shops-1; j++)
                shops[j] = shops[j+1];
            shops[num_of_shops-1] = nullptr;
            num_of_shops--;
            return true;
        }
    return false;
}

int Platform::update_customer_data(int customer_ID, int amount)
{
    CustomerData* now = customer_data, *pre = nullptr;
    
    while(now != nullptr)
    {
        if(now->customer_ID == customer_ID)
        {
            now->amount_spent += amount;
            return now->amount_spent;
        }
        if(now->customer_ID > customer_ID)
        {
            CustomerData* n = new CustomerData(customer_ID);
            n->amount_spent = amount;
            if(now == customer_data)
            {
                n->next = customer_data; 
                customer_data = n;
                return amount;
            }
            else
            {
                n->next = now;
                pre->next = n;
                return amount;
            }
        }
        pre = now;
        now = now->next;
    }
    CustomerData* n = new CustomerData(customer_ID);
    n->amount_spent += amount;
    if(pre) pre->next = n;
    else customer_data = n;
    return amount;
}

bool Platform::delete_customer_data(int customer_ID)
{
    CustomerData* now = customer_data, *pre = nullptr;
    while(now != nullptr)
    {
        if(now->customer_ID == customer_ID)
        {
            if(now == customer_data)
                customer_data = now->next;
            else pre->next = now->next;
            delete now;
            return true;
        }
        pre = now;
        now = now->next;
    }
    return false;
}

bool Platform::check_if_VIP(int customer_ID) const
{
    CustomerData* now = customer_data, *pre = nullptr;
    while(now != nullptr)
    {
        if(now->customer_ID == customer_ID)
            return now->amount_spent >= Platform::VIP_threshold;
        pre = now;
        now = now->next;
    }
    return false;
}

// Only change the part in the to-do section, DO NOT CHANGE ANYTHING ELSE!
void Platform::print_customer_data() const {
    cout << "------------------------ CUSTOMER DATA ------------------------" << endl;
    cout << "Customer Data of Platform ID: " << platform_ID << endl;
    if (customer_data == nullptr) { 
        cout << "- Has no customer data." << endl; 
    } else {
        int total = 0; //The total amount of money spent on this Platform
        int count = 0; //The number of VIP Customers
        
        // To-do: Implement the CustomerData printing here!
        CustomerData* now = customer_data;
        while(now != nullptr)
        {
            now->print();
            total += now->amount_spent;
            count += check_if_VIP(now->customer_ID);
            now = now->next;
        }
        // End of to-do

        cout << "Customers Total Spending: $" << total << endl;
        cout << "Number of VIP Customers: " << count << endl;
    }
    cout << "----------------------------- END -----------------------------" << endl << endl;
}

void Platform::simulate_end_of_month()
{
    for(int i = 0; i < num_of_shops; i++)
    {
        shops[i]->change_balance(-1*subscription_fee);
        this->change_balance(subscription_fee);
        if(shops[i]->get_balance() < 0)
        {
            int gap = shops[i]->get_balance();
            shops[i]->change_balance(-gap);
            this->change_balance(gap);
            delete_shop(shops[i]->get_ID());
            continue;
        }
        if(shops[i]->get_i_th_rating(3) != nullptr
           && shops[i]->get_average_score() < 15)
            delete_shop(shops[i]->get_ID());
    }
    for(int i = 0; i < num_of_shops; i++)
        for(int j = i+1; j < num_of_shops; j++)
            if(shops[i]->get_average_score() < shops[j]->get_average_score())
            {
                Shop* tmp = shops[i];
                shops[i] = shops[j];
                shops[j] = tmp;
                j--;
            }
}

// End of to-do



// PLEASE DO NOT CHANGE!
void Platform::print() const {
    cout << "------------------------ PLATFORM DATA ------------------------" << endl;
    cout << "Platform ID: " << platform_ID << endl;
    cout << "- Balance: $" << balance << endl;
    cout << "- VIP Threshold: $" << VIP_threshold << " (Discount rate: " << (100-VIP_discount*100) << "% off)" << endl;
    cout << "- Subscription Fee: $" << subscription_fee << endl;
    cout << "- Number of shops: " << num_of_shops << endl;
    cout << "----------------------------- END -----------------------------" << endl << endl;
}

void Platform::print_shops(bool show_average) const {
    cout << "-------------------------- SHOPS DATA -------------------------" << endl;
    cout << "Shops of Platform ID: " << platform_ID << endl;
    if (num_of_shops == 0) cout << "- Has no shops." << endl;
    for (int i = 0; i < num_of_shops; i++) {
        if (show_average) {
            cout << i+1 << ". Shop ID: " << shops[i]->get_ID()
                 << " (Balance: $" << shops[i]->get_balance() 
                 << "; Average Rating: " << shops[i]->get_average_score() << ")" << endl;
        } else {
            cout << i+1 << ". Shop ID: " << shops[i]->get_ID()
                 << " (Balance: $" << shops[i]->get_balance() << ")" << endl;
        }
    }
    cout << "----------------------------- END -----------------------------" << endl << endl;
}
