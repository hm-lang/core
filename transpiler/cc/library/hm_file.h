#ifndef LIBRARY__HM_FILE_H
#define LIBRARY__HM_FILE_H

#include "file.h"

namespace hm {

class HmFile {
public:
    // Switch to Iterator<string> here:
    HmFile(File file);

    // Switch to insertable Iterator<string>'s here:
    void transpile(File cc, File h);
};

}

#endif
