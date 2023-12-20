extends WaterGenerator

var testInt: int = 28;
#
#const channel: int = VoxelBuffer.CHANNEL_SDF;
#var channel_mask: int = _get_used_channels_mask();
#
##var mutex: Mutex = Mutex.new()
#
#var terrain_tool: VoxelTool;
#
#func _init(terrain: VoxelTerrain):
#	terrain_tool = terrain.get_voxel_tool();
#
#
#func _get_used_channels_mask() -> int:
#	return 1 << channel;
#
#func debug(msg: String):
#	print(msg);
#
#func tprint(msg: String):
#	call_deferred("debug", msg)
#
#func _generate_block( out_buffer: VoxelBuffer, origin_in_voxels: Vector3i, lod: int ) -> void:
#	if (lod != 0): return;
#
#	if (origin_in_voxels.y > 0): return;
#
#	var size: Vector3i = out_buffer.get_size();
#
#	var terrain_buffer: VoxelBuffer = VoxelBuffer.new(); # this is per thread to avoid multithreading issues, we can use mutext too (profile)
#	terrain_buffer.create(size.x, size.y, size.z);
#
#	terrain_tool.copy(origin_in_voxels, terrain_buffer, channel_mask);
#
##	if (terrain_buffer.is_uniform(channel)): return;
#
#	for y in size.y:
#		if (origin_in_voxels.y + y > 0):
#			break;
#
#		for x in size.x:
#			for z in size.z:
#				var terrain_voxel: float = terrain_buffer.get_voxel_f(x, y, z, channel);
#				if (terrain_voxel > 0):
#					out_buffer.set_voxel_f(-1.0, x, y, z, channel);
#				else:
#					out_buffer.set_voxel_f(1.0, x, y, z, channel);
#
#
#
#
##	for y in size.y:
##		if (y >= 0):
##			continue;
##		for x in size.x:
##			for z in size.z:
##
##				var pos: Vector3i = Vector3i(x, y, z);
##				var terrain_voxel: float = terrain_tool.get_voxel_f(pos);
##
##				if (terrain_voxel >= 0):
##					out_buffer.set_voxel_f(-1.0, x, y, z, channel);
