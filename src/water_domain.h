#ifndef WATER_DOMAIN_H
#define WATER_DOMAIN_H

#include <godot_cpp/classes/voxel_generator_script.hpp>
#include <godot_cpp/classes/voxel_buffer.hpp>
#include <godot_cpp/classes/voxel_terrain.hpp>
#include <godot_cpp/classes/voxel_tool.hpp>
#include "global.h"
#include <godot_cpp/core/math.hpp>

namespace godot {

class WaterDomain {
    // GDCLASS(WaterDomain, Object)

private:

    int stable_levels = 0;

    Ref<VoxelBuffer> read_water_buffer;
    Ref<VoxelBuffer> read_terrain_buffer;


protected:
    // static void _bind_methods();

public:

    Vector3i origin;
    Vector3i size;

    Ref<VoxelBuffer> out_water_buffer;
    
    WaterDomain(Vector3i _origin, Vector3i _size, const Ref<VoxelTool> &water_tool, const Ref<VoxelTool> &terrain_tool);
    ~WaterDomain();

    std::array<float, 4> get_surr(const VoxelBuffer &water_read_buffer, int x, int y, int z, int channel) const;
    // void spread(const VoxelBuffer &water_read_buffer, int x, int y, int z, int channel);


    // GOOD

    bool is_stable();
    void update();




};

}

#endif
