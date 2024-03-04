#include "water_domain.h"
#include <godot_cpp/core/class_db.hpp>

#include "global.h"

// #include <godot_cpp/classes/voxel_buffer.hpp>


#include <vector>

namespace godot
{

	#define WATER 1
	#define NO_WATER 0

	#define HAS_TERRAIN(voxel) voxel < 0

	#define CONTINUE_IF(do_continue) if (do_continue) continue

	std::vector<Vector3i> directions = std::vector<Vector3i>
	{{
		Vector3i(1, 0, 0),
		Vector3i(-1, 0, 0),
		Vector3i(0, 1, 0),
		Vector3i(0, -1, 0),
		Vector3i(0, 0, 1),
		Vector3i(0, 0, -1)
	}};

	// update for independence, new here
	WaterDomain::WaterDomain(Vector3i _origin, Vector3i _size, const Ref<VoxelTool> &water_tool, const Ref<VoxelTool> &terrain_tool)
	{
		const int CH_SDF_MASK = 1 << CH_SDF;
		const int CH_WATER_MASK = 1 << CH_WATER;

		origin = _origin;
		size = _size;

		VoxelBuffer* _water_buffer = new VoxelBuffer();
		_water_buffer->create(size.x, size.y, size.z);
		_water_buffer->set_channel_depth(CH_WATER, godot::VoxelBuffer::Depth::DEPTH_8_BIT);

		VoxelBuffer* _terrain_buffer = new VoxelBuffer();
		_terrain_buffer->create(size.x, size.y, size.z);

		VoxelBuffer* _buffer = new VoxelBuffer();
		_buffer->create(size.x, size.y, size.z);
		_buffer->set_channel_depth(CH_WATER, godot::VoxelBuffer::Depth::DEPTH_8_BIT);
		_buffer->fill((uint8_t) 0, CH_WATER);

		read_buffer_ptr = Ref<VoxelBuffer>(_water_buffer);
		terrain_buffer_ptr = Ref<VoxelBuffer>(_terrain_buffer);
		write_buffer_ptr = Ref<VoxelBuffer>(_buffer);

		water_tool->copy(_origin, read_buffer_ptr, CH_SDF_MASK);
		water_tool->copy(_origin, read_buffer_ptr, CH_WATER_MASK);
		terrain_tool->copy(_origin, terrain_buffer_ptr, CH_SDF_MASK);


		prepare_water_buffer();
	}

	WaterDomain::~WaterDomain()
	{

		// read_water_buffer->~Ref();
		// read_terrain_buffer->~Ref();

		// delete water->ptr();
		// delete water;

		// delete terrain->ptr();
		// delete terrain;
	}

	void WaterDomain::prepare_water_buffer()
	{
		Vector3i size = read_buffer_ptr->get_size();

		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				for (int z = 0; z < size.z; z++)
				{
					float sdf_water_voxel = read_buffer_ptr->get_voxel_f(x, y, z, CH_SDF);

					uint8_t v;
					if (sdf_water_voxel < 0)
					{
						v = 1; // WATER_VOXEL_RESOLUTIUON
					}
					else
					{
						v = 0;
					}

					read_buffer_ptr->set_voxel(v, x, y, z, CH_WATER);
				}
			}
		}
	}



	bool WaterDomain::mod_voxel_down(int x, int y, int z)
	{
		if (!is_inside_bounds(x, y, z))
		{
			return false;
		}

		uint8_t voxel = write_buffer_ptr->get_voxel(x, y, z, CH_WATER);

		if (voxel == NO_WATER)
		{
			write_buffer_ptr->set_voxel(WATER, x, y, z, CH_WATER);
			return true;
		}
		return false;
	}



	bool WaterDomain::mod_voxel(int x, int y, int z)
	{
		if (!is_inside_bounds(x, y, z))
		{
			return false;
		}

		uint8_t voxel_in_read = read_buffer_ptr->get_voxel(x, y, z, CH_WATER);
		uint8_t voxel_in_write = write_buffer_ptr->get_voxel(x, y, z, CH_WATER);

		if (voxel_in_read == NO_WATER && voxel_in_write == NO_WATER)
		{
			write_buffer_ptr->set_voxel(WATER, x, y, z, CH_WATER);
			return true;
		}
		return false;
	}

	bool WaterDomain::is_inside_bounds(int x, int y, int z) const
	{
		if (y < 0 || y == size.y ) return false;
		if (x < 0 || x == size.x ) return false;
		if (z < 0 || z == size.z ) return false;
		return true;
	}

	void WaterDomain::update()
	{
		write_buffer_ptr->fill((uint8_t) 0, CH_WATER);

		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				for (int z = 0; z < size.z; z++)
				{
					uint8_t water_voxel = read_buffer_ptr->get_voxel(x, y, z, CH_WATER);

					if (water_voxel == NO_WATER) continue;
					// float terrain_voxel_down_sdf = terrain_buffer_ptr->get_voxel_f(x, y - 1, z, CH_SDF);


					// down
					CONTINUE_IF(mod_voxel_down(x, y - 1, z));

					// down left
					CONTINUE_IF(mod_voxel_down(x - 1, y - 1, z));
					// down back
					CONTINUE_IF(mod_voxel_down(x, y - 1, z - 1));
					// down right
					CONTINUE_IF(mod_voxel_down(x + 1, y - 1, z));

					// down front
					CONTINUE_IF(mod_voxel_down(x, y - 1, z + 1));


					// front
					CONTINUE_IF(mod_voxel(x, y, z + 1));
					// right
					CONTINUE_IF(mod_voxel(x + 1, y, z));
					// back
					CONTINUE_IF(mod_voxel(x, y, z - 1));
					// left
					CONTINUE_IF(mod_voxel(x - 1, y, z));



					write_buffer_ptr->set_voxel(WATER, x, y, z, CH_WATER);
				}
			}
		}

		read_buffer_ptr->copy_channel_from(write_buffer_ptr, CH_WATER);
	}


	void WaterDomain::update_sdf()
	{
		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				for (int z = 0; z < size.z; z++)
				{
					uint8_t water_voxel = read_buffer_ptr->get_voxel(x, y, z, CH_WATER);

					// if (water_voxel == WATER)
					// {
					// 	read_buffer_ptr->set_voxel_f(-1.0, x, y, z, CH_SDF);
					// }
					// else
					// {
					// 	read_buffer_ptr->set_voxel_f(1.0, x, y, z, CH_SDF);
					// }


					float sum = water_voxel;

					for (int i = 0; i < directions.size(); i++)
					{
						Vector3i d = directions[i];
						if (!is_inside_bounds(x + d.x , y + d.y, z + d.z))
						{
							continue;
						}
						
						uint8_t neigh = read_buffer_ptr->get_voxel(x + d.x , y + d.y, z + d.z, CH_WATER);

						sum += neigh;
					}

					float sdf_value = ((sum / directions.size()) * 2) - 1;

					read_buffer_ptr->set_voxel_f(-sdf_value, x, y, z, CH_SDF);



				}
			}
		}
	}


	bool WaterDomain::is_stable()
	{
		return stable_levels >= size.y - 2;
	}


}
