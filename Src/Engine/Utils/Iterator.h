#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

template<typename Type>
class Iterator
{
public:
	Iterator() = default;
	explicit Iterator(Type* node) : node(node) {}

	Type& operator*() const { return *node; }
	Type* operator->() const { return node; }

	Iterator& operator++() {
		node = node->getNext();
		return *this;
	}

	Iterator operator++(int) {
		Iterator previous = *this;
		node = node->getNext();
		return previous;
	}

	friend bool operator==(const Iterator& a, const Iterator& b) {
		return a.node == b.node;
	}

private:
	Type* node = nullptr;
};