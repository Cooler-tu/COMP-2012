#include "Group.h"

Group::Group(const std::string& name): name(name){}

Group::~Group()
{
    //如果用iterator delete会怎样
    while(!users.empty())
        users.pop_back();
}

void Group::add_user(User& user)
{
    users.push_back(&user);
}

void Group::remove_user(const User& user)
{
    for(std::vector<User*>::iterator i = users.begin();
    i != users.end(); i++)
        if((*i) == &user)
        {
            users.erase(i);
            break;
        }
}

bool Group::has_user(const User& user) const
{
    for(int i = 0; i + users.begin()!= users.end(); i++)
        if(*(users.begin()+i) == &user)
            return true;
    return false;
}