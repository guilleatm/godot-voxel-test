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

WaterDomain::WaterDomain(Vector3i origin, Vector3i size, const Ref<VoxelTool>& _water_tool, const Ref<VoxelTool>& _terrain_tool, bool auto_resize) :
m_aabb( AABB(origin, size) ),
m_inner_aabb( m_aabb ),
water_tool( _water_tool ),
terrain_tool( _terrain_tool ),
m_water_buffer( Ref<VoxelBuffer>( new VoxelBuffer() ) ),
m_terrain_buffer( Ref<VoxelBuffer>( new VoxelBuffer() ) ),
m_auto_resize(auto_resize)
{
	m_water_buffer->set_channel_depth(CH_WATER, godot::VoxelBuffer::Depth::DEPTH_8_BIT);
	prepare();
}

// TODO: terrain into account
bool WaterDomain::can_tr_down(int x, int z, const Ref<VoxelBuffer>& buffer) const
{
	bool in_bounds = inside_bounds(x, z, m_aabb);
	if (!in_bounds) return false;
	int offset = buffer->get_voxel(x, OFFSET, z, CH_WATER);
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


void WaterDomain::update()
{
	pull(m_water_buffer, m_terrain_buffer);

	PRINT(m_aabb.size);

	Ref<VoxelBuffer> new_water_buffer = clone_buffer(m_water_buffer);

	Vector3i min_with_water = Vector3i(INT32_MAX, INT32_MAX, INT32_MAX);
	Vector3i max_with_water = Vector3i(-1, -1, -1);

	for (int x = 0; x < (int) m_aabb.size.x; x++)
	{
		for (int z = 0; z < (int) m_aabb.size.z; z++)
		{
			bool can_transfer_water_down = can_tr_down(x, z, m_water_buffer);
			
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
			p_update_min_max(x, z, m_water_buffer, min_with_water, max_with_water);
		}
	}

	push(new_water_buffer);

	update_inner_aabb(min_with_water, max_with_water, m_aabb, m_inner_aabb);

	update_size(m_aabb, m_inner_aabb);
}

void WaterDomain::p_update_sdf(int x, int z, const Ref<VoxelBuffer>& src_buffer, Ref<VoxelBuffer>& dst_buffer) const
{
	const uint64_t MINUS_ONE_F = 32769;
	const uint64_t PLUS_ONE_F = 32767;

	// if (inside_bounds(x, z, m_aabb))
	// {
	// 	PRINT("inside");
	// }
	// else
	// {
	// 	PRINT("outside");
	// 	return;
	// }

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

void WaterDomain::p_update_min_max(int x, int z, const Ref<VoxelBuffer>& src_buffer, Vector3i& min, Vector3i& max) const
{	
	int n = src_buffer->get_voxel(x, HEIGHT, z, CH_WATER);
	int offset = src_buffer->get_voxel(x, OFFSET, z, CH_WATER);
	int height = n / COMPLETE_WATER;

	if (height > 0)
	{
		min.x = std::min(min.x, x);
		min.y = std::min(min.y, offset);
		min.z = std::min(min.z, z);

		max.x = std::max(max.x, x);
		max.y = std::max(max.y, height + 1); // (height + 1) because after height, we van have a water voxel.
		max.z = std::max(max.z, z);
	}
}

void WaterDomain::update_inner_aabb(const Vector3i& min, const Vector3i& max, const AABB& aabb, AABB& inner_aabb) const
{
	inner_aabb.set_position( aabb.position + min );
	inner_aabb.set_end( aabb.position + max );
}

Ref<VoxelBuffer> WaterDomain::clone_buffer(const Ref<VoxelBuffer>& src_buffer) const
{
	return Ref<VoxelBuffer>( src_buffer );
}

void WaterDomain::clear_heigth_data(Ref<VoxelBuffer>& buffer) const
{
	Vector3i size = buffer->get_size();
	buffer->fill_area(0, Vector3i(0, HEIGHT, 0), Vector3i(size.x, HEIGHT + 1, size.z), CH_WATER );
}



void WaterDomain::prepare()
{
	pull(m_water_buffer, m_terrain_buffer);

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

void WaterDomain::pull(Ref<VoxelBuffer>& dst_water_buffer, Ref<VoxelBuffer>& dst_terrain_buffer ) const
{
	dst_water_buffer->clear();
	dst_water_buffer->create((int) m_aabb.size.x, (int) m_aabb.size.y, (int) m_aabb.size.z);
	water_tool->copy(m_aabb.position, dst_water_buffer, CH_WATER_MASK | CH_SDF_MASK);

	dst_terrain_buffer->clear();
	dst_terrain_buffer->create((int) m_aabb.size.x, (int) m_aabb.size.y, (int) m_aabb.size.z);
	terrain_tool->copy(m_aabb.position, dst_terrain_buffer, CH_SDF_MASK);
}

void WaterDomain::push(const Ref<VoxelBuffer>& src_water_buffer) const
{
	water_tool->paste(m_aabb.position, src_water_buffer, CH_WATER_MASK | CH_SDF_MASK);
}

void WaterDomain::update_size(AABB& aabb, const AABB& inner_aabb) const
{
	if (!m_auto_resize) return;

	AABB aux_inner_aabb = inner_aabb.grow(1);

	if ( !aabb.encloses( aux_inner_aabb ) )
	{
		Vector3i og_size = aabb.get_size();
		aabb.set_end( aux_inner_aabb.get_end() );


		Vector3i og_position = aabb.get_position();
		Vector3i new_position = aux_inner_aabb.get_position();
		// Probably we can just do this if .y doesn't match
		if (og_position != new_position)
		{
			auto og_water_buffer = clone_buffer(m_water_buffer);


			Vector3i v = new_position - og_position;
			aabb.set_position(new_position);
		
			Vector3i new_size = aabb.get_size();

			m_water_buffer->copy_channel_from_area(og_water_buffer, Vector3i(0, 0, 0), new_size, Vector3i(-v.x, 0, -v.z), CH_WATER);
		
		}
	
	}

}
