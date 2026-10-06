#include "inventory.h"
#include <cstring>
using namespace std;

// m_head and m_tail are const pointers, so they must be initialized in the member initializer list.
// The nullptr values below are placeholders so that the skeleton compiles: replace them.
Inventory::Inventory(int capacity): 
    m_head{new Slot(HEAD_NAME, 0, nullptr)}, 
    m_tail{new Slot(TAIL_NAME, 0, this->m_head)},
    m_count{0}, 
    m_capacity{capacity}{
    
}

// Same as above: replace the placeholder initializer list.
Inventory::Inventory(const Inventory& other): 
    m_head{new Slot(other.m_head->get_name(), other.m_head->get_max_items(), other.m_head->get_prev())}, 
    m_tail{new Slot(other.m_tail->get_name(), other.m_tail->get_max_items(), m_head)},
    m_count(other.m_count),
    m_capacity(other.m_capacity) 
    {
        Slot* current = other.m_head->get_next();
        Slot* p = this->m_head;
        while(current != other.m_tail)
        {
            Slot* nxt = new Slot(*current, p);
            current = current->get_next();
            p = nxt;
        }
}

Inventory::~Inventory() {
    Slot* current = m_head; // copy the address is legal
    while(current->get_next() != nullptr)
    {
        current = current->get_next();
        delete current->get_prev();
    }
    delete current;
}

int Inventory::add(const char* name, int max_items, int amount) {
    if(m_capacity == m_count) return -1;
    Slot *s{new Slot(name, max_items, m_tail->get_prev())};
    m_count++;
    return s->fill(amount);
}

int Inventory::take(int index, int amount) {
    Slot *s = get_ith(index);
    if(s == nullptr) return -1;
    int left = s->take(amount);
    if(s->get_count() == 0)
    {
        delete s;
        this->m_count--;
    }
    return left;
}
