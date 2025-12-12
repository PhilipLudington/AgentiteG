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
#include "arrays/array_ops.hpp"

using namespace godot;

void initialize_agentite_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    // Register spatial classes
    ClassDB::register_class<SpatialHash2D>();

    // Register array operations (static methods via singleton)
    ClassDB::register_class<ArrayOps>();
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
