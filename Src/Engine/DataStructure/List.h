#pragma once

#include <stdexcept>
#include <utility>
#include "ListType.h"
#include "ListNode.h"
#include "Engine/Utils.h"

namespace Engine::DataStructure {
	template <typename DataType, ListType Kind>
	class List;

	template <typename DataType>
	class List<DataType, ListType::Singly> {
	private:
		typedef ListNode<DataType, ListType::Singly> Node;
		typedef Iterator<Node> ListIterator;

		Node* head;
		Node* tail;
		int count;

	public:
		List() : head(nullptr), tail(nullptr), count(0) {}

		List(DataType node) : List() {
			pushBack(node);
		}

		List(const List&) = delete;
		List& operator=(const List&) = delete;

		List(List&& other) noexcept
			: head(std::exchange(other.head, nullptr)),
			tail(std::exchange(other.tail, nullptr)),
			count(std::exchange(other.count, 0)) {
		}

		List& operator=(List&& other) noexcept {
			if (this != &other) {
				clear();
				head = std::exchange(other.head, nullptr);
				tail = std::exchange(other.tail, nullptr);
				count = std::exchange(other.count, 0);
			}
			return *this;
		}

		~List() { clear(); }

		bool isEmpty() const { return count == 0; }
		int size() const { return count; }

		ListIterator getIterator() const {
			return ListIterator(head);
		}

		Node* getHead() { return head; }
		Node* getTail() { return tail; }

		void clear() {
			Node* current = head;
			while (current != nullptr) {
				Node* next = current->getNext();
				delete current;
				current = next;
			}

			head = nullptr;
			tail = nullptr;
			count = 0;
		}


		bool pushFront(DataType value) {
			Node* node = new Node(std::move(value));

			if (node == nullptr) {
				return false;
			}

			node->setNext(head);
			if (isEmpty()) {
				tail = node;
			}
			head = node;
			++count;

			return true;
		}


		bool pushBack(DataType value) {
			Node* node = new Node(std::move(value));

			node->setNext(nullptr);
			if (isEmpty()) {
				head = node;
			}
			else {
				tail->setNext(node);
			}
			tail = node;
			++count;

			return true;
		}


		bool push(DataType value, int index) {
			Node* node = new Node(std::move(value));

			if (node == nullptr || index < 0 || index > count) {
				return false;
			}
			if (index == 0) {
				return pushFront(node);
			}
			if (index == count) {
				return pushBack(node);
			}

			Node* previous = head;
			for (int i = 0; i < index - 1; ++i) {
				previous = previous->getNext();
			}

			node->setNext(previous->getNext());
			previous->setNext(node);
			++count;

			return true;
		}


		bool popFront() {
			if (isEmpty()) {
				return false;
			}

			Node* temp = head;
			head = head->getNext();

			if (head == nullptr) {
				tail = nullptr;
			}

			delete temp;
			--count;

			return true;
		}

		bool popBack() {
			if (isEmpty()) {
				return false;
			}

			if (head == tail) {
				delete head;
				head = nullptr;
				tail = nullptr;
				--count;
				return true;
			}

			Node* previous = head;
			while (previous->getNext() != tail) {
				previous = previous->getNext();
			}

			delete tail;
			previous->setNext(nullptr);
			tail = previous;
			--count;

			return true;
		}

		bool pop(int index) {
			if (index < 0 || index >= count) {
				return false;
			}
			if (index == 0) {
				return popFront();
			}
			if (index == count - 1) {
				return popBack();
			}

			Node* previous = head;
			for (int i = 0; i < index - 1; ++i) {
				previous = previous->getNext();
			}

			Node* target = previous->getNext();
			previous->setNext(target->getNext());
			delete target;
			--count;

			return true;
		}
	};

	template <typename DataType>
	class List<DataType, ListType::Doubly> {
	private:
		typedef ListNode<DataType, ListType::Doubly> Node;
		typedef Iterator<Node> ListIterator;

		Node* head;
		Node* tail;
		int count;

	public:
		List() : head(nullptr), tail(nullptr), count(0) {}

		List(DataType value) : List() {
			pushBack(value);
		}

		List(const List&) = delete;
		List& operator=(const List&) = delete;

		List(List&& other) noexcept
			: head(std::exchange(other.head, nullptr)),
			tail(std::exchange(other.tail, nullptr)),
			count(std::exchange(other.count, 0)) {
		}

		List& operator=(List&& other) noexcept {
			if (this != &other) {
				clear();
				head = std::exchange(other.head, nullptr);
				tail = std::exchange(other.tail, nullptr);
				count = std::exchange(other.count, 0);
			}
			return *this;
		}

		~List() { clear(); }

		bool isEmpty() const { return count == 0; }
		int size() const { return count; }

		ListIterator getIterator() const {
			return ListIterator(head);
		}

		Node* getHead() const { return head; }

		Node* getTail() const { return tail; }

		void clear() {
			Node* current = head;
			while (current != nullptr) {
				Node* next = current->getNext();
				delete current;
				current = next;
			}

			head = nullptr;
			tail = nullptr;
			count = 0;
		}

		bool pushFront(DataType value) {
			Node* node = new Node(std::move(value));

			if (node == nullptr) {
				return false;
			}

			node->setPrevious(nullptr);
			node->setNext(head);

			if (isEmpty()) {
				tail = node;
			}
			else {
				head->setPrevious(node);
			}

			head = node;
			++count;

			return true;
		}

		bool pushBack(DataType value) {
			Node* node = new Node(std::move(value));

			if (node == nullptr) {
				return false;
			}

			node->setNext(nullptr);
			node->setPrevious(tail);

			if (isEmpty()) {
				head = node;
			}
			else {
				tail->setNext(node);
			}

			tail = node;
			++count;

			return true;
		}

		// Inserts so that `node` ends up at position `index`.
		// On false the list took no ownership.
		bool push(DataType value, int index) {
			Node* node = new Node(std::move(value));

			if (node == nullptr || index < 0 || index > count) {
				return false;
			}
			if (index == 0) {
				return pushFront(node);
			}
			if (index == count) {
				return pushBack(node);
			}

			ListIterator iterator = getIterator();

			for (int loopIndex = 0; loopIndex < index; ++index) {
				++iterator;
			}

			Node* current = iterator.getCurrent();
			Node* previous = current->getPrevious();

			node->setPrevious(previous);
			node->setNext(current);
			previous->setNext(node);
			current->setPrevious(node);
			++count;

			return true;
		}

		bool popFront() {
			if (isEmpty()) {
				return false;
			}

			Node* temp = head;
			head = head->getNext();

			if (head == nullptr) {
				tail = nullptr;
			}
			else {
				head->setPrevious(nullptr);
			}

			delete temp;
			--count;

			return true;
		}

		bool popBack() {
			if (isEmpty()) {
				return false;
			}

			Node* temp = tail;
			tail = tail->getPrevious();

			if (tail == nullptr) {
				head = nullptr;
			}
			else {
				tail->setNext(nullptr);
			}

			delete temp;
			--count;

			return true;
		}

		bool pop(int index) {
			if (index < 0 || index >= count) {
				return false;
			}
			if (index == 0) {
				return popFront();
			}
			if (index == count - 1) {
				return popBack();
			}

			ListIterator iterator = getIterator();

			for (int loopIndex = 0; loopIndex < index; ++index) {
				++iterator;
			}

			Node* current = iterator.getCurrent();

			current->getPrevious()->setNext(current->getNext());
			current->getNext()->setPrevious(current->getPrevious());

			delete current;
			--count;

			return true;
		}
	};
}