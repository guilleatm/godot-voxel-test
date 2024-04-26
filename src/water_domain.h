#ifndef WATER_DOMAIN_H
#define WATER_DOMAIN_H

#include <godot_cpp/classes/voxel_tool.hpp>
#include <array>

namespace godot
{
    class VoxelBuffer;
    class AABB;
    class WaterSimulation;

    class WaterDomain
    {

    private:

        ~WaterDomain() = default;

        const Ref<VoxelTool>& water_tool;
        const Ref<VoxelTool>& terrain_tool;

        Ref<VoxelBuffer> m_water_buffer;
        Ref<VoxelBuffer> m_terrain_buffer;

        AABB m_aabb;
        AABB m_inner_aabb;
        bool m_auto_resize;

        void pull(Ref<VoxelBuffer>& dst_water_buffer, Ref<VoxelBuffer>& dst_terrain_buffer) const;
        void push(const Ref<VoxelBuffer>& src_water_buffer) const;

        void prepare();


        Ref<VoxelBuffer> clone_water_buffer(const Ref<VoxelBuffer>& src_buffer) const;

        void clear_heigth_data(Ref<VoxelBuffer>& buffer) const;

        bool can_tr_down(int x, int z, const Ref<VoxelBuffer>& buffer) const;
        bool can_tr_spread(int x, int z, const Ref<VoxelBuffer>& buffer) const;

        void tr_down(int x, int z, const Ref<VoxelBuffer>& src_buffer, Ref<VoxelBuffer>& dst_buffer) const;
        void tr_spread(int x, int z, const Ref<VoxelBuffer>& src_buffer, Ref<VoxelBuffer>& dst_buffer) const;

        void p_update_sdf(int x, int z, const Ref<VoxelBuffer>& src_buffer, Ref<VoxelBuffer>& dst_buffer) const;
        void p_update_min_max(int x, int z, const Ref<VoxelBuffer>& src_buffer, Vector3i& min, Vector3i& max) const;

        void update_inner_aabb(const Vector3i& min, const Vector3i& max, AABB& aabb) const;

        bool inside_bounds(int x, int z, const AABB& aabb) const;
        
    public:

        WaterDomain(Vector3i _origin, Vector3i _size, const Ref<VoxelTool> &water_tool, const Ref<VoxelTool> &terrain_tool, bool auto_resize);

        void update();

        // void reset_resize_buffer();
        // void update_resize_buffer(Vector2i xz, int origin, int height);
        void update_size();

        // bool is_stable();
        // void update();
        // void update_sdf();

        // bool is_inside_bounds(int x, int y, int z) const;


        friend WaterSimulation;
    };

}

#endif