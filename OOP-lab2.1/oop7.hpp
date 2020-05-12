#ifndef LABA2_OOP7_HPP
#define LABA2_OOP7_HPP

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

        // insert methods

        iterator insert(const_iterator pos, const Type& value) {
            //if ((pos >= m_last) || (pos < m_first)) return nullptr;
            iterator p = m_first + (pos - m_first);

            iterator nova = new value_type[size() + 1]();
            std::copy(m_first, p, nova);
            std::copy(p, m_last, nova + (p - m_first) + 1);

            *(nova + (p - m_first)) = value;

            iterator last = nova + (p - m_first);
            m_last = nova + size() + 1;
            delete[] m_first;
            m_first = std::move(nova);
            return last;
        }

        template <typename InputIterator>
        iterator insert(const_iterator pos, InputIterator first, InputIterator last) {
            //if ((pos >= m_last) || (pos < m_first)) return nullptr;
            iterator p = m_first + (pos - m_first);
            difference_type count = last - first;

            iterator nova = new value_type[size() + count]();
            std::copy(m_first, p, nova);
            std::copy(p, m_last, nova + (p - m_first) + count);

            std::copy(first, last, nova + (p - m_first));

            iterator ret = nova + (p - m_first);
            m_last = nova + size() + count;
            delete[] m_first;
            m_first = std::move(nova);
            return ret;
        }

        // push_back methods

        void push_back(const value_type& value) {
            //if ((pos >= m_last) || (pos < m_first)) return nullptr;

            iterator nova = new value_type[size() + 1]();
            std::copy(m_first, m_last, nova);
            *(nova + size()) = value;

            iterator last = nova + (m_last - m_first);
            m_last = nova + size() + 1;
            delete[] m_first;
            m_first = std::move(nova);
        }

        // resize methods

        void resize(size_t count) {
            if (count > this->size()) {
                iterator nova = new value_type[count]();
                std::copy(m_first, m_last, nova);
                for (iterator i = nova + size(); i < nova + count; ++i) {
                    *i = value_type();
                }

                delete[] m_first;
                m_first = std::move(nova);
                m_last = m_first + count;
            } if (count < this->size()) {
                erase(m_first + count, m_last);
            }
        }

        // erase methods

        iterator erase(const_iterator pos) {
            //if ((pos >= m_last) || (pos < m_first)) return nullptr;
            iterator p = m_first + (pos - m_first);

            iterator nova = new value_type[size() - 1]();
            std::copy(m_first, p, nova);
            std::copy(p + 1, m_last, nova + (p - m_first));

            iterator last = nova + (p - m_first);
            m_last = nova + size() - 1;
            delete[] m_first;
            m_first = std::move(nova);
            return last;
        }

        iterator erase(const_iterator first, const_iterator last) {
            //if ((last < first) || (last > m_last) || (first < m_first)) return nullptr;
            iterator f = m_first + (first - m_first);
            iterator l = m_first + (last - m_first);

            difference_type count = l - f;
            iterator nova = new value_type[size() - count]();
            std::copy(m_first, f, nova);
            std::copy(l, m_last, nova + (f - m_first));

            iterator ret = nova + (f - m_first);
            m_last = nova + size() - count;
            delete[] m_first;
            m_first = std::move(nova);
            return ret;
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

#endif //LABA2_OOP7_HPP
