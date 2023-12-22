#include "water_simulation.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

WaterSimulation::WaterSimulation() {
	

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
	terrain = get_node<VoxelTerrain>(terrain_node_path);
	water = get_node<VoxelTerrain>(water_node_path);

	time = Time::get_singleton();

}

void WaterSimulation::_process(double delta) {

	int current_time = time->get_ticks_msec();
	int elapsed_time = current_time - last_step_time;

	if (elapsed_time < simulation_timestep_mseconds) return;

	last_step_time = current_time;

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

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "Terrain"), "set_terrain", "get_terrain");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "Water"), "set_water", "get_water");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "Simulation Timestep"), "set_simulation_timestep", "get_simulation_timestep");
}