/**
 * SpatialHash2D Implementation
 */

#include "spatial_hash_2d.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace godot {

void SpatialHash2D::_bind_methods() {
    // Properties
    ClassDB::bind_method(D_METHOD("set_cell_size", "size"), &SpatialHash2D::set_cell_size);
    ClassDB::bind_method(D_METHOD("get_cell_size"), &SpatialHash2D::get_cell_size);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cell_size"), "set_cell_size", "get_cell_size");

    ClassDB::bind_method(D_METHOD("get_count"), &SpatialHash2D::get_count);

    // Core methods
    ClassDB::bind_method(D_METHOD("build", "positions"), &SpatialHash2D::build);
    ClassDB::bind_method(D_METHOD("clear"), &SpatialHash2D::clear);
    ClassDB::bind_method(D_METHOD("insert", "position"), &SpatialHash2D::insert);
    ClassDB::bind_method(D_METHOD("update", "index", "new_position"), &SpatialHash2D::update);

    // Query methods
    ClassDB::bind_method(D_METHOD("query_radius", "origin", "radius"), &SpatialHash2D::query_radius);
    ClassDB::bind_method(D_METHOD("query_rect", "rect"), &SpatialHash2D::query_rect);
    ClassDB::bind_method(D_METHOD("query_nearest", "origin", "k"), &SpatialHash2D::query_nearest);
    ClassDB::bind_method(D_METHOD("query_nearest_one", "origin"), &SpatialHash2D::query_nearest_one);

    // Batch queries
    ClassDB::bind_method(D_METHOD("query_radius_batch", "origins", "radii"), &SpatialHash2D::query_radius_batch);
    ClassDB::bind_method(D_METHOD("query_radius_batch_uniform", "origins", "radius"), &SpatialHash2D::query_radius_batch_uniform);

    // Utility queries
    ClassDB::bind_method(D_METHOD("has_any_in_radius", "origin", "radius"), &SpatialHash2D::has_any_in_radius);
    ClassDB::bind_method(D_METHOD("count_in_radius", "origin", "radius"), &SpatialHash2D::count_in_radius);
}

SpatialHash2D::SpatialHash2D() {
}

SpatialHash2D::~SpatialHash2D() {
    clear();
}

void SpatialHash2D::set_cell_size(float p_size) {
    if (p_size > 0.0f) {
        cell_size = p_size;
    }
}

float SpatialHash2D::get_cell_size() const {
    return cell_size;
}

int32_t SpatialHash2D::get_count() const {
    return item_count;
}

int64_t SpatialHash2D::hash_position(const Vector2& pos) const {
    int32_t cx, cy;
    get_cell_coords(pos, cx, cy);
    // Combine into single 64-bit key
    return (static_cast<int64_t>(cx) << 32) | static_cast<uint32_t>(cy);
}

void SpatialHash2D::get_cell_coords(const Vector2& pos, int32_t& cx, int32_t& cy) const {
    cx = static_cast<int32_t>(std::floor(pos.x / cell_size));
    cy = static_cast<int32_t>(std::floor(pos.y / cell_size));
}

void SpatialHash2D::build(const PackedVector2Array& positions) {
    clear();

    stored_positions = positions;
    item_count = positions.size();

    // Pre-size hint for cells map based on expected density
    // Assume roughly uniform distribution

    const Vector2* pos_ptr = positions.ptr();
    for (int32_t i = 0; i < item_count; i++) {
        int64_t key = hash_position(pos_ptr[i]);
        cells[key].push_back(i);
    }
}

void SpatialHash2D::clear() {
    cells.clear();
    stored_positions.resize(0);
    item_count = 0;
}

int32_t SpatialHash2D::insert(const Vector2& position) {
    int32_t index = item_count;
    stored_positions.push_back(position);
    item_count++;

    int64_t key = hash_position(position);
    cells[key].push_back(index);

    return index;
}

void SpatialHash2D::update(int32_t index, const Vector2& new_position) {
    if (index < 0 || index >= item_count) {
        return;
    }

    Vector2 old_position = stored_positions[index];
    int64_t old_key = hash_position(old_position);
    int64_t new_key = hash_position(new_position);

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

PackedInt32Array SpatialHash2D::query_radius(const Vector2& origin, float radius) const {
    PackedInt32Array result;

    if (radius <= 0.0f || item_count == 0) {
        return result;
    }

    float radius_sq = radius * radius;

    int32_t cx, cy;
    get_cell_coords(origin, cx, cy);
    int32_t range = static_cast<int32_t>(std::ceil(radius / cell_size));

    const Vector2* pos_ptr = stored_positions.ptr();

    for (int32_t dy = -range; dy <= range; dy++) {
        for (int32_t dx = -range; dx <= range; dx++) {
            int64_t key = (static_cast<int64_t>(cx + dx) << 32) |
                          static_cast<uint32_t>(cy + dy);

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

    return result;
}

PackedInt32Array SpatialHash2D::query_rect(const Rect2& rect) const {
    PackedInt32Array result;

    if (item_count == 0) {
        return result;
    }

    int32_t min_cx, min_cy, max_cx, max_cy;
    get_cell_coords(rect.position, min_cx, min_cy);
    get_cell_coords(rect.position + rect.size, max_cx, max_cy);

    const Vector2* pos_ptr = stored_positions.ptr();

    for (int32_t cy = min_cy; cy <= max_cy; cy++) {
        for (int32_t cx = min_cx; cx <= max_cx; cx++) {
            int64_t key = (static_cast<int64_t>(cx) << 32) | static_cast<uint32_t>(cy);

            auto it = cells.find(key);
            if (it != cells.end()) {
                for (int32_t idx : it->second) {
                    if (rect.has_point(pos_ptr[idx])) {
                        result.push_back(idx);
                    }
                }
            }
        }
    }

    return result;
}

PackedInt32Array SpatialHash2D::query_nearest(const Vector2& origin, int32_t k) const {
    PackedInt32Array result;

    if (k <= 0 || item_count == 0) {
        return result;
    }

    // For small k relative to total items, use expanding search
    // For large k, just sort everything

    const Vector2* pos_ptr = stored_positions.ptr();

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

int32_t SpatialHash2D::query_nearest_one(const Vector2& origin) const {
    if (item_count == 0) {
        return -1;
    }

    const Vector2* pos_ptr = stored_positions.ptr();

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

Array SpatialHash2D::query_radius_batch(
    const PackedVector2Array& origins,
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

Array SpatialHash2D::query_radius_batch_uniform(
    const PackedVector2Array& origins,
    float radius
) const {
    Array results;

    int32_t query_count = origins.size();
    for (int32_t i = 0; i < query_count; i++) {
        results.push_back(query_radius(origins[i], radius));
    }

    return results;
}

bool SpatialHash2D::has_any_in_radius(const Vector2& origin, float radius) const {
    if (radius <= 0.0f || item_count == 0) {
        return false;
    }

    float radius_sq = radius * radius;

    int32_t cx, cy;
    get_cell_coords(origin, cx, cy);
    int32_t range = static_cast<int32_t>(std::ceil(radius / cell_size));

    const Vector2* pos_ptr = stored_positions.ptr();

    for (int32_t dy = -range; dy <= range; dy++) {
        for (int32_t dx = -range; dx <= range; dx++) {
            int64_t key = (static_cast<int64_t>(cx + dx) << 32) |
                          static_cast<uint32_t>(cy + dy);

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

    return false;
}

int32_t SpatialHash2D::count_in_radius(const Vector2& origin, float radius) const {
    if (radius <= 0.0f || item_count == 0) {
        return 0;
    }

    float radius_sq = radius * radius;
    int32_t count = 0;

    int32_t cx, cy;
    get_cell_coords(origin, cx, cy);
    int32_t range = static_cast<int32_t>(std::ceil(radius / cell_size));

    const Vector2* pos_ptr = stored_positions.ptr();

    for (int32_t dy = -range; dy <= range; dy++) {
        for (int32_t dx = -range; dx <= range; dx++) {
            int64_t key = (static_cast<int64_t>(cx + dx) << 32) |
                          static_cast<uint32_t>(cy + dy);

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

    return count;
}

}
