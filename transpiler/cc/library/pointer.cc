#include "pointer.h"

namespace hm {
#ifndef NDEBUG
void test_library__pointer() {
    TEST(
        // Default initialization
        Scoped<int> s;
        EXPECT_POINTER_EQUAL(s.get(), 0);

        ScopedQ<int> sQ;
        EXPECT_EQUAL(sQ.get(), nullptr);

        Ref<int> r(s.get());
        EXPECT_EQUAL(r.get(), s.get());
        *s.get() = 1234;
        EXPECT_POINTER_EQUAL(r.get(), 1234);

        RefQ<int> rQ;
        EXPECT_EQUAL(rQ.get(), nullptr);
    );
}
#endif
}
