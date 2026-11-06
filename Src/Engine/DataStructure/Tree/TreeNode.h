#pragma once
#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include "Engine/Core.h"
#include "Element.h"
#include "TreeVisitor.h"

namespace Engine::DataStructure {
    template<class Type, std::size_t Size>
    class TreeNode {
    public:
        using Node = TreeNode<Type, Size>;

        template<class... Args>
        explicit TreeNode(Args&&... args)
            : data(std::forward<Args>(args)...)
        {
        }

        virtual ~TreeNode() = default;

        TreeNode(const TreeNode&) = delete;
        TreeNode& operator=(const TreeNode&) = delete;

        Type& getData() {
            return data;
        }

        const Type& getData() const {
            return data;
        }

        static constexpr std::size_t childCapacity() {
            return Size;
        }

        Node* getChild(std::size_t index) {
            check(index);
            return children[index].get();
        }

        const Node* getChild(std::size_t index) const {
            check(index);
            return children[index].get();
        }

        bool hasChild(std::size_t index) const {
            check(index);
            return children[index] != nullptr;
        }

        template<class... Args>
        Node& updateChild(std::size_t index, Args&&... args) {
            check(index);

            if (children[index] != nullptr) {
                throw std::logic_error("Child already exists");
            }

            children[index] = makeUnique<Node>(
                std::forward<Args>(args)...
            );

            return *children[index];
        }

        void removeChild(std::size_t index) {
            check(index);
            children[index].reset();
        }

        // Pre-order traversal.
        void accept(TreeVisitor& visitor) {
            data.accept(visitor);
            visitor.visit(data);

            for (UniquePtr<Node>& child : children) {
                if (child != nullptr) {
                    child->accept(visitor);
                }
            }

            visitor.leave(data);
        }

    private:
        Type data;
        std::array<UniquePtr<Node>, Size> children;

        static void check(std::size_t index) {
            if (index >= Size) {
                throw std::out_of_range("Invalid child index");
            }
        }
    };
}