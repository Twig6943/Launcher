#include <pch.h>
#include <Cypress/Presence/PresenceManager.h>
#include <fb/Engine/ClientGameContext.h>
#include <fb/Engine/Message.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <External/EA_GW/EASTL/include/EASTL/hash_map.h>

namespace Cypress
{
    PresenceManager* g_presenceManager = nullptr;
    nlohmann::json g_saveFile;

    PresenceManager::PresenceManager()
    {
        fb::ClientGameContext* clientCtx = fb::ClientGameContext::GetInstance();
        CYPRESS_ASSERT( clientCtx != nullptr, "Trying to create presence manager on the server?");
        CYPRESS_ASSERT( g_presenceManager == nullptr, "Tried to create PresenceManager twice!" );

        clientCtx->m_messageManager.registerMessageListener(fnvHashConstexpr( "Presence" ), this);

        InitializePresenceState();

        std::ifstream saveIn("cypsave.json");
        if (saveIn.is_open())
            g_saveFile = nlohmann::json::parse( saveIn );
        saveIn.close();
    }

    void PresenceManager::Initialize()
    {
#ifdef CYPRESS_GW1
        g_presenceManager = new PresenceManager();
        CYPRESS_LOGMESSAGE( LogLevel::Info, "Initialized PresenceManager" );
#else
        CYPRESS_LOGMESSAGE( LogLevel::Warning, "Presence has not been implemented for this game yet" );
#endif
    }

    void PresenceManager::InitializePresenceState()
    {
        void* presenceState = *(void**)OFFSET_FB_G_PRESENCESTATE;
        CYPRESS_ASSERT( presenceState != nullptr, "Trying to create presence manager before engine's presence state has been created" );

        // change presencestate's state to Online so we get bytevault update requests from our client
#ifdef CYPRESS_GW1
        ptrset<int>(presenceState, 0xC4, 2);
#elif CYPRESS_GW2
        uint8_t* raw = (uint8_t*)presenceState;

        uint64_t base = *(uint64_t*)(raw + 0xDB9 * sizeof(uint64_t));

        // 0 = local player id
        uint32_t* statePtr = (uint32_t*)(base + 0xED8ull * 0 + 4);

        *statePtr = 2;
#else
#endif
    }

    void PresenceManager::onMessage( fb::Message& inMessage )
    {
        CYPRESS_LOGMESSAGE( LogLevel::Debug, "Got presence message: {}", inMessage.getType()->getName() );
        switch (inMessage.m_type)
        {
            case fnvHashConstexpr( "PresencePVZGetByteVaultSubRecordMessageBase" ):
                {
                    auto& requestMsg = reinterpret_cast<fb::PresencePVZGetByteVaultSubRecordMessageBase&>(inMessage);
                    CYPRESS_DEBUG_LOGMESSAGE(LogLevel::Debug, "Requesting save category: {}", requestMsg.subcategory.c_str());

                    auto categoryIt = g_saveFile.find( requestMsg.subcategory.c_str() );
                    if (categoryIt != g_saveFile.end() && categoryIt->is_object())
                    {
                        fb::PVZRecordMap records;
                        for (auto& [key, entry] : categoryIt->items())
                        {
                            auto it = records.insert( key.c_str() );
                            auto& jvalue = it.first->second;

                            int type = entry["t"].get<int>();
                            jvalue.Type = (fb::PVZRecordInfoType)type;

                            switch (type)
                            {
                                case fb::Bool:    jvalue.SetBool(entry["v"].get<bool>()); break;
                                case fb::Integer: jvalue.Int = entry["v"].get<int>(); break;
                                case fb::Float:   jvalue.Float = entry["v"].get<float>(); break;
                                default: CYPRESS_ASSERT( false, "Unknown json type" );
                            }

                            jvalue.UnkBool = true;
                        }

                        auto* clientCtx = fb::ClientGameContext::GetInstance();

#ifdef CYPRESS_GW1
                        fb::Message* resultMsg = CallFunc<fb::Message*, const char*, void*>(
                            0x140BF1C50,
                        requestMsg.subcategory.c_str(), &records
                        );

                        clientCtx->m_messageManager.queueMessage( resultMsg );
#else
                        fb::PresencePVZGetByteVaultSubRecordResultMessageBase resultMsg(requestMsg.subcategory, records);
                        ptrset<uintptr_t>(&resultMsg, 0x0, 0x14230FB48);
                        resultMsg.m_localPlayerId = 0;
#endif
                    }

                    break;
                }
            case fnvHashConstexpr( "PresencePVZUpdateByteVaultRecordMessageBase" ):
                {
                    auto& updateMsg = reinterpret_cast<fb::PresencePVZUpdateByteVaultRecordMessage&>(inMessage);
                    CYPRESS_DEBUG_LOGMESSAGE(LogLevel::Debug, "Updating save category: {}", updateMsg.subcategory.c_str());

                    auto& subcat = g_saveFile[updateMsg.subcategory.c_str()];
                    for (const auto& it : updateMsg.records)
                    {
                        int valueType = it.second.Type;

                        subcat[it.first.c_str()]["t"] = valueType;
                        switch (it.second.Type)
                        {
                            case fb::Bool:    subcat[it.first.c_str()]["v"] = it.second.GetBool(); break;
                            case fb::Integer: subcat[it.first.c_str()]["v"] = it.second.Int; break;
                            case fb::Float:   subcat[it.first.c_str()]["v"] = it.second.Float; break;
                            default: CYPRESS_ASSERT(false, "Unknown value type"); break;
                        }
                    }

                    std::ofstream saveout("cypsave.json");
                    saveout << g_saveFile.dump(2);
                    saveout.close();

                    break;
                }
        }
    }
}
