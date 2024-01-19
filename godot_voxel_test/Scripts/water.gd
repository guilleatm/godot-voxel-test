extends VoxelTerrain

@export var terrain: VoxelTerrain;

const ch_sdf: int = VoxelBuffer.CHANNEL_SDF;
const ch_water: int = VoxelBuffer.CHANNEL_DATA5;

const channel_mask: int = 1 << ch_sdf;

var water_vt: VoxelTool;
var terrain_vt: VoxelTool;

var origin: Vector3i;
var size: Vector3i;
@export var draw: bool;
var draw_started: bool = false;

func _ready():

	var water_generator = WaterGenerator.new();
	water_generator.create(terrain);
	
	generator = water_generator;
	
	water_vt = get_voxel_tool();
	water_vt.channel = ch_water;
	terrain_vt = terrain.get_voxel_tool();
	
	#water_vt.channel = global.CHANNEL;


func _process(delta):
	
	if (draw_started and draw):
		debug_draw()
	pass
	

func debug_draw() -> void:
	var water_buffer: VoxelBuffer = VoxelBuffer.new();
	water_buffer.create(size.x, size.y, size.z);
	water_buffer.set_channel_depth(ch_water, VoxelBuffer.DEPTH_8_BIT);
	
	water_vt.copy(origin, water_buffer, 1 << ch_water | 1 << ch_sdf);
	
	for y in size.y:
		for x in size.x:
			for z in size.z:
				draw_voxel(water_buffer, x, y, z);

func draw_voxel(water_buffer: VoxelBuffer, x: int, y: int, z: int) -> void:
	
	var water_voxel: int = water_buffer.get_voxel(x, y, z, ch_water);

	if (water_voxel == 0): return;
	
	var color: Color;
	var n: float = 0.1;
	
	if (water_voxel > 250):
		color = Color.RED;
	else:
		n += inverse_lerp(float(0), float(255), water_voxel);
		color = Color.DARK_BLUE

#	var color: Color;
#	if (water_voxel == 0):
#		return;
#		color = Color.BLACK;
#	elif (water_voxel == 5):
#		color = Color.NAVY_BLUE;
#	elif (0 < water_voxel and water_voxel < 5):
#		color = Color.WEB_PURPLE;
#	else:
#		color = Color.RED;


	DebugDraw3D.draw_sphere(origin + Vector3i(x, y, z), n, color);

func start_debug_draw(o: Vector3i, s: Vector3) -> void:
	origin = o;
	size = s;
	draw_started = true;

#func is_there_water(area_origin: Vector3i, area_size: Vector3i) -> Array:
#
#	var water_buffer: VoxelBuffer = VoxelBuffer.new();
#	water_buffer.create(area_size.x, area_size.y, area_size.z);
#	water_vt.copy(area_origin, water_buffer, channel_mask);
#
#	for y in area_size.y:
#		for x in area_size.x:
#			for z in area_size.z:
#				if (water_buffer.get_voxel_f(x, y, z, channel) < 0):
#					return [true, Vector3i(x, y, z)];
#
#	return [false, Vector3i.ZERO]; 



	
func on_area_edited(area_origin: Vector3i, area_size: Vector3i) -> void:
	pass
	
#	var is_there_water_result: Array = is_there_water(area_origin - Vector3i.ONE, area_size + Vector3i.ONE * 2);
#
#	if (is_there_water_result[0]):
#		print("water in " + str(is_there_water_result[1]));
#	else:
#		print("no water");
#
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


func _on_terrain_on_area_edited(area_origin, area_size):
	pass # Replace with function body.
