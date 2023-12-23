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

    Ref<VoxelBuffer>* water;
    Ref<VoxelBuffer>* terrain;

    int stable_levels = 0;

    // WaterDomain();
    
    WaterDomain(Vector3i origin, Ref<VoxelBuffer>* _water, Ref<VoxelBuffer>* _terrain);
    ~WaterDomain();

    std::array<float, 4> get_surr(const VoxelBuffer &water_read_buffer, int x, int y, int z, int channel) const;
    // void spread(const VoxelBuffer &water_read_buffer, int x, int y, int z, int channel);

    bool is_stable();

};

}

#endif
