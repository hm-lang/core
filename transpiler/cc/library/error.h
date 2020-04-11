#ifndef LIBRARY__ERROR_H
#define LIBRARY__ERROR_H

#include <cmath>
#include <exception>
#include <iostream>
#include <streambuf>
#include <string>
#include <string.h>
#include <sstream>
#include <type_traits>

class Error : public std::exception {
public:
    const std::string message;
    const std::string at;
    Error(std::string m, const char *a) throw() : message(m), at(a) {}
    Error(const char *m, const char *a) throw() : message(m), at(a) {}
    Error(std::string m, std::string a) throw() : message(m), at(a) {}
    Error(const char *m, std::string a) throw() : message(m), at(a) {}

    const char* what() const throw() {
        return message.c_str();
    }
};

class Capturer {
public:
    Capturer(std::ostream &os)
            : ostream(os), out(), old_out(os.rdbuf(out.rdbuf())) {
    }
    ~Capturer() {
        ostream.rdbuf(old_out);
    }
    std::string str() {
        return out.str();
    }
private:
    std::ostream& ostream;
    const std::stringstream out;
    std::streambuf *const old_out;
};

template <class T, class U>
bool check_equal(const T &x, const U &y) {
    if constexpr (std::is_floating_point<T>::value || std::is_floating_point<U>::value) {
        return std::abs(x - y) < 1e-7;
    }
    return x == y;
}

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TO_STRING(__LINE__)

#ifdef NDEBUG
#define DEBUG_ONLY(x) ((void)0)
// Do not log when running NDEBUG:
#define LOG(x) ((void)0)
#define ERR(x) ((void)0)
#define ASSERT(x) ((void)0)
#define TRY(x) x
#define TEST(x) ((void)0)

#else // DEBUG
#define DEBUG_ONLY(x) x
#define LOG(x) std::cout << "[" AT "]: " << x << "\n";
#define ERR(x) std::cerr << "[" AT "]: " << x << "\n";
#define ASSERT(x) if (!(x)) throw Error("expected " #x, AT)
#define TRY(x) {try { x; } catch (const Error& e) {\
        throw Error(e.message, e.at + "\n    " AT); \
    }}
#define TEST(x) { \
        Capturer test_cout(std::cout); \
        TRY(x); \
    }
#define EXPECT_EQUAL(a, b) { \
    auto av = (a); \
    auto bv = (b); \
    if (!check_equal(av, bv)) { \
        ERR(#a " = " << av << " != " #b " = " << bv); \
        throw Error("expected " #a " to equal " #b, AT); \
    } \
}
#define EXPECT_POINTER_EQUAL(a, b) { \
    auto p = (a); \
    auto bv = (b); \
    if (p == nullptr) { \
        ERR(#a " is a nullptr, can't hold value " #b " = " << bv); \
        throw Error("expected pointer at " #a " to hold value " #b ", was nullptr", AT); \
    } else if (!check_equal(*p, bv)) { \
        ERR(#a " = " << *p << " != " #b " = " << bv); \
        throw Error("expected pointer at " #a " to hold value " #b, AT); \
    } \
}
#define EXPECT_THROW(exp, err) try { \
        exp; \
        throw Error("expected " #exp " to throw " #err, AT); \
    } catch (const Error& e) { \
        if (strcmp(e.what(), err)) throw; \
    }

#endif

#endif
