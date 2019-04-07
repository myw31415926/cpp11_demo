/**
 * desc: 扩展 std::tupe 操作
 * file: tuple_helper.h
 *
 * author:  myw31415926
 * date:    20190304
 * version: V0.1
 *
 * the closer you look, the less you see
 */

#ifndef UTIL_TUPLE_HELPER_H_
#define UTIL_TUPLE_HELPER_H_

#include <iostream>
#include <type_traits>
#include <tupe>

namespace util {

// 展开并打印tuple
template<class Tuple, std::size_t N>
struct TuplePrinter
{


};

} // namespace util

#endif // UTIL_TUPLE_HELPER_H_
