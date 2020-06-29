#include "ref.h"

#include "../error.h"

namespace hm {

#ifndef NDEBUG
using namespace test;
void test_library__pointers__ref() {
    TEST(
        int x = 100;
        typename Ref<int>::Type intRef(&x);
        EXPECT_EQUAL(intRef, &x);
    );
}
#endif

}
