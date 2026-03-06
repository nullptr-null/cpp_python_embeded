#ifndef SIMPLE_VECTOR_H
#define SIMPLE_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <utility>

template <class T, class Allocator>
class SimpleVector
{
public:
    // 类型定义
    using value_type = T;
    using size_type = size_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using allocator_type = Allocator;

    // 构造/析构
    SimpleVector() noexcept : m_data(nullptr), m_size(0), m_capacity(0)
    {
    }

    explicit SimpleVector(size_type count) : m_data(nullptr), m_size(0), m_capacity(0)
    {
        resize(count);
    }

    SimpleVector(size_type count, const T& value) : m_data(nullptr), m_size(0), m_capacity(0)
    {
        assign(count, value);
    }

    template <typename InputIt>
    SimpleVector(InputIt first, InputIt last) : m_data(nullptr), m_size(0), m_capacity(0)
    {
        assign(first, last);
    }

    SimpleVector(const SimpleVector& other) : m_data(nullptr), m_size(0), m_capacity(0)
    {
        assign(other.begin(), other.end());
    }

    SimpleVector(SimpleVector&& other) noexcept :
        m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    SimpleVector(std::initializer_list<T> init) : m_data(nullptr), m_size(0), m_capacity(0)
    {
        assign(init);
    }

    ~SimpleVector()
    {
        clear();
    }

    // 赋值操作
    SimpleVector& operator=(const SimpleVector& other)
    {
        if (this != &other)
        {
            assign(other.begin(), other.end());
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& other) noexcept
    {
        if (this != &other)
        {
            clear();
            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    SimpleVector& operator=(std::initializer_list<T> init)
    {
        assign(init);
        return *this;
    }

    // 元素访问
    reference operator[](size_type pos)
    {
        return m_data[pos];
    }

    const_reference operator[](size_type pos) const
    {
        return m_data[pos];
    }

    reference at(size_type pos)
    {
        if (pos >= m_size)
            throw std::out_of_range("SimpleVector::at");
        return m_data[pos];
    }

    const_reference at(size_type pos) const
    {
        if (pos >= m_size)
            throw std::out_of_range("SimpleVector::at");
        return m_data[pos];
    }

    reference front()
    {
        return m_data[0];
    }

    const_reference front() const
    {
        return m_data[0];
    }

    reference back()
    {
        return m_data[m_size - 1];
    }

    const_reference back() const
    {
        return m_data[m_size - 1];
    }

    pointer data() noexcept
    {
        return m_data;
    }

    const_pointer data() const noexcept
    {
        return m_data;
    }

    // 迭代器
    pointer begin() noexcept
    {
        return m_data;
    }
    const_pointer begin() const noexcept
    {
        return m_data;
    }
    pointer end() noexcept
    {
        return m_data + m_size;
    }
    const_pointer end() const noexcept
    {
        return m_data + m_size;
    }

    // 容量
    bool empty() const noexcept
    {
        return m_size == 0;
    }
    size_type size() const noexcept
    {
        return m_size;
    }
    size_type capacity() const noexcept
    {
        return m_capacity;
    }

    void reserve(size_type new_cap)
    {
        if (new_cap > m_capacity)
        {
            reallocate(new_cap);
        }
    }

    void shrink_to_fit()
    {
        if (m_size < m_capacity)
        {
            reallocate(m_size);
        }
    }

    // 修改器
    void clear() noexcept
    {
        if (m_data)
        {
            for (size_type i = 0; i < m_size; ++i)
            {
                Allocator::destroy(&m_data[i]);
            }
            Allocator::deallocate(m_data);
            m_data = nullptr;
        }
        m_size = 0;
        m_capacity = 0;
    }

    void push_back(const T& value)
    {
        if (m_size == m_capacity)
        {
            reserve(m_capacity == 0 ? 1 : m_capacity * 2);
        }
        Allocator::construct(&m_data[m_size++], value);
    }

    void push_back(T&& value)
    {
        if (m_size == m_capacity)
        {
            reserve(m_capacity == 0 ? 1 : m_capacity * 2);
        }
        Allocator::construct(&m_data[m_size++], std::move(value));
    }

    void pop_back()
    {
        if (m_size > 0)
        {
            Allocator::destroy(&m_data[--m_size]);
        }
    }

    void resize(size_type count)
    {
        if (count > m_size)
        {
            reserve(count);
            for (size_type i = m_size; i < count; ++i)
            {
                Allocator::construct(&m_data[i]);
            }
        }
        else if (count < m_size)
        {
            for (size_type i = count; i < m_size; ++i)
            {
                Allocator::destroy(&m_data[i]);
            }
        }
        m_size = count;
    }

    void resize(size_type count, const T& value)
    {
        if (count > m_size)
        {
            reserve(count);
            for (size_type i = m_size; i < count; ++i)
            {
                Allocator::construct(&m_data[i], value);
            }
        }
        else if (count < m_size)
        {
            for (size_type i = count; i < m_size; ++i)
            {
                Allocator::destroy(&m_data[i]);
            }
        }
        m_size = count;
    }

    // 分配器
    allocator_type get_allocator() const noexcept
    {
        return Allocator{};
    }

private:
    T* m_data;
    size_type m_size;
    size_type m_capacity;

    void reallocate(size_type new_capacity)
    {
        if (new_capacity == 0)
        {
            clear();
            return;
        }

        T* new_data = Allocator::allocate(new_capacity);

        try
        {
            for (size_type i = 0; i < m_size; ++i)
            {
                Allocator::construct(&new_data[i], std::move(m_data[i]));
            }
        }
        catch (...)
        {
            for (size_type i = 0; i < m_size; ++i)
            {
                Allocator::destroy(&new_data[i]);
            }
            Allocator::deallocate(new_data);
            throw;
        }

        for (size_type i = 0; i < m_size; ++i)
        {
            Allocator::destroy(&m_data[i]);
        }

        Allocator::deallocate(m_data);
        m_data = new_data;
        m_capacity = new_capacity;
    }

    void assign(size_type count, const T& value)
    {
        clear();
        reserve(count);
        for (size_type i = 0; i < count; ++i)
        {
            Allocator::construct(&m_data[i], value);
        }
        m_size = count;
    }

    template <typename InputIt>
    void assign(InputIt first, InputIt last)
    {
        clear();
        size_type count = static_cast<size_type>(std::distance(first, last));
        reserve(count);
        for (size_type i = 0; first != last; ++first, ++i)
        {
            Allocator::construct(&m_data[i], *first);
        }
        m_size = count;
    }

    void assign(std::initializer_list<T> init)
    {
        assign(init.begin(), init.end());
    }
};

#endif // SIMPLE_VECTOR_H
