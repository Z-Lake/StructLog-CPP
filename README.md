# StructLog-CPP 📝

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

**StructLog-CPP** 是一个轻量级的 C++ 结构化日志库，旨在简化复杂数据结构的日志记录。支持标准容器、关联容器、原生数组、指针等类型的一键格式化输出，并提供流式接口与文件日志功能。

---

## ✨ 核心特性

- **自动格式化**：支持常见数据类型的友好输出，包括：
  - **序列容器**：`vector`, `list`, `array`, `deque` 等。
  - **关联容器**：`map`, `set`, `unordered_map`, `unordered_set`。
  - **键值对**：`std::pair`。
  - **字符串类型**：`std::string`, `char*`, `const char*`。
  - **原生数组**：`int[]`, `char[]` 等。
  - **指针**：多级指针、`void*` 和容器指针的解引用输出。
- **流式接口**：支持链式调用，如 `SLog::CmdLog << data << std::endl;`。
- **文件日志**：通过 `SLog::FileLog` 输出到文件(需手动调用 `SLog::set_file()` 初始化路径)。
- **自定义分隔符**：灵活控制多参数打印的分隔符。
- **零依赖**：仅需 C++17 标准库支持。

---

## 🛠️ 快速开始

### 安装
头文件库，直接引入 `SLog.h` 即可使用：
```cpp
#include "SLog.h"
```

### 基本用法
```cpp
#include "../h_file/SLog.h"

int main() {
    // 基本类型
    int num = 42;
    double pi = 3.1415;

    // 字符串类型
    const char* cstr = "C-style string";
    std::string str = "std::string";

    // 序列容器（单维、多维）
    std::vector<int> vec = {1, 2, 3};
    std::list<std::string> list_str = {"apple", "banana"};
    std::array<std::array<int, 2>, 3> arr_2d = {{{1, 2}, {3, 4}, {5, 6}}};

    // 关联容器（单维、多维）
    std::map<int, std::string> map_data = {{1, "one"}, {2, "two"}};
    std::unordered_map<std::string, std::vector<int>> umap = {
        {"a", {1, 2}}, {"b", {3, 4}}
    };

    // 键值对
    auto pair = std::make_pair("key", 100);

    // 指针（单级、多级、空指针）
    int* ptr = &num;
    int** ptr_to_ptr = &ptr;
    void* void_ptr = static_cast<void*>(ptr);
    int* null_ptr = nullptr;

    // 原生数组
    int int_arr[3] = {10, 20, 30};
    char mutable_str[] = "Mutable char array";

    // 多维复杂结构
    std::vector<std::map<int, std::vector<double>>> complex_struct = {
        {{1, {1.1, 1.2}}, {2, {2.1, 2.2}}}
    };

    // 输出测试
    SLog::print("===== 基本类型 =====\n");
    SLog::print(num, pi, 'A');

    SLog::print("\n===== 字符串类型 =====\n");
    SLog::print(cstr, str);

    SLog::print("\n===== 序列容器 =====\n");
    SLog::print(vec, list_str, arr_2d);

    SLog::print("\n===== 关联容器 =====\n");
    SLog::print(map_data, umap);

    SLog::print("\n===== 键值对 =====\n");
    SLog::print(pair);

    SLog::print("\n===== 指针 =====\n");
    SLog::print(ptr, ptr_to_ptr, void_ptr, null_ptr);

    SLog::print("\n===== 原生数组 =====\n");
    SLog::print(int_arr, mutable_str);

    SLog::print("\n===== 多维复杂结构 =====\n");
    SLog::CmdLog << complex_struct << std::endl;

    SLog::set_file("output.log");

    SLog::FileLog << "file_test \n" << complex_struct << std::endl;

    return 0;
}
```

### 输出示例
```
===== 基本类型 =====
 42 3.1415 A 
===== 字符串类型 =====
 "C-style string" "std::string" 
===== 序列容器 =====
 [1, 2, 3]
 ["apple", "banana"]
 [[1, 2]
, [3, 4]
, [5, 6]
]
 
===== 关联容器 =====
 {1: "one", 2: "two"} {"b": [3, 4]
, "a": [1, 2]
} 
===== 键值对 =====
 ("key", 100) 
===== 指针 =====
 42 「42」 <void*: 0x16bc13020> nullptr 
===== 原生数组 =====
 [10, 20, 30]
 Mutable char array 
===== 多维复杂结构 =====
 [{1: [1.1, 1.2]
, 2: [2.1, 2.2]
}]

```

### 文件日志
```cpp
int main() {
    SLog::set_file("output.log"); // 初始化文件路径
    SLog::FileLog << "This is logged to a file." << std::endl;
    return 0;
}
```

---

## 🧩 支持的数据结构示例

| 类型                  | 输出示例                          |
|-----------------------|----------------------------------|
| `std::vector<int>`    | `[1, 2, 3]`                     |
| `std::map<str, int>`  | `{"Alice": 95, "Bob": 88}`      |
| `std::pair<int, str>` | `(42, "Answer")`                |
| `int[3]`              | `[10, 20, 30]`                  |
| `const char*`         | `"Hello"`                       |
| `std::string`         | `"World"`                       |
| `int**`（多级指针）   | `「[1, 2, 3]」`（自动解引用）    |

---

## ⚠️ 注意事项

1. **C++17 要求**：确保编译器支持 C++17 及以上标准（如 GCC 9+、Clang 10+、MSVC 19.28+）。
2. **文件日志**：需手动调用 `SLog::set_file()` 初始化路径。
3. **指针处理**：`void*` 会直接输出地址，其他指针类型尝试递归解引用。

---

## 🚀 未来计划

- [ ] 支持更多容器。
- [ ] 自定义格式化选项（如缩进控制）。
- [ ] 性能优化（减少模板实例化开销）。

---

## 许可协议

[MIT License](LICENSE) © 2025 [Z-Lake]

---

**Star ⭐ 这个项目以获取更新！**  
如有问题或建议，请提交 [Issue](https://github.com/yourusername/StructLog-CPP/issues)。
