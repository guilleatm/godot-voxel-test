#include "water_simulation.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;


WaterSimulation::WaterSimulation() {
	active_domains = std::vector<WaterDomain*>();
}

WaterSimulation::~WaterSimulation() {

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


	PRINT("process");

	// for (int i = active_domains.size() - 1; i >= 0; i--) {
	// 	update_domain(active_domains[i]);

	// 	// if (active_domains[i]->stable) {
	// 	// 	WaterDomain* stable_domain = active_domains[i];
	// 	// 	active_domains.erase(active_domains.begin() + i);
	// 	// 	delete stable_domain;
	// 	// }
	// }
}

bool WaterSimulation::voxel_is_empty(float voxel) {
	return voxel >= 0;
}

void WaterSimulation::update_domain(WaterDomain* domain) {

	PRINT("update domain");

	const int CHANNEL = VoxelBuffer::CHANNEL_SDF;

	for (int y = 1; y < domain->size.y; y++) {
		for (int x = 0; x < domain->size.x; x++) {
			for (int z = 0; z < domain->size.z; z++) {
				float water_voxel = domain->water.ptr()->get_voxel_f(x, y, z, CHANNEL);
				float terrain_voxel_down = domain->terrain.ptr()->get_voxel_f(x, y - 1, z, CHANNEL);

				PRINT("accessed to voxels");

				if (water_voxel < 0) {
					if (voxel_is_empty(terrain_voxel_down)) {
						float water_voxel_down = domain->water->get_voxel_f(x, y - 1, z, CHANNEL);

						if (voxel_is_empty(water_voxel_down)) {
							domain->water->set_voxel_f(0, x, y, z, CHANNEL);
							domain->water->set_voxel_f(-1, y - 1, z, CHANNEL);

							PRINT("set voxels");

							if (domain->stable) {
								domain->stable = false;
							}
						}
					}
				}

			}
		}
	}

	water_tool.ptr()->paste(domain->origin, domain->water, 1 << CHANNEL);

	PRINT("pasted");

}


void WaterSimulation::update_water(Vector3i origin, Vector3i size) {

	PRINT("Update water start");

	const int CHANNEL_MASK = 1 << VoxelBuffer::CHANNEL_SDF;

	VoxelBuffer terrain_buffer = VoxelBuffer();
	terrain_buffer.create(size.x, size.y, size.z);

	VoxelBuffer water_buffer = VoxelBuffer();
	water_buffer.create(size.x, size.y, size.z);

	Ref<VoxelBuffer> ref_terrain_buffer = Ref<VoxelBuffer>(&terrain_buffer);
	Ref<VoxelBuffer> ref_water_buffer = Ref<VoxelBuffer>(&water_buffer);

	PRINT("Created buffers");

	terrain_tool.ptr()->copy(origin, ref_terrain_buffer, CHANNEL_MASK);
	water_tool.ptr()->copy(origin, ref_water_buffer, CHANNEL_MASK);

	PRINT("Copied buffers");

	WaterDomain* water_domain = new WaterDomain(origin, ref_water_buffer, ref_terrain_buffer);
	active_domains.push_back(water_domain);

	PRINT("Created and pushed water domain");


	update_simulation = true;
}

void WaterSimulation::_bind_methods() {
	
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