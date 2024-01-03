#include "water_simulation.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;


WaterSimulation::WaterSimulation() {
	active_domains = std::vector<WaterDomain*>();
}

WaterSimulation::~WaterSimulation() {
	for (int i = 0; i < active_domains.size(); i++) {
		delete active_domains[i];
	}
}


NodePath WaterSimulation::get_terrain() const {
	return terrain_node_path;
}

void WaterSimulation::set_terrain(NodePath nodePath) {
	terrain_node_path = nodePath;
}

NodePath WaterSimulation::get_water() const {
	return water_node_path;
}

void WaterSimulation::set_water(NodePath nodePath) {
	water_node_path = nodePath;
}

int WaterSimulation::get_simulation_timestep() const {
	return simulation_timestep_mseconds;
}

void WaterSimulation::set_simulation_timestep(int _simulation_timestep_mseconds) {
	simulation_timestep_mseconds = _simulation_timestep_mseconds;
}


void WaterSimulation::_ready() {

	DO_NOT_PLAY_IN_EDITOR;

	terrain = get_node<VoxelTerrain>(terrain_node_path);
	water = get_node<VoxelTerrain>(water_node_path);

	terrain_tool = terrain->get_voxel_tool();
	water_tool = water->get_voxel_tool();


	time = Time::get_singleton();


	Callable callable = Callable(this, "update_water");
	terrain->connect("on_area_edited", callable);


}

void WaterSimulation::_process(double delta) {

	DO_NOT_PLAY_IN_EDITOR;

	int current_time = time->get_ticks_msec();
	int elapsed_time = current_time - last_step_time;

	if (elapsed_time < simulation_timestep_mseconds) return;

	last_step_time = current_time;


	for (int i = active_domains.size() - 1; i >= 0; i--)
	{
		WaterDomain* domain = active_domains[i];
		domain->update();
		water_tool.ptr()->paste(domain->origin, domain->out_water_buffer, 1 << CH_SDF);

		if (active_domains[i]->is_stable())
		{
			active_domains.erase(active_domains.begin() + i);
			delete domain;
		}
	}


	// for (int i = active_domains.size() - 1; i >= 0; i--) {
	// 	update_domain(active_domains[i]);

	// 	if (active_domains[i]->is_stable()) {
	// 		WaterDomain* stable_domain = active_domains[i];
	// 		active_domains.erase(active_domains.begin() + i);
	// 		delete stable_domain;

	// 		PRINT("Domain stable");
	// 	}
	// }
}


void WaterSimulation::update_domain(WaterDomain* domain) {

	// const int CHANNEL = VoxelBuffer::CHANNEL_SDF;
	// const float SPREAD_AMOUNT = 0.2;


	// VoxelBuffer water_read_buffer = VoxelBuffer();
	// water_read_buffer.create(domain->size.x, domain->size.y, domain->size.z);
	// water_read_buffer.copy_channel_from(*domain->read_water_buffer, CHANNEL);

	
	// int voxels_per_level = (domain->size.x - 2) * (domain->size.z - 2);

	// for (int y = Math::max(1, domain->stable_levels); y < domain->size.y; y++)
	// {
	// 	int stable_voxels_count = 0;

	// 	for (int x = 1; x < domain->size.x - 1; x++)
	// 	{
	// 		for (int z = 1; z < domain->size.z - 1; z++)
	// 		{
				
	// 			float water_voxel = water_read_buffer.get_voxel_f(x, y, z, CHANNEL);

	// 			// If voxel has no water continue
	// 			if (water_voxel >= 0) continue;

	// 			float terrain_voxel_down = domain->read_terrain_buffer->ptr()->get_voxel_f(x, y - 1, z, CHANNEL);
	// 			float water_voxel_down = water_read_buffer.get_voxel_f(x, y - 1, z, CHANNEL); // can go deeper


	// 			if (terrain_voxel_down < 0)
	// 			{
	// 				// Spread
	// 				std::array<float, 4> surr = domain->get_surr(water_read_buffer, x, y, z, CHANNEL);

	// 				domain->read_water_buffer->ptr()->set_voxel_f((water_voxel - surr[0]) * SPREAD_AMOUNT, x + 1, y, z, CHANNEL);
	// 				domain->read_water_buffer->ptr()->set_voxel_f((water_voxel - surr[1]) * SPREAD_AMOUNT, x, y, z + 1, CHANNEL);
	// 				domain->read_water_buffer->ptr()->set_voxel_f((water_voxel - surr[2]) * SPREAD_AMOUNT, x - 1, y, z, CHANNEL);
	// 				domain->read_water_buffer->ptr()->set_voxel_f((water_voxel - surr[3]) * SPREAD_AMOUNT, x, y, z - 1, CHANNEL);

	// 				if (water_voxel < -.9)
	// 				{
	// 					stable_voxels_count += 1;
	// 				}
	// 			}
	// 			else if (water_voxel_down < 0)
	// 			{
	// 				// Transfer down + spread

	// 				// Amount of water to transfer to fill the voxel
	// 				float transferred_water = 1.0 + water_voxel_down; // Water voxel down is negative
	// 				//water_voxel += transferred_water; // ??

	// 				domain->read_water_buffer->ptr()->set_voxel_f(water_voxel + transferred_water, x, y, z, CHANNEL);
	// 				domain->read_water_buffer->ptr()->set_voxel_f(-1.0, x, y - 1, z, CHANNEL); // -1.0 = my_water(water_voxel_down) - transferred_water
					
	// 				std::array<float, 4> surr = domain->get_surr(water_read_buffer, x, y, z, CHANNEL);

	// 				domain->read_water_buffer->ptr()->set_voxel_f((water_voxel - surr[0]) * SPREAD_AMOUNT, x + 1, y, z, CHANNEL);
	// 				domain->read_water_buffer->ptr()->set_voxel_f((water_voxel - surr[1]) * SPREAD_AMOUNT, x, y, z + 1, CHANNEL);
	// 				domain->read_water_buffer->ptr()->set_voxel_f((water_voxel - surr[2]) * SPREAD_AMOUNT, x - 1, y, z, CHANNEL);
	// 				domain->read_water_buffer->ptr()->set_voxel_f((water_voxel - surr[3]) * SPREAD_AMOUNT, x, y, z - 1, CHANNEL);

	// 				if (water_voxel < -.9 && water_voxel_down < -.9)
	// 				{
	// 					stable_voxels_count += 1;
	// 				}
	// 			}
	// 			else
	// 			{
	// 				// Transfer down

	// 				// Transfer all water
	// 				domain->read_water_buffer->ptr()->set_voxel_f(1.0, x, y, z, CHANNEL);
	// 				domain->read_water_buffer->ptr()->set_voxel_f(water_voxel, x, y - 1, z, CHANNEL);
	// 			}
	// 		}
	// 	}

	// 	if (stable_voxels_count == voxels_per_level)
	// 	{
	// 		domain->stable_levels += 1;
	// 		PRINT("STABLE LEVEL");
	// 	}
	// }

	// // domain->water->ptr()->fill_f(1, CHANNEL);
	// water_tool.ptr()->paste(domain->origin, *domain->read_water_buffer, 1 << CHANNEL);

}


void WaterSimulation::update_water(Vector3i origin, Vector3i size)
{
	WaterDomain* water_domain = new WaterDomain(origin, size, water_tool, terrain_tool);
	active_domains.push_back(water_domain);
}

void WaterSimulation::_bind_methods()
{	
	// Terrain getter / setter
	ClassDB::bind_method(D_METHOD("get_terrain"), &WaterSimulation::get_terrain);
	ClassDB::bind_method(D_METHOD("set_terrain", "terrain"), &WaterSimulation::set_terrain);

	// Water getter / setter
	ClassDB::bind_method(D_METHOD("get_water"), &WaterSimulation::get_water);
	ClassDB::bind_method(D_METHOD("set_water", "water"), &WaterSimulation::set_water);

	// Simulation Timestep getter / setter
	ClassDB::bind_method(D_METHOD("get_simulation_timestep"), &WaterSimulation::get_simulation_timestep);
	ClassDB::bind_method(D_METHOD("set_simulation_timestep", "simulation_timestep"), &WaterSimulation::set_simulation_timestep);


	ClassDB::bind_method(D_METHOD("update_water", "origin", "size"), &WaterSimulation::update_water);

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "Terrain"), "set_terrain", "get_terrain");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "Water"), "set_water", "get_water");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "Simulation Timestep (miliseconds)"), "set_simulation_timestep", "get_simulation_timestep");
}