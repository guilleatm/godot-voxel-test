#include "water_simulation.h"
#include <godot_cpp/core/class_db.hpp>

#include "global.h"

#include <godot_cpp/classes/time.hpp>

#include "water_domain.h"

using namespace godot;

WaterSimulation::WaterSimulation()
{
	domains = std::vector<WaterDomain*>();
}

WaterSimulation::~WaterSimulation()
{
	for (int i = 0; i < domains.size(); i++)
	{
		delete domains[i];
	}
}


NodePath WaterSimulation::get_terrain() const
{
	return terrain_node_path;
}

void WaterSimulation::set_terrain(NodePath node_path)
{
	terrain_node_path = node_path;
}

NodePath WaterSimulation::get_water() const
{
	return water_node_path;
}

void WaterSimulation::set_water(NodePath node_path)
{
	water_node_path = node_path;
}

int WaterSimulation::get_simulation_timestep() const
{
	return simulation_timestep_mseconds;
}

void WaterSimulation::set_simulation_timestep(int _simulation_timestep_mseconds)
{
	simulation_timestep_mseconds = _simulation_timestep_mseconds;
}


void WaterSimulation::_enter_tree()
{

	DO_NOT_PLAY_IN_EDITOR;

	terrain = get_node<VoxelTerrain>(terrain_node_path);
	water = get_node<VoxelTerrain>(water_node_path);

	terrain_tool = terrain->get_voxel_tool();
	water_tool = water->get_voxel_tool();

	time = Time::get_singleton();
}

void WaterSimulation::_process(double delta)
{
	DO_NOT_PLAY_IN_EDITOR;

	if (!update_simulation) return;

	int current_time = time->get_ticks_msec();
	int elapsed_time = current_time - last_step_time;

	if (elapsed_time < simulation_timestep_mseconds) return;

	last_step_time = current_time;


	for (int i = domains.size() - 1; i >= 0; i--)
	{
		WaterDomain* domain = domains[i];
		domain->update();

		// if (domains[i]->is_stable())
		// {
		// 	domains.erase(domains.begin() + i);
		// 	delete domain;
		// }
	}
}


void WaterSimulation::create_domain(Vector3i origin, Vector3i size)
{
	WaterDomain* water_domain = new WaterDomain(origin, size, water_tool, terrain_tool);
	domains.push_back(water_domain);
}

// void WaterSimulation::domain_to_water(int index)
// {
// 	domains[index]->to_water();
// }

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


	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "terrain"), "set_terrain", "get_terrain");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "water"), "set_water", "get_water");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "simulation_timestep_msec"), "set_simulation_timestep", "get_simulation_timestep");

	ClassDB::bind_method(D_METHOD("create_domain", "origin", "size"), &WaterSimulation::create_domain);

	ClassDB::bind_method(D_METHOD("get_domain_count"), &WaterSimulation::get_domain_count);
	ClassDB::bind_method(D_METHOD("get_domain_aabb", "index"), &WaterSimulation::get_domain_aabb);
	ClassDB::bind_method(D_METHOD("pause"), &WaterSimulation::pause);
	ClassDB::bind_method(D_METHOD("resume"), &WaterSimulation::resume);
	ClassDB::bind_method(D_METHOD("get_water_node"), &WaterSimulation::get_water_node);
	ClassDB::bind_method(D_METHOD("get_terrain_node"), &WaterSimulation::get_terrain_node);

}

// API

int WaterSimulation::get_domain_count() const
{
	return domains.size();
}

AABB WaterSimulation::get_domain_aabb(int index) const
{
	return domains[index]->aabb;
}

void WaterSimulation::pause() const
{
	update_simulation = false;
}

void WaterSimulation::resume() const
{
	update_simulation = true;
}

VoxelTerrain* WaterSimulation::get_water_node() const
{
	return water;
}

VoxelTerrain* WaterSimulation::get_terrain_node() const
{
	return terrain;
}