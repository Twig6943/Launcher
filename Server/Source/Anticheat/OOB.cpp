#include "pch.h"

// WIP
// Removed for now! will be reimplemented later.

/*
Notes for future implementation / anyone improving this:

Initial idea:
- Scale up (expand) the boundary OBBs to prevent players from leaving the playable area.
- This approach is not sufficient, as many exploits do not require actually crossing
  the main boundary.

Current idea:
- Manually patch each known out-of-bounds exploit.
- This involves placing invisible blockers or performing position checks
  in specific problematic areas to detect when a player leaves valid bounds.

Issues:
- Highly manual and map-specific
- Easy to miss new or unknown exploit paths
- It's a pain in the ass
*/