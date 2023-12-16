class_name test_script extends Node3D

@export var terrain : VoxelLodTerrain
@export var camera : Camera3D

var vt: VoxelTool
var buffer : VoxelBuffer = VoxelBuffer.new();

func _ready():
	vt = terrain.get_voxel_tool()

	vt.channel = VoxelBuffer.CHANNEL_SDF
	
	# vt.mode = VoxelTool.MODE_ADD # Not necessary
	# vt.value = 1 # Not necessary

	#vt.do_sphere(Vector3.ONE * 3, 2.5)
	#vt.do_box(Vector3i.ONE, Vector3i.ONE * 10)


func _unhandled_input(event):
	if event is InputEventKey:
#		if event.pressed and event.keycode == KEY_SPACE:
#			test()
#		if event.pressed and event.keycode == KEY_R:
#			test2()
		if event.pressed and event.keycode == KEY_C:
			my_copy()
		if event.pressed and event.keycode == KEY_V:
			my_paste()
	
	
	
func my_copy() -> void:
	var forward : Vector3 = -camera.basis.z.normalized();
	
	var max_distance : float = 10000.0;
	var raycast_result : VoxelRaycastResult = vt.raycast(global_position, forward, max_distance);

	if (raycast_result != null):
		buffer.create(terrain.mesh_block_size, terrain.mesh_block_size, terrain.mesh_block_size);

		var channels_mask : int = 1 << VoxelBuffer.CHANNEL_SDF;
		# var channels_mask : int = 0xff;
		vt.copy(raycast_result.position, buffer, channels_mask);
		
		print("Buffer of size " + str(buffer.get_size()) + " copied from voxel " + str(raycast_result.position));
	
	
func my_paste() -> void:
	var forward : Vector3 = -camera.basis.z.normalized();
	
	var max_distance : float = 10000.0;
	var raycast_result : VoxelRaycastResult = vt.raycast(global_position, forward, max_distance);

	if (raycast_result != null):
		var channels_mask : int = 1 << VoxelBuffer.CHANNEL_SDF;
		# var channels_mask : int = 0xff;

		vt.paste(raycast_result.position, buffer, channels_mask);
		
		print("Buffer of size " + str(buffer.get_size()) + " pasted to voxel " + str(raycast_result.position));

#func test2():
#	var forward : Vector3 = -camera.basis.z.normalized();
#
#	var max_distance : float = 10000.0;
#	var raycast_result : VoxelRaycastResult = vt.raycast(global_position, forward, max_distance);
#
#	if (raycast_result == null):
#		print("Raycast from " + str(global_position) + " dir: " + str(forward) + " is null.");
#
#	else:
#		print("Raycast from " + str(global_position) + " dir: " + str(forward) + " hit voxel " + str(raycast_result.position));
#
#		var buffer : VoxelBuffer = VoxelBuffer.new();
#		buffer.create(terrain.mesh_block_size, terrain.mesh_block_size, terrain.mesh_block_size);
#		var channels_mask : int = 1 << VoxelBuffer.CHANNEL_SDF;
#		# var channels_mask : int = 0xff;
#
#		vt.copy(raycast_result.position, buffer, channels_mask);
#
#		var offset : Vector3i = Vector3i.RIGHT * 4;
#
#		vt.paste(raycast_result.position + offset, buffer, channels_mask);
#
#func test():
#
#	var origin : Vector3i = Vector3i(global_position);
#
#	var buffer : VoxelBuffer = VoxelBuffer.new();
#	var channels_mask : int = 1 << VoxelBuffer.CHANNEL_SDF;
#
#	vt.copy(origin, buffer, channels_mask);
#
#	var offset : Vector3i = Vector3i.ONE * 4;
#
#	vt.paste(origin + offset, buffer, channels_mask);
#

