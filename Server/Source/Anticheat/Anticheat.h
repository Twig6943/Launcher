#pragma once
#include <format>
#include <string>

//simple macro to add setter getter and bool
#define AC_ADD_FEATURE(name, defaultValue) \
private: \
    bool name = defaultValue; \
public: \
    bool Get##name() const { return name; } \
    void Set##name(bool value) { name = value; }

class Anticheat
{
public:
	static Anticheat& getInstance()
	{
		static Anticheat anticheat_instance;
		return anticheat_instance;
	}

	Anticheat(const Anticheat&) = delete;
	Anticheat& operator=(const Anticheat&) = delete;

	//enable only when necessary, useful for debugging but kills performance
	AC_ADD_FEATURE(Verbose, false);

	AC_ADD_FEATURE(Enabled, true);
	AC_ADD_FEATURE(PreventClientBuffs, true);
	AC_ADD_FEATURE(PreventBlacklistedEventSyncs, true);
	AC_ADD_FEATURE(PreventInvalidLoadouts, true);
	AC_ADD_FEATURE(PreventPlayerSwap, true);
	AC_ADD_FEATURE(PreventAliveWeaponChange, true);
	AC_ADD_FEATURE(PreventSelfRevive, true);
	AC_ADD_FEATURE(PreventServerCrash, true);
	AC_ADD_FEATURE(PreventClientLevelLoading, true);
	AC_ADD_FEATURE(PreventSyncSettingsFromClients, true);

	//commands
	static void AnticheatVerbose(fb::ConsoleContext& cc);
	static void AnticheatEnabled(fb::ConsoleContext& cc);

	static void AnticheatPreventClientBuffs(fb::ConsoleContext& cc);
	static void AnticheatPreventBlacklistedEventSyncs(fb::ConsoleContext& cc);
	static void AnticheatPreventInvalidLoadouts(fb::ConsoleContext& cc);
	static void AnticheatPreventPlayerSwap(fb::ConsoleContext& cc);
	static void AnticheatPreventAliveWeaponChange(fb::ConsoleContext& cc);
	static void AnticheatPreventSelfRevive(fb::ConsoleContext& cc);
	static void AnticheatPreventServerCrash(fb::ConsoleContext& cc);
	static void AnticheatPreventClientLevelLoading(fb::ConsoleContext& cc);
	static void AnticheatPreventSyncSettingsFromClients(fb::ConsoleContext& cc);

	static void AnticheatReloadWeaponSets(fb::ConsoleContext& cc);
	static void AnticheatClearKitBlacklist(fb::ConsoleContext& cc);
	static void AnticheatPrintBlacklistedKits(fb::ConsoleContext& cc);
	static void AnticheatAddKitToBlacklist(fb::ConsoleContext& cc);
	static void AnticheatRemoveKitFromBlacklist(fb::ConsoleContext& cc);

	template<typename... Args>
	void AC_LogMessage(LogLevel level, std::format_string<Args...> fmt, Args&&... args)
	{
		if (!Verbose) return;

		auto str = std::format(fmt, std::forward<Args>(args)...);
		CYPRESS_LOGTOSERVER(level, "{}",str.c_str());
		CYPRESS_LOGMESSAGE(level, "{}",str.c_str());
	}

private:
	Anticheat() = default;
};