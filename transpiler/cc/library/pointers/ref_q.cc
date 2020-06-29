#include "ref_q.h"

#include "../error.h"

namespace hm {

#ifndef NDEBUG
using namespace test;

void test_library__pointers__ref_q() {
    TEST(
        typename RefQ<Parent<int>>::Type refQ(nullptr);
        EXPECT_EQUAL(refQ, nullptr);
    );
}
#endif
}
