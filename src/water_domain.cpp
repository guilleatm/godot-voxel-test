#include "water_domain.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;


// WaterDomain::WaterDomain() {

// }


WaterDomain::WaterDomain(Vector3i _origin, Ref<VoxelBuffer>* _water, Ref<VoxelBuffer>* _terrain) {
    origin = _origin;
    size = _water->ptr()->get_size();
    water = _water;
    terrain = _terrain;
}

WaterDomain::~WaterDomain() {

    water->~Ref();
    terrain->~Ref();

    // delete water->ptr();
    // delete water;

    // delete terrain->ptr();
    // delete terrain;
}


//		1
//	2 - X - 0
//		3
std::array<float, 4> WaterDomain::get_surr(const VoxelBuffer &water_read_buffer, int x, int y, int z, int channel) const {
    
    std::array<float, 4> _surr;

    _surr[0] = water_read_buffer.get_voxel_f(x + 1, y, z, channel);
    _surr[1] = water_read_buffer.get_voxel_f(x, y, z + 1, channel);
    _surr[2] = water_read_buffer.get_voxel_f(x - 1, y, z, channel);
    _surr[3] = water_read_buffer.get_voxel_f(x, y, z - 1, channel);

    return _surr;
}


// void WaterDomain::spread(const VoxelBuffer &water_read_buffer, int x, int y, int z, int channel) {

// 	const float SPREAD_AMOUNT = 0.2;

// 	float water_voxel = water->ptr()->get_voxel_f(x, y, z, channel);
	
// 	std::array<float, 4> surr = get_surr(water_read_buffer, x, y, z, channel);

// 	water->ptr()->set_voxel_f((water_voxel - surr[0]) * SPREAD_AMOUNT, x + 1, y, z, channel);
// 	water->ptr()->set_voxel_f((water_voxel - surr[1]) * SPREAD_AMOUNT, x, y, z + 1, channel);
// 	water->ptr()->set_voxel_f((water_voxel - surr[2]) * SPREAD_AMOUNT, x - 1, y, z, channel);
// 	water->ptr()->set_voxel_f((water_voxel - surr[3]) * SPREAD_AMOUNT, x, y, z - 1, channel);
// }

bool WaterDomain::is_stable() {
	return stable_levels >= size.y - 2;
}