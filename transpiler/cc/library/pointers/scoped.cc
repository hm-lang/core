#include "scoped.h"

#include "../error.h"

namespace hm {

#ifndef NDEBUG
using namespace test;

void test_library__pointers__scoped() {
    TEST(
        // Scoped<Parent> type is a unique_ptr
        typename Scoped<Parent<int>>::Type scoped(new Parent<int>(100));
        EXPECT_EQUAL(scoped->id, 100);
        EXPECT_EQUAL(Scoped<Parent<int>>::get(scoped)->id, 100);
    );
    TEST(
        // Scoped<int> type is NOT a pointer
        typename Scoped<int>::Type scoped(1001);
        EXPECT_EQUAL(&scoped, Scoped<int>::get(scoped));
        EXPECT_POINTER_EQUAL(Scoped<int>::get(scoped), 1001);
    );
    TEST(
        // Constructing/destructing Scoped should work
        {
            typename Scoped<Parent<int>>::Type p(new Parent<int>(1234));
            ++Scoped<Parent<int>>::get(p)->id;
        }
        EXPECT_EQUAL(test_cout.str(), "Parent(1234)~Parent(1235)");
    );
}
#endif

}
