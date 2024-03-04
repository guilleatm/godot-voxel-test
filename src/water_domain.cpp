#include "water_domain.h"
// #include <godot_cpp/core/class_db.hpp>

#include <vector>
#include "global.h"

#include <godot_cpp/classes/voxel_tool.hpp>


#define WATER 1
#define NO_WATER 0

#define HAS_TERRAIN(voxel) voxel < 0

using namespace godot;

const std::vector<Vector2i> directions = std::vector<Vector2i>
{{
	Vector2i(1, 0),
	Vector2i(-1, 0),
	Vector2i(0, 1),
	Vector2i(0, -1)
}};



WaterDomain::WaterDomain(Vector3i origin, Vector3i size, const Ref<VoxelTool>& _water_tool, const Ref<VoxelTool>& _terrain_tool) :
aabb( AABB(origin, size) ),
water_tool( _water_tool ),
terrain_tool( _terrain_tool ),
water_buffer( Ref<VoxelBuffer>( new VoxelBuffer() ) ),
terrain_buffer( Ref<VoxelBuffer>( new VoxelBuffer() ) )
{
	water_buffer->set_channel_depth(CH_WATER, godot::VoxelBuffer::Depth::DEPTH_8_BIT);
}


void WaterDomain::update()
{
	copy_to_buffers();

	for (int x = 0; x < aabb.size.x; x++)
	{
		for (int z = 0; z < aabb.size.z; z++)
		{
			float terrain_voxel = terrain_buffer->get_voxel_f(x, 10, z, CH_SDF);

			if (HAS_TERRAIN(terrain_voxel))
			{
				PRINT("TERRAIN IN 10");
			}
			else
			{
				water_buffer->set_voxel(WATER, x, 10, z, CH_WATER);
			}
		}
	}

	paste_from_buffers();
}

void WaterDomain::copy_to_buffers()
{
	water_buffer->clear();
	water_buffer->create(aabb.size.x, aabb.size.y, aabb.size.z);
	water_tool->copy(aabb.size, water_buffer, CH_WATER_MASK | CH_SDF_MASK);

	terrain_buffer->clear();
	terrain_buffer->create(aabb.size.x, aabb.size.y, aabb.size.z);
	terrain_tool->copy(aabb.size, terrain_buffer, CH_SDF_MASK);
}

void WaterDomain::paste_from_buffers()
{
	water_tool->paste(aabb.size, water_buffer, CH_WATER_MASK | CH_SDF_MASK);
}