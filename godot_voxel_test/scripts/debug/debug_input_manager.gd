extends Node;

@export var water_simulation_manager: WaterSimulationManager;
#@export var water: VoxelTerrain;

func _input(event: InputEvent) -> void:
	
	if (event is InputEventMouseButton):
		handle_mouse_button(event);
		
	if (event is InputEventKey):
		handle_key_button(event);

func handle_mouse_button(event: InputEventMouseButton) -> void:
	if (event.button_index == MOUSE_BUTTON_LEFT and event.is_pressed()):
		water_simulation_manager.create_water();

var is_playing = true;
func handle_key_button(event: InputEventKey):
	if (event.is_action_pressed("toggle_simulation_pause")):
		if (is_playing):
			water_simulation_manager.pause_simulation();
		else:
			water_simulation_manager.resume_simulation();
		is_playing = !is_playing;
