#pragma once
#include <cstddef>

namespace Engine::DataStructure {
	class TreeVisitor;

	class Element {
	public:
		virtual ~Element() = default;

		virtual void visit(TreeVisitor& visitor) = 0;
	};
}