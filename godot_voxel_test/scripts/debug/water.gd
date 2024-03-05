extends VoxelTerrain;

@export var terrain: VoxelTerrain;

var terrain_vt: VoxelTool; # Keep reference

func _ready():

	var water_generator = WaterGenerator.new();
	terrain_vt = terrain.get_voxel_tool();
	water_generator.prepare(terrain_vt);
	generator = water_generator;
