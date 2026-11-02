#pragma once

#include <utility>
#include <optional>
#include "ListNode.h"
#include "List.h"

namespace Engine::DataStructure {
    template<typename DataType>
    class Queue {
    private:
        typedef ListNode<DataType, ListType::Doubly> Node;
        typedef Iterator<Node> ListIterator;

        List<DataType, Doubly> list;
        int max;

    public:
        Queue(): max(0) {}

        Queue(DataType value, int max): max(max) {
            enqueue(value);
        }

        Queue(const Queue&) = delete;
        Queue& operator=(const Queue&) = delete;

        Queue(Queue&& other) noexcept = default;
        Queue& operator=(Queue&& other) noexcept = default;

        ~Queue() {
            clear();
        }

        bool isEmpty() const {
            return list.isEmpty();
        }

        int size() const {
            return list.size();
        }

        bool isFull() const {
            return max > 0 && list.size() >= max;
        }

        int getMax() const { return max; }

        void setMax(int max) { this->max = max; }

        ListIterator getIterator() const {
            return list.getIterator();
        }

        Node* front() const {
            return list.getHead();
        }

        Node* back() const {
            return list.getTail();
        }

        void clear() {
            list.clear();
        }

        // Add an element to the back of the queue.
        std::optional<DataType> enqueue(DataType value) {
            if (isFull()) {
                return std::nullopt;
            }

            if (list.pushBack(value)) {
                return std::make_optional(value);
            }

            return std::nullopt;
        }

        // Remove the element at the front of the queue.
        std::optional<DataType> dequeue() {
			if (isEmpty()) {
				return std::nullopt;
			}

            DataType value = list.getHead()->getValue();

			if (list.popFront()) {
				return std::make_optional(value);
			}

            return std::nullopt;
        }
    };
}
