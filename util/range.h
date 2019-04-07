/**
 * desc: 简单迭代器类的实现
 * file: range.h
 *
 * author:  myw31415926
 * date:    20190218
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_RANGE_H_
#define UTIL_RANGE_H_

#include <stdexcept>

namespace util {

// 迭代器实现模板，实现Begin、End、Size方法
template <typename T>
class RangeImpl
{
    // 迭代器
    class Iterator
    {
    public:
        Iterator(size_t start, const RangeImpl& range)
            : index_(start), range_(range)
        {
            value_ = range_.begin_ + range_.step_ * index_;
        }

        // 获取迭代器中的值
        T operator*()
        {
            return value_;
        }

        // 迭代器比较
        bool operator==(const Iterator& other) const
        {
            return index_ == other.index_;
        }

        bool operator!=(const Iterator& other) const
        {
            return index_ != other.index_;
        }

        // 对迭代器做正向迭代
        const Iterator& operator++(void)
        {
            value_ += range_.step_;
            index_++;
            return (*this);
        }

        // 对迭代器做负向迭代
        const Iterator& operator--(void)
        {
            value_ -= range_.step_;
            index_--;
            return (*this);
        }

    private:
        T                value_;  // 当前值
        size_t           index_;  // 当前下标
        const RangeImpl& range_;
    };  // calss Iterator

public:
    RangeImpl(T begin, T end, T step = 1)
        : begin_(begin), end_(end), step_(step), step_end_(GetAdjustedSize())
    {}

    size_t size(void) const
    {
        return step_end_;
    }

    // 实现begin和end方法供 for (auto i : util::Range()) 循环迭代
    const Iterator begin(void) const
    {
        return { 0, *this };
    }

    const Iterator end(void) const
    {
        return { step_end_, *this };
    }

    T operator[](size_t idx)
    {
        return begin_ + idx * step_;
    }

private:
    // 计算最大的迭代次数
    size_t GetAdjustedSize(void) const
    {
        if (step_ > 0 && begin_ >= end_) {
            throw std::logic_error("End value must be greater than begin value.");
        } else if (step_ < 0 && begin_ <= end_) {
            throw std::logic_error("End value must be less than begin value.");
        }

        size_t sz = static_cast<size_t>((end_ - begin_) / step_);
        if (begin_ + (step_ * sz) != end_) { // 不能整除的情况
            ++sz;
        }

        return sz;
    }

private:
    const T      begin_;
    const T      end_;
    const T      step_;
    const size_t step_end_;
};  // class RangeImpl

////////////////////////////////////////////////////////////////
// Range 函数模板定义
template <typename T>
RangeImpl<T> Range(T end)
{
    return RangeImpl<T>(T(), end, 1);
}

template <typename T>
RangeImpl<T> Range(T begin, T end)
{
    return RangeImpl<T>(begin, end, 1);
}

template <typename T, typename U>
auto Range(T begin, T end, U step) -> RangeImpl<decltype(begin + step)>
{
    return RangeImpl<decltype(begin + step)>(begin, end, step);
}

} // namespace util

#endif // UTIL_RANGE_H_
