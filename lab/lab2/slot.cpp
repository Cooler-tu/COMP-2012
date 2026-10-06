#include "slot.h"
#include <cstring>
using namespace std;


Slot::Slot(const char* name, int max_items, Slot* prev) {
    m_name = new char[strlen(name)+1];
    strcpy(this->m_name, name);
    this->m_max_items = max_items;
    this->m_count = 0;
    this->m_prev = prev;
    this->m_next = nullptr;

    if(prev == nullptr);
    else if(prev->m_next == nullptr)
        prev->m_next = this;
    else if(prev->m_next != nullptr)
    {
        this->m_next = prev->m_next;
        prev->m_next->m_prev = this;
        prev->m_next = this;
    }
}

Slot::Slot(const Slot& other, Slot* prev) 
    : Slot(other.m_name, other.m_max_items, prev)
{ 
    this->m_count = other.m_count;
}

Slot::~Slot() {
    if(this->m_prev != nullptr) this->m_prev->m_next = this->m_next;
    if(this->m_next != nullptr) this->m_next->m_prev = this->m_prev;
    this->m_prev = nullptr; this->m_next = nullptr;
    
    delete [] this->m_name;
}
