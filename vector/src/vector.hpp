#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <iostream>
#include <climits>
#include <cstddef>

namespace sjtu
{
  /**
   * a data container like std::vector
   * store data in a successive memory and support random access.
   */
  template<typename T>
  class vector
  {
  public:
    /**
     * a type for actions of the elements of a vector, and you should write
     *   a class named const_iterator with same interfaces.
     */
     /**
      * you can see RandomAccessIterator at CppReference for help.
      */
    class const_iterator;
    class iterator
    {
      // The following code is written for the C++ type_traits library.
      // Type traits is a C++ feature for describing certain properties of a type.
      // For instance, for an iterator, iterator::value_type is the type that the 
      // iterator points to. 
      // STL algorithms and containers may use these type_traits (e.g. the following 
      // typedef) to work properly. In particular, without the following code, 
      // @code{std::sort(iter, iter1);} would not compile.
      // See these websites for more information:
      // https://en.cppreference.com/w/cpp/header/type_traits
      // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
      // About iterator_category: https://en.cppreference.com/w/cpp/iterator
      friend class vector;
    public:
      using difference_type = std::ptrdiff_t;
      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using iterator_category = std::output_iterator_tag;

    private:
      int pos;
      pointer beg;
    public:
      /**
       * return a new iterator which pointer n-next elements
       * as well as operator-
       */
      iterator(int p = 0, pointer begin = nullptr) {
        pos = p;
        beg = begin;
      }
      iterator operator+(const int& n) const
      {
        iterator newIterator(pos + n, beg);
        return newIterator;
      }
      iterator operator-(const int& n) const
      {
        iterator newIterator(pos - n, beg);
        return newIterator;
      }
      // return the distance between two iterators,
      // if these two iterators point to different vectors, throw invaild_iterator.
      int operator-(const iterator& rhs) const
      {
        if (beg != rhs.beg) throw(invalid_iterator());
        return pos - rhs.pos;
      }
      iterator& operator+=(const int& n)
      {
        pos += n;
        return (*this);
      }
      iterator& operator-=(const int& n)
      {
        pos -= n;
        return (*this);
      }
      iterator operator++(int) {
        iterator newIterator = (*this);
        pos++;
        return newIterator;
      }
      iterator& operator++() {
        pos++;
        return (*this);
      }
      iterator operator--(int) {
        iterator newIterator = (*this);
        pos--;
        return newIterator;
      }
      iterator& operator--() {
        pos--;
        return (*this);
      }
      T& operator*() const {
        return *(beg + pos);
      }
      /**
       * a operator to check whether two iterators are same (pointing to the same memory address).
       */
      bool operator==(const iterator& rhs) const {
        return beg == rhs.beg && pos == rhs.pos;
      }
      bool operator==(const const_iterator& rhs) const {
        return beg == rhs.beg && pos == rhs.pos;
      }
      /**
       * some other operator for iterator.
       */
      bool operator!=(const iterator& rhs) const {
        return !((*this) == rhs);
      }
      bool operator!=(const const_iterator& rhs) const {
        return !((*this) == rhs);
      }
    };
    class const_iterator
    {
      friend class vector;
    public:
      using difference_type = std::ptrdiff_t;
      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using iterator_category = std::output_iterator_tag;

      const_iterator(int p = 0, pointer begin = nullptr) {
        pos = p;
        beg = begin;
      }
      const_iterator operator+(const int& n) const
      {
        const_iterator newIterator(pos + n, beg);
        return newIterator;
      }
      iterator operator-(const int& n) const
      {
        const_iterator newIterator(pos - n, beg);
        return newIterator;
      }
      int operator-(const const_iterator& rhs) const
      {
        if (beg != rhs.beg) throw(invalid_iterator());
        return pos - rhs.pos;
      }
      const_iterator& operator+=(const int& n)
      {
        pos += n;
        return (*this);
      }
      const_iterator& operator-=(const int& n)
      {
        pos -= n;
        return (*this);
      }
      const_iterator operator++(int) {
        const_iterator newIterator = (*this);
        pos++;
        return newIterator;
      }
      const_iterator& operator++() {
        pos++;
        return (*this);
      }
      const_iterator operator--(int) {
        const_iterator newIterator = (*this);
        pos--;
        return newIterator;
      }
      const_iterator& operator--() {
        pos--;
        return (*this);
      }
      T operator*() const {
        return *(beg + pos);
      }
      bool operator==(const iterator& rhs) const {
        return beg == rhs.beg && pos == rhs.pos;
      }
      bool operator==(const const_iterator& rhs) const {
        return beg == rhs.beg && pos == rhs.pos;
      }
      bool operator!=(const iterator& rhs) const {
        return !((*this) == rhs);
      }
      bool operator!=(const const_iterator& rhs) const {
        return !((*this) == rhs);
      }
    private:
      int pos;
      pointer beg;
    };
  private:
    T* head;
    int capacity, siz;
    void debugInfo() {
      std::cerr << "\nvector information:siz:" << siz << "\ncapacity:" << capacity << "\n";
      //for (int i = 0; i < siz; i++) {
      //	std::cerr << "Element " << i << " " << head[i] << "\n";
      //}
    }
    void doubleCapacity() {
      //capacity *= 2;
      //T* tmp = new T[capacity];
      //for (int i = 0; i < size; i++) tmp[i] = head[i];
      //delete(head);
      //head = tmp;
      //T没有默认构造函数的时候使用new会错误
      //malloc函数在分配空间的时候不会进行初始化
      //同理free函数也不会执行对象的析构函数，需要手动调用
      //malloc返回值为void*，需要强转
      capacity <<= 1;
      T* tmp = (T*)malloc(capacity * sizeof(T));
      for (int i = 0; i < siz; i++) {
        new (tmp + i) T(head[i]);
        head[i].~T();
      }
      free(head);
      head = tmp;
    }
  public:
    vector() {
      capacity = 4;
      siz = 0;
      head = (T*)malloc(capacity * sizeof(T));
    }
    vector(const vector& other) {
      capacity = other.capacity; siz = other.siz;
      head = (T*)malloc(capacity * sizeof(T));
      for (int i = 0; i < siz; i++) new (head + i) T(other.head[i]);
    }
    ~vector() {
      for (int i = 0; i < siz; i++) head[i].~T();
      free(head); head = nullptr;
      capacity = siz = 0;
    }
    vector& operator=(const vector& other) {
      if (head == other.head) return (*this);
      for (int i = 0; i < siz; i++) head[i].~T();
      free(head);
      capacity = other.capacity; siz = other.siz;
      head = (T*)malloc(capacity * sizeof(T));
      for (int i = 0; i < siz; i++) new (head + i) T(other.head[i]);
      return (*this);
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T& at(const size_t& pos) {
      if (pos < 0 || pos >= siz) throw(index_out_of_bound());
      return head[pos];
    }
    const T& at(const size_t& pos) const {
      if (pos < 0 || pos >= siz) throw(index_out_of_bound());
      return head[pos];
    }
    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T& operator[](const size_t& pos) {
      if (pos < 0 || pos >= siz) throw(index_out_of_bound());
      return head[pos];
    }
    const T& operator[](const size_t& pos) const {
      if (pos < 0 || pos >= siz) throw(index_out_of_bound());
      return head[pos];
    }
    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T& front() const {
      if (!siz) throw(container_is_empty());
      return head[0];
    }
    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T& back() const {
      if (!siz) throw(container_is_empty());
      return head[siz - 1];
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() {
      return iterator(0, head);
    }
    const_iterator cbegin() const {
      return const_iterator(0, head);
    }
    /**
     * returns an iterator to the end.
     */
    iterator end() {
      return iterator(siz, head);
    }
    const_iterator cend() const {
      return const_iterator(siz, head);
    }
    /**
     * checks whether the container is empty
     */
    bool empty() const {
      return !siz;
    }
    /**
     * returns the number of elements
     */
    size_t size() const {
      return siz;
    }
    /**
     * clears the contents
     */
    void clear() {
      for (int i = 0; i < siz; i++) head[i].~T();
      siz = 0;
    }
    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert(iterator pos, const T& value) {
      if (pos.pos > siz) throw(index_out_of_bound());
      if (capacity == siz) doubleCapacity();
      for (int i = siz; i > pos.pos; i--) new (head + i) T(head[i - 1]);
      head[pos.pos] = value; siz++;
      return pos;
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert(const size_t& ind, const T& value) {
      if (ind > siz || ind < 0) throw(index_out_of_bound());
      if (capacity == siz) doubleCapacity();
      for (int i = siz; i > ind; i--) new (head + i) T(head[i - 1]);
      head[ind] = value; siz++;
      return iterator(ind, head);
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase(iterator pos) {
      if (pos.pos > siz) throw(index_out_of_bound());
      head[pos.pos].~T();
      for (int i = pos.pos; i < siz; i++) head[i] = head[i + 1];
      siz--;
      return pos;
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase(const size_t& ind) {
      if (ind >= siz || ind < 0) throw(index_out_of_bound());
      head[ind].~T();
      for (int i = ind; i < siz; i++) head[i] = head[i + 1];
      siz--;
      return iterator(ind, head);
    }
    /**
     * adds an element to the end.
     */
    void push_back(const T& value) {
      //debugInfo();
      if (capacity == siz) doubleCapacity();
      //std::cerr << "fuck1" << std::endl;
      new (head + siz) T(value);
      siz++;
      //注意使用malloc的时候如果对象内部有指针而=会用到这个指针
      //那么就会出现段错误，这时候就需要使用placement new将new的对象放置在相应的位置
      //此时就可以调用拷贝构造函数了
      //std::cerr << "fuck2" << std::endl;
      //debugInfo();
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
      if (!siz) throw(container_is_empty());
      head[--siz].~T();
    }
  };
}

#endif
