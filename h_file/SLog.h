#pragma once
// 输入输出
#include <iostream> // 标准输入输出流
#include <fstream>  // 文件输入输出流
#include <sstream>  // 字符串流
#include <iomanip>  // 格式化输入输出

#include <type_traits>
#include <vector>
#include <map>
#include <string>
#include <string_view>
#include <list>          // 双向链表
#include <set>           // 集合
#include <unordered_set> // 无序集合
#include <array>         // 固定大小的数组
#include <deque>         // 双端队列
#include <forward_list>  // 单向链表
#include <utility>

// 类型特征判断
namespace detail
{
    // 序列容器
    template <typename T> struct is_sequence_impl : std::false_type {};
    template <typename... Ts> struct is_sequence_impl<std::vector<Ts...>> : std::true_type {};
    template <typename... Ts> struct is_sequence_impl<std::list<Ts...>> : std::true_type {};
    template <typename... Ts> struct is_sequence_impl<std::deque<Ts...>> : std::true_type {};
    template <typename T, size_t N> struct is_sequence_impl<std::array<T, N>> : std::true_type {};
    template <typename... Ts> struct is_sequence_impl<std::forward_list<Ts...>> : std::true_type {};
    // 原生数组
    template <typename T, size_t N> struct is_sequence_impl<T[N]> : std::true_type {};
    // 序列容器主模板，去除cv限定符后判断
    template <typename T> struct is_sequence : is_sequence_impl<std::remove_cv_t<T>> {};


    // 关联容器
    template <typename> struct is_associative_impl : std::false_type {};
    template <typename... Ts> struct is_associative_impl<std::map<Ts...>> : std::true_type {};
    template <typename... Ts> struct is_associative_impl<std::set<Ts...>> : std::true_type {};
    template <typename... Ts> struct is_associative_impl<std::unordered_map<Ts...>> : std::true_type {};
    template <typename... Ts> struct is_associative_impl<std::unordered_set<Ts...>> : std::true_type {};
    // 关联容器主模板：去除引用和cv限定符后判断
    template <typename T> struct is_associative : is_associative_impl<std::remove_cv_t<std::remove_reference_t<T>>> {};

    // 键值对特征
    template <typename> struct is_pair : std::false_type{};
    template <typename T1, typename T2> struct is_pair<std::pair<T1, T2>> : std::true_type {};

    // 字符串类型特征判断
    template <typename T>
    struct is_string_like : std::disjunction<
        std::is_same<std::decay_t<T>, std::string>,
        std::is_same<std::decay_t<T>, std::string_view>,
        std::is_same<std::decay_t<T>, char*>,
        std::is_same<std::decay_t<T>, const char*>
    > {};
}

// 主打印模板
template <typename T>
struct SLogImpl
{
    static void print(const T &value, std::ostream &os)
    {
        if constexpr (std::is_arithmetic_v<T>) {
            // 数值
            os << value;
        }
        else if constexpr (std::is_array_v<T> && 
            std::is_same_v<std::remove_const_t<std::remove_extent_t<T>>, char>
        ) {
            // 显式处理字符数组（char[N] 或 const char[N]）
            os << static_cast<const char*>(value);
        }
        else if constexpr (detail::is_string_like<T>::value) {
            // 处理 char*、const char*、std::string 和 std::string_view
            os << "\"" << value << "\"";
            // os << value;
        }
        else if constexpr (detail::is_sequence<T>::value) {
            // 序列容器
            os << "[";
            bool first = true;
            for (const auto &elem : value) {
                if (!first) os << ", ";
                // 使用 remove_reference_t 保留数组类型，避免退化为指针
                using ElemType = std::remove_reference_t<decltype(elem)>;
                SLogImpl<ElemType>::print(elem, os);
                first = false;
            }
            os << "]";
            // if (std::is_compound_v<T>) os << "\n";
        }
        else if constexpr (detail::is_associative<T>::value)
        {
            // 关联容器
            os << "{";
            bool first = true;
            for (const auto &elem : value)
            {
                if (!first) os << ", ";
                const auto &k = elem.first;
                const auto &v = elem.second;
                SLogImpl<std::decay_t<decltype(k)>>::print(k, os);
                os << ": ";
                SLogImpl<std::decay_t<decltype(v)>>::print(v, os);
                first = false;
            }
            os << "}";
        }
        else if constexpr (detail::is_pair<T>::value)
        {
            os << "(";
            SLogImpl<typename T::first_type>::print(value.first, os);
            os << ", ";
            SLogImpl<typename T::second_type>::print(value.second, os);
            os << ")";
        }
        else if constexpr (std::is_pointer_v<T>)
        {
            // 指针相关的
            if (value == nullptr)
            {
                os << "nullptr";
            }
            else
            {
                using PointeeType = std::remove_pointer_t<T>;
                if constexpr (std::is_same_v<PointeeType, void>)
                {
                    // 处理 void*：输出地址
                    os << "<void*: " << value << ">";
                }
                else if constexpr (
                    detail::is_sequence<PointeeType>::value ||
                    detail::is_associative<PointeeType>::value ||
                    detail::is_pair<PointeeType>::value ||
                    std::is_arithmetic_v<PointeeType> ||
                    detail::is_string_like<PointeeType>::value)
                {
                    // 已知类型（非指针）：递归解引用并打印实际内容
                    SLogImpl<PointeeType>::print(*value, os);
                }
                else if constexpr (std::is_pointer_v<PointeeType>)
                {
                    // 多级指针：递归解引用（例如 int**、std::vector<int>**）
                    os << "「";
                    SLogImpl<PointeeType>::print(*value, os);
                    os << "」";
                }
                else
                {
                    // 未知类型：输出地址
                    os << "<unknown*: " << value << ">";
                }
            }
        }
        else
        {
            os << "Not supported";
        }
    }
};

// 流式包装器
// 修改后的LogStream类
class LogStream
{
public:
    explicit LogStream(std::ostream &os) : os_(os) {}

    template <typename T>
    LogStream &operator<<(const T &value)
    {
        SLogImpl<T>::print(value, os_);
        return *this;
    }

    LogStream &operator<<(std::ostream &(*manip)(std::ostream &))
    {
        os_ << manip;
        return *this;
    }

private:
    std::ostream &os_;
};

// 入口类
struct SLog
{
private:
// 文件输出流对象（静态成员）
inline static std::ofstream log_file_stream;

public:
    // 多参数打印（每个参数后添加空格）
    template <typename... Args>
    static void print(const Args &...args) {
        // 使用逗号运算符展开，每个参数后添加空格
        ((LogStream(std::cout) << args << " "), ...);
    }

    // 支持自定义分隔符的打印（如逗号、分号等）
    template <typename... Args>
    static void print_with_sep(const std::string& sep, const Args &...args) {
        LogStream ls(std::cout);
        ((ls << args << sep), ...);
        std::cout << std::endl;
    }

    // 流式接口
    inline static LogStream CmdLog{std::cout};

    // 静态文件输出流（需通过set_file初始化）
    inline static LogStream FileLog{log_file_stream};

    // 设置文件输出路径（需手动调用）
    static void set_file(const std::string& filename) {
        log_file_stream.open(filename);
        if (!log_file_stream.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filename);
        }
    }
};
