#include "water_generator.h"
#include <godot_cpp/core/class_db.hpp>

#include "global.h"

#include <godot_cpp/classes/voxel_terrain.hpp>
#include <godot_cpp/classes/voxel_tool.hpp>

using namespace godot;

WaterGenerator::WaterGenerator() {}


void WaterGenerator::prepare(VoxelTool* _terrain_tool_ptr)
{
	ERR_FAIL_COND(_terrain_tool_ptr == nullptr);
	terrain_tool_ptr = _terrain_tool_ptr;
}

void WaterGenerator::_generate_block(const Ref<VoxelBuffer> &out_buffer, const Vector3i &origin_in_voxels, int lod)
{
	// SET BUFFERS
	out_buffer->set_channel_depth(CH_WATER, godot::VoxelBuffer::Depth::DEPTH_8_BIT);
	out_buffer->set_channel_depth(CH_COL, godot::VoxelBuffer::Depth::DEPTH_8_BIT);

	// NO PROCESS CONDITIONS
	if (lod != 0 || origin_in_voxels.y > 0 || origin_in_voxels.y < -100)
	{
		out_buffer->fill_f(+1.0, CH_SDF);
		out_buffer->fill_f(0, CH_WATER);
		return;
	}


	// PREPARE READ BUFFERS
	Vector3i size = out_buffer->get_size();

	// We create a VoxelBuffer each time to avoid using mutex.
	Ref<VoxelBuffer> terrain_buffer = Ref<VoxelBuffer>( new VoxelBuffer() );
	terrain_buffer->create(size.x, size.y, size.z);
	terrain_tool_ptr->copy(origin_in_voxels, terrain_buffer, CH_SDF_MASK);
	
	out_buffer->fill_f(+1.0, CH_SDF);
	out_buffer->fill(0, CH_WATER);

	for (int x = 0; x < size.x; x++)
	{
		for (int z = 0; z < size.z; z++)
		{
			for (int y = size.y - 1; y >= 0; y--)
			{
				if (origin_in_voxels.y + y > 0) continue;

				float terrain_voxel = terrain_buffer->get_voxel_f(x, y, z, CH_SDF);
				
				if (terrain_voxel > 0)
				{
					// NO TERRAIN
					out_buffer->set_voxel_f(-1.0, x, y, z, CH_SDF);
					out_buffer->set_voxel(1, x, y, z, CH_WATER);
				}
				else
				{
					// TERRAIN
					break;
				}
			}
		}
	}
}

int WaterGenerator::_get_used_channels_mask() const
{
	return CH_SDF_MASK | CH_WATER_MASK | CH_COL_MASK;
}


void WaterGenerator::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("prepare", "terrain_tool_ptr"), &WaterGenerator::prepare);
}