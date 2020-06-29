#include "pointers.h"

namespace hm {

Object::~Object() {}

template <>
Object *copy_held_value_into_new_pointer(const Object *o) {
    return o->copy();
}

#ifndef NDEBUG

using namespace test;

void test_library__pointers() {
    TEST(
        // Default initialization
        IScoped<int> s;
        EXPECT_POINTER_EQUAL(s.get(), 0);

        IScopedQ<int> sQ;
        EXPECT_EQUAL(sQ.get(), nullptr);

        // IRef cannot be default-initialized...
        IRef<int> r(s.get());
        EXPECT_EQUAL(r.get(), s.get());
        *s.get() = 1234;
        EXPECT_POINTER_EQUAL(r.get(), 1234);

        IRefQ<int> rQ;
        EXPECT_EQUAL(rQ.get(), nullptr);
    );

    TEST(
        // Constructing IScoped
        {
            IScoped<Parent<int>> p(new Parent<int>(1234));
            ASSERT(p.get() != nullptr);
            ++p.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "Parent(1234)~Parent(1235)");
    );

    TEST(
        // Copy-constructing IScoped
        {
            IScoped<Parent<int>> p(new Parent<int>(1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)");

            IScoped<Parent<int>> p2(p);
            EXPECT_EQUAL(test_cout.str(), "Parent(1){CC}");
            ASSERT(p2.get() != nullptr);
            ++p2.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Parent(2)~Parent(1)");
    );

    TEST(
        // Copy-constructing IScoped with explicit Child class does deep copy
        {
            IScoped<Child<int>> c(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");

            IScoped<Parent<int>> p(c);
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1){CC}");
            ASSERT(p.get() != nullptr);
            ++p.get()->id;
            ASSERT(c.get() != nullptr);
            c.get()->name = "A prime";
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)~Child(A prime, 1)~Parent(1)");
    );

    TEST(
        // Copy-constructing IScoped with implicit Child class does deep copy
        {
            IScoped<Parent<int>> p(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");

            IScoped<Parent<int>> p2(p);
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1){CC}");
            ASSERT(p.get() != nullptr);
            --p.get()->id;
            ASSERT(p2.get() != nullptr);
            ++p2.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)~Child(A, 0)~Parent(0)");
    );

    TEST(
        // Copy-constructing IScoped with related Child class uses that constructor
        {
            IScoped<Child<int>> c(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");
            ASSERT(c.get() != nullptr);

            IScoped<Aunt<int>> a(c);
            EXPECT_EQUAL(test_cout.str(), "Aunt(A's Aunt){CC}");
            ASSERT(c.get() != nullptr);
            c.get()->name = "formerly A";
            ASSERT(a.get() != nullptr);
            a.get()->name = "Best " + a.get()->name;
        }
        EXPECT_EQUAL(test_cout.str(), "~Aunt(Best A's Aunt)~Child(formerly A, 1)~Parent(1)");
    );

    TEST(
        // Copy-assigning IScoped
        {
            IScoped<Parent<int>> p(new Parent<int>(100));
            EXPECT_EQUAL(test_cout.str(), "Parent(100)");
            ASSERT(p.get() != nullptr);
            ++p.get()->id;

            IScoped<Parent<int>> p2(new Parent<int>(3));
            EXPECT_EQUAL(test_cout.str(), "Parent(3)");
            ASSERT(p2.get() != nullptr);
            ++p2.get()->id;

            p2 = p;
            EXPECT_EQUAL(test_cout.str(), "Parent(101){CC}~Parent(4)");
            ASSERT(p2.get() != nullptr);
            ++p2.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Parent(102)~Parent(101)");
    );

    TEST(
        // Copy-assigning IScoped with explicit Child class does deep copy
        {
            IScoped<Child<int>> c(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");

            IScoped<Parent<int>> p(new Parent<int>(3));
            EXPECT_EQUAL(test_cout.str(), "Parent(3)");

            p = c;
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1){CC}~Parent(3)");
            ASSERT(p.get() != nullptr);
            ++p.get()->id;
            ASSERT(c.get() != nullptr);
            c.get()->name = "A prime";
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)~Child(A prime, 1)~Parent(1)");
    );

    TEST(
        // Copy-assigning IScoped with implicit Child class does deep copy
        {
            IScoped<Parent<int>> p(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");

            IScoped<Parent<int>> p2(new Parent<int>(2));
            EXPECT_EQUAL(test_cout.str(), "Parent(2)");

            p2 = p;
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1){CC}~Parent(2)");
            ASSERT(p.get() != nullptr);
            --p.get()->id;
            ASSERT(p2.get() != nullptr);
            ++p2.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)~Child(A, 0)~Parent(0)");
    );

    TEST(
        // Copy-assigning IScoped with related Child class uses that constructor
        {
            IScoped<Child<int>> c(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");
            ASSERT(c.get() != nullptr);

            IScoped<Aunt<int>> a(new Aunt<int>("Sally"));
            EXPECT_EQUAL(test_cout.str(), "Aunt(Sally)");

            a = c;
            EXPECT_EQUAL(test_cout.str(), "Aunt(A's Aunt){CC}~Aunt(Sally)");
            ASSERT(c.get() != nullptr);
            c.get()->name = "formerly A";
            ASSERT(a.get() != nullptr);
            a.get()->name = "Best " + a.get()->name;
        }
        EXPECT_EQUAL(test_cout.str(), "~Aunt(Best A's Aunt)~Child(formerly A, 1)~Parent(1)");
    );

    TEST(
        // Move-constructing IScoped only moves pointer
        {
            IScoped<Parent<int>> p(new Parent<int>(123));
            EXPECT_EQUAL(test_cout.str(), "Parent(123)");
            Parent<int> *ptr = p.get();

            IScoped<Parent<int>> p2(std::move(p));
            EXPECT_EQUAL(test_cout.str(), "");
            ASSERT(p2.get() != nullptr);
            EXPECT_EQUAL(p2.get(), ptr);
            --p2.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Parent(122)");
    );

    TEST(
        // Move-constructing IScoped with explicit Child class only moves pointer
        {
            IScoped<Child<int>> c(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");
            Child<int> *ptr = c.get();

            IScoped<Parent<int>> p(std::move(c));
            EXPECT_EQUAL(test_cout.str(), "");
            ASSERT(p.get() != nullptr);
            EXPECT_EQUAL(p.get(), (Parent<int>*)ptr);
            ++p.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)");
    );

    TEST(
        // Move-constructing IScoped with implicit Child class only moves pointer
        {
            IScoped<Parent<int>> p(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");
            Parent<int> *ptr = p.get();

            IScoped<Parent<int>> p2(std::move(p));
            EXPECT_EQUAL(test_cout.str(), "");
            ASSERT(p2.get() != nullptr);
            EXPECT_EQUAL(p2.get(), ptr);
            ++p2.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)");
    );

    TEST(
        // Move-constructing IScoped with related Child class will use move constructor
        {
            IScoped<Child<int>> c(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");
            ASSERT(c.get() != nullptr);

            IScoped<Aunt<int>> a(std::move(c));
            EXPECT_EQUAL(test_cout.str(), "Aunt(A's Aunt){MC}~Child(A, 1)~Parent(1)");
            ASSERT(a.get() != nullptr);
            a.get()->name = "Best " + a.get()->name;
        }
        EXPECT_EQUAL(test_cout.str(), "~Aunt(Best A's Aunt)");
    );

    TEST(
        // Move-assigning IScoped
        {
            IScoped<Parent<int>> p(new Parent<int>(34));
            EXPECT_EQUAL(test_cout.str(), "Parent(34)");
            ASSERT(p.get() != nullptr);
            ++p.get()->id;

            p = IScoped<Parent<int>>(new Parent<int>(5));
            EXPECT_EQUAL(test_cout.str(), "Parent(5)~Parent(35)");
            ASSERT(p.get() != nullptr);
            --p.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Parent(4)");
    );

    TEST(
        // Move assigning IScoped with explicit Child class moves pointer
        {
            IScoped<Parent<int>> p(new Parent<int>(10));
            EXPECT_EQUAL(test_cout.str(), "Parent(10)");

            p = IScoped<Child<int>>(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)~Parent(10)");
            ASSERT(p.get() != nullptr);
            ++p.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)");
    );

    TEST(
        // Move-assigning IScoped with implicit Child class moves pointer
        {
            IScoped<Parent<int>> p(new Parent<int>(10));
            EXPECT_EQUAL(test_cout.str(), "Parent(10)");

            p = IScoped<Parent<int>>(new Child<int>("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)~Parent(10)");
            ASSERT(p.get() != nullptr);
            --p.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 0)~Parent(0)");
    );

    TEST(
        // Move-assigning IScoped with related Child class uses that constructor
        {
            IScoped<Aunt<int>> a(new Aunt<int>("Gladys"));
            EXPECT_EQUAL(test_cout.str(), "Aunt(Gladys)");

            a = IScoped<Child<int>>(new Child<int>("B", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(B, 1)Aunt(B's Aunt){MC}~Child(B, 1)~Parent(1)~Aunt(Gladys)");
            ASSERT(a.get() != nullptr);
            a.get()->name = "Best " + a.get()->name;
        }
        EXPECT_EQUAL(test_cout.str(), "~Aunt(Best B's Aunt)");
    );

    // TODO: IScopedQ, IRef, and IRefQ tests.  also mixing from each type to each other.
}
#endif
}
