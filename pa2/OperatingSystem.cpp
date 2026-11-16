#include "OperatingSystem.h"
#include "Diagnose.h"

OperatingSystem::OperatingSystem(): filesys(nullptr), cur_user(nullptr)
{
    Group* root_group = new Group("root");
    groups.push_back(root_group);
    User* root_user = new User("root", "root", *root_group);
    users.push_back(root_user);
    root_group->add_user(*root_user);
    filesys = new FileSystem(*root_user);
}

OperatingSystem::~OperatingSystem()
{
    while(!users.empty())
    {
        delete users.back();
        users.pop_back();
    }
    while(!groups.empty())
    {
        delete groups.back();
        groups.pop_back();
    }
    delete filesys;
}

User* OperatingSystem::search_user(const std::string& name) const
{
    for(int i = 0; i < users.size(); i++)
    {
        if(users[i]->get_name() == name)
            return users[i];
    }
    return nullptr;
}

Group* OperatingSystem::search_group(const std::string& name) const
{
    for(int i = 0; i < groups.size(); i++)
    {
        if(groups[i]->get_name() == name)
            return groups[i];
    }
    return nullptr;
}

void OperatingSystem::groupadd(const std::string& group_name) // groupadd GROUP
{
    Group* group = nullptr;
    if(cur_user->is_root() && search_group(group_name) == nullptr)
    {
        group = new Group(group_name);
        groups.push_back(group);
    } 
}

void OperatingSystem::usermod(const std::string& user, const std::vector<std::string>& groups, bool append)
{
    if(cur_user->is_root())
    {
        User* tar_user = search_user(user);
        if(!tar_user)
        {
            diagnose::report("usermod", user, diagnose::NoUserOrGroup);
            return;
        }
        if(!append)
            tar_user->clear_groups(); 
        for(std::vector<std::string>::const_iterator i = groups.begin(); i != groups.end(); i++)
        {
            Group* g = search_group(*i);
            if(!g)
            {
                diagnose::report("usermod", user, diagnose::NoUserOrGroup);
                return;
            }
            if (g == &tar_user->get_personal_group())
                continue;
            g->add_user(*tar_user);
            tar_user->add_group(*g);
        }
    }
    else
    {
        diagnose::report("usermod", user, diagnose::PermissionDenied);
        return;
    }
}