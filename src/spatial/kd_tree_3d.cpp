/**
 * KDTree3D Implementation
 */

#include "kd_tree_3d.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace godot {

void KDTree3D::_bind_methods() {
    // Core methods
    ClassDB::bind_method(D_METHOD("build", "points"), &KDTree3D::build);
    ClassDB::bind_method(D_METHOD("clear"), &KDTree3D::clear);
    ClassDB::bind_method(D_METHOD("size"), &KDTree3D::size);

    // Query methods
    ClassDB::bind_method(D_METHOD("query_nearest_one", "point"), &KDTree3D::query_nearest_one);
    ClassDB::bind_method(D_METHOD("query_nearest", "point", "k"), &KDTree3D::query_nearest);
    ClassDB::bind_method(D_METHOD("query_radius", "point", "radius"), &KDTree3D::query_radius);

    // Batch queries
    ClassDB::bind_method(D_METHOD("query_nearest_one_batch", "points"), &KDTree3D::query_nearest_one_batch);
    ClassDB::bind_method(D_METHOD("query_nearest_batch", "points", "k"), &KDTree3D::query_nearest_batch);
}

KDTree3D::KDTree3D() {
}

KDTree3D::~KDTree3D() {
    clear();
}

void KDTree3D::build(const PackedVector3Array& points) {
    clear();

    int32_t n = points.size();
    if (n == 0) {
        return;
    }

    stored_points = points;

    // Create index array
    std::vector<int32_t> indices(n);
    for (int32_t i = 0; i < n; i++) {
        indices[i] = i;
    }

    // Reserve space for nodes (exactly n nodes for n points)
    nodes.reserve(n);

    // Build tree recursively
    root_index = build_recursive(indices, 0, n, 0);
}

int32_t KDTree3D::build_recursive(std::vector<int32_t>& indices, int32_t start, int32_t end, int depth) {
    if (start >= end) {
        return -1;
    }

    int32_t count = end - start;
    uint8_t axis = depth % 3;  // Cycle through x (0), y (1), z (2)

    const Vector3* points_ptr = stored_points.ptr();

    // Sort by the current axis
    std::sort(indices.begin() + start, indices.begin() + end,
        [points_ptr, axis, this](int32_t a, int32_t b) {
            return get_axis_value(points_ptr[a], axis) < get_axis_value(points_ptr[b], axis);
        });

    // Median is the middle element
    int32_t mid = start + count / 2;

    // Create node
    Node node;
    node.point_index = indices[mid];
    node.axis = axis;

    int32_t node_idx = static_cast<int32_t>(nodes.size());
    nodes.push_back(node);

    // Build children
    nodes[node_idx].left = build_recursive(indices, start, mid, depth + 1);
    nodes[node_idx].right = build_recursive(indices, mid + 1, end, depth + 1);

    return node_idx;
}

void KDTree3D::clear() {
    nodes.clear();
    stored_points.resize(0);
    root_index = -1;
}

int32_t KDTree3D::size() const {
    return stored_points.size();
}

int32_t KDTree3D::query_nearest_one(const Vector3& point) const {
    if (root_index < 0) {
        return -1;
    }

    int32_t best_idx = -1;
    float best_dist_sq = std::numeric_limits<float>::max();

    nearest_one_recursive(root_index, point, best_idx, best_dist_sq);

    return best_idx;
}

void KDTree3D::nearest_one_recursive(int32_t node_idx, const Vector3& target,
                                      int32_t& best_idx, float& best_dist_sq) const {
    if (node_idx < 0) {
        return;
    }

    const Node& node = nodes[node_idx];
    const Vector3& node_point = stored_points[node.point_index];

    // Check distance to this node's point
    float dist_sq = target.distance_squared_to(node_point);
    if (dist_sq < best_dist_sq) {
        best_dist_sq = dist_sq;
        best_idx = node.point_index;
    }

    // Determine which side to search first
    float diff = get_axis_value(target, node.axis) - get_axis_value(node_point, node.axis);

    int32_t first_child = (diff < 0) ? node.left : node.right;
    int32_t second_child = (diff < 0) ? node.right : node.left;

    // Search the closer side first
    nearest_one_recursive(first_child, target, best_idx, best_dist_sq);

    // Only search the other side if the splitting plane is closer than current best
    if (diff * diff < best_dist_sq) {
        nearest_one_recursive(second_child, target, best_idx, best_dist_sq);
    }
}

PackedInt32Array KDTree3D::query_nearest(const Vector3& point, int32_t k) const {
    PackedInt32Array result;

    if (root_index < 0 || k <= 0) {
        return result;
    }

    int32_t n = stored_points.size();
    if (k >= n) {
        // Return all points sorted by distance
        const Vector3* points_ptr = stored_points.ptr();
        std::vector<std::pair<float, int32_t>> all_items;
        all_items.reserve(n);

        for (int32_t i = 0; i < n; i++) {
            float dist_sq = point.distance_squared_to(points_ptr[i]);
            all_items.emplace_back(dist_sq, i);
        }

        std::sort(all_items.begin(), all_items.end());

        for (const auto& item : all_items) {
            result.push_back(item.second);
        }
        return result;
    }

    // Max-heap to track k nearest (stores farthest at top for easy pruning)
    std::priority_queue<std::pair<float, int32_t>> heap;

    nearest_k_recursive(root_index, point, k, heap);

    // Extract results and reverse to get nearest first
    std::vector<std::pair<float, int32_t>> sorted;
    sorted.reserve(heap.size());
    while (!heap.empty()) {
        sorted.push_back(heap.top());
        heap.pop();
    }

    // Reverse to get nearest first
    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
        result.push_back(it->second);
    }

    return result;
}

void KDTree3D::nearest_k_recursive(int32_t node_idx, const Vector3& target, int32_t k,
                                    std::priority_queue<std::pair<float, int32_t>>& heap) const {
    if (node_idx < 0) {
        return;
    }

    const Node& node = nodes[node_idx];
    const Vector3& node_point = stored_points[node.point_index];

    // Check distance to this node's point
    float dist_sq = target.distance_squared_to(node_point);

    if (static_cast<int32_t>(heap.size()) < k) {
        heap.emplace(dist_sq, node.point_index);
    } else if (dist_sq < heap.top().first) {
        heap.pop();
        heap.emplace(dist_sq, node.point_index);
    }

    // Determine which side to search first
    float diff = get_axis_value(target, node.axis) - get_axis_value(node_point, node.axis);

    int32_t first_child = (diff < 0) ? node.left : node.right;
    int32_t second_child = (diff < 0) ? node.right : node.left;

    // Search the closer side first
    nearest_k_recursive(first_child, target, k, heap);

    // Only search the other side if needed
    float worst_dist_sq = (static_cast<int32_t>(heap.size()) < k)
        ? std::numeric_limits<float>::max()
        : heap.top().first;

    if (diff * diff < worst_dist_sq) {
        nearest_k_recursive(second_child, target, k, heap);
    }
}

PackedInt32Array KDTree3D::query_radius(const Vector3& point, float radius) const {
    PackedInt32Array result;

    if (root_index < 0 || radius <= 0.0f) {
        return result;
    }

    float radius_sq = radius * radius;
    radius_recursive(root_index, point, radius_sq, result);

    return result;
}

void KDTree3D::radius_recursive(int32_t node_idx, const Vector3& target, float radius_sq,
                                 PackedInt32Array& results) const {
    if (node_idx < 0) {
        return;
    }

    const Node& node = nodes[node_idx];
    const Vector3& node_point = stored_points[node.point_index];

    // Check distance to this node's point
    float dist_sq = target.distance_squared_to(node_point);
    if (dist_sq <= radius_sq) {
        results.push_back(node.point_index);
    }

    // Calculate distance to splitting plane
    float diff = get_axis_value(target, node.axis) - get_axis_value(node_point, node.axis);

    // Always search the side we're on
    if (diff < 0) {
        radius_recursive(node.left, target, radius_sq, results);
        // Search other side if splitting plane is within radius
        if (diff * diff <= radius_sq) {
            radius_recursive(node.right, target, radius_sq, results);
        }
    } else {
        radius_recursive(node.right, target, radius_sq, results);
        // Search other side if splitting plane is within radius
        if (diff * diff <= radius_sq) {
            radius_recursive(node.left, target, radius_sq, results);
        }
    }
}

PackedInt32Array KDTree3D::query_nearest_one_batch(const PackedVector3Array& points) const {
    PackedInt32Array results;

    int32_t query_count = points.size();
    results.resize(query_count);

    for (int32_t i = 0; i < query_count; i++) {
        results.set(i, query_nearest_one(points[i]));
    }

    return results;
}

Array KDTree3D::query_nearest_batch(const PackedVector3Array& points, int32_t k) const {
    Array results;

    int32_t query_count = points.size();
    for (int32_t i = 0; i < query_count; i++) {
        results.push_back(query_nearest(points[i], k));
    }

    return results;
}

}
