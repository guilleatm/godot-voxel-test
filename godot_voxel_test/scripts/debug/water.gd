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
	water_generator.prepare(terrain.get_voxel_tool());

	generator = water_generator;
#
#	water_vt = get_voxel_tool();
#	water_vt.channel = ch_water;
#	terrain_vt = terrain.get_voxel_tool();
	
	#water_vt.channel = global.CHANNEL;
	pass;

func _process(_delta: float):
	
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
	
#	var water_voxel: int = water_buffer.get_voxel(x, y, z, ch_water);
	var sdf: float = water_buffer.get_voxel_f(x, y, z, ch_sdf);

#	if (water_voxel == 0): return;
		
	var n: float = inverse_lerp(-1, +1, sdf);
	var color: Color = lerp(Color.BLACK, Color.WHITE, n);


	DebugDraw3D.draw_sphere(origin + Vector3i(x, y, z), .1, color);

func start_debug_draw(o: Vector3i, s: Vector3) -> void:
	origin = o;
	size = s;
	draw_started = true;
