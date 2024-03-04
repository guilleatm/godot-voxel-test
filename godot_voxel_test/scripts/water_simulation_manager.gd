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

func _enter_tree():
	
	if (auto_create_domain):
		var terrain: Terrain = water_simulation.get_node(water_simulation.terrain);
		(terrain as Terrain).area_edited.connect( create_domain );


func _process(_delta: float) -> void:
	
	if (debug_draw_domains):
		draw_domains();


func draw_domains() -> void:
	var domain_count: int = water_simulation.get_domain_count();
	
	for i in range(domain_count):
		var domain_aabb: AABB = water_simulation.get_domain_aabb(i);
		DebugDraw3D.draw_aabb(domain_aabb, Color.YELLOW_GREEN);

func create_domain(area_origin: Vector3i, area_size: Vector3i) -> void:
	water_simulation.create_domain(area_origin, area_size);

func pause_simulation() -> void:
	water_simulation.pause();

func resume_simulation() -> void:
	water_simulation.resume();
