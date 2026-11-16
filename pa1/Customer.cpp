#include <iostream>
using namespace std;

#include "Customer.h"
#include "Shop.h"
#include "Product.h"
#include "Rating.h"

// To do: Implement the member functions here!

Customer::Customer(int customer_ID):
    customer_ID(customer_ID), balance(0), belongings(new Product* [2]),
    num_of_belongings(0), belongings_capacity(2), ratings(nullptr)
{}

Customer::~Customer()
{
    for(int i = 0; i < num_of_belongings; i++)
        delete belongings[i];
    delete [] belongings;

    Rating* now = ratings;
    while (now != nullptr)
    {
        Rating* next = now->customer_next;
        now->customer = nullptr;
        now->customer_next = nullptr;
        //相当于这个pointer被设成空了, 但是这个pointer本来指向的地址已经被next存进去了, 
        //delete pointer不会影响本身, 只是要注意它存的那个地址有没有空掉

        if (now->shop == nullptr)
            delete now;

        now = next;
    }
}

bool Customer::add_product(Product* product)
{
    if(product == nullptr) return false;
    for(int i = 0; i < num_of_belongings; i++)
        if(belongings[i] == product) return false;
    if(num_of_belongings == belongings_capacity)
    {
        belongings_capacity *= 2;
        Product** belongings_new = new Product* [belongings_capacity];
        for(int j = 0; j < num_of_belongings; j++)
            belongings_new[j] = belongings[j];
        delete [] belongings;
        belongings = belongings_new;
    }
    belongings[num_of_belongings++] = product;
    return true;
}

bool Customer::rate_shop(Shop* shop, int score)
{
    if(shop == nullptr) return false;
    if(score < 0) score = 0;
    else if(score > 100) score = 100;
    for(int i = 0; i < num_of_belongings; i++)
    {
        if(belongings[i]->get_origin_ID() == shop->get_ID())
        {
            Rating* now = ratings, *pre = nullptr;
            while(now != nullptr)
            {
                if(now->shop_ID == shop->get_ID())
                {
                    now->shop = shop;
                    if(now == ratings)
                    {
                        now->score = score;
                        shop->set_ratings_head(now);
                        return true;
                    }
                    now->score = score;
                    if(pre) pre->customer_next = now->customer_next;
                    now->customer_next = ratings;
                    ratings = now;
                    shop->set_ratings_head(now);
                    return true;
                }
                pre = now;
                now = now->customer_next;
            }
            Rating* n = new Rating(this, shop, score);
            n->customer_next = ratings;
            ratings = n;
            shop->set_ratings_head(n);
            return true;
        }
    }
    return false;
}

const Rating* Customer::get_i_th_rating(int i)
{
    if(i < 0) return nullptr;
    Rating* now = ratings, *pre = nullptr;
    while(now != nullptr && i != 0)
    {
        pre = now;
        now = now->customer_next;
        i--;
    }
    if(i == 0)
        return now;
    else return nullptr;
}

// Only write in the to-do section, PLEASE DO NOT CHANGE ANYTHING ELSE!
void Customer::print_ratings() const {
    cout << "--------------------- CUSTOMER RATING DATA --------------------" << endl;
    cout << "Reviews by Customer ID: " << customer_ID << endl;
    if (ratings == nullptr) { 
        cout << "- No reviews written." << endl;
    } else {
        // To-do: Implement the Rating printing here!
        Rating* now = ratings, *pre = nullptr;
        while(now != nullptr)
        {
            now->print(false, true);
            pre = now;
            now = now->customer_next;
        }
        // End of to-do
    }
    cout << "----------------------------- END -----------------------------" << endl << endl;
}

bool Customer::delete_rating(int shop_ID)
{
    Rating* now = ratings, *pre = nullptr;
    while(now != nullptr)
    {
        if(now->shop_ID == shop_ID)
        {
            if(pre) pre->customer_next = now->customer_next;
            else ratings = now->customer_next;
            now->customer_next = nullptr;
            if(now->shop) now->shop->remove_rating(customer_ID);
            delete now;
            return true;
        }
        pre = now;
        now = now->customer_next;
    }
    return false;
}

// End of to-do



// PLEASE DO NOT CHANGE!
void Customer::print() const {
    cout << "------------------------ CUSTOMER DATA ------------------------" << endl;
    cout << "Customer ID: " << customer_ID << endl;
    cout << "- Balance: $" << balance << endl;
    cout << "- Number of Products in Belongings: " << num_of_belongings << endl;
    cout << "----------------------------- END -----------------------------" << endl << endl;
}

void Customer::print_belongings() const {
    cout << "----------------------- BELONGINGS DATA -----------------------" << endl;
    cout << "Belongings of Customer ID: " << customer_ID << endl;
    if (num_of_belongings == 0) cout << "- Has no belongings." << endl;
    for (int i = 0; i < num_of_belongings; i++) {
        belongings[i]->print();
    }
    cout << "----------------------------- END -----------------------------" << endl << endl;
}