#pragma once
#include "ServerPlayerManager.h"
#include "ServerPeer.h"

#define OFFSET_SERVERGAMECONTEXT CYPRESS_GW_SELECT(0x141FC3180, 0x142B65680, 0x1445C3410)

namespace fb
{
    class ServerGameContext
    {
    public:
#ifdef CYPRESS_BFN
        char pad_0000[176];
#else
        char pad_0000[104]; //0x0000
#endif
        ServerPlayerManager* m_serverPlayerManager;
        ServerPeer* m_serverPeer;

        void* getLevel()
        {
            return ptrread<void*>(this, 0x30);
        }

        static ServerGameContext* GetInstance(void)
        {
            return *(ServerGameContext**)OFFSET_SERVERGAMECONTEXT;
        }
    }; //Size: 0x0080
}