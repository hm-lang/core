#include "hm.h"

#include "library/error.h"

Hm::Hm(std::vector<fs::directory_entry> f, bool s) : files(f), selected_files_only(s) {
}

void Hm::convertAll() {
    for (const fs::directory_entry &file : files) {
        convert(file);
    }
}

void Hm::convert(const fs::directory_entry &file) {
    LOG("converting " << file);
}

#ifndef NDEBUG
void test_hm() {

}
#endif
