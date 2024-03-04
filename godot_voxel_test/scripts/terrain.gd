extends VoxelTerrain
class_name Terrain

signal area_edited(area_origin: Vector3i, area_size: Vector3i);

func _enter_tree():
	area_edit_notification_enabled = true;


func _on_area_edited (area_origin: Vector3i, area_size: Vector3i) -> void:
	area_edited.emit(area_origin, area_size);
