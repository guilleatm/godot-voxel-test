#include "water_domain.h"
#include <godot_cpp/core/class_db.hpp>

#include "global.h"

// #include <godot_cpp/classes/voxel_buffer.hpp>


#include <vector>

#define OFFSET 0
#define HEIGHT 1

#define COMPLETE_WATER 5

#define HAS_TERRAIN(voxel) voxel < 0

using namespace godot;

const std::array<Vector2i, 4> directions =
{
	Vector2i(1, 0),
	Vector2i(-1, 0),
	Vector2i(0, 1),
	Vector2i(0, -1),
};

WaterDomain::WaterDomain(Vector3i origin, Vector3i size, const Ref<VoxelTool>& _water_tool, const Ref<VoxelTool>& _terrain_tool) :
m_aabb( AABB(origin, size) ),
// m_inner_aabb( m_aabb ),
m_water_tool( _water_tool ),
m_terrain_tool( _terrain_tool ),
m_water_buffer( Ref<VoxelBuffer>( new VoxelBuffer() ) ),
m_terrain_buffer( Ref<VoxelBuffer>( new VoxelBuffer() ) )
{
	m_water_buffer->set_channel_depth(CH_WATER, godot::VoxelBuffer::Depth::DEPTH_8_BIT);
	prepare();
}

// TODO: terrain into account
bool WaterDomain::can_tr_down(int x, int z, const Ref<VoxelBuffer>& water_buffer, const Ref<VoxelBuffer>& terrain_buffer) const
{
	bool in_bounds = inside_bounds(x, z, m_aabb);
	if (!in_bounds) return false;

	int offset = water_buffer->get_voxel(x, OFFSET, z, CH_WATER);

	float terrain_value = terrain_buffer->get_voxel_f(x, offset, z, VoxelBuffer::CHANNEL_SDF);
	if (terrain_value < 0) return false;

	return offset > 0;
}

// TODO: terrain into account
bool WaterDomain::can_tr_spread(int x, int z, const Ref<VoxelBuffer>& buffer) const
{
	bool in_bounds = inside_bounds(x, z, m_aabb);
	if (!in_bounds) return false;

	int n = buffer->get_voxel(x, HEIGHT, z, CH_WATER);
	return n >= COMPLETE_WATER;
}

// TODO: terrain into account
void WaterDomain::tr_down(int x, int z, const Ref<VoxelBuffer>& src_buffer, Ref<VoxelBuffer>& dst_buffer) const
{
	int offset = src_buffer->get_voxel(x, OFFSET, z, CH_WATER);
	int n = src_buffer->get_voxel(x, HEIGHT, z, CH_WATER);
	int height = n / COMPLETE_WATER;
	int water = n % COMPLETE_WATER;

	int new_offset = offset - 1;

	dst_buffer->set_voxel(new_offset, x, OFFSET, z, CH_WATER);
}

// TODO: terrain into account
void WaterDomain::tr_spread(int x, int z, const Ref<VoxelBuffer>& src_buffer, Ref<VoxelBuffer>& dst_buffer) const
{
	Vector2i cell = Vector2i(x, z);
	
	int n = src_buffer->get_voxel(x, HEIGHT, z, CH_WATER);
	int height = n / COMPLETE_WATER;
	int water = n % COMPLETE_WATER;

	int lost_water = 0;

	for (int i = 0; i < directions.size(); i++)
	{
		Vector2i neighbor_cell = cell + directions[i];
		bool in_bounds = inside_bounds(neighbor_cell.x, neighbor_cell.y, m_aabb);

		if (in_bounds)
		{
			int neighbor_n = src_buffer->get_voxel(neighbor_cell.x, HEIGHT, neighbor_cell.y, CH_WATER);
			int neighbor_height = neighbor_n / COMPLETE_WATER;
			// int neighbor_water = neighbor_n % COMPLETE_WATER;

			if (height > neighbor_height)
			{
				dst_buffer->set_voxel(neighbor_n + 1, neighbor_cell.x, HEIGHT, neighbor_cell.y, CH_WATER);
				lost_water += 1;
			}
		}
	}

	if (lost_water > 0)
	{
		dst_buffer->set_voxel(n - lost_water, x, HEIGHT, z, CH_WATER);
	}
}


bool WaterDomain::inside_bounds(int x, int z, const AABB& aabb) const
{
	const Vector3 BOUNDARY_CHECK_OFFSET = Vector3(.5, .5, .5);
	return aabb.has_point( aabb.position + Vector3i(x, OFFSET, z) + BOUNDARY_CHECK_OFFSET);
}

// Vector3i WaterDomain::get_aabb_size(const AABB& aabb) const
// {
// 	return Vector3i( (int) aabb.size.x, (int) aabb.size.y, (int) aabb.size.z);
// }



void WaterDomain::update()
{
	pull();

	Ref<VoxelBuffer> new_water_buffer = clone_buffer(m_water_buffer);

	Vector3i aabb_min = m_aabb.get_size();
	Vector3i aabb_max = Vector3i(0, 0, 0);

	for (int x = 0; x < (int) m_aabb.size.x; x++)
	{
		for (int z = 0; z < (int) m_aabb.size.z; z++)
		{
			bool can_transfer_water_down = can_tr_down(x, z, m_water_buffer, m_terrain_buffer);
			
			if (can_transfer_water_down)
			{
				tr_down(x, z, m_water_buffer, new_water_buffer);
			}
			else
			{
				bool can_spread_water = can_tr_spread(x, z, m_water_buffer);

				if (can_spread_water)
				{
					tr_spread(x, z, m_water_buffer, new_water_buffer);
				}
			}

			p_update_sdf(x, z, m_water_buffer, new_water_buffer);
			p_update_aabb_min_max(x, z, aabb_min, aabb_max);
		}
	}

	push(new_water_buffer);
	
	update_aabb(aabb_min, aabb_max);
}

void WaterDomain::p_update_sdf(int x, int z, const Ref<VoxelBuffer>& src_buffer, Ref<VoxelBuffer>& dst_buffer) const
{
	const uint64_t MINUS_ONE_F = 32769;
	const uint64_t PLUS_ONE_F = 32767;

	int offset = src_buffer->get_voxel(x, OFFSET, z, CH_WATER);
	int n = src_buffer->get_voxel(x, HEIGHT, z, CH_WATER);
	int height = n / COMPLETE_WATER;
	int water = n % COMPLETE_WATER;
	
	dst_buffer->fill_area(PLUS_ONE_F, Vector3i(x, 0, z), Vector3i(x + 1, src_buffer->get_size().y, z + 1), CH_SDF);
	dst_buffer->fill_area(MINUS_ONE_F, Vector3i(x, offset, z), Vector3i(x + 1, offset + height, z + 1), CH_SDF);
	// 	dst_buffer->fill_area(MINUS_ONE_F, Vector3i(x, offset - 1, z), Vector3i(x + 1, offset + height - 1, z + 1), CH_SDF);

	if (water > 0)
	{
		float normalized_water = Math::inverse_lerp(0.0f, COMPLETE_WATER, water);
		float sdf_water = -normalized_water;
	
		dst_buffer->set_voxel_f(sdf_water, x, offset + height + 1, z);
	}
}

void WaterDomain::p_update_aabb_min_max(int x, int z, Vector3i& min, Vector3i& max) const
{	
	int water_amount = m_water_buffer->get_voxel(x, HEIGHT, z, CH_WATER);
	int offset = m_water_buffer->get_voxel(x, OFFSET, z, CH_WATER);
	int water_voxels = water_amount / COMPLETE_WATER;

	if (water_amount > 0)
	{
		min.x = std::min(min.x, x);
		min.y = std::min(min.y, offset);
		min.z = std::min(min.z, z);

		max.x = std::max(max.x, x);
		max.y = std::max(max.y, water_voxels + 1); // (height + 1) because after height, we van have a water voxel.
		max.z = std::max(max.z, z);
	}
}

// void WaterDomain::update_world_data() const
// {
// }

void WaterDomain::update_aabb(const Vector3i& min, const Vector3i& max)
{
	// PRINT(m_aabb);

	// PRINT(min);
	// PRINT(max);

	Vector3i v_one = Vector3i(1, 1, 1);

	Vector3i og_position = m_aabb.get_position();
	Vector3i new_position = og_position + min - v_one;
	Vector3i new_end_position = og_position + max + v_one;

	Vector3i new_size = new_end_position - new_position;

	// delete(&m_aabb);
	
	pull();

	Vector3i diff_position = new_position - og_position;
	Vector3i diff_position_y = Vector3i(0, diff_position.y, 0);

	m_water_tool->paste(og_position + diff_position_y, m_water_buffer, 1 << VoxelBuffer::CHANNEL_DATA5);

	// Update AABB
	m_aabb.set_position( new_position );
	m_aabb.set_end( new_end_position );


	// Update offsets
	for (int x = 0; x < (int) m_aabb.size.x; x++)
	{
		for (int z = 0; z < (int) m_aabb.size.z; z++)
		{
			int offset = m_water_buffer->get_voxel(x, OFFSET, z, VoxelBuffer::CHANNEL_DATA5);
			m_water_buffer->set_voxel(offset - diff_position.y, x, OFFSET, z, VoxelBuffer::CHANNEL_DATA5);

			// int water_amount = m_water_buffer->get_voxel(x, HEIGHT, z, VoxelBuffer::CHANNEL_DATA5);
			// m_water_buffer->set_voxel(water_amount, x, OFFSET, z, VoxelBuffer::CHANNEL_DATA5);
		}
	}

	push(m_water_buffer);
	
	// PRINT(m_aabb);
	// m_aabb = AABB(new_position, new_size);
}

Ref<VoxelBuffer> WaterDomain::clone_buffer(const Ref<VoxelBuffer>& src_buffer) const
{
	Ref<VoxelBuffer> clone = Ref<VoxelBuffer>( new VoxelBuffer() );
	clone->set_channel_depth(CH_WATER, VoxelBuffer::DEPTH_8_BIT);
	Vector3i src_size = src_buffer->get_size();
	clone->create(src_size.x, src_size.y, src_size.z);
	clone->copy_channel_from(src_buffer, CH_WATER);
	return clone;
	// return Ref<VoxelBuffer>( src_buffer );
}

void WaterDomain::clear_heigth_data(Ref<VoxelBuffer>& buffer) const
{
	Vector3i size = buffer->get_size();
	buffer->fill_area(0, Vector3i(0, HEIGHT, 0), Vector3i(size.x, HEIGHT + 1, size.z), CH_WATER );
}



void WaterDomain::prepare()
{
	pull();

	// water_buffer->set_voxel_f(-1.0f, 0, 0, 0, CH_SDF);
	// PRINT(water_buffer->get_voxel(0, 0, 0, CH_SDF));

	for (int x = 0; x < (int) m_aabb.size.x; x++)
	{
		for (int z = 0; z < (int) m_aabb.size.z; z++)
		{
			bool water_found = false;
			int height = 0;
			for (int y = 0; y < (int) m_aabb.size.y; y++)
			{
				float sdf_water = m_water_buffer->get_voxel_f(x, y, z, CH_SDF);
			
				if (sdf_water < 0)
				{
					// WATER
					if (!water_found)
					{
						m_water_buffer->set_voxel(y, x, 0, z, CH_WATER);
						water_found = true;
					}
					height += 1;
				}
			}
			m_water_buffer->set_voxel(height * COMPLETE_WATER, x, 1, z, CH_WATER);
		}
	}
	push(m_water_buffer);
}

void WaterDomain::create_water_channel(Ref<VoxelBuffer>& buffer) const
{
	buffer->set_channel_depth(VoxelBuffer::CHANNEL_DATA5, VoxelBuffer::DEPTH_8_BIT);
	buffer->fill(0, VoxelBuffer::CHANNEL_DATA5);
}

void WaterDomain::pull()
{
	Vector3i size = m_aabb.get_size();
	
	m_water_buffer->clear();
	m_water_buffer->create(size.x, size.y, size.z);
	m_water_tool->copy(m_aabb.position, m_water_buffer, WaterDomain::WATER_CHANNELS);

	m_terrain_buffer->clear();
	m_terrain_buffer->create(size.x, size.y, size.z);
	m_terrain_tool->copy(m_aabb.position, m_terrain_buffer, WaterDomain::TERRAIN_CHANNELS);
}

void WaterDomain::push(const Ref<VoxelBuffer>& water_buffer) const
{
	m_water_tool->paste(m_aabb.position, water_buffer, WaterDomain::WATER_CHANNELS);
}

void WaterDomain::update_outer_aabb(AABB& aabb, const AABB& inner_aabb) const
{
	// if (!m_auto_resize) return;

	// PRINT("aabb: " + aabb);
	// PRINT("inner aabb: " + inner_aabb);

	AABB aux_inner_aabb = inner_aabb.grow(1);

	if ( !aabb.encloses( aux_inner_aabb ) )
	{
		Vector3i og_position = aabb.get_position();
		Vector3i new_position = aux_inner_aabb.get_position();

		Vector3i diff_position = new_position - og_position;

		// Vector3i og_size = aabb.get_position();
		Vector3i new_size = aux_inner_aabb.get_size();

		PRINT("Diff position: " + diff_position);

		// m_aabb = m_inner_aabb.cop;

		// Create buffer
		// Ref<VoxelBuffer> new_buffer = Ref<VoxelBuffer>( new VoxelBuffer() );
		// create_water_channel(new_buffer);
		// new_buffer->create(new_size.x, new_size.y, new_size.z);

		// m_water_buffer->copy_channel_from_area(og_water_buffer, Vector3i(0, 0, 0), new_size, Vector3i(v.x, 0, v.z), CH_WATER);


		

	}

}




		// // Vector3i og_size = aabb.get_size();
		// // aabb.set_end( aux_inner_aabb.get_end() );


		// Vector3i new_position = aux_inner_aabb.get_position();
		// // Probably we can just do this if .y doesn't match
		// if (og_position != new_position)
		// {
		// 	auto og_water_buffer = clone_buffer(m_water_buffer);

		// 	// Vector3i v = new_position - og_position;
			
		// 	Vector3i v = Vector3i(1, 0, 0);
			
		// 	aabb.set_position(og_position + v);
		
		// 	Vector3i new_size = aabb.get_size();

		// 	m_water_buffer->copy_channel_from_area(og_water_buffer, Vector3i(0, 0, 0), new_size, Vector3i(v.x, 0, v.z), CH_WATER);
		
		// }