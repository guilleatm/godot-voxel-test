 
#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <godot_cpp/classes/voxel_generator.hpp>

namespace godot {

class WaterGenerator : public VoxelGenerator {
	GDCLASS(WaterGenerator, VoxelGenerator)

private:


protected:
	static void _bind_methods();

public:
	WaterGenerator();
	~WaterGenerator();

	void _generate_block() override;

	void _process(double delta) override;
};

}

#endif
