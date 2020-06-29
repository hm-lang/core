#ifndef LIBRARY__POINTERS__SCOPED_Q_H
#define LIBRARY__POINTERS__SCOPED_Q_H

#include "../error.h"
#include "../pointers.h"

#include <memory>

namespace hm {

template <typename T>
struct ScopedQ {
    typedef std::unique_ptr<T> Type;

    static inline T *get(Type &scopedQ) {
        return scopedQ.get();
    }
};

}

#endif
