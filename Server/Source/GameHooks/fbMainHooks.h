#pragma once

#include <fb/Main.h>
#ifdef CYPRESS_BFN
#include <fb/Engine/ScriptContext.h>
#endif

#define OFFSET_ENVIRONMENT_GETHOSTIDENTIFIER CYPRESS_GW_SELECT(0x1403A9170, 0x1401F7C00, 0x14047D6F0)

#define OFFSET_CONSOLE__WRITECONSOLE CYPRESS_GW_SELECT(0x140392720, 0x1401A7850, 0x1404622D0)
#ifndef CYPRESS_BFN
#define OFFSET_CONSOLE__ENQUEUECOMMAND CYPRESS_GW_SELECT(0x14038FA50, 0x1401A7680, 0)
#endif

#ifndef CYPRESS_BFN
#define OFFSET_FB_ISINTERACTIONALLOWED 0x140B8F820
#define OFFSET_FB_TICKERSALLOWEDTOSHOW 0x140E0A8F0
#endif

#define OFFSET_FB_MAIN_INITSETTINGS CYPRESS_GW_SELECT(0x14000D030, 0, 0x142218250)
#ifndef CYPRESS_BFN
#define OFFSET_BFN_FB_LUAOPTAPPLYSETTINGS 0
#else
#define OFFSET_BFN_FB_LUAOPTAPPLYSETTINGS 0x140F07F60
#endif

DECLARE_HOOK(
	fb_Main_initSettings,
	__fastcall,
	void,

	void* thisPtr
);

DECLARE_HOOK(
	fb_Environment_getHostIdentifier,
	__cdecl,
	const char*
);

DECLARE_HOOK(
	fb_Console_writeConsole,
	__cdecl,
	void,

	const char* tag,
	const char* buffer,
	unsigned int size
);

#ifdef CYPRESS_BFN
DECLARE_HOOK(
	fb_realMain,
	__cdecl,
	__int64,

	HINSTANCE hIntance,
	HINSTANCE hPrevInstance,
	const char* lpCmdLine,
	__int64 a4,
	__int64 a5
);

DECLARE_HOOK(
	fb_main_createWindow,
	__fastcall,
	void,

	void* thisPtr
);

DECLARE_HOOK(
	fb_luaApplySettings,
	__cdecl,
	void,

	uintptr_t luaOptionsManager,
	fb::ScriptContext* scontext,
	bool inmediate
);
#else
DECLARE_HOOK(
	fb_realMain,
	__cdecl,
	__int64,

	HINSTANCE hIntance,
	HINSTANCE hPrevInstance,
	const char* lpCmdLine
);

DECLARE_HOOK(
	fb_PVZMain_Ctor,
	__fastcall,
	__int64,

	void* a1,
	bool isDedicatedServer
);

DECLARE_HOOK(
	fb_getServerBackendType,
	__fastcall,
	int,

	void* a1
);

DECLARE_HOOK(
	fb_main_createWindow,
	__fastcall,
	__int64,

	HINSTANCE hInstance,
	bool dedicatedServer,
	bool consoleClient,
	DWORD gameLoopThreadId
);

DECLARE_HOOK(
	fb_isInteractionAllowed,
	__fastcall,
	bool,

	void* a1,
	unsigned int a2
);

DECLARE_HOOK(
	fb_tickersAllowedToShow,
	__fastcall,
	bool,

	void* a1
);
#endif