#ifndef LIBRARY__ITERATOR_H
#define LIBRARY__ITERATOR_H

#include "error.h"
#include "pointer.h"

#include <functional>
#include <type_traits>
#include <utility>

namespace hm {

template<class P>
class RawIterator {
public:
    virtual ~RawIterator() {}

    virtual P next() = 0;
};

template<class F>
class Iterator {
public:
    /**
     * P is the return-type of f() (where f is an instance of F),
     * should be a PointerQ<T> of some internal type T.  Note that
     * P() should be a null pointer type, i.e. P().get() == nullptr.
     */
    typedef decltype(std::declval<F>()()) P;
    typedef typename std::remove_pointer<decltype(std::declval<P>().get())>::type T;

    Iterator(F next_i) : next_internal(next_i) {
        current = next_internal();
    }

    P next() {
        ASSERT(P().get() == nullptr);
        if (current.get() == nullptr) return P();
        P return_value = std::move(current);
        current = next_internal();
        return return_value;
    }

    const T *peak() {
        return current.get();
    }

private:
    P current;
    F next_internal;
};

#ifndef NDEBUG
class Range {
    int i = 0;
    const int end;
public:
    Range(int _end) : end(_end) {
    }

    ScopedQ<int> operator() () {
        if (i >= end) return ScopedQ<int>();
        return ScopedQ<int>::create(i++);
    }
};

void test_library__iterator() {
    {
        int index = 0;
        Iterator<std::function<ScopedQ<double>()>> iterator([&]() {
            ERR("index = " << index);
            if (index < 5) return ScopedQ<double>::create(2.0*index++);
            return ScopedQ<double>();
        });

        TEST(
            ScopedQ<double> next = iterator.next();
            EXPECT_POINTER_EQUAL(next.get(), 0.0);
            next = iterator.next();
            EXPECT_POINTER_EQUAL(next.get(), 2.0);
            next = iterator.next();
            EXPECT_POINTER_EQUAL(next.get(), 4.0);
            next = iterator.next();
            EXPECT_POINTER_EQUAL(next.get(), 6.0);
            next = iterator.next();
            EXPECT_POINTER_EQUAL(next.get(), 8.0);
            next = iterator.next();
            EXPECT_EQUAL(next.get(), nullptr);
            EXPECT_EQUAL(iterator.peak(), nullptr);
        );
    }

    TEST(
        Range range(3);
        Iterator iterator(range);

        EXPECT_POINTER_EQUAL(iterator.peak(), 0);
        EXPECT_POINTER_EQUAL(iterator.peak(), 0);
        EXPECT_POINTER_EQUAL(iterator.peak(), 0);
        EXPECT_POINTER_EQUAL(iterator.peak(), 0);

        ScopedQ<int> next = iterator.next();
        EXPECT_POINTER_EQUAL(next.get(), 0);

        EXPECT_POINTER_EQUAL(iterator.peak(), 1);
        EXPECT_POINTER_EQUAL(iterator.peak(), 1);
        EXPECT_POINTER_EQUAL(iterator.peak(), 1);

        next = iterator.next();
        EXPECT_POINTER_EQUAL(next.get(), 1);

        EXPECT_POINTER_EQUAL(iterator.peak(), 2);
        EXPECT_POINTER_EQUAL(iterator.peak(), 2);
        EXPECT_POINTER_EQUAL(iterator.peak(), 2);

        next = iterator.next();
        EXPECT_POINTER_EQUAL(next.get(), 2);

        EXPECT_EQUAL(iterator.peak(), nullptr);
        EXPECT_EQUAL(iterator.next().get(), nullptr);
        EXPECT_EQUAL(iterator.peak(), nullptr);
        EXPECT_EQUAL(iterator.next().get(), nullptr);
        EXPECT_EQUAL(iterator.peak(), nullptr);
    );
}
#endif
}
#endif
