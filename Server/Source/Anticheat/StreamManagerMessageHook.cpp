#include "pch.h"
#ifdef CYPRESS_GW2
#include "StreamManagerMessageHook.h"

#include <Anticheat/LoadoutValidator.h>
#include <fb/Engine/ServerPlayer.h>
#include <fb/TypeInfo/PVZCharacterWeaponUnlockAsset.h>

#include "Cypress/Core/Program.h"
#include "fb/Engine/ServerGameContext.h"

DEFINE_HOOK(
	fb_network_StreamManagerMessage_addMessagePart,
	__fastcall,
	fb::NetworkableMessage*,

	__int64 a1,
	fb::NetworkableMessage* inMsgPart,
	__int64 a3
)
{
	if (!g_program->GetServer() || !g_program->GetServer()->GetAnticheat()->GetEnabled())
		return Orig_fb_network_StreamManagerMessage_addMessagePart(a1, inMsgPart, a3);

	// inMsgPart should not be used after this point, since it might be a message part instead of an actual message.
	// addedMsg will always be an actual message (or NULL).
	fb::NetworkableMessage* addedMsg = Orig_fb_network_StreamManagerMessage_addMessagePart(a1, inMsgPart, a3);

	if (addedMsg)
	{
		Anticheat* anticheat = g_program->GetServer()->GetAnticheat();

		Anticheat::ValidationResult result = anticheat->ValidateNetworkableMessage( addedMsg, &addedMsg->m_serverConnection->m_reasonText );
		switch (result)
		{
			case Anticheat::Valid: return addedMsg;
			case Anticheat::InvalidDiscard:
				addedMsg->m_validationResult = fb::NetworkableMessage::ValidationResult_FailDiscard;
				break;
			case Anticheat::InvalidKick:
				addedMsg->m_validationResult = fb::NetworkableMessage::ValidationResult_FailDiscard;
				addedMsg->m_serverConnection->m_shouldDisconnect = true;
				addedMsg->m_serverConnection->m_disconnectReason = fb::SecureReason_KickedViaFairFight;
				CYPRESS_LOGMESSAGE( LogLevel::Warning, "Anticheat: kicking player for {} ({} was marked as invalid)",
					addedMsg->m_serverConnection->m_reasonText.c_str(),
					addedMsg->getType()->getName() );
				break;
		}

		//todo: add an exception for when the player swap to an AI in ops or bosshunt
		//if (msg->IsMsgType("NetworkPlayerSelectedWeaponMessage"))
		//{
		//	if (!GetPreventAliveWeaponChange() || !GetEnabled())
		//		return ret;
		//
		//	void* unk = ret->m_serverConnection->validateLocalPlayer(ret->m_localPlayerId, false);
		//
		//	if (!unk)
		//	{
		//		AC_LogMessage(LogLevel::Debug, "[{}] Couldn't validate LocalPlayer!", ret->getType()->getName());
		//		return nullptr;
		//	}
		//
		//	serverPlayer = ptrread<fb::ServerPlayer*>(unk, 0xF8);
		//
		//	const char* playerName = serverPlayer ? serverPlayer->m_name : "Null player";
		//
		//	if (serverPlayer != nullptr && serverPlayer->getServerPVZCharacterEntity() != nullptr)
		//	{
		//		AC_LogMessage(LogLevel::Info, "{} tried to change weapons while being alive ({})", playerName, ret->getType()->getName());
		//		return nullptr;
		//	}
		//}
	}

	return addedMsg;
}
#endif