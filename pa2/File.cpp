#include "File.h"
#include "FileEntry.h"
#include "Diagnose.h"
#include "Directory.h"
#include <iostream>

File::File(const std::string& name, const User& owner, const Group& group, const std::string& content, Directory *parent)
    :FileEntry(name, owner, group, parent), content(content)
{}

std::optional<std::string> File::get_content(const User& user, const std::string& cmd) const
{
    if(has_permissions(user, Permission::Read, cmd))
        return content;
    return std::nullopt;
}

bool File::set_content(const User& user, const std::string& content, const std::string& cmd)
{
    if(has_permissions(user, Permission::Write, cmd)){
        this->content = content;
        return true;
    }
    return false;
}

unsigned int File::get_size() const
{
    return content.length();
}

char File::get_type() const
{
    return '-';
}

void File::cat(const User& user) const
{
    if(has_permissions(user, Permission::Read, "cat"))
        std::cout << content << std::endl;
}

bool File::ls(const User& user, bool all, bool recursive, const std::optional<FormatData>& long_format, const std::string& display_name) const
{
    if(has_permissions(user, Permission::Read, "ls")){
        FileEntry::ls(user, all, recursive, long_format, display_name);
        return true;
    }
    return false;
}

bool File::chmod(const User& user, const std::string &new_perm, bool recursive)
{
    if (!is_owned_by(user, "chmod"))
        return false;
    return FileEntry::chmod(user, new_perm, recursive);
}

bool File::cp(const User& user, FileEntry& dest, bool preserve_perms)
{
    Directory* dir = dynamic_cast<Directory*>(&dest);
    File* file = dynamic_cast<File*>(&dest);
    if(dir)
    {
        FileEntry* entry = dir->search(user, get_name(), "cp", CreateFileIfNotExists);
        if(entry)
        {
            file = dynamic_cast<File*>(entry);
            if(file == nullptr) 
            {
                diagnose::report("cp", entry->get_name(), diagnose::FileOrDirectoryExists);
                return false;
            } 
        }
        else
            return false;
    }
    
    std::optional<std::string> source_content = get_content(user, "cp");
    
    if(!source_content.has_value())
        return false;
    if(!file->set_content(user, source_content.value(), "cp"))
        return false;
    if(preserve_perms)
        file->copy_permission(*this);
    return true;
} 

bool File::rm(const User& user)
{
    if(!has_permissions(user, Permission::Write, "rm"))
        return false;
    return get_parent()->remove_entry(user, *this, "rm");
}