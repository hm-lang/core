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
 * The "Q" in ScopedQ stands for question mark, i.e. Scoped?
 */
template<class T>
class ScopedQ : public Pointer<T> {
public:
    ScopedQ() : ptr(nullptr) {}

    T *get() {
        return ptr.get();
    }

    template<class... Args>
    static ScopedQ<T> create(Args&&... args) {
        return ScopedQ(new T(std::forward<Args>(args)...));
    }

private:
    ScopedQ(T *value) : ptr(value) {}
    std::unique_ptr<T> ptr;
};

/**
 * A non-null pointer that MUST OUTLIVE the current scope. 
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
