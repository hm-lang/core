#include "file.h"

namespace hm {

File::File(fs::directory_entry _entry)
        : entry(_entry), input_stream(_entry.path().c_str()), line_number(0) {
    ASSERT(entry.is_regular_file());
}

ScopedQ<File::Line> File::operator() () {
    if (!input_stream.is_open()) return ScopedQ<File::Line>(nullptr);
    std::string line_content;
    if (!std::getline(input_stream, line_content)) {
        input_stream.close();
        return ScopedQ<File::Line>(nullptr);
    }
    return ScopedQ<File::Line>::create(
        ++line_number,
        line_content
    );
}

#ifndef NDEBUG
void test_library__file() {
    TEST(
        File file(fs::directory_entry(__FILE__));
        ScopedQ<File::Line> current_line = file();
        ASSERT(current_line.get() != nullptr);
        EXPECT_EQUAL(current_line.get()->content, "#include \"file.h\"");
        EXPECT_EQUAL(current_line.get()->number, 1);

        int line_number = 1;
        // Keep these in, they are being tested in the code :)
        // SPECIAL SENTINEL 1
        // SPECIAL SENTINEL 2
        while (current_line = file(), current_line.get() != nullptr) {
            EXPECT_EQUAL(current_line.get()->number, ++line_number);
            if (current_line.get()->content == "        // SPECIAL SENTINEL 1") {
                break;
            }
            ASSERT(line_number <= 1000);
        }
        current_line = file();
        EXPECT_EQUAL(current_line.get()->content, "        // SPECIAL SENTINEL 2");
        EXPECT_EQUAL(current_line.get()->number, ++line_number);
        while (current_line = file(), current_line.get() != nullptr) {
            EXPECT_EQUAL(current_line.get()->number, ++line_number);
            ASSERT(line_number <= 1000);
        }

        // Ensure that calling it afterwards is also ok:
        current_line = file();
        EXPECT_EQUAL(current_line.get(), nullptr);
        current_line = file();
        EXPECT_EQUAL(current_line.get(), nullptr);
    );
}
#endif

}
