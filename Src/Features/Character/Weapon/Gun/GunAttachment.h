#pragma once

#include <cstddef>
#include <string>

enum class GunAttachment {
	Scope,
	Compensator,
	Suppressor,
	Count
};

inline constexpr size_t AttachmentCount = static_cast<size_t>(GunAttachment::Count);

inline constexpr size_t indexOf(GunAttachment attachment)
{
	return static_cast<size_t>(attachment);
}

inline std::string attachmentName(GunAttachment attachment)
{
	switch (attachment) {
	case GunAttachment::Scope:       return "Scope";
	case GunAttachment::Compensator: return "Compensator";
	case GunAttachment::Suppressor:  return "Suppressor";
	default:                         return "";
	}
}