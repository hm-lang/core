#include "scoped.h"

#include "../error.h"

namespace hm {

#ifndef NDEBUG
using namespace test;

void test_library__pointers__scoped() {
    TEST(
        // Scoped<Parent> type is a unique_ptr
        typename Scoped<Parent<int>>::Type scoped(new Parent<int>(100));
        EXPECT_EQUAL(Scoped<Parent<int>>::get(scoped)->id, 100);
    );
    TEST(
        // Scoped<int> type is NOT a pointer
        typename Scoped<int>::Type scoped(1001);
        EXPECT_POINTER_EQUAL(Scoped<int>::get(scoped), 1001);
    );
}
#endif

}
