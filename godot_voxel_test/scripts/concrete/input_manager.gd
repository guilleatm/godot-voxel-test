extends Node

@export var water_simulation_manager: WaterSimulationManager;
@export var water: VoxelTerrain;

func _input(event: InputEvent) -> void:
	
	if (event is InputEventMouseButton):
		handle_mouse_button(event);

func handle_mouse_button(event: InputEventMouseButton) -> void:
	if (event.button_index == MOUSE_BUTTON_LEFT and event.is_pressed()):
		create_water();


func create_water():
	print("create water!");
