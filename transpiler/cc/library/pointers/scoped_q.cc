#include "scoped_q.h"

#include "../error.h"

namespace hm {

#ifndef NDEBUG
using namespace test;

void test_library__pointers__scoped_q() {
    TEST(
        // ScopedQ<Parent<int>> type is a unique_ptr
        typename ScopedQ<Parent<int>>::Type scopedQ;
    );
    TEST(
        // ScopedQ<int> type is a unique_ptr
        typename ScopedQ<int>::Type scopedQ;
    );
}
#endif

}
