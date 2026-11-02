#pragma once

#include <cstddef>
#include <string>

enum class SwordAttachment {
	Serrations,   // saw teeth along the blade: more damage
	Fuller,       // groove that lightens the blade: faster swings
	Crescent,     // curved tip: wider swing arc
	Count
};

inline constexpr size_t SwordAttachmentCount = static_cast<size_t>(SwordAttachment::Count);

inline constexpr size_t indexOf(SwordAttachment attachment)
{
	return static_cast<size_t>(attachment);
}

inline std::string attachmentName(SwordAttachment attachment)
{
	switch (attachment) {
	case SwordAttachment::Serrations: return "Serrations";
	case SwordAttachment::Fuller:     return "Fuller";
	case SwordAttachment::Crescent:   return "Crescent Tip";
	default:                          return "";
	}
}

inline std::string attachmentDescription(SwordAttachment attachment)
{
	switch (attachment) {
	case SwordAttachment::Serrations: return "+25% damage";
	case SwordAttachment::Fuller:     return "faster swings";
	case SwordAttachment::Crescent:   return "wider swing arc";
	default:                          return "";
	}
}