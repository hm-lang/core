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

    TEST(
        int p = 10;
        EXPECT_THROW(ScopedQ sQ(&p), "expected ok_for_non_null || value == nullptr");
    );

    TEST(
        // Assigning a Scoped::create or Scoped()
        Scoped<int> s = Scoped<int>::create(1234);
        EXPECT_POINTER_EQUAL(s.get(), 1234);

        s = Scoped<int>::create(34);
        EXPECT_POINTER_EQUAL(s.get(), 34);

        s = Scoped<int>();
        EXPECT_POINTER_EQUAL(s.get(), 0);
    );

    TEST(
        // Assigning a ScopedQ::create or ScopedQ()
        ScopedQ<int> s = ScopedQ<int>::create(1234);
        EXPECT_POINTER_EQUAL(s.get(), 1234);

        s = ScopedQ<int>::create(34);
        EXPECT_POINTER_EQUAL(s.get(), 34);

        s = ScopedQ<int>();
        EXPECT_EQUAL(s.get(), nullptr);
    );

    // TODO: test that destructors are being called for Scoped(Q) but not Ref(Q).
}
#endif
}
