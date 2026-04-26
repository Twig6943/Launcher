#pragma once

#include <MemUtil.h>
#include <unordered_set>

#include "fb/Engine/Message.h"

DECLARE_HOOK(
	fb_ServerSettingEntity_onMessage,
	__fastcall,
	void,

	void* thisPtr, 
	fb::Message* message,
	__int64 a3, //unused
	__int64 a4, //unused
	int a5      //unused
);

inline std::unordered_set<std::string> g_settingBlacklist = {
	"GameMode.CrazyOption1",
	"GameMode.CrazyOption2",
	"GameMode.CrazyOption3",
	"GameMode.CrazyOption4",
	"GameMode.CrazyOption5",
	"GameMode.CrazyOption6",
	"GameMode.CrazyOption7",
	"GameMode.CrazyOption8",
	"GameMode.UnlimitedPrimaryAmmo",
	"GameMode.Force1HpCharacterHealth"
};
