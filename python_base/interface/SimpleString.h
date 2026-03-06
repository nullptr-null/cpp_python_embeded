#ifndef SIMPLE_STRING_H
#define SIMPLE_STRING_H

#include <cstddef>
#include <cstring>
#include <memory>

template <class CharT, class Allocator>
class SimpleString
{
public:
    // 构造/析构
    SimpleString() noexcept : m_data(nullptr), m_size(0)
    {
    }

    SimpleString(const CharT* str) : m_data(nullptr), m_size(0)
    {
        if (str)
            assign(str);
    }

    SimpleString(const SimpleString& other) : m_data(nullptr), m_size(0)
    {
        if (other.m_size > 0)
            assign(other.m_data, other.m_size);
    }

    SimpleString(SimpleString&& other) noexcept : m_data(other.m_data), m_size(other.m_size)
    {
        other.m_data = nullptr;
        other.m_size = 0;
    }

    ~SimpleString()
    {
        clear();
    }

    // 赋值操作
    SimpleString& operator=(const SimpleString& other)
    {
        if (this != &other)
            assign(other.m_data, other.m_size);
        return *this;
    }

    SimpleString& operator=(SimpleString&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            m_data = other.m_data;
            m_size = other.m_size;
            other.m_data = nullptr;
            other.m_size = 0;
        }
        return *this;
    }

    SimpleString& operator=(const CharT* str)
    {
        assign(str);
        return *this;
    }

    // 基本接口
    const CharT* c_str() const noexcept
    {
        return m_data ? m_data : &s_empty;
    }

    const CharT* data() const noexcept
    {
        return m_data ? m_data : &s_empty;
    }

    operator const CharT*() const noexcept
    {
        return c_str();
    }

    size_t size() const noexcept
    {
        return m_size;
    }
    size_t length() const noexcept
    {
        return m_size;
    }
    bool empty() const noexcept
    {
        return m_size == 0;
    }

    void clear() noexcept
    {
        if (m_data)
        {
            Allocator::deallocate(m_data);
            m_data = nullptr;
        }
        m_size = 0;
    }

    // 内存分配器访问
    using allocator_type = Allocator;
    allocator_type get_allocator() const noexcept
    {
        return Allocator{};
    }

private:
    CharT* m_data;
    size_t m_size;
    static const CharT s_empty;

    void assign(const CharT* str, size_t len)
    {
        clear();
        if (len > 0)
        {
            m_data = Allocator::allocate(len + 1);
            std::memcpy(m_data, str, len * sizeof(CharT));
            m_data[len] = CharT();
            m_size = len;
        }
    }

    void assign(const CharT* str)
    {
        assign(str, str ? std::char_traits<CharT>::length(str) : 0);
    }
};

template <typename CharT, typename Allocator>
const CharT SimpleString<CharT, Allocator>::s_empty = CharT();

// 比较操作符
template <typename CharT, typename Allocator>
bool operator==(const SimpleString<CharT, Allocator>& lhs, const SimpleString<CharT, Allocator>& rhs)
{
    return lhs.size() == rhs.size() && std::char_traits<CharT>::compare(lhs.c_str(), rhs.c_str(), lhs.size()) == 0;
}

template <typename CharT, typename Allocator>
bool operator!=(const SimpleString<CharT, Allocator>& lhs, const SimpleString<CharT, Allocator>& rhs)
{
    return !(lhs == rhs);
}

template <typename CharT, typename Allocator>
bool operator<(const SimpleString<CharT, Allocator>& lhs, const SimpleString<CharT, Allocator>& rhs)
{
    return std::char_traits<CharT>::compare(lhs.c_str(), rhs.c_str(), std::min(lhs.size(), rhs.size())) < 0 ||
           (lhs.size() < rhs.size() && std::char_traits<CharT>::compare(lhs.c_str(), rhs.c_str(), lhs.size()) == 0);
}

#endif // SIMPLE_STRING_H
