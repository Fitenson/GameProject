#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

template<typename Type>
class UniquePtr {
public:
	UniquePtr() = default;
	UniquePtr(std::nullptr_t) {}

	// takes ownership of a pointer made with new
	explicit UniquePtr(Type* rawPtr) : ptr(rawPtr) {}

	~UniquePtr() { delete ptr; }

	UniquePtr(const UniquePtr&) = delete;
	UniquePtr& operator=(const UniquePtr&) = delete;

	UniquePtr(UniquePtr&& other) : ptr(other.ptr) {
		other.ptr = nullptr;
	}

	// Converting move: UniquePtr<Derived> -> UniquePtr<Base>
	template<typename Other,
		typename = std::enable_if_t<std::is_convertible_v<Other*, Type*>>>
	UniquePtr(UniquePtr<Other>&& other) : ptr(other.release()) {}

	UniquePtr& operator=(UniquePtr&& other) noexcept {
		if (this != &other) {
			reset(other.release());
		}
		return *this;
	}

	UniquePtr& operator=(std::nullptr_t) {
		reset();
		return *this;
	}

	Type& operator*() const { return *ptr; }
	Type* operator->() const { return ptr; }
	Type* get() const { return ptr; }

	operator bool() const { return ptr != nullptr; }

	bool operator==(const UniquePtr& other) const {
		return ptr == other.ptr;
	}

	bool operator!=(const UniquePtr& other) const {
		return ptr != other.ptr;
	}

	bool operator==(std::nullptr_t) const {
		return ptr == nullptr;
	}

	bool operator!=(std::nullptr_t) const {
		return ptr != nullptr;
	}

	Type* release() {
		Type* old = ptr;
		ptr = nullptr;
		return old;
	}

	void reset(Type* newPtr = nullptr) {
		if (ptr != newPtr) {
			delete ptr;
			ptr = newPtr;
		}
	}

private:
	Type* ptr = nullptr;
};

template<typename Type, typename... Args>
UniquePtr<Type> makeUnique(Args&&... args) {
	return UniquePtr<Type>(new Type(std::forward<Args>(args)...));
}