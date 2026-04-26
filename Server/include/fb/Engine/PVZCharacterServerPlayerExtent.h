#pragma once

namespace fb
{
	class Asset;
	class PVZCharacterServerPlayerExtent
	{
	public:
		char pad_0000[1944]; //0x0000
		fb::Asset* m_primary; //0x0798
		fb::Asset* m_upgrade[8]; //0x07A0
		uint64_t m_numUpgrades; //0x07E0
		char pad_07E8[16]; //0x07E8
		fb::Asset* m_ability1; //0x07F8
		char pad_0800[88]; //0x0800
		fb::Asset* m_ability2; //0x0858
		char pad_0860[88]; //0x0860
		fb::Asset* m_ability3; //0x08B8
		char pad_08C0[88]; //0x08C0
		fb::Asset* m_alternate; //0x0918
		char pad_0920[1848]; //0x0920
	}; //Size: 0x1058
}