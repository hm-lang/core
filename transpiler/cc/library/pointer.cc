#include "pointer.h"

namespace hm {

Object::~Object() {}

template <>
Object *copy_held_value_into_new_pointer(const Object *o) {
    return o->copy();
}

#ifndef NDEBUG
// TODO: add test namespace
class Parent : public Object {
public:
    int id;

    Parent(int i = -1) : id(i) {
        std::cout << "Parent(" << id << ")";
    }
    Parent(const Parent& p) : Parent(p.id) {
        std::cout << "{CC}";
    }
    Parent(Parent&& p) : Parent(p.id) {
        std::cout << "{MC}";
    }
    virtual ~Parent() {
        std::cout << "~Parent(" << id << ")";
    }

    virtual Parent *copy() const {
        return new Parent(*this);
    }
};

// TODO: why isn't the original Object copy_held_value_into_new_pointer good enough?
template <>
Parent *copy_held_value_into_new_pointer(const Parent *p) {
    return p->copy();
}

class Child : public Parent {
public:
    std::string name;

    Child(std::string n = "nil", int i = -2) : Parent(i), name(n) {
        std::cout << "Child(" << name << ", " << this->id << ")";
    }
    Child(const Child& c) : Child(c.name, c.id) {
        std::cout << "{CC}";
    }
    Child(Child&& c) : Child(c.name, c.id) {
        std::cout << "{MC}";
    }
    virtual ~Child() {
        std::cout << "~Child(" << name << ", " << this->id << ")";
    }

    virtual Child *copy() const {
        return new Child(*this);
    }
};

class Aunt : public Object {
public:
    std::string name;

    Aunt(std::string n = "nil") : name(n) {
        std::cout << "Aunt(" << name << ")";
    }
    Aunt(const Aunt& a) : Aunt(a.name) {
        std::cout << "{CC}";
    }
    Aunt(Aunt&& a) : Aunt(std::move(a.name)) {
        std::cout << "{MC}";
    }
    Aunt(const Child& c) : Aunt(c.name + "'s Aunt") {
        std::cout << "{CC}";
    }
    Aunt(Child&& c) : Aunt(c.name + "'s Aunt") {
        std::cout << "{MC}";
    }
    virtual ~Aunt() {
        std::cout << "~Aunt(" << name << ")";
    }

    virtual Aunt *copy() const {
        return new Aunt(*this);
    }
};

void test_library__pointer() {
    TEST(
        // Default initialization
        Scoped<int> s;
        EXPECT_POINTER_EQUAL(s.get(), 0);

        ScopedQ<int> sQ;
        EXPECT_EQUAL(sQ.get(), nullptr);

        // Ref cannot be default-initialized...
        Ref<int> r(s.get());
        EXPECT_EQUAL(r.get(), s.get());
        *s.get() = 1234;
        EXPECT_POINTER_EQUAL(r.get(), 1234);

        RefQ<int> rQ;
        EXPECT_EQUAL(rQ.get(), nullptr);
    );

    TEST(
        // Constructing Scoped
        {
            Scoped<Parent> p(new Parent(1234));
            ASSERT(p.get() != nullptr);
            ++p.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "Parent(1234)~Parent(1235)");
    );

    TEST(
        // Copy-constructing Scoped
        {
            Scoped<Parent> p(new Parent(1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)");

            Scoped<Parent> p2(p);
            EXPECT_EQUAL(test_cout.str(), "Parent(1){CC}");
            ASSERT(p2.get() != nullptr);
            ++p2.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Parent(2)~Parent(1)");
    );

    TEST(
        // Copy-constructing Scoped with explicit Child class does deep copy
        {
            Scoped<Child> c(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");

            Scoped<Parent> p(c);
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1){CC}");
            ASSERT(p.get() != nullptr);
            ++p.get()->id;
            ASSERT(c.get() != nullptr);
            c.get()->name = "A prime";
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)~Child(A prime, 1)~Parent(1)");
    );

    TEST(
        // Copy-constructing Scoped with implicit Child class does deep copy
        {
            Scoped<Parent> p(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");

            Scoped<Parent> p2(p);
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1){CC}");
            ASSERT(p.get() != nullptr);
            --p.get()->id;
            ASSERT(p2.get() != nullptr);
            ++p2.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)~Child(A, 0)~Parent(0)");
    );

    TEST(
        // Copy-constructing Scoped with related Child class uses that constructor
        {
            Scoped<Child> c(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");
            ASSERT(c.get() != nullptr);

            Scoped<Aunt> a(c);
            EXPECT_EQUAL(test_cout.str(), "Aunt(A's Aunt){CC}");
            ASSERT(c.get() != nullptr);
            c.get()->name = "formerly A";
            ASSERT(a.get() != nullptr);
            a.get()->name = "Best " + a.get()->name;
        }
        EXPECT_EQUAL(test_cout.str(), "~Aunt(Best A's Aunt)~Child(formerly A, 1)~Parent(1)");
    );

    TEST(
        // Copy-assigning Scoped
        {
            Scoped<Parent> p(new Parent(100));
            EXPECT_EQUAL(test_cout.str(), "Parent(100)");
            ASSERT(p.get() != nullptr);
            ++p.get()->id;

            Scoped<Parent> p2(new Parent(3));
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
        // Copy-assigning Scoped with explicit Child class does deep copy
        {
            Scoped<Child> c(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");

            Scoped<Parent> p(new Parent(3));
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
        // Copy-assigning Scoped with implicit Child class does deep copy
        {
            Scoped<Parent> p(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");

            Scoped<Parent> p2(new Parent(2));
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
        // Copy-assigning Scoped with related Child class uses that constructor
        {
            Scoped<Child> c(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");
            ASSERT(c.get() != nullptr);

            Scoped<Aunt> a(new Aunt("Sally"));
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
        // Move-constructing Scoped only moves pointer
        {
            Scoped<Parent> p(new Parent(123));
            EXPECT_EQUAL(test_cout.str(), "Parent(123)");
            Parent *ptr = p.get();

            Scoped<Parent> p2(std::move(p));
            EXPECT_EQUAL(test_cout.str(), "");
            ASSERT(p2.get() != nullptr);
            EXPECT_EQUAL(p2.get(), ptr);
            --p2.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Parent(122)");
    );

    TEST(
        // Move-constructing Scoped with explicit Child class only moves pointer
        {
            Scoped<Child> c(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");
            Child *ptr = c.get();

            Scoped<Parent> p(std::move(c));
            EXPECT_EQUAL(test_cout.str(), "");
            ASSERT(p.get() != nullptr);
            EXPECT_EQUAL(p.get(), (Parent*)ptr);
            ++p.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)");
    );

    TEST(
        // Move-constructing Scoped with implicit Child class only moves pointer
        {
            Scoped<Parent> p(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");
            Parent *ptr = p.get();

            Scoped<Parent> p2(std::move(p));
            EXPECT_EQUAL(test_cout.str(), "");
            ASSERT(p2.get() != nullptr);
            EXPECT_EQUAL(p2.get(), ptr);
            ++p2.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)");
    );

    TEST(
        // Move-constructing Scoped with related Child class will use move constructor
        {
            Scoped<Child> c(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)");
            ASSERT(c.get() != nullptr);

            Scoped<Aunt> a(std::move(c));
            EXPECT_EQUAL(test_cout.str(), "Aunt(A's Aunt){MC}~Child(A, 1)~Parent(1)");
            ASSERT(a.get() != nullptr);
            a.get()->name = "Best " + a.get()->name;
        }
        EXPECT_EQUAL(test_cout.str(), "~Aunt(Best A's Aunt)");
    );

    TEST(
        // Move-assigning Scoped
        {
            Scoped<Parent> p(new Parent(34));
            EXPECT_EQUAL(test_cout.str(), "Parent(34)");
            ASSERT(p.get() != nullptr);
            ++p.get()->id;

            p = Scoped<Parent>(new Parent(5));
            EXPECT_EQUAL(test_cout.str(), "Parent(5)~Parent(35)");
            ASSERT(p.get() != nullptr);
            --p.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Parent(4)");
    );

    TEST(
        // Move assigning Scoped with explicit Child class moves pointer
        {
            Scoped<Parent> p(new Parent(10));
            EXPECT_EQUAL(test_cout.str(), "Parent(10)");

            p = Scoped<Child>(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)~Parent(10)");
            ASSERT(p.get() != nullptr);
            ++p.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 2)~Parent(2)");
    );

    TEST(
        // Move-assigning Scoped with implicit Child class moves pointer
        {
            Scoped<Parent> p(new Parent(10));
            EXPECT_EQUAL(test_cout.str(), "Parent(10)");

            p = Scoped<Parent>(new Child("A", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(A, 1)~Parent(10)");
            ASSERT(p.get() != nullptr);
            --p.get()->id;
        }
        EXPECT_EQUAL(test_cout.str(), "~Child(A, 0)~Parent(0)");
    );

    TEST(
        // Move-assigning Scoped with related Child class uses that constructor
        {
            Scoped<Aunt> a(new Aunt("Gladys"));
            EXPECT_EQUAL(test_cout.str(), "Aunt(Gladys)");

            a = Scoped<Child>(new Child("B", 1));
            EXPECT_EQUAL(test_cout.str(), "Parent(1)Child(B, 1)Aunt(B's Aunt){MC}~Child(B, 1)~Parent(1)~Aunt(Gladys)");
            ASSERT(a.get() != nullptr);
            a.get()->name = "Best " + a.get()->name;
        }
        EXPECT_EQUAL(test_cout.str(), "~Aunt(Best B's Aunt)");
    );

    // TODO: ScopedQ, Ref, and RefQ tests.  also mixing from each type to each other.
}
#endif
}
