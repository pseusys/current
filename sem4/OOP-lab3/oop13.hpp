#ifndef CUCUMBER_TEST_OOP13_HPP
#define CUCUMBER_TEST_OOP13_HPP

namespace stepik
{
    template <typename T>
    class shared_ptr
    {
    public:
        explicit shared_ptr(T* ptr = 0)
        {
            ptr_ = ptr;
            cnt = new int(1);
        }

        ~shared_ptr()
        {
            --(*cnt);

            if (*cnt == 0)
            {
                delete ptr_;
                delete cnt;
            }
        }

        template<typename U>
        shared_ptr(const shared_ptr<U>& other)
        {
            cnt = other.get_cnt();
            ptr_ = other.get();

            ++(*cnt);
        }

        template<typename U>
        shared_ptr& operator=(const shared_ptr<U>& other)
        {
            if ((&other)->get() != this->get())
            {
                if (ptr_ != other.get())
                    this->~shared_ptr();

                cnt = other.get_cnt();
                ptr_ = other.get();

                ++(*cnt);
            }

            return *this;
        }

        explicit operator bool() const
        {
            return ptr_ != nullptr;
        }

        T* get() const
        {
            return ptr_;
        }

        long use_count() const
        {
            if (ptr_ == nullptr)
                return 0;
            return *cnt;
        }

        int* get_cnt() const
        {
            return cnt;
        }

        T& operator*() const
        {
            return *ptr_;
        }

        T* operator->() const
        {
            return &(*ptr_);
        }

        template<typename U>
        void swap(shared_ptr<U>& x) noexcept
        {
            std::swap(ptr_, x.get());
            std::swap(cnt, x.get_cnt);
        }

        void reset(T* ptr = 0)
        {
            if (ptr != ptr_)
            {
                this->~shared_ptr();

                cnt = new int(1);
            }

            ptr_ = ptr;
        }

    private:
        T* ptr_;
        int* cnt;
    };


    template <typename T, typename U>
    bool operator==(shared_ptr<T> const& a, shared_ptr<U> const& b)
    {
        return a.get() == b.get();
    }

    template <typename T, typename U>
    bool operator!=(shared_ptr<T> const& a, shared_ptr<U> const& b)
    {
        return !(a == b);
    }

}// namespace stepik

#endif //CUCUMBER_TEST_OOP13_HPP
