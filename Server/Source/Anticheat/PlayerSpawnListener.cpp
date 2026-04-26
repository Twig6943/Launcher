#include "pch.h"
#ifdef CYPRESS_GW2
#include "PlayerSpawnListener.h"

#include <Core/Logging.h>
#include "LoadoutValidator.h"
#include <Core/Program.h>

#include "fb/TypeInfo/SecureReason.h"

DEFINE_HOOK(
	fb_PVZSpawnManager_spawnOnSpawnPoint,
	__fastcall,
	bool,

	fb::ServerPlayer* player,
	void* serverCharacterEntity
)
{
    if (!g_program->GetServer()->GetAnticheat()->GetPreventInvalidLoadouts() || !g_program->GetServer()->GetAnticheat()->GetEnabled())
        return Orig_fb_PVZSpawnManager_spawnOnSpawnPoint(player, serverCharacterEntity);

    g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Player {} spawned", player->m_name);

    ValidationResult result = LoadoutValidator::validatePlayer(player);

    if (!result.isValid)
    {
        g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Player {} removed from lobby", player->m_name);

        if (HasFlag(result.flags, ValidationFlag::InvalidTeam))
            g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Invalid Team: {}", result.teamId);

        if (HasFlag(result.flags, ValidationFlag::InvalidSoldier))
            g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Invalid Soldier: {}", result.invalidSoldier);

        if (HasFlag(result.flags, ValidationFlag::InvalidPrimary))
            g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Invalid Primary: {}", result.invalidPrimary);

        if (HasFlag(result.flags, ValidationFlag::InvalidAbility1))
            g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Invalid Ability1: {}", result.invalidAbility1);

        if (HasFlag(result.flags, ValidationFlag::InvalidAbility2))
            g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Invalid Ability2: {}", result.invalidAbility2);

        if (HasFlag(result.flags, ValidationFlag::InvalidAbility3))
            g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Invalid Ability3: {}", result.invalidAbility3);

        if (HasFlag(result.flags, ValidationFlag::InvalidAlternate))
            g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Invalid Alternate: {}", result.invalidAlternate);

        if (!result.invalidUpgrades.empty())
        {
            g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "Upgrades:");

            for (const auto& upgradeName : result.invalidUpgrades)
            {
                g_program->GetServer()->GetAnticheat()->AC_LogMessage(LogLevel::Info, "{}{}", HasFlag(result.flags, ValidationFlag::InvalidUpgrade) ? ">> " : "", upgradeName);
            }
        }
        eastl::string reasonText("Invalid loadout");
        player->disconnect(fb::SecureReason::SecureReason_KickedOut, reasonText);
    }

    return Orig_fb_PVZSpawnManager_spawnOnSpawnPoint(player, serverCharacterEntity);
}
#endif