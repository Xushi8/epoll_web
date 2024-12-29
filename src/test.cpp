#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <iomanip>
#include <fmt/core.h>
#include <cstring>

namespace
{

void print_permissions(mode_t mode)
{
    char permissions[10];

    // 文件类型
    if (S_ISDIR(mode))
        permissions[0] = 'd';
    else if (S_ISLNK(mode))
        permissions[0] = 'l';
    else
        permissions[0] = '-';

    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';

    fmt::print("{}", permissions);
}

void print_file_info(const struct dirent* entry, const struct stat& file_stat)
{
    print_permissions(file_stat.st_mode);

    fmt::print(" {}", file_stat.st_nlink);

    struct passwd* owner = getpwuid(file_stat.st_uid);
    fmt::print(" {}", (owner ? owner->pw_name : std::to_string(file_stat.st_uid)));

    // 打印文件所属组
    struct group* grp = getgrgid(file_stat.st_gid);
    fmt::print(" {}", (grp ? grp->gr_name : std::to_string(file_stat.st_gid)));

    // 打印文件大小
    fmt::print(" {}", file_stat.st_size);

    // 打印文件修改时间
    char time_buff[80];
    struct tm* time_info = localtime(&file_stat.st_mtime);
    strftime(time_buff, sizeof(time_buff), "%b %d %H:%M", time_info);
    fmt::print(" {}", time_buff);

    // 打印文件名
    fmt::print(" {}\n", entry->d_name);
}

// 列出目录中的文件
void list_directory(const char* path)
{
    DIR* dir = opendir(path);
    if (!dir)
    {
        fmt::print(stderr, "Failed to open directory: {}\n", path);
        return;
    }

    struct dirent* entry;
    struct stat file_stat;

    while ((entry = readdir(dir)) != nullptr)
    {
        using namespace std::string_view_literals;
        if (entry->d_name == "."sv || entry->d_name == ".."sv)
        {
            continue;
        }

        // 获取文件的详细信息
        std::string full_path = std::string(path) + "/" + entry->d_name;
        if (stat(full_path.c_str(), &file_stat) == -1)
        {
            fmt::print(stderr, "Failed to stat file: {}\n", full_path);
            continue;
        }

        // 打印文件的详细信息
        print_file_info(entry, file_stat);
    }

    closedir(dir);
}

// 递归列出目录及其子目录的所有文件
void list_directory_recursive(const char* path)
{
    DIR* dir = opendir(path);
    if (!dir)
    {
        fmt::print(stderr, "Failed to open directory: {}\n", path);
        return;
    }

    struct dirent* entry;
    struct stat file_stat;

    // 打印当前目录的文件
    fmt::print("\n{}:\n", path);

    // 遍历目录中的每个文件
    while ((entry = readdir(dir)) != nullptr)
    {
        // 跳过 "." 和 ".."
        if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        std::string full_path = std::string(path) + "/" + entry->d_name;
        if (stat(full_path.c_str(), &file_stat) == -1)
        {
            fmt::print(stderr, "Failed to stat file: {}\n", full_path);
            continue;
        }

        print_file_info(entry, file_stat);

        if (S_ISDIR(file_stat.st_mode))
        {
            list_directory_recursive(full_path.c_str());
        }
    }

    closedir(dir);
}

} // namespace

int main(int argc, char* argv[])
{
    const char* path = (argc == 2) ? argv[1] : ".";

    list_directory(path);

    if (argc == 3)
    {
        list_directory_recursive(path);
    }

    return 0;
}
