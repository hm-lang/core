#include "file.h"

namespace hm {

File::File(fs::directory_entry _entry)
        : entry(_entry), input_stream(_entry.path().c_str()), line_number(0) {
    ASSERT(entry.is_regular_file());
}

std::unique_ptr<File::Line> File::operator() () {
    if (!input_stream.is_open()) return nullptr;
    std::string line_content;
    if (!std::getline(input_stream, line_content)) {
        input_stream.close();
        return nullptr;
    }
    return std::unique_ptr<File::Line>(new File::Line(
        ++line_number,
        line_content
    ));
}

#ifndef NDEBUG
void test_library__file() {
    TEST(
        File file(fs::directory_entry(__FILE__));
        std::unique_ptr<File::Line> current_line = file();
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
        }
        current_line = file();
        EXPECT_EQUAL(current_line.get()->content, "        // SPECIAL SENTINEL 2");
        EXPECT_EQUAL(current_line.get()->number, ++line_number);
        while (current_line = file(), current_line.get() != nullptr) {
            EXPECT_EQUAL(current_line.get()->number, ++line_number);
            ASSERT(line_number <= 1000);
        }
    );
}
#endif

}
