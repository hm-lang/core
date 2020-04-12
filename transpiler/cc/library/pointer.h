#ifndef LIBRARY__POINTER_H
#define LIBRARY__POINTER_H

#include "error.h"

#include <memory>

namespace hm {

template<class T>
class Pointer {
public:
    virtual ~Pointer() {}
    virtual T *get() = 0;
};

/**
 * A non-null pointer that is tied to the current scope. 
 * The underlying type (T) must be default-initializable.
 * If you want to create a non-default T value, use the static create function.
 * We want to make sure that Scoped really owns the pointer, so we do not
 * allow passing in a pointer here.
 */
template<class T>
class Scoped : public Pointer<T> {
public:
    Scoped() : ptr(new T()) {}

    T *get() {
        ASSERT(ptr.get() != nullptr);
        return ptr.get();
    }

    template<class... Args>
    static Scoped<T> create(Args&&... args) {
        return Scoped(new T(std::forward<Args>(args)...));
    }

private:
    Scoped(T *value) : ptr(value) {
        ASSERT(value != nullptr);
    }
    std::unique_ptr<T> ptr;
};

/**
 * A possibly null pointer that is tied to the current scope. 
 * The underlying type (T) must be default-initializable.
 * The "Q" in ScopedQ stands for question mark, i.e. Scoped?.
 * If passing in a non-null pointer, use the static create function.
 * We want to make sure that ScopedQ really owns the pointer.
 */
template<class T>
class ScopedQ : public Pointer<T> {
public:
    /**
     * Throws if the passed-in pointer is non-null.
     * Use ScopedQ<T>::create() instead.
     */
    ScopedQ(T *_ptr = nullptr) : ScopedQ(_ptr, false) {
    }

    T *get() {
        return ptr.get();
    }

    template<class... Args>
    static ScopedQ<T> create(Args&&... args) {
        return ScopedQ(new T(std::forward<Args>(args)...), true);
    }

private:
    ScopedQ(T *value, bool ok_for_non_null)
            : ptr(ok_for_non_null ? value : nullptr) {
        ASSERT(ok_for_non_null || value == nullptr);
    }
    std::unique_ptr<T> ptr;
};

/**
 * A non-null pointer that MUST OUTLIVE the current scope.
 * The pointer is not owned by this class, and so will not be freed.
 */
template<class T>
class Ref : public Pointer<T> {
public:
    Ref(T *_ptr) : ptr(_ptr) {
        ASSERT(ptr != nullptr);
    }

    T *get() {
        ASSERT(ptr != nullptr);
        return ptr;
    }

private:
    T *ptr;
};

/**
 * A possibly null pointer that MUST OUTLIVE the current scope. 
 * The pointer is not owned by this class, and so will not be freed.
 * The "Q" in RefQ stands for question mark, i.e. Ref?.
 */
template<class T>
class RefQ : public Pointer<T> {
public:
    RefQ(T *_ptr = nullptr) : ptr(_ptr) {
    }

    T *get() {
        return ptr;
    }

private:
    T *ptr;
};

}

#endif
