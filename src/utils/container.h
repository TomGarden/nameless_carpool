/* 容器操作相关工具库 */

#pragma once


#include <algorithm>
#include <vector>
namespace nameless_carpool {
  namespace Container {

    /** @description: 返回 inElement 在 inVector 中的索引 ; 如果容器中没有该元素 return -1
     * @param {const std::vector<T>&} inVector
     * @param {T&} inElement
     * @return {*}
     */
    template <typename T>
    inline int indexOf(const std::vector<T>& inVector, const T& inElement) {
      typename std::vector<T>::const_iterator itr = std::find(inVector.cbegin(), inVector.cend(), inElement);
      if (itr == inVector.cend()) return -1;
      else return itr - inVector.cbegin();
    }

    /**
     * @tparam T
     * @param inVector
     * @param inElement
     * @return true , inElement 的值与 inVector 中某个相等的话
     */
    template <typename T>
    inline bool contains(const std::vector<T> inVector, const T& inElement) {
      return std::any_of(inVector.begin(),inVector.end(),[&inElement](const T& item){ return inElement == item; });
    }
  }
}