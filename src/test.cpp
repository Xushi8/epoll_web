#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdexcept>
#include <string_view>
#include <fmt/core.h>
#include <cstdio>
#include <cerrno>
#include <dirent.h>
#include <cstring>
#include <sys/types.h>
#include <libgen.h>

namespace
{

void check_error(std::string_view msg, int res)
{
    if (res < 0) [[unlikely]]
    {
        throw std::runtime_error(fmt::format("{}: {}", msg, strerror(errno)));
    }
}

// 复制一个文件
void copy_file(const char* source, const char* destination)
{
    int source_fd, dest_fd;
    ssize_t bytes_read, bytes_written;
    constexpr size_t buf_size = 1024;
    char buffer[buf_size];

    source_fd = open(source, O_RDONLY);
    check_error("Failed to open source file", source_fd);

    struct stat st; // NOLINT(cppcoreguidelines-pro-type-member-init)
    check_error("Failed to get source file status", fstat(source_fd, &st));

    // 打开目标文件进行写入，如果目标文件不存在则创建它
    dest_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode);
    check_error("Failed to create destination file", dest_fd);

    // 读取源文件并写入目标文件
    while ((bytes_read = read(source_fd, buffer, buf_size)) > 0)
    {
        bytes_written = write(dest_fd, buffer, bytes_read);
        check_error("Write error", bytes_written);
    }

    check_error("Read error", bytes_read);

    close(source_fd);
    close(dest_fd);
}

// 递归地复制一个目录及其内容
void copy_directory(const char* source_dir, const char* dest_dir)
{
    DIR* dir = opendir(source_dir);
    if (!dir)
    {
        throw std::runtime_error(fmt::format("Failed to open directory: {}", strerror(errno)));
    }

    struct stat st; // NOLINT(cppcoreguidelines-pro-type-member-init)
    if (stat(source_dir, &st) == 0 && S_ISDIR(st.st_mode))
    {
        // 仅在目标目录不存在时才尝试创建目标目录
        struct stat dest_stat; // NOLINT(cppcoreguidelines-pro-type-member-init)
        if (stat(dest_dir, &dest_stat) != 0)
        {
            // 目标目录不存在，创建它
            check_error("Failed to create destination directory", mkdir(dest_dir, 0755));
        }
        else if (!S_ISDIR(dest_stat.st_mode))
        {
            // 如果目标存在但不是目录，抛出错误
            throw std::runtime_error(fmt::format("Target '{}' exists but is not a directory.", dest_dir));
        }
    }

    // 遍历源目录中的所有条目
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        using namespace std::string_view_literals;
        if (entry->d_name == "."sv || entry->d_name == ".."sv)
            continue;

        using namespace std::string_literals;
        std::string source_path = std::string(source_dir) + "/"s + entry->d_name;
        std::string dest_path = std::string(dest_dir) + "/"s + entry->d_name;

        struct stat entry_stat; // NOLINT(cppcoreguidelines-pro-type-member-init)
        if (stat(source_path.c_str(), &entry_stat) == 0)
        {
            // 如果是目录，递归调用复制目录函数
            if (S_ISDIR(entry_stat.st_mode))
            {
                copy_directory(source_path.c_str(), dest_path.c_str());
            }
            // 如果是常规文件，调用复制文件函数
            else if (S_ISREG(entry_stat.st_mode))
            {
                copy_file(source_path.c_str(), dest_path.c_str());
            }
        }
    }

    closedir(dir);
}

bool is_directory(const char* path)
{
    struct stat st; // NOLINT(cppcoreguidelines-pro-type-member-init)
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

bool is_regular_file(const char* path)
{
    struct stat st; // NOLINT(cppcoreguidelines-pro-type-member-init)
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

} // namespace

int main(int argc, char* argv[])
{
    // 检查命令行参数
    if (argc != 3)
    {
        fmt::print(stderr, "Usage: {} <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }

    try
    {
        using namespace std::string_literals;
        struct stat st; // NOLINT(cppcoreguidelines-pro-type-member-init)
        if (stat(argv[1], &st) != 0)
        {
            fmt::print(stderr, "Error: Unable to stat source path '{}': {}\n", argv[1], strerror(errno));
            return EXIT_FAILURE;
        }

        bool source_is_dir = is_directory(argv[1]);
        bool destination_is_dir = is_directory(argv[2]);

        // 如果源路径是目录
        if (source_is_dir)
        {
            if (destination_is_dir)
            {
                copy_directory(argv[1], argv[2]); // 目标是目录，递归复制
            }
            else
            {
                struct stat dst_stat; // NOLINT(cppcoreguidelines-pro-type-member-init)
                if (stat(argv[2], &dst_stat) != 0)
                {
                    // 如果目标路径不存在且不是目录，则创建目标目录
                    check_error("Failed to create destination directory", mkdir(argv[2], 0755));
                }
                else if (!S_ISDIR(dst_stat.st_mode))
                {
                    fmt::print(stderr, "Error: Target '{}' is not a directory when source is a directory.\n", argv[2]);
                    return 1;
                }
                copy_directory(argv[1], argv[2]); // 递归复制
            }
        }
        else if (is_regular_file(argv[1]))
        {
            if (destination_is_dir)
            {
                // 如果目标是目录，则在目标目录中创建源文件同名的文件
                std::string dest_file = std::string(argv[2]) + "/"s + argv[1];
                copy_file(argv[1], dest_file.c_str());
            }
            else
            {
                copy_file(argv[1], argv[2]); // 否则直接复制
            }
        }
        else
        {
            fmt::print(stderr, "Error: Source '{}' is neither a file nor a directory.\n", argv[1]);
            return 1;
        }

        fmt::print("Copy completed successfully.\n");
    }
    catch (const std::exception& e)
    {
        fmt::print(stderr, "Error: {}\n", e.what());
        return 1;
    }

    return 0;
}
