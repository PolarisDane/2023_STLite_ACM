#include <iostream>
#include <type_traits>

template<typename T>
struct my_type_traits {
  using iterator_assignable = typename T::iterator_assignable;
};

struct my_true_type {
  static const bool key = true;
};

struct my_false_type {
  static const bool key = false;
};

class iterator {
public:
  using iterator_assignable = my_true_type;
};

class const_iterator {
public:
  using iterator_assignable = my_false_type;
};

template<typename T>
void is_assignable(const T& tar) {
  if (T::iterator_assignable::key == my_true_type::key) std::cout << "Assignable" << std::endl;
  else std::cout << "Not Assignable" << std::endl;
}

signed main(void) {
  iterator it1;
  const_iterator it2;
  is_assignable(it1);
  is_assignable(it2);
  return 0;
}