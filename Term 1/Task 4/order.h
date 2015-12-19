#ifndef ORDER_H_
#define ORDER_H_

#include <memory>

template <typename T>
std::size_t NodeOrder(const std::shared_ptr<T> node) {
  std::size_t order = 0;

  if (node != nullptr)
    order = 1 + std::max(NodeOrder(node->left), NodeOrder(node->right));

  return order;
}

#endif // ORDER_H_
