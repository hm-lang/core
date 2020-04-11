#ifndef LIBRARY__ITERATOR_H
#define LIBRARY__ITERATOR_H

#include "error.h"

#include <memory>
#include <type_traits>
#include <utility>

template<class F>
class Iterator {
public:
    /**
     * P is the return-type of a f() (where f is an instance of F),
     * should be std::unique_ptr<T> of some internal type T.
     */
    typedef decltype(std::declval<F>()()) P;
    typedef typename std::remove_pointer<decltype(std::declval<P>().get())>::type T;


    Iterator(F next_i) : next_internal(next_i) {
        current = next_internal();
    }

    P next() {
        if (current.get() == nullptr) return P(nullptr);
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
#include "error.h"

#include <functional>

class Range {
    int i = 0;
    const int end;
public:
    Range(int _end) : end(_end) {
    }

    std::unique_ptr<int> operator() () {
        if (i >= end) return std::unique_ptr<int>(nullptr);
        return std::unique_ptr<int>(new int(i++));
    }
};

void test_library__iterator() {
    {
        int index = 0;
        Iterator<std::function<std::unique_ptr<double>()>> iterator([&]() {
            ERR("index = " << index);
            if (index < 5) return std::unique_ptr<double>(new double(2.0*index++));
            return std::unique_ptr<double>(nullptr);
        });

        TEST(
            std::unique_ptr<double> next = iterator.next();
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

        std::unique_ptr<int> next = iterator.next();
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

#endif
