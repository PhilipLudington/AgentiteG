/**
 * SpatialHash3D Implementation
 */

#include "spatial_hash_3d.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace godot {

void SpatialHash3D::_bind_methods() {
    // Properties
    ClassDB::bind_method(D_METHOD("set_cell_size", "size"), &SpatialHash3D::set_cell_size);
    ClassDB::bind_method(D_METHOD("get_cell_size"), &SpatialHash3D::get_cell_size);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cell_size"), "set_cell_size", "get_cell_size");

    ClassDB::bind_method(D_METHOD("get_count"), &SpatialHash3D::get_count);

    // Core methods
    ClassDB::bind_method(D_METHOD("build", "positions"), &SpatialHash3D::build);
    ClassDB::bind_method(D_METHOD("clear"), &SpatialHash3D::clear);
    ClassDB::bind_method(D_METHOD("insert", "position"), &SpatialHash3D::insert);
    ClassDB::bind_method(D_METHOD("update", "index", "new_position"), &SpatialHash3D::update);

    // Query methods
    ClassDB::bind_method(D_METHOD("query_radius", "origin", "radius"), &SpatialHash3D::query_radius);
    ClassDB::bind_method(D_METHOD("query_box", "box"), &SpatialHash3D::query_box);
    ClassDB::bind_method(D_METHOD("query_nearest", "origin", "k"), &SpatialHash3D::query_nearest);
    ClassDB::bind_method(D_METHOD("query_nearest_one", "origin"), &SpatialHash3D::query_nearest_one);

    // Batch queries
    ClassDB::bind_method(D_METHOD("query_radius_batch", "origins", "radii"), &SpatialHash3D::query_radius_batch);
    ClassDB::bind_method(D_METHOD("query_radius_batch_uniform", "origins", "radius"), &SpatialHash3D::query_radius_batch_uniform);

    // Utility queries
    ClassDB::bind_method(D_METHOD("has_any_in_radius", "origin", "radius"), &SpatialHash3D::has_any_in_radius);
    ClassDB::bind_method(D_METHOD("count_in_radius", "origin", "radius"), &SpatialHash3D::count_in_radius);
}

SpatialHash3D::SpatialHash3D() {
}

SpatialHash3D::~SpatialHash3D() {
    clear();
}

void SpatialHash3D::set_cell_size(float p_size) {
    if (p_size > 0.0f) {
        cell_size = p_size;
    }
}

float SpatialHash3D::get_cell_size() const {
    return cell_size;
}

int32_t SpatialHash3D::get_count() const {
    return item_count;
}

void SpatialHash3D::get_cell_coords(const Vector3& pos, int32_t& cx, int32_t& cy, int32_t& cz) const {
    cx = static_cast<int32_t>(std::floor(pos.x / cell_size));
    cy = static_cast<int32_t>(std::floor(pos.y / cell_size));
    cz = static_cast<int32_t>(std::floor(pos.z / cell_size));
}

uint64_t SpatialHash3D::coords_to_key(int32_t cx, int32_t cy, int32_t cz) const {
    // Use a hash combining function for 3 coordinates
    // This uses FNV-1a inspired mixing to combine the coordinates
    uint64_t hash = 14695981039346656037ULL; // FNV offset basis
    const uint64_t prime = 1099511628211ULL; // FNV prime

    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(cx));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(cy));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(cz));
    hash *= prime;

    return hash;
}

uint64_t SpatialHash3D::hash_position(const Vector3& pos) const {
    int32_t cx, cy, cz;
    get_cell_coords(pos, cx, cy, cz);
    return coords_to_key(cx, cy, cz);
}

void SpatialHash3D::build(const PackedVector3Array& positions) {
    clear();

    stored_positions = positions;
    item_count = positions.size();

    const Vector3* pos_ptr = positions.ptr();
    for (int32_t i = 0; i < item_count; i++) {
        uint64_t key = hash_position(pos_ptr[i]);
        cells[key].push_back(i);
    }
}

void SpatialHash3D::clear() {
    cells.clear();
    stored_positions.resize(0);
    item_count = 0;
}

int32_t SpatialHash3D::insert(const Vector3& position) {
    int32_t index = item_count;
    stored_positions.push_back(position);
    item_count++;

    uint64_t key = hash_position(position);
    cells[key].push_back(index);

    return index;
}

void SpatialHash3D::update(int32_t index, const Vector3& new_position) {
    if (index < 0 || index >= item_count) {
        return;
    }

    Vector3 old_position = stored_positions[index];
    uint64_t old_key = hash_position(old_position);
    uint64_t new_key = hash_position(new_position);

    // Update stored position
    stored_positions.set(index, new_position);

    // If cell changed, update the hash
    if (old_key != new_key) {
        // Remove from old cell
        auto& old_cell = cells[old_key];
        old_cell.erase(std::remove(old_cell.begin(), old_cell.end(), index), old_cell.end());
        if (old_cell.empty()) {
            cells.erase(old_key);
        }

        // Add to new cell
        cells[new_key].push_back(index);
    }
}

PackedInt32Array SpatialHash3D::query_radius(const Vector3& origin, float radius) const {
    PackedInt32Array result;

    if (radius <= 0.0f || item_count == 0) {
        return result;
    }

    float radius_sq = radius * radius;

    int32_t cx, cy, cz;
    get_cell_coords(origin, cx, cy, cz);
    int32_t range = static_cast<int32_t>(std::ceil(radius / cell_size));

    const Vector3* pos_ptr = stored_positions.ptr();

    for (int32_t dz = -range; dz <= range; dz++) {
        for (int32_t dy = -range; dy <= range; dy++) {
            for (int32_t dx = -range; dx <= range; dx++) {
                uint64_t key = coords_to_key(cx + dx, cy + dy, cz + dz);

                auto it = cells.find(key);
                if (it != cells.end()) {
                    for (int32_t idx : it->second) {
                        float dist_sq = origin.distance_squared_to(pos_ptr[idx]);
                        if (dist_sq <= radius_sq) {
                            result.push_back(idx);
                        }
                    }
                }
            }
        }
    }

    return result;
}

PackedInt32Array SpatialHash3D::query_box(const AABB& box) const {
    PackedInt32Array result;

    if (item_count == 0) {
        return result;
    }

    int32_t min_cx, min_cy, min_cz, max_cx, max_cy, max_cz;
    get_cell_coords(box.position, min_cx, min_cy, min_cz);
    get_cell_coords(box.position + box.size, max_cx, max_cy, max_cz);

    const Vector3* pos_ptr = stored_positions.ptr();

    for (int32_t cz = min_cz; cz <= max_cz; cz++) {
        for (int32_t cy = min_cy; cy <= max_cy; cy++) {
            for (int32_t cx = min_cx; cx <= max_cx; cx++) {
                uint64_t key = coords_to_key(cx, cy, cz);

                auto it = cells.find(key);
                if (it != cells.end()) {
                    for (int32_t idx : it->second) {
                        if (box.has_point(pos_ptr[idx])) {
                            result.push_back(idx);
                        }
                    }
                }
            }
        }
    }

    return result;
}

PackedInt32Array SpatialHash3D::query_nearest(const Vector3& origin, int32_t k) const {
    PackedInt32Array result;

    if (k <= 0 || item_count == 0) {
        return result;
    }

    const Vector3* pos_ptr = stored_positions.ptr();

    if (k >= item_count) {
        // Return all items sorted by distance
        std::vector<std::pair<float, int32_t>> all_items;
        all_items.reserve(item_count);

        for (int32_t i = 0; i < item_count; i++) {
            float dist_sq = origin.distance_squared_to(pos_ptr[i]);
            all_items.emplace_back(dist_sq, i);
        }

        std::sort(all_items.begin(), all_items.end());

        for (const auto& item : all_items) {
            result.push_back(item.second);
        }
    } else {
        // Use partial sort for efficiency
        std::vector<std::pair<float, int32_t>> all_items;
        all_items.reserve(item_count);

        for (int32_t i = 0; i < item_count; i++) {
            float dist_sq = origin.distance_squared_to(pos_ptr[i]);
            all_items.emplace_back(dist_sq, i);
        }

        std::partial_sort(all_items.begin(), all_items.begin() + k, all_items.end());

        for (int32_t i = 0; i < k; i++) {
            result.push_back(all_items[i].second);
        }
    }

    return result;
}

int32_t SpatialHash3D::query_nearest_one(const Vector3& origin) const {
    if (item_count == 0) {
        return -1;
    }

    const Vector3* pos_ptr = stored_positions.ptr();

    int32_t nearest_idx = 0;
    float nearest_dist_sq = origin.distance_squared_to(pos_ptr[0]);

    for (int32_t i = 1; i < item_count; i++) {
        float dist_sq = origin.distance_squared_to(pos_ptr[i]);
        if (dist_sq < nearest_dist_sq) {
            nearest_dist_sq = dist_sq;
            nearest_idx = i;
        }
    }

    return nearest_idx;
}

Array SpatialHash3D::query_radius_batch(
    const PackedVector3Array& origins,
    const PackedFloat32Array& radii
) const {
    Array results;

    int32_t query_count = origins.size();
    if (query_count != radii.size()) {
        UtilityFunctions::push_error("AgentiteG: origins and radii arrays must have same size");
        return results;
    }

    for (int32_t i = 0; i < query_count; i++) {
        results.push_back(query_radius(origins[i], radii[i]));
    }

    return results;
}

Array SpatialHash3D::query_radius_batch_uniform(
    const PackedVector3Array& origins,
    float radius
) const {
    Array results;

    int32_t query_count = origins.size();
    for (int32_t i = 0; i < query_count; i++) {
        results.push_back(query_radius(origins[i], radius));
    }

    return results;
}

bool SpatialHash3D::has_any_in_radius(const Vector3& origin, float radius) const {
    if (radius <= 0.0f || item_count == 0) {
        return false;
    }

    float radius_sq = radius * radius;

    int32_t cx, cy, cz;
    get_cell_coords(origin, cx, cy, cz);
    int32_t range = static_cast<int32_t>(std::ceil(radius / cell_size));

    const Vector3* pos_ptr = stored_positions.ptr();

    for (int32_t dz = -range; dz <= range; dz++) {
        for (int32_t dy = -range; dy <= range; dy++) {
            for (int32_t dx = -range; dx <= range; dx++) {
                uint64_t key = coords_to_key(cx + dx, cy + dy, cz + dz);

                auto it = cells.find(key);
                if (it != cells.end()) {
                    for (int32_t idx : it->second) {
                        if (origin.distance_squared_to(pos_ptr[idx]) <= radius_sq) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

int32_t SpatialHash3D::count_in_radius(const Vector3& origin, float radius) const {
    if (radius <= 0.0f || item_count == 0) {
        return 0;
    }

    float radius_sq = radius * radius;
    int32_t count = 0;

    int32_t cx, cy, cz;
    get_cell_coords(origin, cx, cy, cz);
    int32_t range = static_cast<int32_t>(std::ceil(radius / cell_size));

    const Vector3* pos_ptr = stored_positions.ptr();

    for (int32_t dz = -range; dz <= range; dz++) {
        for (int32_t dy = -range; dy <= range; dy++) {
            for (int32_t dx = -range; dx <= range; dx++) {
                uint64_t key = coords_to_key(cx + dx, cy + dy, cz + dz);

                auto it = cells.find(key);
                if (it != cells.end()) {
                    for (int32_t idx : it->second) {
                        if (origin.distance_squared_to(pos_ptr[idx]) <= radius_sq) {
                            count++;
                        }
                    }
                }
            }
        }
    }

    return count;
}

}
