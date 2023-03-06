#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

  /**
   * a container like std::priority_queue which is a heap internal.
   */
  template<class T>
  class Node {
  public:
    Node<T>* lc, * rc;
    int dis, siz;
    T val;
    Node<T>() : lc(nullptr), rc(nullptr), dis(0), siz(0) {}
    Node<T>(const Node* other) : lc(nullptr), rc(nullptr),
      dis(other->dis), val(other->val), siz(other->siz) {}
    Node<T>(const T& e) : lc(nullptr), rc(nullptr), siz(1), dis(0), val(e) {}
  };
  template<typename T, class Compare = std::less<T>>
  class priority_queue {
  public:
    /**
     * constructors
     */
    Node<T>* root;
    void update(Node<T>* x) {
      x->siz = 1;
      if (x->lc) x->siz += x->lc->siz;
      if (x->rc) x->siz += x->rc->siz;
    }
    Node<T>* mergeNode(Node<T>* x, Node<T>* y) {
      if (!x) return y;
      if (!y) return x;
      if (Compare()(x->val, y->val)) std::swap(x, y);
      x->rc = mergeNode(x->rc, y);
      if (!x->lc) {
        x->dis = 0; std::swap(x->rc, x->lc);
        update(x);
        return x;
      }
      if (x->rc->dis > x->lc->dis) std::swap(x->rc, x->lc);
      x->dis = x->rc->dis + 1; update(x);
      return x;
    }
    priority_queue() :root(nullptr) {}
    void copy(Node<T>*& x, const Node<T>* other) {
      if (!other) return;
      x = new Node<T>(other);
      copy(x->lc, other->lc); copy(x->rc, other->rc);
    }
    priority_queue(const priority_queue& other) {
      copy(root, other.root);
    }
    /**
     * deconstructor
     */
    void del(Node<T>* x) {
      if (!x) return;
      del(x->lc); del(x->rc);
      delete x;
    }
    ~priority_queue() {
      del(root);
      root = nullptr;
    }
    /**
     * Assignment operator
     */
    priority_queue& operator=(const priority_queue& other) {
      if (&other == this) return (*this);
      del(root);
      copy(root, other.root);
      return (*this);
    }
    /**
     * get the top of the queue.
     * @return a reference of the top element.
     * throw container_is_empty if empty() returns true;
     */
    const T& top() const {
      if (!root) throw(container_is_empty());
      return root->val;
    }
    /**
     * push new element to the priority queue.
     */
    void push(const T& e) {
      Node<T>* x = new Node<T>(e);
      try {
        root = mergeNode(root, x);
      }
      catch (runtime_error) {
        delete x;
        throw (runtime_error());
      }
    }
    /**
     * delete the top element.
     * throw container_is_empty if empty() returns true;
     */
    void pop() {
      if (!root) throw(container_is_empty());
      Node<T>* tmp = root;
      root = mergeNode(root->lc, root->rc);
      delete tmp;
    }
    /**
     * return the number of the elements.
     */
    size_t size() const {
      return root->siz;
    }
    /**
     * check if the container has at least an element.
     * @return true if it is empty, false if it has at least an element.
     */
    bool empty() const {
      return !root;
    }
    /**
     * merge two priority_queues with at least O(logn) complexity.
     * clear the other priority_queue.
     */
    void merge(priority_queue& other) {
      root = mergeNode(root, other.root);
      other.root = nullptr;
    }
  };

}

#endif
