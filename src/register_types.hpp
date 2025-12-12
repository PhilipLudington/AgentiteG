/**
 * AgentiteG - GDExtension Registration Header
 */

#ifndef AGENTITE_REGISTER_TYPES_HPP
#define AGENTITE_REGISTER_TYPES_HPP

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_agentite_module(ModuleInitializationLevel p_level);
void uninitialize_agentite_module(ModuleInitializationLevel p_level);

#endif // AGENTITE_REGISTER_TYPES_HPP
