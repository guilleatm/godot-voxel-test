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

	// VoxelBuffer* _out_buffer = new VoxelBuffer();
	// _out_buffer->create(size.x, size.y, size.z);


	water_buffer = Ref<VoxelBuffer>(_water_buffer);
	terrain_buffer = Ref<VoxelBuffer>(_terrain_buffer);
	// out_water_buffer = Ref<VoxelBuffer>(_out_buffer);


	water_tool->copy(_origin, water_buffer, CH_SDF_MASK);
	water_tool->copy(_origin, water_buffer, CH_WATER_MASK);
	terrain_tool->copy(_origin, terrain_buffer, CH_SDF_MASK);

	prepare_buffer();
}

WaterDomain::~WaterDomain() {

	// read_water_buffer->~Ref();
	// read_terrain_buffer->~Ref();

	// delete water->ptr();
	// delete water;

	// delete terrain->ptr();
	// delete terrain;
}

void WaterDomain::prepare_buffer()
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

	VoxelBuffer buffer = VoxelBuffer();
	buffer.create(size.x, size.y, size.z);
	buffer.set_channel_depth(CH_WATER, godot::VoxelBuffer::Depth::DEPTH_8_BIT);
	


	for (int y = 1; y < size.y; y++)
	{
		for (int x = 1; x < size.x - 1; x++)
		{
			for (int z = 1; z < size.z - 1; z++)
			{

				// uint8_t water_voxel = water_buffer->get_voxel(x, y, z, CH_WATER);

				// if (water_voxel == 0)
				// {
				// 	water_buffer->set_voxel_f(1.0f , x, y, z, CH_SDF);
				// }
				// else
				// {
				// 	float value = -water_voxel / (float) WATER_VOXEL_RESOLUTION;

				// 	water_buffer->set_voxel_f(value, x, y, z, CH_SDF);
				// }




				float sdf_water_voxel = water_buffer->get_voxel_f(x, y, z, CH_SDF);
				uint8_t water_voxel = water_buffer->get_voxel(x, y, z, CH_WATER);

				// No water
				if (water_voxel == 0) continue;

				float sdf_terrain_voxel_down = terrain_buffer->get_voxel_f(x, y - 1, z, CH_SDF);
				uint8_t water_voxel_down = water_buffer->get_voxel(x, y - 1, z, CH_WATER);
				float sdf_water_voxel_down = water_buffer->get_voxel_f(x, y - 1, z, CH_SDF);


				// Has terrain voxel down
				if (sdf_terrain_voxel_down < 0)
				{
					continue;
				}
				else
				{
					// Has water voxel down
					if (water_voxel_down > 0)
					{
						uint8_t q = MIN(WATER_VOXEL_RESOLUTION - water_voxel_down, water_voxel);
						uint8_t r = water_voxel - q;

						water_buffer->set_voxel(r, x, y, z, CH_WATER);
						water_buffer->set_voxel(water_voxel_down + q, x, y - 1, z, CH_WATER);


						float _q = -q / (float) WATER_VOXEL_RESOLUTION;
						float _r = -r / (float) WATER_VOXEL_RESOLUTION;

						if (q == 0) _q = 0;
						if (r == 0) _r = 1.0f;

						water_buffer->set_voxel_f(_r , x, y, z, CH_SDF);
						water_buffer->set_voxel_f(sdf_water_voxel_down - _q, x, y - 1, z, CH_SDF);


						uint8_t s = r / 5;

						uint8_t water_right = water_buffer->get_voxel(x + 1, y, z, CH_WATER);
						uint8_t water_left = water_buffer->get_voxel(x - 1, y, z, CH_WATER);
						uint8_t water_front = water_buffer->get_voxel(x, y, z + 1, CH_WATER);
						uint8_t water_back = water_buffer->get_voxel(x, y, z - 1, CH_WATER);

						water_right = MIN(water_right + s, WATER_VOXEL_RESOLUTION);
						water_left = MIN(water_left + s, WATER_VOXEL_RESOLUTION);
						water_front = MIN(water_front + s, WATER_VOXEL_RESOLUTION);
						water_back = MIN(water_back + s, WATER_VOXEL_RESOLUTION);

						water_buffer->set_voxel(r, x, y, z, CH_WATER);
						water_buffer->set_voxel(water_right, x + 1, y, z, CH_WATER);
						water_buffer->set_voxel(water_left, x - 1, y, z, CH_WATER);
						water_buffer->set_voxel(water_front, x, y, z + 1, CH_WATER);
						water_buffer->set_voxel(water_back, x, y, z - 1, CH_WATER);




						// uint8_t s = r / 5;
						// water_buffer->set_voxel(s, x, y, z, CH_WATER);
						// water_buffer->set_voxel(s, x + 1, y, z, CH_WATER);
						// water_buffer->set_voxel(s, x - 1, y, z, CH_WATER);
						// water_buffer->set_voxel(s, x, y, z + 1, CH_WATER);
						// water_buffer->set_voxel(s, x, y, z - 1, CH_WATER);


						// uint8_t s = r / 5;
						
						// buffer.set_voxel(	buffer.get_voxel(x, y, z, CH_WATER)			+ s, x, y, z, CH_WATER);
						// buffer.set_voxel(	buffer.get_voxel(x + 1, y, z, CH_WATER)		+ s, x + 1, y, z, CH_WATER);
						// buffer.set_voxel(	buffer.get_voxel(x - 1, y, z, CH_WATER)		+ s, x - 1, y, z, CH_WATER);
						// buffer.set_voxel(	buffer.get_voxel(x, y, z + 1, CH_WATER)		+ s, x, y, z + 1, CH_WATER);
						// buffer.set_voxel(	buffer.get_voxel(x, y, z - 1, CH_WATER)		+ s, x, y, z - 1, CH_WATER);
					}
					else
					{					

						water_buffer->set_voxel(0, x, y, z, CH_WATER);
						water_buffer->set_voxel(water_voxel, x, y - 1, z, CH_WATER);

						water_buffer->set_voxel_f(1.0f, x, y, z, CH_SDF);
						water_buffer->set_voxel_f(-1.0f, x, y - 1, z, CH_SDF);
					}
				}
	        }
	    }
	}
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