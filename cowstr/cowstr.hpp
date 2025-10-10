#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>

namespace HomeworkCOW {

// template <typename CharT>
struct CowStr {
    using CharT = char;
    using Traits = std::char_traits<CharT>;

    struct SharedStr {
        // shared_ptr-like structure. holds pointer to the buffer itself
        // before it, the ref_counter is stored
        // ref_counter = ref_count-1 (for 0 initialisation)
        using RefCT = std::atomic<int>;
        inline static const int DATA_OFFSET = sizeof(RefCT);

        // TODO: how to dealloc?
        inline static CharT *empty_buf = ::new char[sizeof(RefCT) + 1] + DATA_OFFSET;

        CharT *buf;

        RefCT &ptr_to_refc() { return *(std::atomic<int> *)(buf - DATA_OFFSET); }

        const RefCT &ptr_to_refc() const {
            return *(std::atomic<int> *)(buf - DATA_OFFSET);
        }

        SharedStr() {
            buf = empty_buf;
            ptr_to_refc()++;
        }

        explicit SharedStr(size_t n = 0) {
            if (!n) {
                buf = empty_buf;
                ptr_to_refc()++;
                return;
            }
            // +1 to account for \0 at the end
            auto tot_size = DATA_OFFSET + sizeof(CharT) * (n + 1);
            buf = ::new char[tot_size];
            buf += DATA_OFFSET;
        }

        void free_buf() {
            auto &refc = ptr_to_refc();
            if (refc) {
                refc--;
                return;
            }
            delete (char *)&refc;
        }

        ~SharedStr() { free_buf(); }

        SharedStr(const SharedStr &other) : buf(other.buf) { ptr_to_refc()++; }
        SharedStr(SharedStr &&other) noexcept : buf(other.buf) { other.buf = empty_buf; }
        SharedStr &operator=(const SharedStr &other) {
            if (this == &other) return *this;
            free_buf();
            buf = other.buf;
            ptr_to_refc()++;
            return *this;
        }
        SharedStr &operator=(SharedStr &&other) noexcept {
            if (this == &other) return *this;
            std::swap(buf, other.buf);
            return *this;
        }
    };

    inline static const size_t SSO_SIZE = 16;

    CharT *data = nullptr;

    SharedStr &shared() { return *(SharedStr *)&data; }
    const SharedStr &shared() const { return *(SharedStr *)&data; }

    size_t size = 0;
    union {
        size_t capacity = 0;
        CharT small_str[SSO_SIZE];
    } sso;
    // using SSO may harm asm code as more checks are required
    // However, all write operations still require atomic checks so doing a few more
    // integer comparison does not hurt much compared to atomic read
    // And all non-const operations do not require it (except for capacity())
    // since we have size and data inside struct itself

    bool is_sso() const {
        return (char *)data < (char *)this + sizeof(*this) and
               (char *) data > (char *)this;
    }

   public:
    CowStr() : data(sso.small_str), size(0) {}

    CowStr(const CharT *s) {
        size = Traits::length(s);
        std::cout << size << '\n';
        if (size < SSO_SIZE) {
            data = sso.small_str;
            Traits::copy(sso.small_str, s, size);
            return;
        }

        sso.capacity = size;
        new (&data) SharedStr(size);
        Traits::copy(shared().buf, s, size);
    }

    ~CowStr() {
        if (is_sso()) return;
        shared().~SharedStr();
    }

    CowStr(const CowStr &other) : size(other.size) {
        if (other.is_sso()) {
            data = sso.small_str;
            Traits::copy(data, other.sso.small_str, size);
            return;
        }
        sso.capacity = other.sso.capacity;
        std::cout << "heer\n";
        new(&data) SharedStr(other.shared());
        std::cout << "copp\n";
    }
    CowStr(const CowStr &&other) noexcept : size(other.size) {
        if (other.is_sso()) {
            data = sso.small_str;
            Traits::copy(data, other.sso.small_str, size);
            return;
        }
        sso.capacity = other.sso.capacity;
        new(&data) SharedStr(std::move(other.shared()));
    }

    CowStr &operator=(const CowStr &other) {
        if (this == &other) return *this;
        if (!is_sso()) shared().~SharedStr();

        size = other.size;
        if (other.is_sso()) {
            data = sso.small_str;
            Traits::copy(data, other.sso.small_str, size);
        } else {
            sso.capacity = other.sso.capacity;
            new (&data) SharedStr(other.shared());
        }
        return *this;
    }

    CowStr &operator=(CowStr &&other) {
        if (this == &other) return *this;
        if (!is_sso()) shared().~SharedStr();

        size = other.size;
        if (other.is_sso()) {
            data = sso.small_str;
            Traits::copy(data, other.sso.small_str, size);
        } else {
            sso.capacity = other.sso.capacity;
            new (&data) SharedStr(std::move(other.shared()));
        }
        return *this;
    }

    void realloc_buf(size_t n) {
        std::cout << "realloc" << n << '\n';
        SharedStr other{n};
        std::cout << "other" << n << '\n';
        Traits::copy(other.buf, data, size);
        std::cout << "copy" << n << '\n';
        sso.capacity = n;
        if (!is_sso())
            shared() = other;
        else
            new (&data) SharedStr(std::move(other));
        std::cout << "alloced" << n << '\n';
        std::cout << "from ccopy " << data << ' '<< size << sso.capacity << '\n';
    }

    const char *c_str() const { return data; }

    void push_back(CharT ch) {
        size_t cap = is_sso() ? SSO_SIZE : sso.capacity;
        if (size + 1 >= cap) realloc_buf(cap * 2);
        Traits::assign(data[size], ch);
        size++;
    }

    void append(const CharT *s) { append(s, Traits::length(s)); }

    void append(const CharT *s, size_t count) {
        size_t cap = is_sso() ? SSO_SIZE : sso.capacity;
        std::cout << cap << is_sso() << '\n';
                std::cout << "HERE " << data << ' '<< size << sso.capacity << '\n';
        if (size + count >= cap) realloc_buf(std::max(cap * 2, size + count));

        std::cout << "to ccopy " << data << ' '<< size << sso.capacity << '\n';

        Traits::copy(&data[size], s, count);
        std::cout << "act ccopy" << '\n';
        size += count;
    }
};

}  // namespace HomeworkCOW
