#ifndef HM_H
#define HM_H

#include <filesystem>
namespace fs = std::filesystem;
#include <string>
#include <vector>

class Hm {
public:
    Hm(std::vector<fs::directory_entry> f, bool s);

    void convertAll();

private:
    void convert(const fs::directory_entry &file);

    std::vector<fs::directory_entry> files;
    const bool selected_files_only;
};

#endif
