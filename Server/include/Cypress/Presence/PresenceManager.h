#pragma once
#include <EASTL/fixed_hash_map.h>
#include <EASTL/fixed_map.h>
#include <fb/Engine/MessageListener.h>
#include <EASTL/map.h>
#include <EASTL/string.h>
#include <External/EA_GW/EASTL/include/EASTL/fixed_list.h>
#include <fb/Engine/Message.h>

#define OFFSET_FB_G_PRESENCESTATE CYPRESS_GW_SELECT( 0x142044038, 0x142B69630, 0 )

namespace fb
{
    enum PVZRecordInfoType
    {
        Bool = 0,
        Integer = 2,
        Float = 3,
        Unknown = 4
    };

#ifdef CYPRESS_GW1
    struct PVZRecordInfo
    {
        eastl::string String;
        float Float;
        int Int;
        PVZRecordInfoType Type;
        bool UnkBool;
        bool Unk5;
        char pad[24];

        PVZRecordInfo()
            : Float(0.0f)
            , Int(0)
            , Type(Unknown)
            , UnkBool(false)
            , Unk5(false)
            , pad()
        {}

        bool GetBool() const
        {
            return Int;
        }

        void SetBool(bool value)
        {
            Int = value;
        }
    };
    static_assert( sizeof( eastl::pair<eastl::string, PVZRecordInfo> ) == 0x68 );
#else
    struct PVZRecordInfo
    {
        eastl::string String;
        union
        {
            bool Bool;
            float Float;
            int Int;
        };
        PVZRecordInfoType Type;
        bool UnkBool;

        PVZRecordInfo()
            : Int(0)
            , Type(Unknown)
            , UnkBool(false)
        {}

        bool GetBool() const
        {
            return Bool;
        }

        void SetBool(bool value)
        {
            Bool = value;
        }
    };
#endif

    using PVZRecordMap = eastl::map<eastl::string, PVZRecordInfo>;
    using PVZRecordMapRef = eastl::map<eastl::string, PVZRecordInfo&>&;

    struct PresencePVZUpdateByteVaultRecordMessage : public Message
    {
        eastl::string subcategory;
#ifdef CYPRESS_GW1
        eastl::map<eastl::string, PVZRecordInfo> records;
#else
        PVZRecordMapRef records;
#endif
    };

    struct PresencePVZGetByteVaultSubRecordMessageBase : public Message
    {
        eastl::string subcategory;
    };

    struct PresencePVZGetByteVaultSubRecordResultMessageBase : public Message
    {
        PresencePVZGetByteVaultSubRecordResultMessageBase( const eastl::string& subcategory, PVZRecordMap& recordResults)
            : Message( 0x9C775C5C, 0xF98CE8B8 )
            , subcategory(subcategory)
            , records(recordResults)
        {}

        ~PresencePVZGetByteVaultSubRecordResultMessageBase() {}
        ClassInfo* getType() override {return nullptr;}

        eastl::string subcategory;
#ifdef CYPRESS_GW1
        eastl::map<eastl::string, PVZRecordInfo> records;
#else
        PVZRecordMap& records;
#endif
    };
}

namespace Cypress
{
    class PresenceManager : public fb::MessageListener
    {
    public:
        static void Initialize();

    private:
        PresenceManager();

        void InitializePresenceState();
        void onMessage(fb::Message& inMessage) override;
    };

    extern PresenceManager* g_presenceManager;
}