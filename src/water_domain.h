#ifndef WATER_DOMAIN_H
#define WATER_DOMAIN_H

#include <godot_cpp/classes/voxel_tool.hpp>

namespace godot
{

    class WaterDomain
    {

    private:

        int stable_levels = 0;

        Ref<VoxelBuffer> terrain_buffer_ptr;
        Ref<VoxelBuffer> write_buffer_ptr;

        void prepare_water_buffer();

    protected:

    public:

        Ref<VoxelBuffer> read_buffer_ptr;

        AABB aabb;

        // #O pot ser que no siga necessari, si el juntem en el water_read_buffer se estalviem 2 còpies
        Ref<VoxelBuffer> out_water_buffer;
        
        WaterDomain(Vector3i _origin, Vector3i _size, const Ref<VoxelTool> &water_tool, const Ref<VoxelTool> &terrain_tool);
        ~WaterDomain();

        bool mod_voxel_down(int x, int y, int z);
        bool mod_voxel(int x, int y, int z);


        // GOOD

        bool is_stable();
        void update();
        void update_sdf();

        bool is_inside_bounds(int x, int y, int z) const;


    };

}

#endif
