#ifndef WATER_SIMULATION_H
#define WATER_SIMULATION_H

#include <godot_cpp/classes/voxel_terrain.hpp>


namespace godot
{

	class WaterDomain;
	class Time;


	class WaterSimulation : public Node
	{
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

		void add_domain(Vector3i origin, Vector3i size);



		// GOOD

		// void add_domain();
		// void remove_domain();
	};

}

#endif // WATER_SIMULATION_H