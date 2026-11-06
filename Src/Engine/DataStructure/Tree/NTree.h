#pragma once
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "Engine/Core.h"
#include "Element.h"
#include "TreeVisitor.h"
#include "TreeNode.h"

namespace Engine::DataStructure {
    template<class Type, std::size_t Size>
    class NTree {
    public:
        using Node = TreeNode<Type, Size>;

        static_assert(
            std::is_base_of_v<ElementVisitor, Type>,
            "Type must inherit from ElementVisitor"
            );

        NTree() = default;

        bool isEmpty() const {
            return root == nullptr;
        }

        Node* getRoot() {
            return root.get();
        }

        const Node* getRoot() const {
            return root.get();
        }

        template<class... Args>
        Node& updateRoot(Args&&... args) {
            if (root != nullptr) {
                throw std::logic_error("Root already exists");
            }

            root = makeUnique<Node>(
                std::forward<Args>(args)...
            );

            return *root;
        }

        void clear() {
            root.reset();
        }

        void accept(TreeVisitor& visitor) {
            if (root != nullptr) {
                root->accept(visitor);
            }
        }

    private:
        UniquePtr<Node> root;
    };
}