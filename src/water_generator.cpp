#include "water_generator.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void WaterGenerator::_bind_methods() {
}

WaterGenerator::WaterGenerator() {

}

WaterGenerator::~WaterGenerator() {

}

void WaterGenerator::_generate_block(const Ref<VoxelBuffer> &out_buffer, const Vector3i &origin_in_voxels, int lod) {

}

int WaterGenerator::_get_used_channels_mask() const {
	return 1 << VoxelBuffer::CHANNEL_SDF;
}