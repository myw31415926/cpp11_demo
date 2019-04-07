/**
 * desc: variant类接口，类似union集合
 * file: variant.h
 *
 * author:  myw31415926
 * date:    201903024
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_VARIANT_H_
#define UTIL_VARIANT_H_

#include <stdexcept>
#include <typeindex>
#include <type_traits>
#include <functional>

namespace util {

// 前置声明。获取最大的整数，用来申请union的内存
template<size_t arg, size_t... rest>
struct IntegerMax;

// std::integral_constant包装特定类型的静态常量
template<size_t arg>
struct IntegerMax<arg> : std::integral_constant<size_t, arg> {};

template<size_t arg1, size_t arg2, size_t... rest>
struct IntegerMax<arg1, arg2, rest...> : std::integral_constant<size_t,
    arg1 >= arg2 ? IntegerMax<arg1, rest...>::value : IntegerMax<arg1, rest...>::value > {};

// 获取字节对齐的最大的align
template<typename... Args>
struct MaxAlign : std::integral_constant<int, IntegerMax<std::alignment_of<Args>::value...>::value> {};


// 前置声明。根据索引获取索引位置的类型
template<int index, typename... Types>
struct IndexType;

template<int index, typename First, typename... Types>
struct IndexType<index, First, Types...> : IndexType<index - 1, Types...> {};

// 特化
template<typename First, typename... Types>
struct IndexType<0, First, Types...>
{
    using DataType = First;
};


// 获取T的偏移量（从右至左的偏移量，通过计算剩余参数的sizeof得到）
// 前置声明
template<typename T, typename... Types>
struct GetLeftSize;

template<typename T, typename First, typename... Types>
struct GetLeftSize<T, First, Types...> : GetLeftSize<T, Types...> {};

// 特化, 若与T类型相同，则获取其偏移长度
template<typename T, typename... Types>
struct GetLeftSize<T, T, Types...> : std::integral_constant<int, sizeof...(Types)> {};

// 特化，没有则返回-1
template<typename T>
struct GetLeftSize<T> : std::integral_constant<int, -1> {};

// 获取第一个T的索引位置，总长度减去偏移量
template<typename T, typename... Types>
struct IndexOf : std::integral_constant<int, sizeof...(Types) - GetLeftSize<T, Types...>::value - 1> {};


// 是否包含某个类型
template<typename T, typename... Types>
struct Contains : std::true_type {};

// 递归比较类型是否一致
template<typename T, typename First, typename... Types>
struct Contains<T, First, Types...> :
    std::conditional<std::is_same<T, First>::value, std::true_type, Contains<T, Types...>>::type {};

// 终止条件
template<typename T>
struct Contains<T> : std::false_type {};


// 前置声明。Variant协助类，提供Destroy, Move, Copy接口
template<typename... Args>
struct VariantHelper;

template<typename T, typename... Args>
struct VariantHelper<T, Args...>
{
    inline static void Destroy(std::type_index id, void* data)
    {
        if (id == std::type_index(typeid(T))) {
            // reinterpret_cast允许将任何指针转换为任何其他指针类型，也允许将任何整数类型转换为任何指针类型以及反向转换
            reinterpret_cast<T*>(data)->~T();   // 先强转，再调用析构函数
        } else {
            VariantHelper<Args...>::Destroy(id, data);  // 递归向后执行
        }
    }

    inline static void Move(std::type_index old_id, void* old_data, void* new_data)
    {
        if (old_id == std::type_index(typeid(T))) {
            new (new_data)T(std::move(*reinterpret_cast<T*>(old_data))); // palcement new
        } else {
            VariantHelper<Args...>::Move(old_id, old_data, new_data);
        }
    }

    inline static void Copy(std::type_index old_id, void* old_data, void* new_data)
    {
        if (old_id == std::type_index(typeid(T))) {
            new (new_data)T(*reinterpret_cast<T*>(old_data));   // palcement new
        } else {
            VariantHelper<Args...>::Copy(old_id, old_data, new_data);
        }
    }
};

// 特化，终止条件
template<>
struct VariantHelper<>
{
    inline static void Destroy(std::type_index id, void* data) {}
    inline static void Move(std::type_index old_id, void* old_data, void* new_data) {}
    inline static void Copy(std::type_index old_id, void* old_data, void* new_data) {}
};


template<typename... Types>
class Variant
{
    using VarHelper = VariantHelper<Types...>;

    enum
    {
        DataSize = IntegerMax<sizeof(Types)...>::value, // 数据常量，表示最大的数据长度
        AlignSize = MaxAlign<Types...>::value           // 数据常量，表示最大的对齐长度
    };
    using Data = typename std::aligned_storage<DataSize, AlignSize>::type;

public:
    template<int index>
    using IndexType = typename IndexType<index, Types...>::DataType;

    Variant(void) : typeindex_(typeid(void)) {}

    Variant(Variant<Types...>&& othre) : typeindex_(othre.typeindex_)
    {
        VarHelper::Move(othre.typeindex_, &othre.data_, &data_);
    }

    Variant(const Variant<Types...>& othre) : typeindex_(othre.typeindex_)
    {
        VarHelper::Copy(othre.typeindex_, &othre.data_, &data_);
    }

    virtual ~Variant()
    {
        VarHelper::Destroy(typeindex_, &data_);
    }

    Variant& operator=(const Variant& othre)
    {
        VarHelper::Copy(othre.typeindex_, &othre.data_, &data_);
        typeindex_ = othre.typeindex_;
        return *this;
    }

    Variant& operator=(Variant&& othre)
    {
        VarHelper::Move(othre.typeindex_, &othre.data_, &data_);
        typeindex_ = othre.typeindex_;
        return *this;
    }

    // remove_reference若类型 T 为引用类型，则提供成员 typedef type ，其为 T 所引用的类型。否则 type 为 T
    template<class T, class = typename std::enable_if<
        Contains<typename std::remove_reference<T>::type, Types...>::value>::type>
    Variant(T&& value) : typeindex_(typeid(void))
    {
        VarHelper::Destroy(typeindex_, &data_);
        using U = typename std::remove_reference<T>::type;
        new (&data_) U(std::forward<T>(value));
        typeindex_ = std::type_index(typeid(T));
    }

    template<typename T>
    bool Is() const
    {
        return (typeindex_ == std::type_index(typeid(T)));
    }

    bool Empty() const
    {
        return (typeindex_ == std::type_index(typeid(void)));
    }

    std::type_index Type() const
    {
        return typeindex_;
    }

    // maybe throw std::bad_cast
    template<typename T>
    typename std::decay<T>::type& Get()     // std::decay退化类型的修饰，去掉引用、const修饰符
    {
        using U = typename std::decay<T>::type;
        if (!Is<U>()) {
            std::string errmsg = typeid(U).name();
            errmsg += " is not define, current type is ";
            errmsg += typeindex_.name();
            throw std::logic_error(errmsg);
        }
        return *reinterpret_cast<U*>(&data_);
    }

    template<typename T>
    int GetIndexOf() const
    {
        // std::cout << "sizeof...(Types)=" << sizeof...(Types) << std::endl;
        // std::cout << "GetLeftSize<T, Types...>::value=" << GetLeftSize<T, Types...>::value << std::endl;
        // std::cout << sizeof...(Types) - GetLeftSize<T, Types...>::value - 1 << std::endl;
        return IndexOf<T, Types...>::value;
    }

    bool operator==(const Variant& rhs) const
    {
        return (typeindex_ == rhs.typeindex_);
    }

    bool operator<(const Variant& rhs) const
    {
        return (typeindex_ < rhs.typeindex_);
    }

private:
    Data data_;
    std::type_index typeindex_;
};

} // namespace util

#endif // UTIL_VARIANT_H_
