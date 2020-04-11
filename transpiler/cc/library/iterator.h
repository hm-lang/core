#ifndef LIBRARY__ITERATOR_H
#define LIBRARY__ITERATOR_H

#include <functional>
#include <memory>

template<class T>
class Iterator {
public:
    // TODO: switch to using a class with a `T operator()` function instead of a std::function
    Iterator<T>(std::function<std::unique_ptr<T>()> next_i) : next_internal(next_i) {
        current = next_internal();
    }

    std::unique_ptr<T> next() {
        if (current.get() == nullptr) return nullptr;
        std::unique_ptr<T> return_value = std::move(current);
        current = next_internal();
        return return_value;
    }

    const T *peak() {
        return current.get();
    }

private:
    std::unique_ptr<T> current;
    std::function<std::unique_ptr<T>()> next_internal;
};

#ifndef NDEBUG
#include "error.h"
void test_library__iterator() {
    TEST(
        int i = 0;
        Iterator<double> iterator([&]() {
            if (i < 5) return std::unique_ptr<double>(new double(2.0*i++));
            return std::unique_ptr<double>(nullptr);
        });

        EXPECT_EQUAL(*iterator.next().get(), 0.0);
        EXPECT_EQUAL(*iterator.next().get(), 2.0);
        EXPECT_EQUAL(*iterator.next().get(), 4.0);
        EXPECT_EQUAL(*iterator.peak(), 6.0);
        EXPECT_EQUAL(*iterator.next().get(), 6.0);
        EXPECT_EQUAL(*iterator.next().get(), 8.0);
        EXPECT_EQUAL(iterator.next().get(), nullptr);
        EXPECT_EQUAL(iterator.peak(), nullptr);
    );
}
#endif

#endif
