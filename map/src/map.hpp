/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

 // only for std::less<T>
#include <iostream>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

  template<
    class Key,
    class T,
    class Compare = std::less<Key>
  > class map {
  public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
    typedef pair<const Key, T> value_type;
    class node {
    public:
      bool col;
      value_type* val;
      node* c[2], * fa;
      node() {
        col = 1;
        val = nullptr;
        fa = c[0] = c[1] = nullptr;
      }
      node(const Key& _val1, const T& _val2, node* tmp = nullptr) {
        col = 1;
        val = new value_type(_val1, _val2);
        fa = c[0] = c[1] = tmp;
      }
      node(const node& other) {
        col = other.col;
        val = new value_type(*(other.val));
        fa = c[0] = c[1] = nullptr;
      }
      ~node() {
        if (val) delete val;
        fa = c[0] = c[1] = nullptr;
        col = 0;
      }
    };

    class RedBlackTree {
    public:

      node* root;
      node* nullNode;
      int siz;

    public:
      RedBlackTree() {
        nullNode = new node;
        nullNode->c[0] = nullNode->c[1] = nullNode->fa = nullNode;
        nullNode->col = 0; siz = 0;
        root = nullNode;
      }
      void clear(node* x) {
        if (x == nullNode) return;
        clear(x->c[0]); clear(x->c[1]);
        delete x;
      }
      ~RedBlackTree() {
        clear(root); siz = 0;
        delete nullNode;
      }
      void copy(node*& x, node* other, node* other_nullNode) {
        if (other == other_nullNode) return;
        x = new node(*other);
        x->c[0] = x->c[1] = x->fa = nullNode;
        copy(x->c[0], other->c[0], other_nullNode);
        if (x->c[0] != nullNode) x->c[0]->fa = x;
        copy(x->c[1], other->c[1], other_nullNode);
        if (x->c[1] != nullNode) x->c[1]->fa = x;
      }
      RedBlackTree(const RedBlackTree& other) {
        nullNode = new node;
        nullNode->c[0] = nullNode->c[1] = nullNode->fa = nullNode;
        nullNode->col = 0; siz = other.siz;
        root = nullNode;
        copy(root, other.root, other.nullNode);
      }
      RedBlackTree& operator =(const RedBlackTree& other) {
        if (this == &other) return (*this);
        clear(root); siz = other.siz;
        copy(root, other.root, other.nullNode);
        return (*this);
      }
      void swapNode(node*& x, node*& y) {
        if (x->fa != nullNode && x->fa != y) x->fa->c[get_son(x)] = y;
        if (y->fa != nullNode && y->fa != x) y->fa->c[get_son(y)] = x;
        if (x->c[0] != nullNode && x->c[0] != y) x->c[0]->fa = y;
        if (x->c[1] != nullNode && x->c[1] != y) x->c[1]->fa = y;
        if (y->c[0] != nullNode && y->c[0] != x) y->c[0]->fa = x;
        if (y->c[1] != nullNode && y->c[1] != x) y->c[1]->fa = x;
        if (x->fa != y && y->fa != x) {
          std::swap(x->c[0], y->c[0]);
          std::swap(x->c[1], y->c[1]);
          std::swap(x->fa, y->fa);
        }
        if (x->fa == y) {
          bool p = get_son(x);
          y->c[p] = x->c[p]; x->c[p] = y;
          std::swap(x->c[!p], y->c[!p]);
          x->fa = y->fa; y->fa = x;
        }
        else if (y->fa == x) {
          bool p = get_son(y);
          x->c[p] = y->c[p]; y->c[p] = x;
          std::swap(x->c[!p], y->c[!p]);
          y->fa = x->fa; x->fa = y;
        }
        std::swap(x->col, y->col);
        if (root == x) root = y;
        else if (root == y) root = x;
        std::swap(x, y);
      }

    public:
      bool get_son(node* x)const {
        return x->fa->c[1] == x;
      }
      void rotate(node* x, int p) {
        node* s1 = x->c[!p];
        if (x->fa == nullNode) root = s1;
        else x->fa->c[get_son(x)] = s1;
        s1->fa = x->fa;
        x->c[!p] = s1->c[p];
        if (s1->c[p] != nullNode) s1->c[p]->fa = x;
        s1->c[p] = x; x->fa = s1;
      }
      void ins_fix(node* x) {
        while (x->fa->col) {
          node* f = x->fa, * gf = f->fa;
          bool p = !get_son(f);
          node* unc = gf->c[p];
          if (unc->col) {
            f->col = unc->col = 0;
            gf->col = 1;
            x = gf;
          }
          else {
            if (x == f->c[p]) rotate(x = f, !p);
            else {
              gf->col = 1; f->col = 0;
              rotate(gf, p);
            }
          }
        }
        root->col = 0;
      }
      node* ins(const value_type& v) {
        node* x = root, * f = nullNode;
        while (x != nullNode) {
          f = x;
          x = x->c[Compare()((*(x->val)).first, v.first)];
        }
        x = new node(v.first, v.second, nullNode); siz++;
        if (f != nullNode) f->c[Compare()((*(f->val)).first, v.first)] = x;
        else root = x;
        x->fa = f; ins_fix(x);
        return x;
      }
      void del_fix(node* x) {
        while (x != root && !x->col) {
          bool p = !get_son(x);
          node* f = x->fa;
          node* sib = x->fa->c[p];
          if (sib->col) {
            sib->col = 0; f->col = 1;
            rotate(f, !p);
            sib = f->c[p];
          }
          else {
            if (!sib->c[0]->col && !sib->c[1]->col) {
              sib->col = 1;
              x = f;
            }
            else {
              if (!sib->c[p]->col) {
                sib->col = 1; sib->c[!p]->col = 0;
                rotate(sib, p);
                sib = f->c[p];
              }
              sib->col = f->col;
              f->col = sib->c[p]->col = 0;
              rotate(f, !p);
              break;
            }
          }
        }
        x->col = 0;
      }
      bool del(const value_type& v) {
        node* x = root;
        while (x != nullNode && (Compare()((*(x->val)).first, v.first) || Compare()(v.first, (*(x->val)).first)))
          x = x->c[Compare()((*(x->val)).first, v.first)];
        if (x == nullNode) return false;
        node* ex = x, * g = nullptr;
        if (x->c[0] != nullNode && x->c[1] != nullNode) {
          ex = x->c[1];
          while (ex->c[0] != nullNode) ex = ex->c[0];
        }
        g = ex->c[ex->c[0] == nullNode];
        if (x != ex) swapNode(x, ex);
        g->fa = ex->fa;
        if (ex->fa == nullNode) root = g;
        else ex->fa->c[get_son(ex)] = g;
        if (!ex->col) del_fix(g);
        siz--; delete ex;
        return true;
      }
      node* get_min()const {
        node* x = root;
        while (x->c[0] != nullNode) x = x->c[0];
        return x;
      }
      node* get_max()const {
        node* x = root;
        while (x->c[1] != nullNode) x = x->c[1];
        return x;
      }
      node* get_nxt(node* x)const {
        if (x == nullNode) throw invalid_iterator();
        if (x->c[1] == nullNode) {
          while (x->fa != nullNode && get_son(x)) x = x->fa;
          return x->fa;//can be nullNode here
        }
        x = x->c[1];
        while (x->c[0] != nullNode) x = x->c[0];
        return x;
      }
      node* get_pre(node* x)const {
        if (x == nullNode) {
          if (root == nullNode) throw invalid_iterator();
          return get_max();
        }
        if (x->c[0] == nullNode) {
          while (x->fa != nullNode && !get_son(x)) x = x->fa;
          if (x->fa == nullNode) throw invalid_iterator();//can't be nullNode here
          return x->fa;
        }
        x = x->c[0];
        while (x->c[1] != nullNode) x = x->c[1];
        return x;
      }
      node* find(const Key& key)const {
        node* x = root;
        while (x != nullNode && (Compare()((*(x->val)).first, key) || Compare()(key, (*(x->val)).first)))
          x = x->c[Compare()((*(x->val)).first, key)];
        return x;
      }
    };
    RedBlackTree RBT;
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    class const_iterator;
    class iterator {
      friend class map;
    private:
      node* pos;
      map<Key, T, Compare>* beg;
      /**
       * TODO add data members
       *   just add whatever you want.
       */
    public:
      iterator() {
        beg = nullptr;
        pos = nullptr;
        // TODO
      }
      iterator(const iterator& other) {
        beg = other.beg;
        pos = other.pos;
        // TODO
      }
      iterator(const const_iterator& other) {
        beg = other.beg;
        pos = other.pos;
      }
      iterator(node* _pos, map<Key, T, Compare>* _beg) {
        pos = _pos; beg = _beg;
      }
      /**
       * TODO iter++
       */
      iterator operator++(int) {
        iterator tmp = *this;
        pos = (beg->RBT).get_nxt(pos);
        return tmp;
      }
      /**
       * TODO ++iter
       */
      iterator& operator++() {
        pos = (beg->RBT).get_nxt(pos);
        return (*this);
      }
      /**
       * TODO iter--
       */
      iterator operator--(int) {
        iterator tmp = *this;
        pos = (beg->RBT).get_pre(pos);
        return tmp;
      }
      /**
       * TODO --iter
       */
      iterator& operator--() {
        pos = (beg->RBT).get_pre(pos);
        return (*this);
      }
      /**
       * a operator to check whether two iterators are same (pointing to the same memory).
       */
      value_type& operator*() const {
        return *(pos->val);
      }
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

      /**
       * for the support of it->first.
       * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
       */
      value_type* operator->() const noexcept {
        return pos->val;
      }
    };
    class const_iterator {
      // it should has similar member method as iterator.
      //  and it should be able to construct from an iterator.
      friend class map;
    private:
      node* pos;
      const map<Key, T, Compare>* beg;
      // data members.
    public:
      const_iterator() {
        beg = nullptr;
        pos = nullptr;
        // TODO
      }
      const_iterator(const const_iterator& other) {
        beg = other.beg;
        pos = other.pos;
        // TODO
      }
      const_iterator(node* _pos, const map<Key, T, Compare>* _beg) {
        pos = _pos; beg = _beg;
      }
      const_iterator(const iterator& other) {
        beg = other.beg;
        pos = other.pos;
        // TODO
      }
      // And other methods in iterator.
      // And other methods in iterator.
      // And other methods in iterator.
      const_iterator operator++(int) {
        const_iterator tmp = *this;
        pos = (beg->RBT).get_nxt(pos);
        return tmp;
      }
      const_iterator& operator++() {
        pos = (beg->RBT).get_nxt(pos);
        return (*this);
      }
      const_iterator operator--(int) {
        const_iterator tmp = *this;
        pos = (beg->RBT).get_pre(pos);
        return tmp;
      }
      const_iterator& operator--() {
        pos = (beg->RBT).get_pre(pos);
        return (*this);
      }
      value_type operator*() const {
        return *(pos->val);
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
      value_type* operator->() const noexcept {
        return pos->val;
      }
    };
    /**
     * TODO two constructors
     */
    map() {}
    map(const map& other) :RBT(other.RBT) {}
    /**
     * TODO assignment operator
     */
    map& operator=(const map& other) {
      if (this == &other) return (*this);
      RBT = other.RBT;
      return (*this);
    }
    /**
     * TODO Destructors
     */
    ~map() {}
    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    T& at(const Key& key) {
      node* x = RBT.find(key);
      if (x == RBT.nullNode) throw index_out_of_bound();
      return (*(x->val)).second;
    }
    const T& at(const Key& key) const {
      node* x = RBT.find(key);
      if (x == RBT.nullNode) throw index_out_of_bound();
      return (*(x->val)).second;
    }
    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T& operator[](const Key& key) {
      node* x = RBT.find(key);
      if (x == RBT.nullNode)
        x = RBT.ins(pair(key, T()));
      return (*(x->val)).second;
    }
    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T& operator[](const Key& key) const {
      node* x = RBT.find(key);
      if (x == RBT.nullNode) throw index_out_of_bound();
      return (*(x->val)).second;
    }
    /**
     * return a iterator to the beginning
     */
    iterator begin() {
      return iterator(RBT.get_min(), this);
    }
    const_iterator cbegin() const {
      return const_iterator(RBT.get_min(), this);
    }
    /**
     * return a iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() {
      return iterator(RBT.nullNode, this);
    }
    const_iterator cend() const {
      return const_iterator(RBT.nullNode, this);
    }
    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const {
      return RBT.siz == 0;
    }
    /**
     * returns the number of elements.
     */
    size_t size() const {
      return RBT.siz;
    }
    /**
     * clears the contents
     */
    void clear() {
      RBT.clear(RBT.root);
      RBT.root = RBT.nullNode;
      RBT.siz = 0;
    }
    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    pair<iterator, bool> insert(const value_type& value) {
      node* x = RBT.find(value.first);
      if (x != RBT.nullNode)
        return pair(iterator(x, this), false);
      x = RBT.ins(value);
      return pair(iterator(x, this), true);
    }
    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase(iterator pos) {
      if (pos.beg != this) throw invalid_iterator();
      if (pos.pos == RBT.nullNode) throw invalid_iterator();
      if (!RBT.del(*(pos.pos->val))) throw invalid_iterator();
    }
    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key& key) const {
      node* x = RBT.find(key);
      if (x == RBT.nullNode) return 0;
      else return 1;
    }
    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key& key) {
      return iterator(RBT.find(key), this);
    }
    const_iterator find(const Key& key) const {
      return const_iterator(RBT.find(key), this);
    }
  };
}
#endif
