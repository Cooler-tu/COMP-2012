#include "User.h"
#include "Group.h"

User::User(const std::string& name, const std::string& password, Group& g)
    : name(name), password(password), personal_group(g)
    // personal_group需要new一个group吗，不用吧
    {}  

User::~User()
{
    for(std::vector<Group*>::iterator i = other_groups.begin(); 
        i != other_groups.end(); i++)
            (*i)->remove_user(*this);
    while(!other_groups.empty())
    {
        other_groups.pop_back();
    }
    personal_group.remove_user(*this);
}

void User::add_group(Group& group)
{
    other_groups.push_back(&group);
}

void User::remove_group(const Group& group)
{
    for(std::vector<Group*>::iterator i = other_groups.begin();
    i != other_groups.end(); i++)
        if((*i) == &group)
        {
            other_groups.erase(i);
            break;
        }
}

bool User::is_in_group(const Group& group) const
{
    if (&group == &personal_group)
        return true;
    for(int i = 0; i + other_groups.begin()!= other_groups.end(); i++)
        if(*(other_groups.begin()+i) == &group)
            return true;
    return false;
}

void User::clear_groups()
{
    for(std::vector<Group*>::iterator i = other_groups.begin(); 
        i != other_groups.end(); i++)
            (*i)->remove_user(*this);
    while(!other_groups.empty())
    {
        other_groups.pop_back();
    }
}