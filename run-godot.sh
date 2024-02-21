GODOT_PID="$(ps -e | pgrep godot)"

if [ ! -z $GODOT_PID ]
then
    kill $GODOT_PID
fi

scons

GODOT_PATH="../executables/godot-voxel-4.1.x86_64"

./$GODOT_PATH &