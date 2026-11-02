#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

template<typename Type>
class Iterator
{
public:
	Iterator() = default;
	Iterator(Type* current) : current(current) {}

	Type& operator*() const { return *current; }
	Type* operator->() const { return current; }
	Type* getCurrent() const { return current; }

	Iterator& operator++() {
		current = current->getNext();
		return *this;
	}

	Iterator operator++(int) {
		Iterator previous = *this;
		current = current->getNext();
		return previous;
	}

	Iterator<Type> begin() {
		return Iterator<Type>(current);
	}

	Iterator<Type> end() {
		return Iterator<Type>(nullptr);
	}

	friend bool operator==(const Iterator& left, const Iterator& right) {
		return left.current == right.current;
	}

	friend bool operator!=(const Iterator& left, const Iterator& right) {
		return !(left.current == right.current);
	}

private:
	Type* current = nullptr;
};