#pragma once

//#define _SECURE_SCL 0
//#define _SCL_SECURE_NO_WARNINGS

#include <algorithm>        // std::swap
#include <iostream>
#include <vector>
#include <stack>
//#include <stddef.h>         // ptrdiff_t
#include <type_traits>      // std::is_pod

typedef ptrdiff_t   Size;
typedef Size        Index;

template< class Type, class Container >
void reserve(Size const newBufSize, std::stack< Type, Container >& st)
{
    struct Access : std::stack< Type, Container >
    {
        static Container& container(std::stack< Type, Container >& st)
        {
            return st.* & Access::c;
        }
    };

    Access::container(st).reserve(newBufSize);
}

template< class Type, bool elemTypeIsPOD = std::is_pod_v<Type> >
class faststack;

template< class Type >
class faststack< Type, true >
{
private:
    Type*   st_;
    Index   lastIndex_;
    Size    capacity_;

public:
    Size size() const { return lastIndex_ + 1; }
    Size capacity() const { return capacity_; }

    void reserve(Size const newCapacity)
    {
        if (newCapacity > capacity_)
        {
            faststack< Type >(*this, newCapacity).swapWith(*this);
        }
    }

    void reserveAndPush(Type const& x)
    {
        Type const xVal = x;
        reserve(capacity_ == 0 ? 1 : 2 * capacity_);
        push(xVal);
    }

    void push(Type const& x)
    {
        if (size() == capacity())
        {
            return reserveAndPush(x);    // <-- The crucial "return".
        }
        st_[++lastIndex_] = x;
    }

    void pop()
    {
        --lastIndex_;
    }

    Type top() const
    {
        return st_[lastIndex_];
    }

    void swapWith(faststack& other) throw()
    {
        using std::swap;
        swap(st_, other.st_);
        swap(lastIndex_, other.lastIndex_);
        swap(capacity_, other.capacity_);
    }

    void operator=(faststack other)
    {
        other.swapWith(*this);
    }

    ~faststack()
    {
        delete[] st_;
    }

    faststack(Size const aCapacity = 0)
        : st_(new Type[aCapacity])
        , capacity_(aCapacity)
    {
        lastIndex_ = -1;
    }

    faststack(faststack const& other, int const newBufSize = -1)
    {
        capacity_ = (newBufSize < other.size() ? other.size() : newBufSize);
        st_ = new Type[capacity_];
        lastIndex_ = other.lastIndex_;
        std::copy(other.st_, other.st_ + other.size(), st_);   // Can't throw for POD.
    }
};

template< class Type >
void reserve(Size const newCapacity, faststack< Type >& st)
{
    st.reserve(newCapacity);
}

/*template< class StackType >
void test(char const* const description)
{
    for (int it = 0; it < 4; ++it)
    {
        StackType st;
        reserve(200, st);

        // after this two loops, st's capacity will be 141 so there will be no more reallocating
        for (int i = 0; i < 100; ++i) { st.push(i); }
        for (int i = 0; i < 100; ++i) { st.pop(); }

        // when you uncomment this line, std::stack performance will magically rise about 18%
        std::vector<int> magicVector(10);

        const Uint64 fps_start = SDL_GetPerformanceCounter();

        for (Index i = 0; i < 100000000; ++i)
        {
            st.push(i);
            (void)st.top();
            if (i % 100 == 0 && i != 0)
            {
                for (int j = 0; j < 100; ++j) { st.pop(); }
            }
        }

        const Uint64 fps_end = SDL_GetPerformanceCounter();
        const static Uint64 fps_freq = SDL_GetPerformanceFrequency();
        const double fps_seconds = (fps_end - fps_start) / static_cast<double>(fps_freq);

        wcout << description << ": " << fps_seconds << endl;
    }
}*/

/*int main()
{
    typedef stack< Index, vector< Index > > SStack;
    typedef faststack< Index >              FStack;

    test< SStack >("std::stack");
    test< FStack >("faststack");

    cout << "Done";
}*/