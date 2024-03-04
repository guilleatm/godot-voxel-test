#include "water_generator.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

WaterGenerator::WaterGenerator() {
	
	channel = VoxelBuffer::CHANNEL_SDF;
	channel_mask = _get_used_channels_mask();
}

WaterGenerator::~WaterGenerator() {

}


void WaterGenerator::m_create(VoxelTerrain* terrain) {

	ERR_FAIL_COND(terrain == nullptr);
	terrain_tool = terrain->get_voxel_tool();
}

void WaterGenerator::_generate_block(const Ref<VoxelBuffer> &out_buffer, const Vector3i &origin_in_voxels, int lod) {

	if (lod != 0) return;
	
	if (origin_in_voxels.y > 0) return;

	out_buffer->set_channel_depth(CH_WATER, godot::VoxelBuffer::Depth::DEPTH_8_BIT);

	Vector3i size = out_buffer->get_size();

	// We create a VoxelBuffer each time to avoid using mutex.

	VoxelBuffer terrain_buffer = VoxelBuffer();
	terrain_buffer.create(size.x, size.y, size.z);

	Ref<VoxelBuffer> ref_terrain_buffer = Ref<VoxelBuffer>(&terrain_buffer);

	terrain_tool.ptr()->copy(origin_in_voxels, ref_terrain_buffer, channel_mask);
	
	for (int y = 0; y < size.y; y++) {
		if (origin_in_voxels.y + y > 0) break;
		if (origin_in_voxels.y + y < -15) break;

		for (int x = 0; x < size.x; x++) {
			for (int z = 0; z < size.z; z++) {
				float terrain_voxel = terrain_buffer.get_voxel_f(x, y, z, channel);
				
				if (terrain_voxel > 0) {
					out_buffer->set_voxel_f(-1.0, x, y, z, channel);
					out_buffer->set_voxel(0, x, y, z);
				} else {
					out_buffer->set_voxel_f(1.0, x, y, z, channel);
					out_buffer->set_voxel(1, x, y, z);
				}
			}
		}
	}
}

int WaterGenerator::_get_used_channels_mask() const {
	return 1 << channel;
}


void WaterGenerator::_bind_methods() {

	ClassDB::bind_method(D_METHOD("create", "terrain"), &WaterGenerator::m_create);
}