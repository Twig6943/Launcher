#pragma once
#include "fb/Engine/Message.h"

DECLARE_HOOK(
	fb_PVZServerLevelManager_onMessage,
	__fastcall,
	void,

	void* thisPtr,
	fb::Message* message
);