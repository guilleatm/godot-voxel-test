class_name test_script extends Node3D

@export var terrain : VoxelLodTerrain
@export var camera : Camera3D

var vt: VoxelTool
var buffer : VoxelBuffer = VoxelBuffer.new();
var buffer_size : int;

func _ready():
	vt = terrain.get_voxel_tool()

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
	

func my_remove() -> void:
	var forward : Vector3 = -camera.basis.z.normalized();
	var max_distance : float = 10000.0;
	var raycast_result : VoxelRaycastResult = vt.raycast(global_position, forward, max_distance);

	if (raycast_result != null):
		vt.channel = VoxelBuffer.CHANNEL_SDF;
		vt.mode = VoxelTool.MODE_REMOVE;
		
		# DO SPHERE IS WORKING FINE
		#vt.do_sphere(Vector3(raycast_result.position), buffer_size);

		# DO BOX IS NOT WORKING
		var end : Vector3i = raycast_result.position + Vector3i.ONE * buffer_size;
		vt.do_box(raycast_result.position, end);
	

func my_copy() -> void:
	var forward : Vector3 = -camera.basis.z.normalized();
	
	var max_distance : float = 10000.0;
	var raycast_result : VoxelRaycastResult = vt.raycast(global_position, forward, max_distance);

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
	
	var max_distance : float = 10000.0;
	var raycast_result : VoxelRaycastResult = vt.raycast(global_position, forward, max_distance);

	if (raycast_result != null):
		#var channels_mask : int = 1 << VoxelBuffer.CHANNEL_SDF;
		var channels_mask : int = 0xff;
		
		# var pos : Vector3i = raycast_result.position + Vector3i(-forward * buffer_size);

		vt.paste(raycast_result.previous_position, buffer, channels_mask);
		
		print("Buffer of size " + str(buffer.get_size()) + " pasted to voxel " + str(raycast_result.position));
