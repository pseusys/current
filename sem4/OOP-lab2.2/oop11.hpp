#ifndef CUCUMBER_TEST_OOP11_HPP
#define CUCUMBER_TEST_OOP11_HPP

#include <assert.h>
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <utility>

namespace stepik
{
    template <class Type>
    struct node
    {
        Type value;
        node* next;
        node* prev;

        node(const Type& value, node<Type>* next, node<Type>* prev)
                : value(value), next(next), prev(prev)
        {
        }
    };

    template <class Type>
    class list; //forward declaration

    template <class Type>
    class list_iterator
    {
    public:
        typedef ptrdiff_t difference_type;
        typedef Type value_type;
        typedef Type* pointer;
        typedef Type& reference;
        typedef size_t size_type;
        typedef std::forward_iterator_tag iterator_category;

        list_iterator()
                : m_node(NULL)
        {
        }

        list_iterator(const list_iterator& other)
                : m_node(other.m_node)
        {
        }

        list_iterator& operator = (const list_iterator& other)
        {
            // use previous step implementation
        }

        bool operator == (const list_iterator& other) const
        {
            // use previous step implementation
        }

        bool operator != (const list_iterator& other) const
        {
            // use previous step implementation
        }

        reference operator * ()
        {
            // use previous step implementation
        }

        pointer operator -> ()
        {
            // use previous step implementation
        }

        list_iterator& operator ++ ()
        {
            // use previous step implementation
        }

        list_iterator operator ++ (int)
        {
            // use previous step implementation
        }

    private:
        friend class list<Type>;

        list_iterator(node<Type>* p)
                : m_node(p)
        {
        }

        node<Type>* m_node;
    };

    template <class Type>
    class list
    {
    public:
        typedef Type value_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;

        list()
                : m_head(nullptr), m_tail(nullptr)
        {
        }

        ~list()
        {
            while (m_head != nullptr)
            {
                auto tmp = m_head->next;
                delete m_head;

                m_head = tmp;
            }
        }

        list(const list& other)
        {
            auto head_copy = other.m_head;
            if (head_copy == nullptr)
            {
                m_head = m_tail = nullptr;
                return;
            }

            m_head = new node<Type>(head_copy->value, nullptr, nullptr);
            auto tmp = m_head;

            head_copy = head_copy->next;
            while (head_copy != nullptr)
            {
                tmp->next = new node<Type>(head_copy->value, nullptr, tmp);

                tmp = tmp->next;
                head_copy = head_copy->next;
            }

            m_tail = tmp;
        }

        list(list&& other)
        {
            m_head = other.m_head;
            m_tail = other.m_tail;

            other.m_head = other.m_tail = nullptr;
        }

        list& operator= (const list& other)
        {
            if (&other != this)
            {
                auto head_copy = other.m_head;
                if (head_copy == nullptr)
                {
                    m_head = m_tail = nullptr;
                    return *this;
                }

                this->~list();

                m_head = new node<Type>(head_copy->value, nullptr, nullptr);
                auto tmp = m_head;

                head_copy = head_copy->next;
                while (head_copy != nullptr)
                {
                    tmp->next = new node<Type>(head_copy->value, nullptr, tmp);

                    tmp = tmp->next;
                    head_copy = head_copy->next;
                }

                m_tail = tmp;
            }

            return *this;
        }

        struct iterator : std::iterator<std::bidirectional_iterator_tag, Type>
        {
            iterator(node<Type>* cur) : cur(cur)
            {}

            iterator& operator++()
            {
                if (cur != nullptr)
                    cur = cur->next;

                return *this;
            }

            iterator operator++(int)
            {
                iterator tmp(*this);
                ++(*this);

                return tmp;
            }

            bool operator==(iterator const& other)
            {
                return other.cur == cur;
            }

            bool operator!=(iterator const& other)
            {
                return other.cur != cur;
            }

            Type& operator*()
            {
                return cur->value;
            }

            Type* operator->()
            {
                return &(cur->value);
            }


        private:
            node<Type>* cur;
        };

        void push_back(const value_type& value)
        {
            if (m_head == nullptr)
            {
                m_head = new node<Type>(value, nullptr, nullptr);
                m_tail = m_head;
            }
            else
            {
                node<Type>* neew = new node<Type>(value, nullptr, m_tail);

                m_tail->next = neew;
                m_tail = neew;
            }
        }

        void push_front(const value_type& value)
        {
            if (m_head == nullptr)
            {
                m_head = new node<Type>(value, nullptr, nullptr);
                m_tail = m_head;
            }
            else
            {
                node<Type>* neew = new node<Type>(value, m_head, nullptr);

                m_head->prev = neew;
                m_head = neew;
            }
        }

        reference front()
        {
            return m_head->value;
        }

        const_reference front() const
        {
            return m_head->value;
        }

        reference back()
        {
            return m_tail->value;
        }

        const_reference back() const
        {
            return m_tail->value;
        }

        iterator begin()
        {
            return iterator(m_head);
        }

        iterator end()
        {
            return iterator(m_tail->next);
        }

        iterator insert(iterator pos, const Type& value)
        {
            auto it = this->begin();
            auto tmp = m_head;

            while (it != pos)
            {
                ++it;
                tmp = tmp->next;
            }

            if (tmp == m_head)
            {
                push_front(value);
                return begin();
            }
            else if (tmp == nullptr)
            {
                push_back(value);
                return iterator(m_tail);
            }
            else
            {
                node<Type>* neew = new node<Type>(value, tmp, tmp->prev);
                tmp->prev->next = neew;
                tmp->prev = neew;

                return iterator(neew);
            }

        }

        iterator erase(iterator pos)
        {
            auto it = this->begin();
            auto tmp = m_head;

            while (it != pos)
            {
                ++it;
                tmp = tmp->next;
            }

            if (tmp == m_head)
            {
                pop_front();
                return begin();
            }
            else if (tmp == m_tail)
            {
                pop_back();
                return end();
            }
            else
            {
                tmp->prev->next = tmp->next;
                tmp->next->prev = tmp->prev;


                auto save = tmp->next;
                delete tmp;

                return iterator(save);
            }
        }

        void pop_front()
        {
            if (m_head->next == nullptr)
            {
                delete m_head;
                m_head = nullptr;
            }
            else
            {
                m_head->next->prev = nullptr;
                m_head = m_head->next;
            }

        }

        void pop_back()
        {
            if (m_tail->prev == nullptr)
            {
                delete m_tail;
                m_tail = m_head = nullptr;
            }
            else
            {
                m_tail->prev->next = nullptr;

                node<Type>* tmp = m_tail->prev;
                delete m_tail;
                m_tail = tmp;
            }
        }

        void clear()
        {
            // implement this
        }

        bool empty() const
        {
            return m_head == nullptr;
        }

        size_t size() const
        {
            size_t res = 0;
            auto tmp = m_head;

            while (tmp != nullptr)
            {
                tmp = tmp->next;
                ++res;
            }

            return res;
        }

    private:
        //your private functions

        node<Type>* m_head;
        node<Type>* m_tail;
    };


}// namespace stepik

#endif //CUCUMBER_TEST_OOP11_HPP
