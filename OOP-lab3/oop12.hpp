#ifndef CUCUMBER_TEST_OOP12_HPP
#define CUCUMBER_TEST_OOP12_HPP

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

        shared_ptr(const shared_ptr& other)
        {
            cnt = other.cnt;
            ptr_ = other.ptr_;

            ++(*cnt);
        }

        shared_ptr& operator=(const shared_ptr& other)
        {
            if (&other != this)
            {
                if (ptr_ != other.ptr_)
                    this->~shared_ptr();

                cnt = other.cnt;
                ptr_ = other.ptr_;

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

        T& operator*() const
        {
            return *ptr_;
        }

        T* operator->() const
        {
            return &(*ptr_);
        }

        void swap(shared_ptr& x) noexcept
        {
            std::swap(ptr_, x.ptr_);
            std::swap(cnt, x.cnt);
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

}// namespace stepik

#endif //CUCUMBER_TEST_OOP12_HPP
