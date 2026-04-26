#pragma once

namespace fb
{
	class EntityBus
	{
	public:
		void* m_subLevel; //0x00
		void* m_parentBus; //0x08
		char pad_0010[72]; //0x10
		void* m_data;
	};
}