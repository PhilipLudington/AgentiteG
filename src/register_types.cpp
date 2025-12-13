/**
 * AgentiteG - GDExtension Registration
 *
 * This file registers all AgentiteG classes with Godot's ClassDB.
 */

#include "register_types.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>

#include "spatial/spatial_hash_2d.hpp"
#include "spatial/spatial_hash_3d.hpp"
#include "spatial/kd_tree_2d.hpp"
#include "spatial/kd_tree_3d.hpp"
#include "spatial/quad_tree.hpp"
#include "spatial/octree.hpp"
#include "arrays/array_ops.hpp"
#include "math/math_ops.hpp"
#include "batch/batch_ops.hpp"
#include "random/random_ops.hpp"
#include "noise/noise_ops.hpp"
#include "grid/grid_ops.hpp"
#include "pathfinding/pathfinding_ops.hpp"
#include "collision/collision_ops.hpp"
#include "geometry/geometry_ops.hpp"
#include "interpolation/interpolation_ops.hpp"
#include "stats/stat_ops.hpp"

using namespace godot;

void initialize_agentite_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    // Register spatial classes
    ClassDB::register_class<SpatialHash2D>();
    ClassDB::register_class<SpatialHash3D>();
    ClassDB::register_class<KDTree2D>();
    ClassDB::register_class<KDTree3D>();
    ClassDB::register_class<QuadTree>();
    ClassDB::register_class<Octree>();

    // Register array operations (static methods via singleton)
    ClassDB::register_class<ArrayOps>();

    // Register math operations
    ClassDB::register_class<MathOps>();

    // Register batch operations
    ClassDB::register_class<BatchOps>();

    // Register random operations
    ClassDB::register_class<RandomOps>();

    // Register noise operations
    ClassDB::register_class<NoiseOps>();

    // Register grid operations
    ClassDB::register_class<GridOps>();

    // Register pathfinding operations
    ClassDB::register_class<PathfindingOps>();

    // Register collision operations
    ClassDB::register_class<CollisionOps>();

    // Register geometry operations
    ClassDB::register_class<GeometryOps>();

    // Register interpolation operations
    ClassDB::register_class<InterpolationOps>();

    // Register statistics operations
    ClassDB::register_class<StatOps>();
}

void uninitialize_agentite_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    // Cleanup if needed
}

extern "C" {

GDExtensionBool GDE_EXPORT agentiteg_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization *r_initialization
) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_agentite_module);
    init_obj.register_terminator(uninitialize_agentite_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}

}
