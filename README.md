# epoll_web

---

## 项目特点

- 使用 epoll ET 模式进行 IO 多路复用，提升性能。
- 采用多线程进行 `send` 和 `receive`，进一步提高并发能力。
- 使用 `cmake` 进行跨平台构建，确保正确安装和集成第三方库。
- 使用 Github Action 进行多编译器、多种优化选项、多种 `sanitizer` 的测试，确保能够在最新的操作系统、编译器以及各种复杂情况下通过测试。
- 使用 Github Action 进行 Release，同时提供 amd64、arm64、riscv64、armhf 等多架构的二进制可执行程序。
- 适当使用 `locale` 处理文件编码，避免字符集问题。
- 对 `socket` 连接等复杂重复操作进行封装，降低使用复杂度，提高可读性。

---

## 项目依赖库

本项目依赖以下库，请确保满足版本要求并正确配置环境。其中 `Boost` 和 `Ncurses` 需要自行安装，其余库已内置在 `external/include` 目录下。

### Boost

- **版本要求**：1.81 及以上
- **用途**：使用 `unordered_flat_set` 提供高效的哈希表。

### Ncurses

- **用途**：提供终端操作的简易接口。

### fmt

- **用途**：用于格式化输出，比传统的 `printf` 性能更高且类型安全。

### spdlog

- **用途**：用于异步日志输出。

### nlohmann/json

- **用途**：用于 `JSON` 序列化操作。

### Debian/Ubuntu 安装依赖

```bash
sudo apt install libboost-dev libncurses-dev
```

---

## 构建与编译

请按照以下步骤构建和编译项目：

### 构建

使用 `cmake` 生成构建文件：

```bash
cmake -B build -G Ninja
```

### 编译

使用 `cmake` 进行并行编译：

```bash
cmake --build build/ --parallel
```

### 编译产物

- 编译完成后，生成的二进制文件位于 `build/bin/` 目录下。
- 运行方式：
  - **server**：需要指定绑定的端口：
    ```bash
    ./build/bin/server <port>
    ```
  - **client**：需要指定远程服务的 IP 或域名以及端口：
    ```bash
    ./build/bin/client <ip/domain> <port>
    ```
