#ifndef WEAPONSLOT_HPP
#define WEAPONSLOT_HPP

namespace fb
{
	enum WeaponSlot
	{
		WeaponSlot_0 = 0x0,
		WeaponSlot_1 = 0x1,
		WeaponSlot_2 = 0x2,
		WeaponSlot_3 = 0x3,
		WeaponSlot_4 = 0x4,
		WeaponSlot_5 = 0x5,
		WeaponSlot_6 = 0x6,
		WeaponSlot_7 = 0x7,
		WeaponSlot_8 = 0x8,
		WeaponSlot_9 = 0x9,
		WeaponSlot_NumSlots = 0xA,
		WeaponSlot_NotDefined = 0xB
	};

	static const char* toString(fb::WeaponSlot slot) {
		switch (slot) {
		case WeaponSlot_0: return "WeaponSlot_0";
		case WeaponSlot_1: return "WeaponSlot_1";
		case WeaponSlot_2: return "WeaponSlot_2";
		case WeaponSlot_3: return "WeaponSlot_3";
		case WeaponSlot_4: return "WeaponSlot_4";
		case WeaponSlot_5: return "WeaponSlot_5";
		case WeaponSlot_6: return "WeaponSlot_6";
		case WeaponSlot_7: return "WeaponSlot_7";
		case WeaponSlot_8: return "WeaponSlot_8";
		case WeaponSlot_9: return "WeaponSlot_9";
		case WeaponSlot_NumSlots: return "WeaponSlot_NumSlots";
		case WeaponSlot_NotDefined: return "WeaponSlot_NotDefined";
		default: return "Unknown";
		}
	}
} //namespace fb

#endif