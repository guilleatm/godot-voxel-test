#ifndef DOMAIN_H
#define DOMAIN_H

#include <godot_cpp/classes/node3d.hpp>


namespace godot
{

class VoxelTerrain;
class VoxelTool;
class VoxelBuffer;

class Domain : public Node3D
{
	GDCLASS(Domain, Node3D)

private:
	AABB m_aabb;
	VoxelTerrain* m_terrain = nullptr;
	VoxelTerrain* m_water = nullptr;

	Ref<VoxelTool> m_water_tool;

	AABB get_aabb() const;
	void set_aabb(const AABB aabb);

	VoxelTerrain* get_terrain() const;
	void set_terrain(VoxelTerrain* terrain);

	VoxelTerrain* get_water() const;
	void set_water(VoxelTerrain* water);

	void process_water_height();
	void process_water_xz();
	void try_add_water(float value, Vector3i v, Ref<VoxelBuffer> buffer);


protected:
	static void _bind_methods();


public:
	Domain();
	~Domain();

	void _enter_tree() override;
	void _process(double delta) override;
};

}

#endif // DOMAIN_H