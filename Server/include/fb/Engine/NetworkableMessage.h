#pragma once
#include <fb/Engine/Message.h>
#include <fb/Engine/ServerConnection.h>

namespace fb
{
	class NetworkableMessage : public fb::Message
	{
	public:
		enum ValidationResult
		{
			ValidationResult_Success = 0x0,
			ValidationResult_FailDiscard = 0x1,
			ValidationResult_FailDisconnect = 0x2,
		};

		ServerConnection* m_serverConnection;
		void* m_clientConnection; // not confirmed
		ValidationResult m_validationResult;

		int GetLocalPlayerId() const
		{
			int localId = ptrread<int>(m_serverConnection, 0x2B60);
			if (m_localPlayerId == 255)
			{
				if (localId == 255)
					localId = *reinterpret_cast<int*>(0x14294E3FC);
			}
			else
			{
				localId = m_localPlayerId;
			}

			return localId;
		}
	};

	static_assert(offsetof(NetworkableMessage, m_validationResult) == 0x40);
}