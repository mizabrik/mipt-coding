#ifndef KD_TREE_H_
#define KD_TREE_H_

#include <memory>
#include <vector>

#include "geometry.h"
#include "entity.h"

class KDTree {
 public:
  class KDNode {
   public:
    KDNode(std::vector<Entity *> &&entities);

    bool Split();

    int Size() const;

    int Before(SimplePlane split) const;
    int After(SimplePlane split) const;
    int Balance(SimplePlane split) const;

    std::unique_ptr<KDNode> left;
    std::unique_ptr<KDNode> right;

    Box box;
    SimplePlane split;
    std::vector<Entity *> entities_;
  };

  KDTree(std::vector<Entity *> scene);

  std::unique_ptr<KDNode> root_;

  unsigned int split_size = 8;

 private:
  void Build(KDNode &node, int depth);
};

#endif // KD_TREE_H_
