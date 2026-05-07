#pragma once

#define OFFSET_FB_REGISTERMESSAGELISTENER CYPRESS_GW_SELECT(0x14035C100, 0x1401A1450, 0x1404702B0)
#define OFFSET_FB_MESSAGEMANAGER_QUEUEMESSAGE CYPRESS_GW_SELECT(0x14035BA40, 0x1401A13A0, 0)
namespace fb
{
    class MessageListener;

    class MessageManager
    {
    public:
        bool registerMessageListener(int category, MessageListener* listener, int localPlayerId = 255);
        void queueMessage(Message* msg, float delay = 0.0f);

    private:
        class MessageManagerImpl* m_impl;
        bool m_implOwner;
    };

    class MessageManagerImpl
    {
    public:

    };
}