#ifndef WATER_DOMAIN_H
#define WATER_DOMAIN_H

#include <godot_cpp/classes/voxel_tool.hpp>

namespace godot
{

    class VoxelTool;
    class VoxelBuffer;
    class AABB;
    class WaterSimulation;


    class WaterDomain
    {

    private:

        ~WaterDomain() = default;

        const Ref<VoxelTool>& water_tool;
        const Ref<VoxelTool>& terrain_tool;

        Ref<VoxelBuffer> water_buffer;
        Ref<VoxelBuffer> terrain_buffer;

        AABB aabb;

        void copy_to_buffers();
        void paste_from_buffers();

    public:

        WaterDomain(Vector3i _origin, Vector3i _size, const Ref<VoxelTool> &water_tool, const Ref<VoxelTool> &terrain_tool);

        void update();

        // bool is_stable();
        // void update();
        // void update_sdf();

        // bool is_inside_bounds(int x, int y, int z) const;


        friend WaterSimulation;
    };

}

#endif