#ifndef WATER_DOMAIN_H
#define WATER_DOMAIN_H

#include <godot_cpp/classes/voxel_generator_script.hpp>
#include <godot_cpp/classes/voxel_buffer.hpp>
#include <godot_cpp/classes/voxel_terrain.hpp>
#include <godot_cpp/classes/voxel_tool.hpp>

namespace godot {

class WaterDomain {
    // GDCLASS(WaterDomain, Object)

private:

protected:
    // static void _bind_methods();

public:

    Vector3i origin;
    Vector3i size;

    Ref<VoxelBuffer> water = Ref<VoxelBuffer>();
    Ref<VoxelBuffer> terrain = Ref<VoxelBuffer>();

    bool stable = false;

    WaterDomain();
    ~WaterDomain();
    
    WaterDomain(Vector3i origin, const Ref<VoxelBuffer>& _water, const Ref<VoxelBuffer>& _terrain);



};

}

#endif
