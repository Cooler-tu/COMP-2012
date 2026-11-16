#include "Device.h"
#include "Diagnose.h"
#include <iostream>

char Device::get_type() const
{
    return 'c';
}

bool DevNull::set_content(const User& user, const std::string& content, const std::string& cmd)
{
    if(!has_permissions(user, Permission::Write, cmd))
        return false;
    return true;
}

std::optional<std::string> DevNull::get_content(const User& user, const std::string& cmd) const
{
    if(!has_permissions(user, Permission::Read, cmd))
        return std::nullopt;
    return "";
}

bool DevStdIn::set_content(const User& user, const std::string& content, const std::string& cmd)
{
    diagnose::report(cmd, this->get_name(), diagnose::PermissionDenied);
    return false;
}

std::optional<std::string> DevStdIn::get_content(const User& user, const std::string& cmd) const
{
    if(!has_permissions(user, Permission::Read, cmd))
        return std::nullopt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

bool DevStdOut::set_content(const User& user, const std::string& content, const std::string& cmd)
{
    if(!has_permissions(user, Permission::Write, cmd))
        return false;
    std::cout << content << std::endl;
    return true;
}

std::optional<std::string> DevStdOut::get_content(const User& user, const std::string& cmd) const
{
    diagnose::report(cmd, get_name(), diagnose::PermissionDenied);
    return std::nullopt;
}