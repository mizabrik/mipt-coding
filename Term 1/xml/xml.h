#include <list>
#include <memory>
#include <stack>

namespace xml {

struct Node {
  enum class Type { kElement, kText };

  const Type type;

 protected:
  Node(Type type) : type(type) {}
};

struct Element : public Node {
  Element(std::string &&name, std::list<std::unique_ptr<Node>> &&children)
      : Node(Node::Type::kElement),
        name(std::move(name)),
        children(std::move(children)) {}

  const std::string name;
  
  std::list<std::unique_ptr<Node>> children;
};

struct Text : public Node {
  Text(std::string text)
      : Node(Node::Type::kText),
        text(text) {}

  std::string text;
};

template<typename IT>
Element * Parse(IT input) {
  std::stack<std::list<std::unique_ptr<Node>>> children_stack;
  children_stack.emplace();

  std::stack<std::string> tags;

  std::string text = "";
  std::string tag_name = "";

  enum class States { kText, kLangle, kTagOpen, kTagClose, kFinished };

  States state = States::kText;
  while (state != States::kFinished) {
    char c = *input;
    ++input;

    switch (state) {
      case States::kText:
        if (c == '<') {
          if (text.length() > 0)
            children_stack.top().emplace_back(new Text(std::move(text)));

          state = States::kLangle;
        } else {
          text += c;
        }
        break;

      case States::kLangle:
        if (c == '/') {
          state = States::kTagClose;
        } else {
          tag_name += c;
          state = States::kTagOpen;
        }
        break;

      case States::kTagOpen:
        if (c == '>') {
          tags.push(std::move(tag_name));
          children_stack.emplace();
          state = States::kText;
        } else {
          tag_name += c;
        }
        break;

      case States::kTagClose:
        if (c == '>') {
          Element *element = new Element(std::move(tags.top()),
                                         std::move(children_stack.top()));
          tags.pop();
          children_stack.pop();
          children_stack.top().emplace_back(element);
          if (children_stack.size() == 1) {
            state = States::kFinished;
          } else {
            state = States::kText;
          }
        }
        break;
    }
  }

  return static_cast<Element *>(children_stack.top().front().release());
}

}
