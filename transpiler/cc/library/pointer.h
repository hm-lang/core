#ifndef LIBRARY__POINTER_H
#define LIBRARY__POINTER_H

#include "error.h"

#include <memory>

namespace hm {

template<class T>
T *copy_held_value_into_new_pointer(const T *t) {
    // Classes must override this value to avoid chopping out child bits for a parent pointer.
    // e.g., if T is a parent-type, but t is actually holding a child class U, then only
    // the parent-type T will be copy-constructed here:
    return new T(*t);
}

class Object {
public:
    // These two methods must be defined for every Object, so that we can get
    // deep copies when copying the held-values of Pointer class instances.
    virtual Object *copy() const = 0;
    virtual ~Object();
};

template <>
Object *copy_held_value_into_new_pointer(const Object *o);

template<class T>
class Pointer : public Object {
public:
    virtual ~Pointer() {}
    virtual T *get() const = 0;
    /** Returns a pointer which the caller should free. */
    virtual T *take() = 0;
    virtual Pointer *copy() const = 0;

    template<class U>
    static constexpr bool can_hold() {
        return std::is_base_of<T, U>::value;
    }
};

template<class T> class Scoped;
template<class T> class ScopedQ;
template<class T> class Ref;
template<class T> class RefQ;

#define POINTER_SET_EQUAL_GUARD(o, do_this) \
    if ((void *)&o != (void*)this) { \
        do_this; \
    } \
    return *this;

#define POINTER_MAKE_COPY_TEMPLATES(Type) \
    Type(const Type<T>& p) : ptr(copy_with_default(p.get())) {} \
    template<class U> \
    Type(const Pointer<U>& p) : ptr(copy_with_default(p.get())) {} \
    Type& operator = (const Type<T>& p) { \
        POINTER_SET_EQUAL_GUARD(p, reset(copy_with_default(p.get()))); \
    } \
    template<class U> \
    Type& operator = (const Pointer<U>& p) { \
        POINTER_SET_EQUAL_GUARD(p, reset(copy_with_default(p.get()))); \
    }

// TODO: optimize for Ref/Q, where `take() will make a copy,
// even if it won't be used (i.e., when U is not a subclass of T).
#define POINTER_MAKE_MOVE_TEMPLATES(Type) \
    Type(Type<T>&& p) : ptr(take_with_default(p.take())) {} \
    template<class U> \
    Type(Pointer<U>&& p) : ptr(take_with_default(p.take())) {} \
    Type& operator = (Type<T>&& p) { \
        POINTER_SET_EQUAL_GUARD(p, reset(take_with_default(p.take()))); \
    } \
    template<class U> \
    Type& operator = (Pointer<U>&& p) { \
        POINTER_SET_EQUAL_GUARD(p, reset(take_with_default(p.take()))); \
    }

/**
 * A non-null pointer that is tied to the current scope. 
 * The underlying type (T) must be default-initializable.
 * If passing in a pointer, the caller must not attempt to free it.
 */
template<class T>
class Scoped : public Pointer<T> {
public:
    Scoped(T *p = nullptr) : ptr(take_with_default(p)) {}

    T *get() const {
        // TODO: create a Lazy class, which creates the default for a nullptr here:
        ASSERT(ptr.get() != nullptr);
        return ptr.get();
    }

    T *take() {
        // TODO: for Lazy class, make sure to return a new T() if null:
        T *ret = ptr.release();
        ASSERT(ret != nullptr);
        return ret;
    }

    void reset(T *p) {
        ASSERT(p != nullptr);
        ptr.reset(p);
    }

    virtual Scoped *copy() const {
        return new Scoped<T>(*this);
    }

    // Copies from other pointers:
    POINTER_MAKE_COPY_TEMPLATES(Scoped)

    // Moves from other pointers:
    POINTER_MAKE_MOVE_TEMPLATES(Scoped)

protected:
    template<class U>
    T *take_with_default(U *u) const {
        if constexpr (Pointer<T>::template can_hold<U>()) {
            return u ? u : new U();
        } else {
            T *ret = u ? new T(std::move(*u)) : new T(U());
            delete u;
            return ret;
        }
    }

    template<class U>
    T *copy_with_default(const U *u) const {
        if constexpr (Pointer<T>::template can_hold<U>()) {
            return u ? copy_held_value_into_new_pointer(u) : new U();
        } else {
            return u ? new T(*u) : new T(U());
        }
    }

    std::unique_ptr<T> ptr;
};

/**
 * A possibly null pointer that is tied to the current scope. 
 * The underlying type (T) must be default-initializable.
 * The "Q" in ScopedQ stands for question mark, i.e. Scoped?.
 * If passing in a pointer, the caller must not attempt to free it.
 */
template<class T>
class ScopedQ : public Pointer<T> {
public:
    ScopedQ(T *p = nullptr) : ptr(p) {}

    T *get() const {
        return ptr.get();
    }

    T *take() {
        return ptr.release();
    }

    void reset(T *p) {
        ptr.reset(p);
    }

    virtual ScopedQ *copy() const {
        return new ScopedQ<T>(*this);
    }

    // Copies from other pointers:
    POINTER_MAKE_COPY_TEMPLATES(ScopedQ)

    // Moves from other pointers:
    POINTER_MAKE_MOVE_TEMPLATES(ScopedQ)

protected:
    template<class U>
    T *take_with_default(U *u) const {
        if constexpr (Pointer<T>::template can_hold<U>()) {
            return u;
        } else {
            T *ret = u ? new T(*u) : nullptr;
            delete u;
            return ret;
        }
    }

    template<class U>
    T *copy_with_default(U *u) const {
        if constexpr (Pointer<T>::template can_hold<U>()) {
            return u ? new U(*u) : nullptr;
        } else {
            return u ? new T(*u) : nullptr;
        }
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

    T *get() const {
        ASSERT(ptr != nullptr);
        return ptr;
    }

    T *take() {
        ASSERT(ptr != nullptr);
        // Since Ref doesn't own the pointer, it needs to create a new value
        // since the caller expects to own the returned pointer and value.
        return copy_held_value_into_new_pointer(ptr);
    }

    void reset(T *new_ptr) {
        ASSERT(new_ptr != nullptr);
        ptr = new_ptr;
    }

    virtual Ref *copy() const {
        return new Ref<T>(*this);
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

    T *get() const {
        return ptr;
    }

    T *take() {
        if (ptr == nullptr) return nullptr;
        // Since RefQ doesn't own the pointer, it needs to create a new value
        // since the caller expects to own the returned pointer and value.
        return copy_held_value_into_new_pointer(ptr);
    }

    void reset(T *p) {
        ptr = p;
    }

    virtual RefQ *copy() const {
        return new RefQ<T>(*this);
    }

private:
    T *ptr;
};

}

#endif
