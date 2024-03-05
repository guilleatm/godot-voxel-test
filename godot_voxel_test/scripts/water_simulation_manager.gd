extends Node
class_name WaterSimulationManager;

@export_group("Simulation")
@export var water_simulation: WaterSimulation;

## When the terrain is modified a domain is created to update near water.
## Make sure your terrain has the Terrain.gd script attached.
## Changing this at runtime will have no effect.
@export var auto_create_domain: bool;
	
@export_group("Debug")
@export var debug_draw_domains: bool;
@export var debug_draw_water: bool;

func _enter_tree():
	if (auto_create_domain):
		var terrain: Terrain = water_simulation.get_node(water_simulation.terrain);
		(terrain as Terrain).area_edited.connect( create_domain );

func _process(_delta: float) -> void:
	
	if (debug_draw_domains):
		draw_domains();
		
	if (debug_draw_water):
		draw_water();


func draw_domains() -> void:
	var domain_count: int = water_simulation.get_domain_count();
	
	for i in range(domain_count):
		var domain_aabb: AABB = water_simulation.get_domain_aabb(i);
		DebugDraw3D.draw_aabb(domain_aabb, Color.YELLOW_GREEN);

func draw_water() -> void:
	var domain_count: int = water_simulation.get_domain_count();
	var water_vt: VoxelTool = water_simulation.get_water_node().get_voxel_tool();
	
	var buffer: VoxelBuffer = VoxelBuffer.new();
	buffer.create(8, 8, 8);
	buffer.set_channel_depth(VoxelBuffer.CHANNEL_DATA5, VoxelBuffer.DEPTH_8_BIT);
	
	for i in range(domain_count):
		var domain_aabb: AABB = water_simulation.get_domain_aabb(i);
	
		buffer.create(domain_aabb.size.x, domain_aabb.size.y, domain_aabb.size.z)
		water_vt.copy(domain_aabb.position, buffer, 1 << VoxelBuffer.CHANNEL_DATA5);
		
		for x in range(domain_aabb.size.x):
			for y in range(domain_aabb.size.y):
				for z in range(domain_aabb.size.z): 
					var pos: Vector3i = Vector3i(x, y, z);
					var water_voxel = water_vt.get_voxel(pos);
					
					if (water_voxel == 1):
						print("SOMETHIUI");
						DebugDraw3D.draw_sphere(Vector3i(domain_aabb.position) + pos, .5, Color.BLACK);
					else:
						DebugDraw3D.draw_sphere(Vector3i(domain_aabb.position) + pos);
		

func create_domain(area_origin: Vector3i, area_size: Vector3i) -> void:
	water_simulation.create_domain(area_origin, area_size);


func pause_simulation() -> void:
	water_simulation.pause();

func resume_simulation() -> void:
	water_simulation.resume();


func create_water():
	const WATER_SIZE: Vector3 = Vector3.ONE * 8;
	
	var water: VoxelTerrain = water_simulation.get_water_node();
	var terrain: VoxelTerrain = water_simulation.get_terrain_node();

	var water_vt: VoxelTool = water.get_voxel_tool();
	var terrain_vt: VoxelTool = terrain.get_voxel_tool();

	var camera_forward : Vector3 = -get_viewport().get_camera_3d().basis.z.normalized();
	var camera_position : Vector3 = get_viewport().get_camera_3d().global_position;
	var raycast_result : VoxelRaycastResult = terrain_vt.raycast(camera_position, camera_forward, 1000);

	if (raycast_result):
#		water_vt.channel = VoxelBuffer.CHANNEL_DATA5;
		water_vt.do_sphere(raycast_result.position, WATER_SIZE.length() / 2);
		var domain_ix: int = water_simulation.create_domain(raycast_result.position, WATER_SIZE);
		water_simulation.domain_to_water(domain_ix);
