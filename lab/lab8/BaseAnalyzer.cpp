#include "Base.h"
#include "BaseAnalyzer.h"

using ItemMap = Base::ItemMap;
using ItemPair = Base::ItemPair;

int BaseAnalyzer::range_sum(const Base &b, const string &lower_bound, const string &upper_bound) {
    int cnt = 0;
    for(Base::ItemMap::const_iterator item = b.get_items().begin(); item != b.get_items().end(); item++)
    {
        string id = (*item).first;
        if(id >= lower_bound && id <= upper_bound)
            cnt += (*item).second;
    }
    return cnt;
}

ItemPair BaseAnalyzer::max_item(const Base &b) {
    if(b.get_items().empty())
        return ItemPair{"EMPTY", 0};
    int maxn = 0; string name;
    for(Base::ItemMap::const_iterator item = b.get_items().begin(); item != b.get_items().end(); item++)
    {
        int x = (*item).second;
        string id = (*item).first;
        if(x > maxn)
        {
            maxn = x;
            name = id;
        }
        else if(x == maxn && id < name)
        {
            name = id;
        }
    }
    return ItemPair{name, maxn};
}