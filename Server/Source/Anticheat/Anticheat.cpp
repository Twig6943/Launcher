#include "pch.h"
#ifdef CYPRESS_GW2
#include "fb/Engine/ConsoleContext.h"
#include "Anticheat.h"

#include "LoadoutValidator.h"
#include "fb/Engine/SettingsManager.h"

void Anticheat::AnticheatVerbose(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	getInstance().SetVerbose(enable);
	cc.push("{} Verbose", enable ? "Enabled" : "Disabled");
}
void Anticheat::AnticheatEnabled(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	void* gmds = fb::SettingsManager::GetInstance()->getContainer<void*>("GameMode");

	cc.push("GameMode: {}", gmds);

	getInstance().SetEnabled(enable);
	cc.push("{} Anticheat", enable ? "Enabled" : "Disabled");
}
void Anticheat::AnticheatPreventClientBuffs(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	getInstance().SetPreventClientBuffs(enable);
	cc.push("{} ClientBuff protection!", enable ? "Enabled" : "Disabled");
}
void Anticheat::AnticheatPreventBlacklistedEventSyncs(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	getInstance().SetPreventBlacklistedEventSyncs(enable);
	cc.push("{} EventSync protection!", enable ? "Enabled" : "Disabled");
}
void Anticheat::AnticheatPreventInvalidLoadouts(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	getInstance().SetPreventInvalidLoadouts(enable);
	cc.push("{} Loadout protection!", enable ? "Enabled" : "Disabled");
}
void Anticheat::AnticheatPreventPlayerSwap(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	getInstance().SetPreventPlayerSwap(enable);
	cc.push("{} PlayerSwap protection!", enable ? "Enabled" : "Disabled");
}

void Anticheat::AnticheatPreventAliveWeaponChange(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	getInstance().SetPreventAliveWeaponChange(enable);
	cc.push("{} AliveWeaponChange protection!", enable ? "Enabled" : "Disabled");
}

void Anticheat::AnticheatPreventSelfRevive(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	getInstance().SetPreventSelfRevive(enable);
	cc.push("{} SelfRevive protection!", enable ? "Enabled" : "Disabled");
}

void Anticheat::AnticheatPreventServerCrash(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	getInstance().SetPreventServerCrash(enable);
	cc.push("{} ServerCrash protection!", enable ? "Enabled" : "Disabled");
}

void Anticheat::AnticheatPreventClientLevelLoading(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	getInstance().SetPreventClientLevelLoading(enable);
	cc.push("{} ClientLevelLoading protection!", enable ? "Enabled" : "Disabled");
}

void Anticheat::AnticheatPreventSyncSettingsFromClients(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	bool enable;

	stream >> enable;

	getInstance().SetPreventSyncSettingsFromClients(enable);
	cc.push("{} PreventSyncSettingsFromClients protection!", enable ? "Enabled" : "Disabled");
}

void Anticheat::AnticheatReloadWeaponSets(fb::ConsoleContext& cc) {
	LoadoutValidator::getInstance().init();
	cc.push("WeaponSets reloaded");
}

void Anticheat::AnticheatClearKitBlacklist(fb::ConsoleContext& cc) {

	LoadoutValidator::getInstance().kitBlacklist.clear();
	cc.push("Blacklisted Kits vector cleared");
}

void Anticheat::AnticheatPrintBlacklistedKits(fb::ConsoleContext& cc) {

	cc.push("Blacklisted Kits vector:");

	int count = 0;

	for (auto& kit : LoadoutValidator::getInstance().kitBlacklist) {
		cc.push("[{}] {}", count, kit.c_str());
		count++;
	}
}

void Anticheat::AnticheatAddKitToBlacklist(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	std::string kit;

	stream >> kit;

	LoadoutValidator::getInstance().kitBlacklist.insert(kit);

	cc.push("Added {} to kit blacklist", kit);
}

void Anticheat::AnticheatRemoveKitFromBlacklist(fb::ConsoleContext& cc) {
	auto stream = cc.stream();
	std::string kit;

	stream >> kit;

	LoadoutValidator::getInstance().kitBlacklist.erase(kit);

	cc.push("Removed {} to kit blacklist", kit);
}
#endif