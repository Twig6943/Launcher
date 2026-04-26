#include "pch.h"

// WIP
// Removed for now! will be reimplemented later.

/*
Notes for future implementation / anyone improving this:

The original idea was to hook two functions:

1. DispatchMessage, or any other to intercept ServerCharacterCharacterDamageMessage
   - This message contains a ServerDamageInfo struct
   - It provides the total damage dealt by the player

2. fb::ServerBulletEntity::handleCollision
   - Called when a bullet collides with something
   - Gives access to the ServerBullet
   - From there, we can call fb::BulletEntity::calculateDamage
     to compute the expected damage for that bullet

With these two hooks, we can compare:
- Actual damage dealt (from the message)
- Expected damage (from bullet calculation)

Problem:
These hooks are completely separate, and I couldn't find a reliable
way to synchronize them (e.g. matching bullet → damage event).

*/