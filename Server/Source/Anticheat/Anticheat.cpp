#include "pch.h"
#ifdef CYPRESS_GW2
#include "fb/Engine/ConsoleContext.h"
#include "Anticheat.h"

#include <fb/Engine/LevelSetup.h>
#include <fb/Engine/NetworkableMessage.h>
#include <fb/Engine/ServerGameContext.h>
#include <fb/Engine/ServerPlayer.h>
#include <fb/TypeInfo/PVZCharacterWeaponUnlockAsset.h>

#include "LoadoutValidator.h"
#include "fb/Engine/SettingsManager.h"

void Anticheat::AnticheatVerbose( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    getInstance().SetVerbose( enable );
    cc.push( "{} Verbose", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatEnabled( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    void* gmds = fb::SettingsManager::GetInstance()->getContainer<void*>( "GameMode" );

    cc.push( "GameMode: {}", gmds );

    getInstance().SetEnabled( enable );
    cc.push( "{} Anticheat", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatPreventClientBuffs( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    getInstance().SetPreventClientBuffs( enable );
    cc.push( "{} ClientBuff protection!", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatPreventBlacklistedEventSyncs( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    getInstance().SetPreventBlacklistedEventSyncs( enable );
    cc.push( "{} EventSync protection!", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatPreventInvalidLoadouts( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    getInstance().SetPreventInvalidLoadouts( enable );
    cc.push( "{} Loadout protection!", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatPreventPlayerSwap( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    getInstance().SetPreventPlayerSwap( enable );
    cc.push( "{} PlayerSwap protection!", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatPreventAliveWeaponChange( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    getInstance().SetPreventAliveWeaponChange( enable );
    cc.push( "{} AliveWeaponChange protection!", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatPreventSelfRevive( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    getInstance().SetPreventSelfRevive( enable );
    cc.push( "{} SelfRevive protection!", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatPreventServerCrash( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    getInstance().SetPreventServerCrash( enable );
    cc.push( "{} ServerCrash protection!", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatPreventClientLevelLoading( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    getInstance().SetPreventClientLevelLoading( enable );
    cc.push( "{} ClientLevelLoading protection!", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatPreventSyncSettingsFromClients( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    bool enable;

    stream >> enable;

    getInstance().SetPreventSyncSettingsFromClients( enable );
    cc.push( "{} PreventSyncSettingsFromClients protection!", enable ? "Enabled" : "Disabled" );
}

void Anticheat::AnticheatReloadWeaponSets( fb::ConsoleContext& cc )
{
    LoadoutValidator::getInstance().init();
    cc.push( "WeaponSets reloaded" );
}

void Anticheat::AnticheatClearKitBlacklist( fb::ConsoleContext& cc )
{
    LoadoutValidator::getInstance().kitBlacklist.clear();
    cc.push( "Blacklisted Kits vector cleared" );
}

void Anticheat::AnticheatPrintBlacklistedKits( fb::ConsoleContext& cc )
{
    cc.push( "Blacklisted Kits vector:" );

    int count = 0;

    for ( auto& kit : LoadoutValidator::getInstance().kitBlacklist )
    {
        cc.push( "[{}] {}", count, kit.c_str() );
        count++;
    }
}

void Anticheat::AnticheatAddKitToBlacklist( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    std::string kit;

    stream >> kit;

    LoadoutValidator::getInstance().kitBlacklist.insert( kit );

    cc.push( "Added {} to kit blacklist", kit );
}

void Anticheat::AnticheatRemoveKitFromBlacklist( fb::ConsoleContext& cc )
{
    auto stream = cc.stream();
    std::string kit;

    stream >> kit;

    LoadoutValidator::getInstance().kitBlacklist.erase( kit );

    cc.push( "Removed {} to kit blacklist", kit );
}

const char* Anticheat::GetPlayerName( fb::ServerPlayer* player )
{
    if ( player )
        return player->m_name;
    return "Null player";
}

Anticheat::ValidationResult Anticheat::ValidateNetworkableMessage( fb::NetworkableMessage* inMsg, eastl::string* outReasonString )
{
    fb::ServerPlayer* serverPlayer = nullptr;
    switch ( inMsg->m_type )
    {
        case fnvHashConstexpr( "NetworkPlayerSelectedCustomizationAssetMessage" ):
            {
                if ( !GetPreventServerCrash() )
                    return Valid;

                if ( ptrread<void*>( inMsg, 0x48 ) == nullptr )
                {
                    void* unk = inMsg->m_serverConnection->validateLocalPlayer( inMsg->GetLocalPlayerId(), false );

                    if ( !unk )
                    {
                        AC_LogMessage( LogLevel::Debug,
                                       "[{}] Couldn't validate LocalPlayer!",
                                       inMsg->getType()->getName() );
                        return InvalidKick;
                    }

                    serverPlayer = ptrread<fb::ServerPlayer*>( unk, 0xF8 );

                    AC_LogMessage( LogLevel::Info,
                                   "Received {} with null CustomizationAsset from {}!",
                                   inMsg->getType()->getName(),
                                   GetPlayerName( serverPlayer ) );

                    *outReasonString = "Invalid object";

                    return InvalidKick;
                }

                return Valid;
            }
            break;
        case fnvHashConstexpr( "PVZGameplaySelfReviveMessage" ):
            {
                if ( !GetPreventSelfRevive() )
                    return Valid;

                fb::ServerGameContext* gameContext = fb::ServerGameContext::GetInstance();
                if ( !gameContext || !gameContext->getLevel() )
                    return Valid;

                auto* levelSetup = reinterpret_cast<fb::LevelSetup*>(uintptr_t( gameContext->getLevel() ) + 0x28);
                const char* mode = levelSetup->getInclusionOption( "GameMode" );

                bool isAllowedMode =
                        strstr( mode, "Coop" ) ||
                        strstr( mode, "Ops0" ) ||
                        strstr( mode, "BossHunt" );

                if ( isAllowedMode )
                    return Valid;

                void* unk = inMsg->m_serverConnection->validateLocalPlayer( inMsg->GetLocalPlayerId(), false );

                if ( !unk )
                {
                    AC_LogMessage( LogLevel::Debug,
                                   "[{}] Couldn't validate LocalPlayer!",
                                   inMsg->getType()->getName() );
                    return InvalidKick;
                }

                serverPlayer = ptrread<fb::ServerPlayer*>( unk, 0xF8 );

                AC_LogMessage( LogLevel::Info,
                               "{} tried to self revive ({})",
                               GetPlayerName( serverPlayer ),
                               inMsg->getType()->getName() );

                return InvalidKick;
            }
            break;
        case fnvHashConstexpr( "PVZGameplayServerSwapCharactersMessage" ):
            {
                if ( !GetPreventPlayerSwap() )
                    return Valid;

                fb::ServerGameContext* gameContext = fb::ServerGameContext::GetInstance();
                if ( !gameContext || !gameContext->m_serverPlayerManager || !gameContext->getLevel())
                    return Valid;

                auto* levelSetup = reinterpret_cast<fb::LevelSetup*>(uintptr_t( gameContext->getLevel() ) + 0x28);

                const char* mode = levelSetup->getInclusionOption( "GameMode" );
                const char* hostedMode = levelSetup->getInclusionOption( "HostedMode" );

                bool isAllowedMode =
                        ( strstr( mode, "Coop" ) ||
                            strstr( mode, "Ops0" ) ||
                            strstr( mode, "BossHunt" ) ) &&
                        strcmp( hostedMode, "LocalHosted" ) == 0;

                int idx = ptrread<int>( inMsg, 0x4C );
                if ( idx < 0 || idx >= gameContext->m_serverPlayerManager->m_players.size() )
                    return InvalidKick;

                fb::ServerPlayer* player = gameContext->m_serverPlayerManager->m_players[idx];

                //only allow player swapping on Ops and BossHunt
                if ( !isAllowedMode )
                    return InvalidKick;

                //the TargetPlayer must be an AI
                if ( player != nullptr && !player->isAIOrPersistentAIPlayer() )
                {
                    void* unk = inMsg->m_serverConnection->validateLocalPlayer( inMsg->GetLocalPlayerId(), false );

                    if ( !unk )
                    {
                        AC_LogMessage( LogLevel::Debug,
                                       "[{}] Couldn't validate LocalPlayer!",
                                       inMsg->getType()->getName() );
                        return InvalidKick;
                    }

                    serverPlayer = ptrread<fb::ServerPlayer*>( unk, 0xF8 );

                    AC_LogMessage( LogLevel::Info,
                                   "{} tried to swap players ({})",
                                   GetPlayerName( serverPlayer ),
                                   inMsg->getType()->getName() );

                    return InvalidKick;
                }

                return Valid;
            }
            break;
        case fnvHashConstexpr( "NetworkPlayerSelectedWeaponMessage" ):
            {
                void* unk = inMsg->m_serverConnection->validateLocalPlayer( inMsg->GetLocalPlayerId(), false );
                if ( !unk )
                {
                    AC_LogMessage( LogLevel::Debug,
                                   "[{}] Couldn't validate LocalPlayer!",
                                   inMsg->getType()->getName() );
                    return InvalidKick;
                }

                serverPlayer = ptrread<fb::ServerPlayer*>( unk, 0xF8 );

                auto* unlockAssetPtr = ptrread<fb::PVZCharacterWeaponUnlockAsset*>( inMsg, 0x50 );
                auto* upgrades = reinterpret_cast<fb::Array<void*>*>(reinterpret_cast<uint8_t*>(inMsg) + 0x58);

                if ( !unlockAssetPtr )
                {
                    *outReasonString = "Invalid data";
                    return InvalidKick;
                }

                bool isUpgradable = std::ranges::binary_search( LoadoutValidator::upgradableWeaponIds,
                                                                unlockAssetPtr->getIdentifier() );

                if ( !isUpgradable )
                    return Valid;

                int upgradeCount = upgrades->size();
                if ( upgradeCount > 8 || upgradeCount < 0 )
                {
                    *outReasonString = "Invalid data";
                    return InvalidKick;
                }

                for ( int i = 0; i < upgradeCount; i++ )
                {
                    void* upgradePtr = upgrades->at( i );
                    if ( !upgradePtr )
                    {
                        *outReasonString = "Invalid data";
                        return InvalidKick;
                    }
                }

                return Valid;
            }
            break;
        case fnvHashConstexpr( "ClientBuffApplyFromClientMessage" ):
        case fnvHashConstexpr( "ClientBuffKillFromClientMessage" ):
            {
                if ( !GetPreventClientBuffs() )
                    return Valid;

                const char* apply_or_kill = inMsg->m_type == fnvHashConstexpr( "ClientBuffApplyFromClientMessage" )
                                                ? "apply"
                                                : "kill";

                void* unk = inMsg->m_serverConnection->validateLocalPlayer( inMsg->GetLocalPlayerId(), false );

                if ( !unk )
                {
                    AC_LogMessage( LogLevel::Debug,
                                   "[{}] Couldn't validate LocalPlayer!",
                                   inMsg->getType()->getName() );
                    return InvalidKick;
                }

                auto* buffData = ptrread<fb::Asset*>(inMsg, 0x48);
                if (!buffData || !buffData->Name)
                {
                    *outReasonString = "Invalid buff";
                    return InvalidKick;
                }

                serverPlayer = ptrread<fb::ServerPlayer*>( unk, 0xF8 );

                AC_LogMessage( LogLevel::Warning,
                               "{} tried to {} a client buff ({})",
                               GetPlayerName( serverPlayer ),
                               apply_or_kill,
                               buffData->Name );

                return InvalidDiscard;
            }
            break;
    }
    return Valid;
}
#endif
