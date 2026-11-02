#pragma once

#include <utility>
#include "ListType.h"

namespace Engine::DataStructure {
    template <typename DataType, ListType Kind>
    class ListNode;

    template <typename DataType>
    class ListNode<DataType, ListType::Singly> {
    public:
        ListNode() : value(), next(nullptr) {}
        ListNode(DataType value) : value(std::move(value)), next(nullptr) {}

        DataType& getValue() { return value; }
        const DataType& getValue() const { return value; }

        ListNode* getNext() const { return next; }
        void setNext(ListNode* node) { next = node; }

        DataType& operator*() { return value; }
        const DataType& operator*() const { return value; }

    private:
        DataType value;
        ListNode* next;
    };

    template <typename DataType>
    class ListNode<DataType, ListType::Doubly> {
    public:
        ListNode() : value(), next(nullptr), previous(nullptr) {}
        ListNode(DataType value) : value(std::move(value)), next(nullptr), previous(nullptr) {}

        DataType& getValue() { return value; }
        const DataType& getValue() const { return value; }

        ListNode* getNext() const { return next; }
        void setNext(ListNode* node) { next = node; }

        ListNode* getPrevious() const { return previous; }
        void setPrevious(ListNode* node) { previous = node; }

        DataType& operator*() { return value; }
        const DataType& operator*() const { return value; }

    private:
        DataType value;
        ListNode* next;
        ListNode* previous;
    };
}