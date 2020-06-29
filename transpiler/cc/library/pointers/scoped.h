#ifndef LIBRARY__POINTERS__SCOPED_H
#define LIBRARY__POINTERS__SCOPED_H

#include "../error.h"
#include "../pointers.h"

#include <memory>

namespace hm {

template <typename T>
struct Scoped {
    typedef std::unique_ptr<T> Type;

    static inline T *get(Type &scoped) {
        T *heldPointer = scoped.get();
        ASSERT(heldPointer != nullptr);
        return heldPointer;
    }
};

#define PRIMITIVE_SCOPED(TYPE) \
    template <> \
    struct Scoped<TYPE> { \
        typedef TYPE Type; \
    \
        static inline TYPE *get(Type &scoped) { \
            return &scoped; \
        } \
    }

PRIMITIVE_SCOPED(int);

}

#endif
