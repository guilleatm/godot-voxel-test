#include "water_domain.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

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

	water_buffer = Ref<VoxelBuffer>(_water_buffer);
	terrain_buffer = Ref<VoxelBuffer>(_terrain_buffer);
	buffer = Ref<VoxelBuffer>(_buffer);

	water_tool->copy(_origin, water_buffer, CH_SDF_MASK);
	water_tool->copy(_origin, water_buffer, CH_WATER_MASK);
	terrain_tool->copy(_origin, terrain_buffer, CH_SDF_MASK);


	prepare_water_buffer();
}

WaterDomain::~WaterDomain() {

	// read_water_buffer->~Ref();
	// read_terrain_buffer->~Ref();

	// delete water->ptr();
	// delete water;

	// delete terrain->ptr();
	// delete terrain;
}

void WaterDomain::prepare_water_buffer()
{
	Vector3i size = water_buffer->get_size();

	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			for (int z = 0; z < size.z; z++)
			{
				float sdf_water_voxel = water_buffer->get_voxel_f(x, y, z, CH_SDF);

				uint8_t v;
				if (sdf_water_voxel < 0)
				{
					v = WATER_VOXEL_RESOLUTION;
				}
				else
				{
					v = 0;
				}

				water_buffer->set_voxel(v, x, y, z, CH_WATER);
			}
		}
	}
}


void WaterDomain::update()
{
	PRINT("Update domain: " + origin);

	buffer->fill((uint8_t) 0, CH_WATER);

	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			for (int z = 0; z < size.z; z++)
			{
				uint8_t water_voxel = water_buffer->get_voxel(x, y, z, CH_WATER);

				if (water_voxel == 0)
				{
					water_buffer->set_voxel_f(1.0f , x, y, z, CH_SDF);
				}
				else
				{
					float value = -water_voxel / (float) WATER_VOXEL_RESOLUTION;
					water_buffer->set_voxel_f(value, x, y, z, CH_SDF);
				}


				const uint8_t PARTS = 5;
				uint8_t _7 = water_voxel / PARTS;
				uint8_t _7r = water_voxel % PARTS;

				uint8_t value;

				value = buffer->get_voxel(x, y, z, CH_WATER);
				buffer->set_voxel(value + _7r + _7, x, y, z, CH_WATER);

				if (x != size.x - 1)
				{
					value = buffer->get_voxel(x + 1, y, z, CH_WATER);
					buffer->set_voxel(value + _7, x + 1, y, z, CH_WATER);
				}

				if (x != 0)
				{
					value = buffer->get_voxel(x - 1, y, z, CH_WATER);
					buffer->set_voxel(value + _7, x - 1, y, z, CH_WATER);
				}

				if (z != size.z - 1)
				{
					value = buffer->get_voxel(x, y, z + 1, CH_WATER);
					buffer->set_voxel(value + _7, x, y, z + 1, CH_WATER);
				}

				if (z != 0)
				{
					value = buffer->get_voxel(x, y, z - 1, CH_WATER);
					buffer->set_voxel(value + _7, x, y, z - 1, CH_WATER);
				}

			}
	    }
	}

	// Vector3i min = Vector3i(0, 1, 0);
	// Vector3i max = size - Vector3i(1, 0, 1);

	water_buffer->copy_channel_from(buffer, CH_WATER);
	// buffer->fill((uint8_t) 0, CH_WATER);
	//buffer->fill_area((uint8_t) 0, min, max, CH_WATER);

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