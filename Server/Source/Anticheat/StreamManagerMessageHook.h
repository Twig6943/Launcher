#pragma once
#include "fb/Engine/NetworkableMessage.h"

DECLARE_HOOK(
	fb_network_StreamManagerMessage_addMessagePart,
	__fastcall,
	fb::NetworkableMessage*,

	__int64 a1,
	fb::NetworkableMessage* msg,
	__int64 a3
);
