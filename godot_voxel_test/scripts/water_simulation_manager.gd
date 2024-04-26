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
@export var debug_draw_sdf_water: bool;
@export var debug_draw_height_info: bool;

func _enter_tree():
	if (auto_create_domain):
		var terrain: Terrain = water_simulation.get_node(water_simulation.terrain);
		(terrain as Terrain).area_edited.connect( create_domain );

func _process(_delta: float) -> void:
	
	if (debug_draw_domains):
		draw_domains();
		
	if (debug_draw_sdf_water or debug_draw_height_info):
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
	buffer.set_channel_depth(VoxelBuffer.CHANNEL_DATA5, VoxelBuffer.DEPTH_8_BIT);

	
	for i in range(domain_count):
		var domain_aabb: AABB = water_simulation.get_domain_aabb(i);
		
		buffer.clear();
		buffer.create(int(domain_aabb.size.x), int(domain_aabb.size.y), int(domain_aabb.size.z))
		water_vt.copy(domain_aabb.position, buffer, 1 << VoxelBuffer.CHANNEL_SDF | 1 << VoxelBuffer.CHANNEL_DATA5);
		
		for x in range(int(domain_aabb.size.x)):
			for y in range(int(domain_aabb.size.y)):
				for z in range(int(domain_aabb.size.z)): 
					var pos: Vector3i = Vector3i(x, y, z);
					var sdf_water: float = buffer.get_voxel_f(x, y, z, VoxelBuffer.CHANNEL_SDF);

					if (debug_draw_sdf_water and sdf_water < 0):
						DebugDraw3D.draw_sphere(Vector3i(domain_aabb.position) + pos, .2, Color.BLUE);


					if (debug_draw_height_info):
						var origin: int = buffer.get_voxel(x, 0, z, VoxelBuffer.CHANNEL_DATA5);
						var n: int = buffer.get_voxel(x, 1, z, VoxelBuffer.CHANNEL_DATA5);
						
						var height: int = n / 5; # 5 -> COMPLETE_WATER
						var water: int = n % 5; # 5 -> COMPLETE_WATER
					
						var normalized: float = inverse_lerp(0, domain_aabb.size.y, height);
						var color: Color = Color(n, n, .7);
					
						DebugDraw3D.draw_sphere(Vector3i(domain_aabb.position) + Vector3i(pos.x, origin, pos.z), .5, Color.BLACK);
						DebugDraw3D.draw_sphere(Vector3i(domain_aabb.position) + Vector3i(pos.x, origin + max(0, height - 1), pos.z), .5, color);


func create_domain(area_origin: Vector3i, area_size: Vector3i) -> void:
	water_simulation.create_domain(area_origin, area_size);

func pause_simulation() -> void:
	water_simulation.pause();

func resume_simulation() -> void:
	water_simulation.resume();


func create_water():
	const WATER_SIZE: Vector3i = Vector3i.ONE * 8;
	
	var water: VoxelTerrain = water_simulation.get_water_node();
	var terrain: VoxelTerrain = water_simulation.get_terrain_node();

	var water_vt: VoxelTool = water.get_voxel_tool();
	var terrain_vt: VoxelTool = terrain.get_voxel_tool();

	var camera_forward : Vector3 = -get_viewport().get_camera_3d().basis.z.normalized();
	var camera_position : Vector3 = get_viewport().get_camera_3d().global_position;
	var raycast_result : VoxelRaycastResult = terrain_vt.raycast(camera_position, camera_forward, 1000);

	if (raycast_result):
		var base_offset: Vector3i = Vector3i(0, 3, 0);
		var sphere_offset: Vector3i = Vector3i(4, 4, 4) + base_offset;
		water_vt.do_sphere(raycast_result.position + sphere_offset, WATER_SIZE.length() / 4);

		var domain_offset: Vector3i = Vector3i(0, 0, 0) + base_offset;
		water_simulation.create_domain(raycast_result.position + domain_offset, WATER_SIZE);
