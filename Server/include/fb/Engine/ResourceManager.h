#pragma once
#include <EASTL/vector.h>

namespace fb
{
	class Asset;
	class ResourceManager {
	public:
		class Compartment {
		public:
			char pad_0x000[0x110];
			eastl::vector<fb::Asset*> m_dataContainers;
		};

		ResourceManager::Compartment* m_compartments[10];

		static fb::ResourceManager* getInstance() {
#ifdef CYPRESS_GW2
			return (fb::ResourceManager*)0x142BC0C98;
#else
			return nullptr;
#endif
		}
	};
}