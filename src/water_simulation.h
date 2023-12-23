#ifndef WATER_SIMULATION_H
#define WATER_SIMULATION_H

#include <vector>
#include <godot_cpp/classes/voxel_generator_script.hpp>
#include <godot_cpp/classes/voxel_buffer.hpp>
#include <godot_cpp/classes/voxel_terrain.hpp>
#include <godot_cpp/classes/voxel_tool.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <math.h>

#include "water_domain.h"


#define DO_NOT_PLAY_IN_EDITOR if(Engine::get_singleton()->is_editor_hint()) return
#define PRINT(message) UtilityFunctions::print(message)

namespace godot {

class WaterSimulation : public Node {
	GDCLASS(WaterSimulation, Node)

private:

	NodePath terrain_node_path;
	NodePath water_node_path;

	VoxelTerrain* terrain;
	VoxelTerrain* water;

	Ref<VoxelTool> terrain_tool;
	Ref<VoxelTool> water_tool;

	Time* time;
	int simulation_timestep_mseconds;

	int last_step_time = 0;

	bool update_simulation;

	std::vector<WaterDomain*> active_domains;

	void update_domain(WaterDomain* domain);
	bool voxel_is_empty(float voxel);

protected:
	static void _bind_methods();

public:
	WaterSimulation();
	~WaterSimulation();

	NodePath get_terrain() const;
	void set_terrain(NodePath _terrain);

	NodePath get_water() const;
	void set_water(NodePath _water);

	int get_simulation_timestep() const;
	void set_simulation_timestep(int _simulation_timestep_mseconds);

	void _ready() override;
	void _process(double delta) override;

	void update_water(Vector3i origin, Vector3i size);

	// void m_create(VoxelTerrain* terrain);

	// void _generate_block(const Ref<VoxelBuffer> &out_buffer, const Vector3i &origin_in_voxels, int lod) override;
	// int _get_used_channels_mask() const override;
};

}

#endif // WATER_SIMULATION_H