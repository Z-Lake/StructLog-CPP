#pragma once
#include <iostream>
#include <fstream>
#include <type_traits>
#include <vector>
#include <map>
#include <string>
#include <string_view>
#include <list>
#include <set>
#include <unordered_set>
#include <array>
#include <deque>
#include <utility>

namespace detail {
    // 通用容器检测
    template <typename T, typename = void> struct is_container : std::false_type {};
    
    template <typename T>
    struct is_container<T, std::void_t<
        typename T::value_type,
        decltype(std::declval<T>().begin()),
        decltype(std::declval<T>().end())
    >> : std::true_type {};

    // 序列容器检测（包括原生数组）
    template <typename T> struct is_sequence : std::disjunction<is_container<T>, std::is_array<T>> {};

    // 关联容器检测
    template <typename T> struct is_associative : std::false_type {};
    template <typename... Ts> struct is_associative<std::map<Ts...>> : std::true_type {};
    template <typename... Ts> struct is_associative<std::set<Ts...>> : std::true_type {};
    template <typename... Ts> struct is_associative<std::unordered_map<Ts...>> : std::true_type {};
    template <typename... Ts> struct is_associative<std::unordered_set<Ts...>> : std::true_type {};

    // 键值对检测
    template <typename T> struct is_pair : std::false_type {};
    template <typename T1, typename T2> struct is_pair<std::pair<T1, T2>> : std::true_type {};

    // 字符串类型检测（包含char数组）
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
struct SLogImpl {
    static void print(const T& value, std::ostream& os) {
        if constexpr (std::is_arithmetic_v<T>) {
            os << value;
        }
        else if constexpr (std::is_array_v<T> && 
            std::is_same_v<std::remove_const_t<std::remove_extent_t<T>>, char>
        ) {
            // 显式处理字符数组（char[N] 或 const char[N]）
            os << static_cast<const char*>(value);
        }
        else if constexpr (detail::is_string_like<T>::value) {
            os << "\"" << value << "\"";
        }
        else if constexpr (detail::is_sequence<T>::value) {
            os << "[";
            bool first = true;
            for (const auto& elem : value) {
                if (!first) os << ", ";
                SLogImpl<std::decay_t<decltype(elem)>>::print(elem, os);
                first = false;
            }
            os << "]";
        }
        else if constexpr (detail::is_associative<T>::value) {
            os << "{";
            bool first = true;
            for (const auto& [k, v] : value) {
                if (!first) os << ", ";
                SLogImpl<std::decay_t<decltype(k)>>::print(k, os);
                os << ": ";
                SLogImpl<std::decay_t<decltype(v)>>::print(v, os);
                first = false;
            }
            os << "}";
        }
        else if constexpr (detail::is_pair<T>::value) {
            os << "(";
            SLogImpl<typename T::first_type>::print(value.first, os);
            os << ", ";
            SLogImpl<typename T::second_type>::print(value.second, os);
            os << ")";
        }
        else if constexpr (std::is_pointer_v<T>) {
            if (value == nullptr) {
                os << "nullptr";
            } else {
                using PointeeType = std::remove_pointer_t<T>;
                if constexpr (std::is_same_v<PointeeType, void>) {
                    os << "<void*: " << value << ">";
                } else {
                    SLogImpl<PointeeType>::print(*value, os);
                }
            }
        }
        else {
            os << "<unsupported type>";
        }
    }
};

// 流式包装器
class LogStream {
    std::ostream& os_;
public:
    explicit LogStream(std::ostream& os) : os_(os) {}
    
    template <typename T>
    LogStream& operator<<(const T& value) {
        SLogImpl<T>::print(value, os_);
        return *this;
    }
    
    LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
        os_ << manip;
        return *this;
    }
};

// 入口类
struct SLog {
    inline static std::ofstream log_file_stream;
    inline static LogStream CmdLog{std::cout};
    inline static LogStream FileLog{log_file_stream};
    
    template <typename... Args>
    static void print(const Args&... args) {
        ((LogStream(std::cout) << args << " "), ...);
    }
    
    template <typename... Args>
    static void print_with_sep(const std::string& sep, const Args&... args) {
        LogStream ls(std::cout);
        bool first = true;
        ((ls << (first ? (first = false, "") : sep) << args), ...);
        ls << std::endl;
    }

    static void set_file(const std::string& filename) {
        log_file_stream.open(filename);
        if (!log_file_stream.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filename);
        }
    }
};
