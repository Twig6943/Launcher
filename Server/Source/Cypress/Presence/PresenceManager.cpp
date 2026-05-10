#include <pch.h>
#include <Cypress/Presence/PresenceManager.h>
#include <fb/Engine/ClientGameContext.h>
#include <fb/Engine/Message.h>
#include <nlohmann/json.hpp>
#include <fstream>
#ifdef CYPRESS_GW1
#include <Cypress/Presence/GW1PresenceManager.h>
#elif CYPRESS_GW2
#include <Cypress/Presence/GW2PresenceManager.h>
#endif
#include <External/EA_GW/EASTL/include/EASTL/hash_map.h>

namespace Cypress
{
    PresenceManager* g_presenceManager = nullptr;

    void PresenceManager::Create()
    {
#ifdef CYPRESS_BFN
        CYPRESS_LOGMESSAGE( LogLevel::Warning, "Presence has not been implemented for this game yet" );
        return;
#endif
        if (g_presenceManager)
            return;

        fb::ClientGameContext* clientCtx = fb::ClientGameContext::GetInstance();
        CYPRESS_ASSERT( clientCtx != nullptr, "Trying to create presence manager on the server?");

#ifdef CYPRESS_GW1
        g_presenceManager = new GW1PresenceManager();
#elif CYPRESS_GW2
        g_presenceManager = new GW2PresenceManager();
#endif

        if (g_presenceManager)
            g_presenceManager->Initialize();
    }

    bool PresenceManager::LoadSaveFile()
    {
        bool loaded = false;

        std::ifstream saveIn("cypsave.json");
        if (saveIn.is_open())
        {
            m_saveFile = nlohmann::json::parse( saveIn );
            loaded = true;
        }
        saveIn.close();

        return loaded;
    }
}
