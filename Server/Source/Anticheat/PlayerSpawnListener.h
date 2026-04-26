#pragma once
#include <fb/Engine/ServerPlayer.h>

DECLARE_HOOK(
	fb_PVZSpawnManager_spawnOnSpawnPoint,
	__fastcall,
	bool,

	fb::ServerPlayer* player,
	void* serverCharacterEntity
);