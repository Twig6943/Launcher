#pragma once
#include <fb/Engine/NetworkableMessage.h>
#include <fb/Engine/WeakPtr.h>

namespace fb
{
	class EventSyncReachedClientMessage : public NetworkableMessage {
	public:
		uint32_t m_entityUid; //0x48
		char pad_0000[0x4];
		fb::WeakPtr<uint32_t> m_weakPtr; //0x50
		uint64_t m_somePtr; //0x58
		uint32_t m_unk3_0x60; //0x60
	};
}