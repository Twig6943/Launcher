#pragma once

namespace fb
{
	struct ObjectExtentRegistration
	{
	public:
		unsigned int m_offset; //0x0000
		unsigned int m_extentSize; //0x0004
		unsigned int m_alignment; //0x0008
		char pad_000C[4]; //0x000C
		char* m_name; //0x0010
		void* constructor; //0x0018
		void* destructor; //0x0020
	};
}