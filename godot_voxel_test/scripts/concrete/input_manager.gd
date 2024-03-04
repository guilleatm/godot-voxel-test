extends Node

@export var water_simulation_manager: WaterSimulationManager;

func _input(event: InputEvent) -> void:
	
	if (event.is_action("ui_accept")):
		pass;
