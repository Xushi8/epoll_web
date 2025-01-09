
# epoll_web

## 项目依赖库

本项目依赖以下库，确保满足版本要求并正确配置环境：

### Boost
- **版本要求**：1.81及以上  
- **用途**：使用 `unordered_flat_set` 提供高效的哈希表。  

### fmt
- **位置**：`external/include`  
- **用途**：用于格式化输出，性能高且安全，比传统的 `printf` 更佳。  

### spdlog
- **位置**：`external/include`  
- **用途**：用于日志输出，支持异步日志，避免阻塞并提供卓越的性能。  

### nlohmann/json
- **位置**：`external/include`  
- **用途**：用于 JSON 序列化操作，简洁高效。  

---

## 构建与编译

请按照以下步骤构建和编译项目：

### 构建
使用 `cmake` 构建工程文件：
```bash
cmake -Bbuild -GNinja
```

### 编译
使用 `cmake` 执行并行编译：
```bash
cmake --build build/ --parallel
```

### 编译产物
- 编译完成后，生成的二进制文件位于 `build/bin/` 目录下。  
- 运行时的注意事项：  
  - **server**：需要指定绑定的端口。  
    ```bash
    .build/bin/server <port>
    ```
  - **client**：需要指定远程服务的 IP 或域名以及端口。  
    ```bash
    ./build/bin/client <ip/domain> <port>
    ```
