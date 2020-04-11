#include "error.h"

namespace hm {

#ifndef NDEBUG
void test_library__error() {
    TEST(EXPECT_THROW(EXPECT_EQUAL(5, 100), "expected 5 to equal 100"));

    TEST(
        std::string at;
        bool errored = false;
        try {
            at = AT; TRY(throw Error("error!", "other line"));
        } catch (const Error& error) {
            errored = true;
            EXPECT_EQUAL(error.message, "error!");
            EXPECT_EQUAL(error.at, "other line\n    " + at);
        }
        EXPECT_EQUAL(errored, true);
    );

    TEST(EXPECT_EQUAL(1, 1));

    TEST(EXPECT_THROW(throw Error("testing!", AT), "testing!"));

    TEST(EXPECT_THROW(EXPECT_EQUAL(5, 100), "expected 5 to equal 100"));

    TEST(
        std::string at;
        bool errored = false;
        try {
            at = AT; TRY(throw Error("error!", "other line"));
        } catch (const Error& error) {
            errored = true;
            EXPECT_EQUAL(error.message, "error!");
            EXPECT_EQUAL(error.at, "other line\n    " + at);
        }
        EXPECT_EQUAL(errored, true);
    );

    TEST(
        const int a = 3;
        EXPECT_THROW(EXPECT_EQUAL(a, 1), "expected a to equal 1");
    );

    TEST(
        const int a = 3;
        const int b = 5;
        EXPECT_THROW(EXPECT_EQUAL(a, b), "expected a to equal b");
    );

    TEST(
        const int c = 0;
        EXPECT_THROW(EXPECT_EQUAL(-1, c), "expected -1 to equal c");

        EXPECT_THROW(EXPECT_THROW(((void)0), "could throw"),
                "expected ((void)0) to throw \"could throw\""
        );
    );

    TEST(ASSERT(100 == 100));

    TEST(EXPECT_THROW(ASSERT(1 == 2), "expected 1 == 2"));

    TEST(
        Capturer capture(std::cout);
        std::cout << "hey you!";
        std::cout << " are you\n there?";
        EXPECT_EQUAL(capture.str(), "hey you! are you\n there?");
    );

    TEST(
        Capturer capture(std::cerr);
        std::cerr << "there is good;";
        std::cerr << "\n let me know if you think so, too\n";
        EXPECT_EQUAL(capture.str(), "there is good;\n let me know if you think so, too\n");
    );
}
#endif

}
