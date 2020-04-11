#include "test.h"

#include <filesystem>
namespace fs = std::filesystem;
#include <string>
#include <string.h>
#include <vector>

int main(int n_arguments, const char **arguments) {
    #ifndef NDEBUG
    run_tests();
    #endif
    std::vector<fs::directory_entry> files;
    bool selected_only = n_arguments > 1;
    if (n_arguments == 1) {
        for (const fs::directory_entry &entry : fs::directory_iterator(".")) {
            if (entry.is_regular_file()
                    && strcmp(entry.path().extension().c_str(), ".hm") == 0
            ) {
                files.push_back(entry);
            }
        }
    } else {
        files.reserve(n_arguments - 1);
        for (int i = 1; i < n_arguments; ++i) {
            files.emplace_back(arguments[i]);
        }
    }
    Hm hm(std::move(files), selected_only);
    hm.convertAll();
    return 0;
}
