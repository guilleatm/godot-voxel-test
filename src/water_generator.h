 
#ifndef WATER_GENERATOR_H
#define WATER_GENERATOR_H

#include <godot_cpp/classes/voxel_generator_script.hpp>
#include <godot_cpp/classes/voxel_buffer.hpp>

namespace godot {

class WaterGenerator : public VoxelGeneratorScript {
	GDCLASS(WaterGenerator, VoxelGeneratorScript)

private:


protected:
	static void _bind_methods();

public:
	WaterGenerator();
	~WaterGenerator();


	void _generate_block(VoxelBuffer out_buffer, Vector3i origin_in_voxels, int lod) override;

	int _get_used_channels_mask() const override;

};

}

#endif
