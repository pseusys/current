//
// Created by miles on 5/10/2020.
//

#ifndef LABA2_OOP5_HPP
#define LABA2_OOP5_HPP

#include <assert.h>
#include <algorithm> // std::copy, std::rotate
#include <cstddef> // size_t
#include <initializer_list>
#include <stdexcept>

namespace stepik {
    template <typename Type>
    class vector {
    public:
        typedef Type* iterator;
        typedef const Type* const_iterator;

        typedef Type value_type;

        typedef value_type& reference;
        typedef const value_type& const_reference;

        typedef std::ptrdiff_t difference_type;

        explicit vector(size_t count = 0) {
            if (count > 0) {
                m_first = new value_type[count]();
                m_last = m_first + count;
            } else {
                m_first = m_last = nullptr;
            }
        }

        template <typename InputIterator>
        vector(InputIterator first, InputIterator last) : vector(last - first) {
            if (!empty()) std::copy(first, last, m_first);
        }

        vector(std::initializer_list<Type> init) : vector(init.begin(), init.end()) {}

        vector(const vector& other) : vector(other.begin(), other.end()) {}

        vector(vector&& other) noexcept {
            m_first = std::move(other.m_first);
            m_last = m_first + other.size();
            other.m_first = other.m_last = nullptr;
        }

        ~vector() {
            if (!empty()) delete[] m_first;
        }

        // assignment operators

        vector& operator=(const vector& other) {
            if (this != &other) {
                this->assign(other.begin(), other.end());
            }
            return *this;
        }

        vector& operator=(vector&& other) noexcept {
            if (this != &other) {
                if (!empty()) delete[] m_first;
                m_first = std::move(other.m_first);
                m_last = m_first + other.size();
                other.m_first = other.m_last = nullptr;
            }
            return *this;
        }

        // assign method

        template <typename InputIterator>
        void assign(InputIterator first, InputIterator last) {
            if (!empty()) delete[] m_first;

            difference_type count = last - first;
            if (count > 0) {
                m_first = new value_type[count]();
                m_last = m_first + count;

                std::copy(first, last, m_first);
            } else {
                m_first = m_last = nullptr;
            }
        }

        // at methods

        reference at(size_t pos) {
            return checkIndexAndGet(pos);
        }

        const_reference at(size_t pos) const {
            return checkIndexAndGet(pos);
        }

        // [] operators

        reference operator[](size_t pos) {
            return checkIndexAndGet(pos);
        }

        const_reference operator[](size_t pos) const {
            return checkIndexAndGet(pos);
        }

        // *begin methods

        iterator begin() {
            return m_first;
        }

        const_iterator begin() const {
            return m_first;
        }

        // *end methods

        iterator end() {
            return m_last;
        }

        const_iterator end() const {
            return m_last;
        }

        // size method

        size_t size() const {
            return m_last - m_first;
        }

        // empty method

        bool empty() const {
            return m_first == m_last;
        }

    private:
        reference checkIndexAndGet(size_t pos) const {
            if (pos >= size()) {
                throw std::out_of_range("out of range");
            }
            return m_first[pos];
        }

        //your private functions

    private:
        iterator m_first = nullptr;
        iterator m_last = nullptr;
    };
} // namespace stepik


#endif //LABA2_OOP5_HPP
