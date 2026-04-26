#pragma once
#include <fb/Engine/Message.h>
#include <fb/Engine/ServerConnection.h>

namespace fb
{
	class NetworkableMessage : public fb::Message
	{
	public:
		fb::ServerConnection* m_serverConnection;
		char pad_0000[0x10];
	};
}