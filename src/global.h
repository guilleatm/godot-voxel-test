#ifndef GLOBAL_H
#define GLOBAL_H

#include <godot_cpp/classes/voxel_buffer.hpp>
#include <godot_cpp/variant/utility_functions.hpp>



namespace godot
{
	#define DO_NOT_PLAY_IN_EDITOR if(Engine::get_singleton()->is_editor_hint()) return
	#define PRINT(message) UtilityFunctions::print(message)

	const int CH_WATER = VoxelBuffer::CHANNEL_DATA5;
	const int CH_SDF = VoxelBuffer::CHANNEL_SDF;

}

#endif // GLOBAL_H