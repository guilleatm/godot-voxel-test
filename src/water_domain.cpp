#include "water_domain.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;


WaterDomain::WaterDomain() {

}

WaterDomain::~WaterDomain() {

}

WaterDomain::WaterDomain(Vector3i _origin, const Ref<VoxelBuffer>& _water, const Ref<VoxelBuffer>& _terrain) {
    origin = _origin;
    size = _water.ptr()->get_size();
    water = _water;
    terrain = _terrain;
}

// void WaterDomain::_bind_methods() {
// }
