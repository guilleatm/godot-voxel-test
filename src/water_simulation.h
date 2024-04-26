#ifndef WATER_SIMULATION_H
#define WATER_SIMULATION_H

#include <godot_cpp/classes/voxel_terrain.hpp>

namespace godot
{
	class Time;
	class WaterDomain;

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

		mutable bool update_simulation = true;

		std::vector<WaterDomain*> domains;

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

		virtual void _enter_tree() override;
		virtual void _process(double delta) override;

		void create_domain(Vector3i origin, Vector3i size);
		// void domain_to_water(int index);


		// API
		
		int get_domain_count() const;
		AABB get_domain_aabb(int index) const;
		AABB get_domain_inner_aabb(int index) const;
		void pause() const;
		void resume() const;

		VoxelTerrain* get_water_node() const;
		VoxelTerrain* get_terrain_node() const;

	};

}

#endif // WATER_SIMULATION_H