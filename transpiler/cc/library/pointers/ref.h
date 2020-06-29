#ifndef LIBRARY__POINTERS__REF_H
#define LIBRARY__POINTERS__REF_H

#include "../error.h"
#include "../pointers.h"

namespace hm {

template <typename T>
struct Ref {
    typedef T *Type;

    static inline T *get(Type &heldPointer) {
        ASSERT(heldPointer != nullptr);
        return heldPointer;
    }

    /** Sets passed-in Type to the newValue pointer passed in. */
    /*
    static inline T *set(Type &lhs, T *newValue) {
        ASSERT(newValue != nullptr);
        T *oldValue = get(lhs);
        lhs = newValue;
        return oldValue;    
    }
    */

    /** Returns a pointer that should be freed by caller. */
    /*
    static inline T *copy(Type &t) {
        return copy_held_value_into_new_pointer(get(t));
    }
    */
};

}
#endif
