#ifndef LIBRARY__FILE_H
#define LIBRARY__FILE_H

#include "error.h"
#include "pointer.h"

#include <filesystem>
namespace fs = std::filesystem;
#include <fstream>
#include <string>

namespace hm {

class File {
public:
    File(fs::directory_entry _entry);

    struct Line {
        int number;
        std::string content;
        Line(int n, std::string c) : number(n), content(c) {}
    };

    /** Gets the next line of this file. */
    ScopedQ<Line> operator() ();

private:
    const fs::directory_entry entry;
    std::ifstream input_stream;
    int line_number;
};

}

#endif
