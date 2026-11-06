#pragma once

#include <cstddef>

namespace Engine::DataStructure {
	class ElementVisitor;

	class TreeVisitor {
	public:
		virtual ~TreeVisitor() = default;

		virtual void visit(ElementVisitor& element) = 0;
		virtual void leave(ElementVisitor& element) {}
	};
}