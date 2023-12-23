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

// void WaterDomain::_bind_methods() {
// }
