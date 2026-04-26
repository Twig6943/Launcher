#include "pch.h"
#ifdef CYPRESS_GW2
#include "StreamManagerMessageHook.h"
#include <fb/Engine/ServerPlayer.h>

#include "Core/Program.h"
#include "fb/Engine/ServerGameContext.h"

DEFINE_HOOK(
	fb_network_StreamManagerMessage_addMessagePart,
	__fastcall,
	fb::NetworkableMessage*,

	__int64 a1,
	fb::NetworkableMessage* msg,
	__int64 a3
)
{
	fb::NetworkableMessage* ret = Orig_fb_network_StreamManagerMessage_addMessagePart(a1, msg, a3);

	if (ret)
	{
		fb::ServerPlayer* serverPlayer = nullptr;

		if (msg->is("NetworkPlayerSelectedCustomizationAssetMessage"))
		{
			if (!g_program->GetServer()->GetAnticheat()->GetPreventServerCrash() || !g_program->GetServer()->GetAnticheat()->GetEnabled())
				return ret;

			if (ptrread<void*>(msg, 0x48) == nullptr)
			{
				void* unk = ret->m_serverConnection->validateLocalPlayer(ret->m_localPlayerId, false);

				if (!unk)
				{
					g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Debug, "[{}] Couldn't validate LocalPlayer!", ret->getType()->getName());
					return nullptr;
				}

				serverPlayer = ptrread<fb::ServerPlayer*>(unk, 0xF8);

				const char* playerName = serverPlayer ? serverPlayer->m_name : "Null player";

				g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Received {} with null CustomizationAsset from {}!", ret->getType()->getName(), playerName);

				eastl::string reasonText("Invalid object");
				serverPlayer->disconnect(fb::SecureReason_KickedOut, reasonText);

				return nullptr;
			}
		}

		if (msg->is("PVZGameplaySelfReviveMessage"))
		{
			fb::ServerGameContext* gameContext = fb::ServerGameContext::GetInstance();
			if (!gameContext) return ret;
			if (!gameContext->getLevel()) return ret;

			if (!g_program->GetServer()->GetAnticheat()->GetPreventSelfRevive() || !g_program->GetServer()->GetAnticheat()->GetEnabled())
				return ret;

			fb::LevelSetup levelSetup = ptrread<fb::LevelSetup>(gameContext->getLevel(), 0x28);
			const char* mode = levelSetup.getInclusionOption("GameMode");

			bool isAllowedMode =
				strstr(mode, "Coop") ||
				strstr(mode, "Ops0") ||
				strstr(mode, "BossHunt");

			if (isAllowedMode)
				return ret;

			void* unk = ret->m_serverConnection->validateLocalPlayer(ret->m_localPlayerId, false);

			if (!unk)
			{
				g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Debug, "[{}] Couldn't validate LocalPlayer!", ret->getType()->getName());
				return nullptr;
			}

			serverPlayer = ptrread<fb::ServerPlayer*>(unk, 0xF8);

			const char* playerName = serverPlayer ? serverPlayer->m_name : "Null player";

			g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "{} tried to self revive ({})", playerName, ret->getType()->getName());

			return nullptr;
		}

		if (msg->is("PVZGameplayServerSwapCharactersMessage"))
		{
			fb::ServerGameContext* gameContext = fb::ServerGameContext::GetInstance();
			if (!gameContext) return ret;
			if (!gameContext->m_serverPlayerManager) return ret;
			if (!gameContext->getLevel()) return ret;

			if (!g_program->GetServer()->GetAnticheat()->GetPreventPlayerSwap() || !g_program->GetServer()->GetAnticheat()->GetEnabled())
				return ret;

			fb::LevelSetup levelSetup = ptrread<fb::LevelSetup>(gameContext->getLevel(), 0x28);

			const char* mode = levelSetup.getInclusionOption("GameMode");
			const char* hostedMode = levelSetup.getInclusionOption("HostedMode");

			bool isAllowedMode =
				(strstr(mode, "Coop") ||
				strstr(mode, "Ops0") ||
				strstr(mode, "BossHunt")) &&
				strcmp(hostedMode, "LocalHosted") == 0;

			int idx = ptrread<int>(msg, 0x4C);
			if (idx < 0 || idx >= gameContext->m_serverPlayerManager->m_players.size())
				return nullptr;

			fb::ServerPlayer* player = gameContext->m_serverPlayerManager->m_players[idx];

			//only allow player swapping on Ops and BossHunt
			if (!isAllowedMode)
				return nullptr;

			//the TargetPlayer must be an AI
			if (player != nullptr && !player->isAIOrPersistentAIPlayer())
			{
				void* unk = ret->m_serverConnection->validateLocalPlayer(ret->m_localPlayerId, false);

				if (!unk)
				{
					g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Debug, "[{}] Couldn't validate LocalPlayer!", ret->getType()->getName());
					return nullptr;
				}

				serverPlayer = ptrread<fb::ServerPlayer*>(unk, 0xF8);

				const char* playerName = serverPlayer ? serverPlayer->m_name : "Null player";

				g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "{} tried to swap players ({})", playerName, ret->getType()->getName());

				return nullptr;
			}
			
			return ret;
		}

		//todo: add an exception for when the player swap to an AI in ops or bosshunt
		//if (msg->is("NetworkPlayerSelectedWeaponMessage"))
		//{
		//	if (!g_program->GetServer()->GetAnticheat()->GetPreventAliveWeaponChange() || !g_program->GetServer()->GetAnticheat()->GetEnabled())
		//		return ret;
		//
		//	void* unk = ret->m_serverConnection->validateLocalPlayer(ret->m_localPlayerId, false);
		//
		//	if (!unk)
		//	{
		//		g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Debug, "[{}] Couldn't validate LocalPlayer!", ret->getType()->getName());
		//		return nullptr;
		//	}
		//
		//	serverPlayer = ptrread<fb::ServerPlayer*>(unk, 0xF8);
		//
		//	const char* playerName = serverPlayer ? serverPlayer->m_name : "Null player";
		//
		//	if (serverPlayer != nullptr && serverPlayer->getServerPVZCharacterEntity() != nullptr)
		//	{
		//		g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "{} tried to change weapons while being alive ({})", playerName, ret->getType()->getName());
		//		return nullptr;
		//	}
		//}

		if (msg->is("ClientBuffApplyFromClientMessage") || msg->is("ClientBuffKillFromClientMessage"))
		{
			const char* apply_or_kill = msg->is("ClientBuffApplyFromClientMessage") ? "apply" : "kill";

			if (!g_program->GetServer()->GetAnticheat()->GetPreventClientBuffs() || !g_program->GetServer()->GetAnticheat()->GetEnabled())
				return ret;

			void* unk = ret->m_serverConnection->validateLocalPlayer(ret->m_localPlayerId, false);

			if (!unk)
			{
				g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Debug, "[{}] Couldn't validate LocalPlayer!", ret->getType()->getName());
				return nullptr;
			}
				
			serverPlayer = ptrread<fb::ServerPlayer*>(unk, 0xF8);

			const char* playerName = serverPlayer ? serverPlayer->m_name : "Null player";
			
			g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Debug, "{} tried to {} a client buff ({})", playerName, apply_or_kill, ret->getType()->getName());

			return nullptr;
		}
	}

	return ret;
}
#endif