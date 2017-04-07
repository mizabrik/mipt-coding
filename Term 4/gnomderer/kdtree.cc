#include "kdtree.h"

#include <cmath>
#include <algorithm>
#include <memory>
#include <vector>

#include "entity.h"

KDTree::KDTree(std::vector<Entity *> scene) {
  root_ = std::make_unique<KDNode>(std::move(scene));
  if (root_->Size()) {
    root_->box = root_->entities_[0]->BoundingBox();
    for (auto e : root_->entities_) {
      root_->box = BoxUnion(root_->box, e->BoundingBox());
    }
    Build(*root_, (int) std::ceil(2 * std::log2(root_->Size())));
  } else {
    root_->box = Box{{0, 0, 0}, {0, 0, 0}};
  }
}

void KDTree::Build(KDNode &node, int depth) {
  if (!depth || node.Size() < split_size)
    return;

  node.Split();
  Build(*node.left, depth - 1);
  Build(*node.right, depth - 1);
}

KDTree::KDNode::KDNode(std::vector<Entity *> &&entities)
    : entities_(std::move(entities)) {}

void KDTree::KDNode::Split() {
  auto dims = box.Dimensions();
  std::array<unsigned int, 3> axes{0, 1, 2};
  std::sort(begin(axes), end(axes), [dims] (unsigned int a, unsigned int b) {
    return dims[a] > dims[b];
  });

  for (auto axis : axes) { 
    auto comparator = [axis] (Entity *a, Entity *b) -> bool {
      Box a_box = a->BoundingBox();
      Box b_box = b->BoundingBox();
      return a_box.min[axis] < b_box.min[axis]
             || a_box.min[axis] == b_box.min[axis]
                && a_box.max[axis] < b_box.max[axis];
    };
    std::sort(entities_.begin(), entities_.end(), comparator);

    std::vector<Real> splits;
    for (auto entity : entities_)
      splits.push_back(entity->BoundingBox().min[axis]);

    auto split_at = [splits, axis] (int pos) {
      return SimplePlane{axis, splits[pos]};
    };

    unsigned int l = 0, r = splits.size() - 1;
    int balance;
    while (l < r) {
      auto mid = (l + r) / 2;
      balance = Balance(split_at(mid));

      if (balance > 0
          && std::abs(Balance(split_at(mid + 1))) <= balance) {
        l = mid + 1;
      } else if (balance < 0
                 && std::abs(Balance(split_at(mid - 1))) <= -balance) {
        r = mid - 1;
      } else {
        l = r = mid;
      }
    }

    split = split_at(l);
    if (split.position <= box.min[axis] || split.position >= box.max[axis])
      continue;

    auto begin = entities_.begin(), end = entities_.end();
    std::vector<Entity *> before(begin, begin + Before(split));
    left = std::make_unique<KDNode>(std::move(before));
    left->box = box.Before(split);
    std::vector<Entity *> after(end - After(split), end);
    right = std::make_unique<KDNode>(std::move(after));
    right->box = box.After(split);

    entities_ = std::vector<Entity *>();

    return;
  }
}

int KDTree::KDNode::Before(SimplePlane split) const {
  auto is_before = [split] (Entity *e) {
    return e->BoundingBox().min[split.axis] <= split.position;
  };
  return std::count_if(entities_.begin(), entities_.end(), is_before);
}

int KDTree::KDNode::After(SimplePlane split) const {
  auto is_after = [split] (Entity *e) {
    return e->BoundingBox().max[split.axis] >= split.position;
  };
  return std::count_if(entities_.begin(), entities_.end(), is_after);
}

int KDTree::KDNode::Balance(SimplePlane split) const {
  return After(split) - Before(split);
}

int KDTree::KDNode::Size() const {
  return entities_.size();
}
