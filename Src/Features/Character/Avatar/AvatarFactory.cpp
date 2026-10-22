#include "AvatarFactory.h"
#include "Soldier.h"
#include "Assasin.h"
#include "Tanker.h"

UniquePtr<Avatar> AvatarFactory::createAvatar(AvatarVariant variant) {
	switch (variant) {
		case AvatarVariant::Soldier:
			return makeUnique<Soldier>();
		case AvatarVariant::Assasin:
			return makeUnique<Assasin>();
		case AvatarVariant::Tanker:
			return makeUnique<Tanker>();
	}
}