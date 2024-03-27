#include "water_domain.h"
#include <godot_cpp/core/class_db.hpp>

#include "global.h"

// #include <godot_cpp/classes/voxel_buffer.hpp>


#include <vector>

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
	prepare();
}


void WaterDomain::update()
{
	copy_to_buffers();


	for (int x = 0; x < (int) aabb.size.x; x++)
	{
		for (int z = 0; z < (int) aabb.size.z; z++)
		{
			// int water_o = water_buffer->get_voxel(x, 0, z, CH_COL);
			// int water_h = water_buffer->get_voxel(x, 1, z, CH_COL);

			// if (water_h > 0)
			// {
			// 	PRINT("GOOD");
			// }



			// if (water == WATER)
			// {
			// 	PRINT("WATER HERE");
			// }

			// float terrain_voxel = terrain_buffer->get_voxel_f(x, y, z, CH_SDF); // CHANGE

			// if (HAS_TERRAIN(terrain_voxel))
			// {
				
			// }
			// else
			// {
			// 	water_buffer->set_voxel(WATER, x, y, z, CH_WATER); // CHANGE
			// }
		}
	}

	paste_from_buffers();
}

void WaterDomain::prepare()
{
	copy_to_buffers();

	for (int x = 0; x < (int) aabb.size.x; x++)
	{
		for (int z = 0; z < (int) aabb.size.z; z++)
		{
			bool water_found = false;
			for (int y = 0; y < (int) aabb.size.y; y++)
			{
				float sdf_water = water_buffer->get_voxel_f(x, y, z, CH_SDF);

				PRINT(sdf_water);
			
				if (sdf_water < 0)
				{
					// WATER
					if (!water_found)
					{
						PRINT("Set origin");
						water_buffer->set_voxel(y, x, 0, z, CH_WATER);
						water_found = true;
					}

					PRINT("Water found");

				}
				else
				{
					// NO WATER
					if (water_found)
					{
						PRINT("Update height");
						water_buffer->set_voxel(y, x, 1, z, CH_WATER);
					}				
				}
			}

		}
	}
	paste_from_buffers();
}

void WaterDomain::copy_to_buffers()
{
	water_buffer->clear();
	water_buffer->create((int) aabb.size.x, (int) aabb.size.y, (int) aabb.size.z);
	water_tool->copy(aabb.position, water_buffer, CH_WATER_MASK | CH_SDF_MASK);

	terrain_buffer->clear();
	terrain_buffer->create((int) aabb.size.x, (int) aabb.size.y, (int) aabb.size.z);
	terrain_tool->copy(aabb.position, terrain_buffer, CH_SDF_MASK);
}

void WaterDomain::paste_from_buffers()
{
	water_tool->paste(aabb.position, water_buffer, CH_WATER_MASK | CH_SDF_MASK);
}