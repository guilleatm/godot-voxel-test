#include "domain.h"
#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/voxel_terrain.hpp>
#include <godot_cpp/classes/voxel_tool.hpp>
#include <godot_cpp/classes/voxel_buffer.hpp>


using namespace godot;

Domain::Domain()
{
}

Domain::~Domain()
{
}

void Domain::_enter_tree()
{
	if( Engine::get_singleton()->is_editor_hint() ) return;

	UtilityFunctions::print("Domain Enter Tree");

	ERR_FAIL_NULL_MSG(m_terrain, "Terrain is null");
	ERR_FAIL_NULL_MSG(m_water, "Water is null");
}

int frame_count = 0;
void Domain::_process(double delta)
{
	if( Engine::get_singleton()->is_editor_hint() ) return;

	if (frame_count % 60 == 0)
	{
		process_water_height();
		process_water_xz();
	}


	frame_count += 1;
}

void Domain::process_water_height()
{
	const int size = 16;
	const Vector3i position = Vector3i(0, -32, 0);

	Ref<VoxelBuffer> read_buffer = Ref<VoxelBuffer>( new VoxelBuffer() );
	read_buffer->create(size, size, size);

	Ref<VoxelBuffer> write_buffer = Ref<VoxelBuffer>( new VoxelBuffer() );
	write_buffer->create(size, size, size);

	m_water_tool->copy(position, read_buffer, 1 << VoxelBuffer::CHANNEL_SDF);
	m_water_tool->copy(position, write_buffer, 1 << VoxelBuffer::CHANNEL_SDF);
	


	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			for (int z = 0; z < size; z++)
			{
				float me = read_buffer->get_voxel_f(x, y, z, VoxelBuffer::CHANNEL_SDF);

				// float x_right = read_buffer->get_voxel_f(x + 1, y, z, VoxelBuffer::CHANNEL_SDF);
				// float x_left = read_buffer->get_voxel_f(x - 1, y, z, VoxelBuffer::CHANNEL_SDF);
				
				// float z_right = read_buffer->get_voxel_f(x + 1, y, z, VoxelBuffer::CHANNEL_SDF);
				// float z_left = read_buffer->get_voxel_f(x - 1, y, z, VoxelBuffer::CHANNEL_SDF);

				if (y > 0)
				{
					float down = read_buffer->get_voxel_f(x, y - 1, z, VoxelBuffer::CHANNEL_SDF);
					
					if (down > -1)
					{
						float need = -1 - down;
						float can = Math::max(need, me);

						write_buffer->set_voxel_f(down + can, x, y - 1, z, VoxelBuffer::CHANNEL_SDF);
						
						me = me - can;
						
						write_buffer->set_voxel_f(me, x, y, z, VoxelBuffer::CHANNEL_SDF);
					}
				}
			}
		}
	}
	
	m_water_tool->paste(position, write_buffer, 1 << VoxelBuffer::CHANNEL_SDF);
}

void Domain::process_water_xz()
{
	const int size = 16;
	const Vector3i position = Vector3i(0, -32, 0);

	Ref<VoxelBuffer> read_buffer = Ref<VoxelBuffer>( new VoxelBuffer() );
	read_buffer->create(size, size, size);

	Ref<VoxelBuffer> write_buffer = Ref<VoxelBuffer>( new VoxelBuffer() );
	write_buffer->create(size, size, size);

	m_water_tool->copy(position, read_buffer, 1 << VoxelBuffer::CHANNEL_SDF);
	m_water_tool->copy(position, write_buffer, 1 << VoxelBuffer::CHANNEL_SDF);
	

	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			for (int z = 0; z < size; z++)
			{
				float me = read_buffer->get_voxel_f(x, y, z, VoxelBuffer::CHANNEL_SDF);
				float me_02 = me * .2f

				// float x_right = read_buffer->get_voxel_f(x + 1, y, z, VoxelBuffer::CHANNEL_SDF);
				// float x_left = read_buffer->get_voxel_f(x - 1, y, z, VoxelBuffer::CHANNEL_SDF);
				
				// float z_right = read_buffer->get_voxel_f(x + 1, y, z, VoxelBuffer::CHANNEL_SDF);
				// float z_left = read_buffer->get_voxel_f(x - 1, y, z, VoxelBuffer::CHANNEL_SDF);


				if (x > 0)
				{
					float x_left = read_buffer->get_voxel_f(x - 1, y, z, VoxelBuffer::CHANNEL_SDF);

					if (x_left > -1)
					{
						write_buffer->set_voxel_f(x_left + me_02, x, y - 1, z, VoxelBuffer::CHANNEL_SDF);
					}

				}


			}
		}
	}
	
	m_water_tool->paste(position, write_buffer, 1 << VoxelBuffer::CHANNEL_SDF);
}


void Domain::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_aabb"), &Domain::get_aabb);
	ClassDB::bind_method(D_METHOD("set_aabb"), &Domain::set_aabb);
	ADD_PROPERTY(PropertyInfo(Variant::AABB, "AABB"), "set_aabb", "get_aabb");

	ClassDB::bind_method(D_METHOD("get_terrain"), &Domain::get_terrain);
	ClassDB::bind_method(D_METHOD("set_terrain"), &Domain::set_terrain);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "Terrain", PROPERTY_HINT_NODE_TYPE), "set_terrain", "get_terrain");

	ClassDB::bind_method(D_METHOD("get_water"), &Domain::get_water);
	ClassDB::bind_method(D_METHOD("set_water"), &Domain::set_water);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "Water", PROPERTY_HINT_NODE_TYPE), "set_water", "get_water");
}


AABB Domain::get_aabb() const
{
	return m_aabb;
}

void Domain::set_aabb(const AABB aabb)
{
	m_aabb = aabb;
}

VoxelTerrain* Domain::get_terrain() const
{
	return m_terrain;
}

void Domain::set_terrain(VoxelTerrain* terrain)
{
	m_terrain = terrain;
}

VoxelTerrain* Domain::get_water() const
{
	return m_water;
}

void Domain::set_water(VoxelTerrain* water)
{
	m_water = water;

	if (m_water != nullptr)
	{
		m_water_tool = m_water->get_voxel_tool();
	}
}