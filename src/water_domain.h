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

        const int WATER_CHANNELS = 1 << VoxelBuffer::CHANNEL_DATA5 | 1 << VoxelBuffer::CHANNEL_SDF;
        const int TERRAIN_CHANNELS = 1 << VoxelBuffer::CHANNEL_SDF;

        // enum IX: short
        // {
        //     Offset = 0,
        //     Water = 1
        // };

        ~WaterDomain() = default;

        const Ref<VoxelTool>& m_water_tool;
        const Ref<VoxelTool>& m_terrain_tool;

        Ref<VoxelBuffer> m_water_buffer;
        Ref<VoxelBuffer> m_terrain_buffer;

        AABB m_aabb;
        // AABB m_inner_aabb;
        // bool m_auto_resize;

        void pull();
        void push(const Ref<VoxelBuffer>& new_water_buffer) const;

        void prepare();


        Ref<VoxelBuffer> clone_buffer(const Ref<VoxelBuffer>& src_buffer) const;

        void clear_heigth_data(Ref<VoxelBuffer>& buffer) const;

        bool can_tr_down(int x, int z, const Ref<VoxelBuffer>& water_buffer, const Ref<VoxelBuffer>& terrain_buffer) const;
        bool can_tr_spread(int x, int z, const Ref<VoxelBuffer>& buffer) const;

        void tr_down(int x, int z, const Ref<VoxelBuffer>& src_buffer, Ref<VoxelBuffer>& dst_buffer) const;
        void tr_spread(int x, int z, const Ref<VoxelBuffer>& src_buffer, Ref<VoxelBuffer>& dst_buffer) const;

        void p_update_sdf(int x, int z, const Ref<VoxelBuffer>& src_buffer, Ref<VoxelBuffer>& dst_buffer) const;
        void p_update_aabb_min_max(int x, int z, Vector3i& min, Vector3i& max) const;

        void update_aabb(const Vector3i& min, const Vector3i& max);
        void update_world_data() const;

        void update_outer_aabb(AABB& aabb, const AABB& inner_aabb) const;


        bool inside_bounds(int x, int z, const AABB& aabb) const;

        void create_water_channel(Ref<VoxelBuffer>& buffer) const;



    public:

        WaterDomain(Vector3i _origin, Vector3i _size, const Ref<VoxelTool> &water_tool, const Ref<VoxelTool> &terrain_tool);

        void update();

        // void reset_resize_buffer();
        // void update_resize_buffer(Vector2i xz, int origin, int height);


        // bool is_stable();
        // void update();
        // void update_sdf();

        // bool is_inside_bounds(int x, int y, int z) const;


        friend WaterSimulation;
    };

}

#endif