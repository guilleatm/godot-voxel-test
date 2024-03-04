extends Node

@export_group("Water")
@export var water_simulation: WaterSimulation;

@export_group("Debug")
@export var debug_draw_domains: bool;


func _process(_delta: float) -> void:
	
	if (debug_draw_domains):
		draw_domains();


func draw_domains() -> void:
	var domain_count: int = water_simulation.get_domain_count();
	
	for i in range(domain_count):
		var domain_aabb: AABB = water_simulation.get_domain_aabb(i);
		DebugDraw3D.draw_aabb(domain_aabb, Color.YELLOW_GREEN);


func pause_simulation() -> void:
	water_simulation.pause();

func resume_simulation() -> void:
	water_simulation.resume();

