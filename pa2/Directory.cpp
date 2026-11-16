#include "Directory.h"
#include "FileEntry.h"
#include "Diagnose.h"
#include "File.h"

Directory::Directory(const std::string& name, const User& owner, const Group& group, Directory* parent)
    :FileEntry(name, owner, group, parent)
{}

Directory::~Directory()
{
    for (FileEntry* entry : file_entries) {
        delete entry;
    }
}

bool Directory::insert_entry(const User& user, FileEntry* entry, const std::string& cmd)
{
    if(!has_permissions(user, Permission::Write, cmd))
    {
        delete entry;
        return false;
    }

    if (file_entries.empty()) {
        file_entries.push_back(entry);
        return true;
    }

    if((*file_entries.begin())->get_name() > entry->get_name())
    {
        file_entries.insert(file_entries.begin(), entry);
        return true;
    }
    for(entry_vec_t::iterator e = file_entries.begin(); e != file_entries.end(); e++)
    {
        if((*e)->get_name() == entry->get_name())
        {
            diagnose::report(cmd, entry->get_name(), diagnose::FileOrDirectoryExists);
            delete entry;
            return false;
        }
        if((*e)->get_name() > entry->get_name())
        {
            file_entries.insert(e, entry);
            return true;
        }
    }
    file_entries.insert(file_entries.end(), entry);
    return true;
}

bool Directory::remove_entry(const User& user, const FileEntry& entry, const std::string& cmd)
{
    if(!has_permissions(user, Permission::Write, cmd))
        return false;

    for(entry_vec_t::iterator e = file_entries.begin(); e != file_entries.end(); e++)
    {
        if((*e) == &entry)
        {
            file_entries.erase(e);
            return true;
        }
    }
    return false;
}

FileEntry* Directory::search(const User& user, const std::string& target, const std::string& cmd, SearchOption option)
{
    if(!has_permissions(user, Permission::Exe, cmd))
        return nullptr;

    if(target == ".") return this;
    if(target == "..") return get_parent() == nullptr ? this : get_parent();
    for(entry_vec_t::iterator e = file_entries.begin(); e != file_entries.end(); e++)
    {
        if((*e)->get_name() == target)
            return *e;
    }
    
    if (option == Error) {
        diagnose::report(cmd, target, diagnose::NoFileOrDirectory);
        return nullptr;
    }

    FileEntry* entry = nullptr;

    if (option == CreateFileIfNotExists)
        entry = new File(target, user, user.get_personal_group(), "", this);
    else if (option == CreateDirIfNotExists)
        entry = new Directory(target, user, user.get_personal_group(), this);

    if (!insert_entry(user, entry, cmd))
        return nullptr;

    return entry;
}

void Directory::cat(const User& user) const
{
    diagnose::report("cat", get_name(), diagnose::NoAtFile);
}

bool Directory::ls(const User& user, bool all, bool recursive, const std::optional<FormatData>& long_format, const std::string& display_name) const
{
    if (!has_permissions(user, Permission::Exe, "ls") ||
        !has_permissions(user, Permission::Read, "ls"))
        return false;

    if(all)
    {
        FileEntry::ls(user, all, recursive, long_format, ".");
        const Directory* parent = get_parent() ? get_parent() : this;
        parent->FileEntry::ls(user, all, recursive, long_format, "..");
    }

    bool success = true;

    for (FileEntry* entry : file_entries)
    {
        entry->FileEntry::ls(user, all, recursive, long_format);
        if(recursive)
        {
            Directory* subdir = dynamic_cast<Directory*>(entry);
            if (subdir != nullptr && !subdir->ls(user, all, recursive, long_format))
                success = false;
        }
    }
    return success;
}

unsigned int Directory::get_size() const
{
    return get_entry_count()*32;
}

char Directory::get_type() const
{
    return 'd';
}

bool Directory::chmod(const User& user, const std::string &new_perm, bool recursive)
{
    if (!is_owned_by(user, "chmod"))
        return false;
    if (!FileEntry::chmod(user, new_perm, recursive))
        return false;

    bool success = true;
    
    if (recursive)
        for (FileEntry* entry : file_entries)
            if (!entry->chmod(user, new_perm, true))
                success = false;
    return success;
}

bool Directory::cp(const User& user, FileEntry& dest, bool preserve_perms)
{
    if(!has_permissions(user, Permission::Read, "cp")) return false;

    Directory* d = dynamic_cast<Directory*>(&dest);
    if(!d) return false;

    if(is_ancestor_of(*d))
    {
        diagnose::report("cp", get_name(), diagnose::PermissionDenied);
        return false;
    }

    const entry_vec_t sources = file_entries;
    for(FileEntry* source: sources)
    {
        FileEntry* target = nullptr;
        for(FileEntry* candidate: d->file_entries)
        {
            if(candidate->get_name() == source->get_name())
            {
                target = candidate;
                break;
            }
        }

        if(!target)
        {
            if(dynamic_cast<Directory*>(source))
                target = new Directory(source->get_name(), user, user.get_personal_group(), d);
            else
                target = new File(source->get_name(), user, user.get_personal_group(), "", d);
            if (!d->insert_entry(user, target, "cp"))
                return false;
        }
        if (!source->cp(user, *target, preserve_perms))
            return false;
    }

    if (preserve_perms)
        d->copy_permission(*this);
    return true;
}

bool Directory::rm(const User& user)
{
    if(get_parent() == nullptr)
    {
        diagnose::report("rm", get_name(), diagnose::PermissionDenied);
        return false;
    }

    while (!file_entries.empty()) {
        FileEntry* child = file_entries.back();

        if (!child->rm(user))
            return false;

        delete child;
    }

    return get_parent()->remove_entry(user, *this, "rm");
}
