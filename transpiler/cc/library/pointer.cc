#include "pointer.h"

namespace hm {
class Talker {
    const int id;
public:
    Talker(int i) : id(i) {
        std::cout << "Talker(" << id << ")";
    }
    ~Talker() {
        std::cout << "~Talker(" << id << ")";
    }
};

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

    TEST(
        // Scoped calls constructors and destructors
        {
            Scoped<Talker> s = Scoped<Talker>::create(1234);
        }
        EXPECT_EQUAL(test_cout.str(), "Talker(1234)~Talker(1234)");

        {
            Scoped<Talker> s = Scoped<Talker>::create(1);
            s = Scoped<Talker>::create(5);
        }
        EXPECT_EQUAL(test_cout.str(), "Talker(1)Talker(5)~Talker(1)~Talker(5)");
    );

    TEST(
        // ScopedQ calls constructors and destructors
        {
            ScopedQ<Talker> s = ScopedQ<Talker>::create(5432);
        }
        EXPECT_EQUAL(test_cout.str(), "Talker(5432)~Talker(5432)");

        {
            ScopedQ<Talker> s = ScopedQ<Talker>::create(5);
            s = ScopedQ<Talker>::create(7);
        }
        EXPECT_EQUAL(test_cout.str(), "Talker(5)Talker(7)~Talker(5)~Talker(7)");
    );

    TEST(
        // Ref DOES NOT call constructors or destructors
        Talker t(123);
        EXPECT_EQUAL(test_cout.str(), "Talker(123)");
        {
            Ref<Talker> s(&t);
        }
        EXPECT_EQUAL(test_cout.str(), "");

        Talker t2(5);
        EXPECT_EQUAL(test_cout.str(), "Talker(5)");
        {
            Ref<Talker> s(&t);
            EXPECT_EQUAL(s.get(), &t);
            s = Ref<Talker>(&t2);
            EXPECT_EQUAL(s.get(), &t2);
        }
        EXPECT_EQUAL(test_cout.str(), "");
    );

    TEST(
        // RefQ DOES NOT call constructors or destructors
        Talker t(4);
        EXPECT_EQUAL(test_cout.str(), "Talker(4)");
        {
            RefQ<Talker> s(nullptr);
            s = RefQ<Talker>(&t);
        }
        EXPECT_EQUAL(test_cout.str(), "");

        Talker t2(12);
        EXPECT_EQUAL(test_cout.str(), "Talker(12)");
        {
            RefQ<Talker> s(nullptr);
            s = RefQ<Talker>(&t);
            EXPECT_EQUAL(s.get(), &t);
            s = RefQ<Talker>(&t2);
            EXPECT_EQUAL(s.get(), &t2);
        }
        EXPECT_EQUAL(test_cout.str(), "");
    );
}
#endif
}
