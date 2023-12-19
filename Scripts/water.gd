extends VoxelTerrain

@export var terrain: VoxelTerrain;

const channel: int = VoxelBuffer.CHANNEL_SDF;
const channel_mask: int = 1 << channel;
const water_generator = preload("res://Scripts/water_generator.gd");

var water_vt: VoxelTool;
var terrain_vt: VoxelTool;

func _ready():
	generator = water_generator.new(terrain);
	
	water_vt = get_voxel_tool();
	terrain_vt = terrain.get_voxel_tool();
	
	#water_vt.channel = global.CHANNEL;


func _process(delta):
	pass
	

func is_there_water(area_origin: Vector3i, area_size: Vector3i) -> Array:

	var water_buffer: VoxelBuffer = VoxelBuffer.new();
	water_buffer.create(area_size.x, area_size.y, area_size.z);
	water_vt.copy(area_origin, water_buffer, channel_mask);
	
	for y in area_size.y:
		for x in area_size.x:
			for z in area_size.z:
				if (water_buffer.get_voxel_f(x, y, z, channel) < 0):
					return [true, Vector3i(x, y, z)];
					
	return [false, Vector3i.ZERO];



	
func on_area_edited(area_origin: Vector3i, area_size: Vector3i) -> void:
	
	var is_there_water_result: Array = is_there_water(area_origin - Vector3i.ONE, area_size + Vector3i.ONE * 2);
	
	if (is_there_water_result[0]):
		print("water in " + str(is_there_water_result[1]));
	else:
		print("no water");
	
#	var water_buffer: VoxelBuffer = VoxelBuffer.new();
#	water_buffer.create(area_size.x, area_size.y, area_size.z);
#	water_vt.copy(area_origin, water_buffer, channel_mask);
#
#
#	var terrain_buffer: VoxelBuffer = VoxelBuffer.new();
#	terrain_buffer.create(area_size.x, area_size.y, area_size.z);
#	terrain_vt.copy(area_origin, terrain_buffer, channel_mask);
#
#	for y in area_size.y:
#		if (area_origin.y + y > 0):
#			break;
#
#		for x in area_size.x:
#			for z in area_size.z:
#				var terrain_voxel: float = terrain_buffer.get_voxel_f(x, y, z, channel);
#				if (terrain_voxel < 0):
#					water_buffer.set_voxel_f(-1.0, x, y, z, channel);
#
#	water_vt.paste(area_origin, water_buffer, channel);
