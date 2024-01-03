#include "water_domain.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// update for independence, new here
WaterDomain::WaterDomain(Vector3i _origin, Vector3i _size, const Ref<VoxelTool> &water_tool, const Ref<VoxelTool> &terrain_tool)
{
	const int CH_SDF_MASK = 1 << CH_SDF;

	origin = _origin;
	size = _size;

	VoxelBuffer* _water_buffer = new VoxelBuffer();
	_water_buffer->create(size.x, size.y, size.z);

	VoxelBuffer* _terrain_buffer = new VoxelBuffer();
	_terrain_buffer->create(size.x, size.y, size.z);

	VoxelBuffer* _out_buffer = new VoxelBuffer();
	_out_buffer->create(size.x, size.y, size.z);


	read_water_buffer = Ref<VoxelBuffer>(_water_buffer);
	read_terrain_buffer = Ref<VoxelBuffer>(_terrain_buffer);
	out_water_buffer = Ref<VoxelBuffer>(_out_buffer);


	water_tool.ptr()->copy(_origin, read_water_buffer, CH_SDF_MASK);
	terrain_tool.ptr()->copy(_origin, read_terrain_buffer, CH_SDF_MASK);

}

WaterDomain::~WaterDomain() {

	// read_water_buffer->~Ref();
	// read_terrain_buffer->~Ref();

	// delete water->ptr();
	// delete water;

	// delete terrain->ptr();
	// delete terrain;
}


void WaterDomain::update()
{
	PRINT("Update domain: " + origin);

	out_water_buffer.ptr()->copy_channel_from(read_water_buffer, CH_SDF);

	for (int y = 1; y < size.y; y++)
	{
		for (int x = 1; x < size.x - 1; x++)
		{
			for (int z = 1; z < size.z - 1; z++)
			{
				float water_voxel = read_water_buffer.ptr()->get_voxel_f(x, y, z, CH_SDF);

				// No water
				if (water_voxel >= 0) continue;

				float terrain_voxel_down = read_terrain_buffer.ptr()->get_voxel_f(x, y - 1, z, CH_SDF);
				// float water_voxel_down = water_read_buffer.get_voxel_f(x, y - 1, z, CHANNEL);

				if (terrain_voxel_down < 0)
				{
					
				}
				// No terrain down
				else
				{
					// Transfer all water
					out_water_buffer.ptr()->set_voxel_f(1.0f, x, y, z, CH_SDF);
					out_water_buffer.ptr()->set_voxel_f(-1.0f, x, y - 1, z, CH_SDF);
				}
	        }
	    }
	}

	read_water_buffer.ptr()->copy_channel_from(out_water_buffer, CH_SDF);
}










//		1
//	2 - X - 0
//		3
std::array<float, 4> WaterDomain::get_surr(const VoxelBuffer &water_read_buffer, int x, int y, int z, int channel) const {
	
	std::array<float, 4> _surr;

	_surr[0] = water_read_buffer.get_voxel_f(x + 1, y, z, channel);
	_surr[1] = water_read_buffer.get_voxel_f(x, y, z + 1, channel);
	_surr[2] = water_read_buffer.get_voxel_f(x - 1, y, z, channel);
	_surr[3] = water_read_buffer.get_voxel_f(x, y, z - 1, channel);

	return _surr;
}


// void WaterDomain::spread(const VoxelBuffer &water_read_buffer, int x, int y, int z, int channel) {

// 	const float SPREAD_AMOUNT = 0.2;

// 	float water_voxel = water->ptr()->get_voxel_f(x, y, z, channel);
	
// 	std::array<float, 4> surr = get_surr(water_read_buffer, x, y, z, channel);

// 	water->ptr()->set_voxel_f((water_voxel - surr[0]) * SPREAD_AMOUNT, x + 1, y, z, channel);
// 	water->ptr()->set_voxel_f((water_voxel - surr[1]) * SPREAD_AMOUNT, x, y, z + 1, channel);
// 	water->ptr()->set_voxel_f((water_voxel - surr[2]) * SPREAD_AMOUNT, x - 1, y, z, channel);
// 	water->ptr()->set_voxel_f((water_voxel - surr[3]) * SPREAD_AMOUNT, x, y, z - 1, channel);
// }

bool WaterDomain::is_stable() {
	return stable_levels >= size.y - 2;
}