GODOT_PID="$(ps -e | pgrep godot)"

if [ ! -z $GODOT_PID ]
then
    kill $GODOT_PID
fi

GODOT_PATH="../executables/godot-voxel-4.1.x86_64"
PROJECT_PATH="godot_voxel_test"

scons && ./$GODOT_PATH --path $PROJECT_PATH --editor &
