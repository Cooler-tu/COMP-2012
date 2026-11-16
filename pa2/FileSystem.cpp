#include "FileSystem.h"
#include "Directory.h"
#include "Diagnose.h"
#include "FileEntry.h"
#include "File.h"
#include<iostream>

FileSystem::~FileSystem()
{
    delete root;
}

FileEntry* FileSystem::resolve_path(const User& user, const path_t& path, const std::string& cmd) const
{
    if(path.empty())
        return cwd;
    Directory* current = path[0].empty() ? root : cwd;
    std::size_t start = path[0].empty() ? 1 : 0;

    FileEntry* entry = current;
    for(std::size_t i = start; i < path.size(); i++)
    {
        Directory* dir = dynamic_cast<Directory*>(entry);
        if (dir == nullptr) {
            diagnose::report(cmd, entry->get_name(), diagnose::NotADirectory);
            return nullptr;
        }
        
        entry = dir->search(user, path[i], cmd);
        if (entry == nullptr)
            return nullptr;
    }
    return entry;
}

Directory* FileSystem::resolve_dir(const User& user, const path_t& path, const std::string& cmd) const
{
    FileEntry* entry = resolve_path(user, path, cmd);
    if(entry == nullptr)
        return nullptr;
    Directory* dir = dynamic_cast<Directory*>(entry);
    if(!dir)
        diagnose::report(cmd, entry->get_name(), diagnose::NotADirectory);
    return dir;
}

void FileSystem::add_user_dir(const User& root, const User& user, const std::string& cmd)
{
    Directory* home_dir =
    resolve_dir(root, path::split_path_string("/home"), cmd);

    if (home_dir == nullptr)
        return;
    Directory* n = new Directory(user.get_name(), user, user.get_personal_group(), home_dir);
    home_dir->insert_entry(root, n, cmd);
}

void FileSystem::cat(const User& user, const std::string& target) const
{
    FileEntry* entry = resolve_path(user, path::split_path_string(target), "cat");

    if (entry != nullptr)
        entry->cat(user);
}

void FileSystem::ls(const User& user, const path_t& targets, bool all, bool long_format, bool recursive) const
{
    if (targets.empty()) {
        std::optional<FormatData> format;

        if (long_format)
            format = cwd->compute_ls_widths(all, recursive);

        cwd->ls(user, all, recursive, format);
        return;
    }

    for(path_t::const_iterator i = targets.begin(); i != targets.end(); i++)
    {
        FileEntry* entry = resolve_path(user, path::split_path_string(*i), "ls");
        if(!entry)
            continue;
        
        bool multiple = targets.size() > 1;
        if(multiple)
            std::cout << entry->get_name() << ":" << std::endl;
        std::optional<FormatData> format;
        if (long_format) {
            if(dynamic_cast<File*>(entry))
            {
                format = FormatData{entry->get_owner()->get_name().size(), 
                                entry->get_group()->get_name().size(),
                                std::to_string(entry->get_size()).size()};
            }
            else if(dynamic_cast<Directory*>(entry))
                format = dynamic_cast<Directory*>(entry)->compute_ls_widths(all, recursive);
        }
        entry->ls(user, all, recursive, format);
        if(multiple) std::cout << std::endl;
    }
    
}

void FileSystem::mkdir(const User& user, const path_t& targets, bool create_parents) const
{
    for (const std::string& target : targets){
        path_t parts = path::split_path_string(target);
        if(parts.size() == 1 && parts[0].empty())
        {
            diagnose::report("mkdir", target, diagnose::FileOrDirectoryExists);
            return;
        }
        std::string name = parts.back();
        parts.pop_back();          
        
        if (name == "." || name == "..") {
            diagnose::report("mkdir", name, diagnose::FileOrDirectoryExists);
            continue;
        }
        
        Directory* parent = nullptr;     
        
        if(!create_parents)
        {
            parent = resolve_dir(user, parts, "mkdir");
            if (parent == nullptr)
                return;
        }
        else {
            parent = parts.size() > 0 && parts[0].empty() ? root : cwd;
            std::size_t start = parts.size() > 0 && parts[0].empty() ? 1 : 0;

            for (std::size_t i = start; i < parts.size(); i++) {
                FileEntry* entry = parent->search(
                    user, parts[i], "mkdir", CreateDirIfNotExists);

                if (entry == nullptr)
                    return;

                Directory* next = dynamic_cast<Directory*>(entry);
                if (next == nullptr) {
                    diagnose::report("mkdir", entry->get_name(), diagnose::NotADirectory);
                    return;
                }

                parent = next;
            }
        }

        Directory* dir = new Directory(name, user, user.get_personal_group(), parent);
    
        if (!parent->insert_entry(user, dir, "mkdir"))
            return;
        
    }
}

void FileSystem::touch(const User& user, const path_t& targets) const
{
    for(const std::string target: targets)
    {
        path_t parts = path::split_path_string(target);
        if (parts.size() == 1 && parts[0].empty())
            continue;
        const std::string name = parts.back();
        parts.pop_back();

        Directory* parent = resolve_dir(user, parts, "touch");
        if(!parent) return;

        parent->search(user, name, "touch", CreateFileIfNotExists);
    }
}

void FileSystem::chmod(const User& user, const std::string& target, const std::string& new_perm, bool recursive) const
{
    path_t parts = path::split_path_string(target);
    FileEntry* entry = resolve_path(user, parts, "chmod");
    if(entry) entry->chmod(user, new_perm, recursive);
}

void FileSystem::cp(const User& user, const std::string& src_path,
    const std::string& dest_path, bool recursive, bool preserve_perms) const
{
    FileEntry* src = resolve_path(user, path::split_path_string(src_path), "cp");
    if (src == nullptr) return;

    Directory* dir_src = dynamic_cast<Directory*>(src);

    if (dir_src != nullptr && !recursive) {
        diagnose::report("cp", src->get_name(), diagnose::NoRecursionSpecified);
        return;
    }

    path_t dest_parts = path::split_path_string(dest_path);

    if (dest_parts.size() == 1 && dest_parts[0].empty()) {
        src->cp(user, *root, preserve_perms);
        return;
    }

    std::string dest_name = dest_parts.back();
    dest_parts.pop_back();

    Directory* dest_parent = resolve_dir(user, dest_parts, "cp");
    if (dest_parent == nullptr) return;

    SearchOption option = dir_src != nullptr ? CreateDirIfNotExists : CreateFileIfNotExists;

    FileEntry* dest = dest_parent->search(user, dest_name, "cp", option);

    if (dest != nullptr) src->cp(user, *dest, preserve_perms);
}

void FileSystem::rm(const User& user, const path_t& targets, bool recursive) const
{
    for(const std::string target: targets)
    {
        path_t parts = path::split_path_string(target);
        FileEntry* dest = resolve_path(user, parts, "rm");
        if(!dest) continue;
        Directory* dir = dynamic_cast<Directory*>(dest);
        if(dir && !recursive)
        {
            diagnose::report("rm", dir->get_name(), diagnose::NoRecursionSpecified);
            continue;
        }
        if(dir && dir->is_ancestor_of(*cwd))
        {
            diagnose::report("rm", dir->get_name(), diagnose::PermissionDenied);
            continue;
        }
        if (dest->rm(user))
            delete dest;
    }
}

void FileSystem::cd(const User& user, const std::string& target)
{
    path_t parts = path::split_path_string(target);
    FileEntry* entry = resolve_path(user, parts, "cd");
    if(!entry) return;
    Directory* dir = dynamic_cast<Directory*>(entry);
    if(!dir)
    {
        diagnose::report("cd", entry->get_name(), diagnose::NotADirectory);
        return;
    }
    cwd = dir;
}