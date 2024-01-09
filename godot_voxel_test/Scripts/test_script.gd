class_name global extends Node3D


@export var terrain : Terrain
@export var water : VoxelTerrain
@export var camera : Camera3D

#const water_generator = preload("res://Scripts/water_generator.gd");

var vt: VoxelTool
var water_vt: VoxelTool
var buffer : VoxelBuffer = VoxelBuffer.new();
var buffer_size : int;
var raycast_distance : float = 10_000.0;

func _ready():
	vt = terrain.get_voxel_tool();
	water_vt = water.get_voxel_tool();

	vt.channel = VoxelBuffer.CHANNEL_SDF # overriding this in my_remove

	buffer_size = terrain.mesh_block_size;


	
	# vt.mode = VoxelTool.MODE_ADD # Not necessary
	# vt.value = 1 # Not necessary

	#vt.do_sphere(Vector3.ONE * 3, 2.5)
	#vt.do_box(Vector3i.ONE, Vector3i.ONE * 10)


func _unhandled_input(event):
	if event is InputEventKey:
		if event.pressed and event.keycode == KEY_C:
			my_copy()
		if event.pressed and event.keycode == KEY_V:
			my_paste()
		if event.pressed and event.keycode == KEY_R:
			my_remove()
		if event.pressed and event.keycode == KEY_N:
			create_water()

func create_water() -> void:
	var forward : Vector3 = -camera.basis.z.normalized();
	var raycast_result : VoxelRaycastResult = vt.raycast(global_position, forward, raycast_distance);

	if (raycast_result != null):
		water_vt.channel = VoxelBuffer.CHANNEL_SDF;
		water_vt.mode = VoxelTool.MODE_ADD;

		water_vt.do_sphere(Vector3(raycast_result.position) + Vector3.UP * buffer_size / 2, buffer_size / 2);

		var k : int = 10;
		var o: Vector3i = (raycast_result.position - Vector3i.UP * k);
		terrain._on_area_edited(o, Vector3i.ONE * buffer_size + Vector3i.UP * k);


func my_remove() -> void:
	var forward : Vector3 = -camera.basis.z.normalized();
	var raycast_result : VoxelRaycastResult = vt.raycast(global_position, forward, raycast_distance);

	if (raycast_result != null):
		vt.channel = VoxelBuffer.CHANNEL_SDF;
		vt.mode = VoxelTool.MODE_REMOVE;
		
		# DO SPHERE IS WORKING FINE
		vt.do_sphere(Vector3(raycast_result.position), buffer_size / 2);

		# DO BOX IS NOT WORKING
#		var box_size : int = 4;
#		var start : Vector3i = raycast_result.position + Vector3i(forward * box_size / 2);
#		var end : Vector3i = raycast_result.position - Vector3i(forward * box_size / 2);
#		vt.do_box(start, end);
	

func my_copy() -> void:
	var forward : Vector3 = -camera.basis.z.normalized();
	
	var raycast_result : VoxelRaycastResult = vt.raycast(global_position, forward, raycast_distance);

	if (raycast_result != null):
		buffer.create(buffer_size, buffer_size, buffer_size);

		# var channels_mask : int = 1 << VoxelBuffer.CHANNEL_SDF;
		var channels_mask : int = 0xff;
		
		# var pos : Vector3i = raycast_result.position + Vector3i(forward * buffer_size);
		vt.copy(raycast_result.position, buffer, channels_mask);
		
		print("Buffer of size " + str(buffer.get_size()) + " copied from voxel " + str(raycast_result.position));
		
#		vt.paste(raycast_result.position + Vector3i.UP, buffer, channels_mask);
	
	
	
func my_paste() -> void:
	var forward : Vector3 = -camera.basis.z.normalized();
	
	var raycast_result : VoxelRaycastResult = vt.raycast(global_position, forward, raycast_distance);

	if (raycast_result != null):
		#var channels_mask : int = 1 << VoxelBuffer.CHANNEL_SDF;
		var channels_mask : int = 0xff;
		
		# var pos : Vector3i = raycast_result.position + Vector3i(-forward * buffer_size);

		vt.paste(raycast_result.previous_position, buffer, channels_mask);
		
		print("Buffer of size " + str(buffer.get_size()) + " pasted to voxel " + str(raycast_result.position));
