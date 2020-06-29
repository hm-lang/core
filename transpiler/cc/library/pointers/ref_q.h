#ifndef LIBRARY__POINTERS__REF_Q_H
#define LIBRARY__POINTERS__REF_Q_H

#include "../error.h"
#include "../pointers.h"

namespace hm {

template <typename T>
struct RefQ {
    typedef T *Type;

    static inline T *get(Type &heldPointer) {
        return heldPointer;
    }

    /** Sets passed-in Type to the newValue pointer passed in. */
    /*
    static inline T *set(Type &lhs, T *newValue) {
        T *oldValue = get(lhs);
        lhs = newValue;
        return oldValue;    
    }
    */

    /** Returns a possibly-null pointer that should be freed by caller. */
    /*
    static inline T *copy(Type &t) {
        T *pointer = get(t):
        return pointer != nullptr
                ? copy_held_value_into_new_pointer(pointer)
                : nullptr;
    }
    */
};

}

#endif
