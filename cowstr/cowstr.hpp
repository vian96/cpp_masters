#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

namespace HomeworkCOW {

template <typename CharT, typename Traits = std::char_traits<CharT>>
struct BasicCowStr {
    struct SharedStr {
        // shared_ptr-like structure. holds pointer to the buffer itself
        // before it, the ref_counter is stored
        // ref_counter = ref_count-1 (for 0 initialisation)
        using RefCT = std::atomic<int>;
        inline static const int DATA_OFFSET = sizeof(RefCT);

        inline static const std::unique_ptr<char[]> empty_buf_ =
            std::make_unique<char[]>(sizeof(RefCT) + sizeof(CharT));

        static CharT *empty_buf() { return (CharT *)(empty_buf_.get() + DATA_OFFSET); }

        CharT *buf;

        RefCT &ptr_to_refc() { return *(std::atomic<int> *)((char *)buf - DATA_OFFSET); }

        const RefCT &ptr_to_refc() const {
            return *(std::atomic<int> *)((char *)buf - DATA_OFFSET);
        }

        bool unique() { return !ptr_to_refc(); }

        SharedStr() {
            buf = empty_buf();
            ptr_to_refc()++;
        }

        explicit SharedStr(size_t n = 0) {
            if (!n) {
                buf = empty_buf();
                ptr_to_refc()++;
                return;
            }
            // +1 to account for \0 at the end
            auto tot_size = DATA_OFFSET + sizeof(CharT) * (n + 1);
            buf = (CharT *)(::new char[tot_size]() + DATA_OFFSET);
        }

        void free_buf() {
            auto &refc = ptr_to_refc();
            if (refc) {
                refc--;
                return;
            }
            delete[] (char *)&refc;
        }

        ~SharedStr() { free_buf(); }

        SharedStr(const SharedStr &other) : buf(other.buf) { ptr_to_refc()++; }
        SharedStr(SharedStr &&other) noexcept : buf(other.buf) {
            other.buf = empty_buf();
            other.ptr_to_refc()++;
        }
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
        size_t capacity;
        CharT small_str[SSO_SIZE] = {};
    } sso;
    // using SSO may harm asm code as more checks are required
    // However, all write operations still require atomic checks so doing a few more
    // integer comparison does not hurt much compared to atomic read
    // And all non-const operations do not require it (except for capacity())
    // since we have size and data inside struct itself

    bool is_sso() const { return data == sso.small_str; }

   public:
    BasicCowStr() : data(sso.small_str), size(0) {}

    BasicCowStr(const CharT *s) {
        int len = Traits::length(s);
        if (len < SSO_SIZE) {
            data = sso.small_str;
            Traits::copy(sso.small_str, s, len);
            size = len;
            return;
        }

        new (&data) SharedStr(len);
        sso.capacity = len;
        size = len;
        Traits::copy(shared().buf, s, len);
    }

    ~BasicCowStr() {
        if (is_sso()) return;
        shared().~SharedStr();
    }

    BasicCowStr(const BasicCowStr &other) noexcept : size(other.size) {
        if (other.is_sso()) {
            data = sso.small_str;
            Traits::copy(data, other.sso.small_str, size);
            return;
        }
        sso.capacity = other.sso.capacity;
        new (&data) SharedStr(other.shared());
    }
    BasicCowStr(const BasicCowStr &&other) noexcept : size(other.size) {
        if (other.is_sso()) {
            data = sso.small_str;
            Traits::copy(data, other.sso.small_str, size);
            return;
        }
        sso.capacity = other.sso.capacity;
        new (&data) SharedStr(std::move(other.shared()));
    }

    BasicCowStr &operator=(const BasicCowStr &other) noexcept {
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

    BasicCowStr &operator=(BasicCowStr &&other) noexcept {
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
        SharedStr other{n};
        Traits::copy(other.buf, data, size);
        sso.capacity = n;
        if (!is_sso())
            shared() = std::move(other);
        else
            new (&data) SharedStr(std::move(other));
    }

    const CharT &operator[](size_t pos) const { return data[pos]; }

    CharT &operator[](size_t pos) {
        if (!is_sso() and !shared().unique()) realloc_buf(sso.capacity);
        return data[pos];
    }

    const CharT *c_str() const { return data; }

    void push_back(CharT ch) {
        size_t cap = is_sso() ? SSO_SIZE : sso.capacity;
        if (size + 1 >= cap) realloc_buf(cap * 2);
        Traits::assign(data[size], ch);
        size++;
    }

    void append(const CharT *s) { append(s, Traits::length(s)); }

    void append(const CharT *s, size_t count) {
        size_t cap = is_sso() ? SSO_SIZE : sso.capacity;
        if (size + count >= cap) realloc_buf(std::max(cap * 2, size + count));

        Traits::copy(&data[size], s, count);
        size += count;
    }
};

using CowStr = BasicCowStr<char>;
using WCowStr = BasicCowStr<wchar_t>;

}  // namespace HomeworkCOW
