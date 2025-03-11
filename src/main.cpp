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