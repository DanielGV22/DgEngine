#pragma once


namespace DgEngine
{
	class GameObjectHandle
	{
	public:
		GameObjectHandle() = default;
	private:
		friend class GameWorld;
		int mIndex = -1; // is going to be the index of the slot in the game world
		int mGeneration = -1; // to verify if the item is already tagged for deletion/removed

	};
}