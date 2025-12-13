/**
 * GridOps - High-performance 2D grid operations
 *
 * Provides fast operations for 2D grids stored as flat arrays.
 * All grids are assumed to be in row-major order.
 *
 * Usage:
 *   # Convert between coordinates and indices
 *   var index = GridOps.to_index(x, y, width)
 *   var coords = GridOps.to_coords(index, width)
 *
 *   # Get neighbors
 *   var neighbors = GridOps.neighbors_4(index, width, height)
 *
 *   # Flood fill
 *   var filled = GridOps.flood_fill(grid, width, start, target_value)
 *
 *   # Line of sight
 *   var visible = GridOps.line_clear(grid, width, from, to, blocking)
 */

#ifndef AGENTITE_GRID_OPS_HPP
#define AGENTITE_GRID_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>

namespace godot {

class GridOps : public RefCounted {
    GDCLASS(GridOps, RefCounted)

protected:
    static void _bind_methods();

public:
    // ========== COORDINATE CONVERSION ==========
    // Convert (x, y) to flat index
    static int to_index(int x, int y, int width);
    // Convert flat index to (x, y)
    static Vector2i to_coords(int index, int width);
    // Batch convert coordinates to indices
    static PackedInt32Array to_indices(const PackedVector2Array& coords, int width);
    // Batch convert indices to coordinates
    static PackedVector2Array to_coords_batch(const PackedInt32Array& indices, int width);
    // Check if coordinates are within bounds
    static bool in_bounds(int x, int y, int width, int height);
    // Check if index is within bounds
    static bool index_in_bounds(int index, int width, int height);

    // ========== NEIGHBOR QUERIES ==========
    // Get 4-connected neighbors (cardinal directions: N, S, E, W)
    static PackedInt32Array neighbors_4(int index, int width, int height);
    // Get 8-connected neighbors (cardinal + diagonal)
    static PackedInt32Array neighbors_8(int index, int width, int height);
    // Batch get 4-connected neighbors (returns flat array)
    static PackedInt32Array neighbors_4_batch(const PackedInt32Array& indices, int width, int height);
    // Batch get 8-connected neighbors (returns flat array)
    static PackedInt32Array neighbors_8_batch(const PackedInt32Array& indices, int width, int height);
    // Get neighbors with specific offsets
    static PackedInt32Array neighbors_custom(int index, int width, int height, const PackedVector2Array& offsets);

    // ========== FLOOD FILL ==========
    // Flood fill starting from index, returns all connected cells with target_value
    static PackedInt32Array flood_fill(const PackedInt32Array& grid, int width, int start_index, int target_value);
    // Flood fill only through walkable cells (stops at non-walkable)
    static PackedInt32Array flood_fill_walkable(const PackedInt32Array& grid, int width, int start_index, int walkable_value);
    // Flood fill with maximum distance limit
    static PackedInt32Array flood_fill_limited(const PackedInt32Array& grid, int width, int start_index, int target_value, int max_distance);

    // ========== LINE OF SIGHT (BRESENHAM) ==========
    // Get all grid indices along a line from 'from' to 'to'
    static PackedInt32Array line_indices(const Vector2i& from, const Vector2i& to, int width);
    // Check if line is clear (no blocking values)
    static bool line_clear(const PackedInt32Array& grid, int width, const Vector2i& from, const Vector2i& to, int blocking_value);
    // Raycast: return indices until hitting blocking value or max distance
    static PackedInt32Array raycast_grid(const PackedInt32Array& grid, int width, const Vector2i& from, const Vector2i& direction, int max_distance, int blocking_value);
    // Get the first blocking cell along a line (returns -1 if none)
    static int line_first_blocking(const PackedInt32Array& grid, int width, const Vector2i& from, const Vector2i& to, int blocking_value);

    // ========== FIELD OF VIEW (SHADOWCASTING) ==========
    // Calculate visible cells from origin within radius using shadowcasting
    static PackedInt32Array fov_shadowcast(const PackedInt32Array& grid, int width, int height, const Vector2i& origin, int radius, int blocking_value);
    // Simpler FOV using ray tracing (slower but simpler)
    static PackedInt32Array fov_raycast(const PackedInt32Array& grid, int width, int height, const Vector2i& origin, int radius, int blocking_value, int ray_count);

    // ========== DISTANCE TRANSFORMS ==========
    // Manhattan distance field from all cells with target_value
    static PackedInt32Array manhattan_distance_field(const PackedInt32Array& grid, int width, int height, int target_value);
    // Chebyshev (chessboard) distance field
    static PackedInt32Array chebyshev_distance_field(const PackedInt32Array& grid, int width, int height, int target_value);
    // Euclidean distance field (approximation using 3-4-5 weights)
    static PackedFloat32Array euclidean_distance_field(const PackedInt32Array& grid, int width, int height, int target_value);

    // ========== CONNECTED COMPONENTS ==========
    // Label all connected components (4-connected), returns labels array
    static PackedInt32Array label_connected_components(const PackedInt32Array& grid, int width, int height, int target_value);
    // Count number of connected components
    static int count_connected_components(const PackedInt32Array& grid, int width, int height, int target_value);
    // Get sizes of each connected component
    static PackedInt32Array component_sizes(const PackedInt32Array& grid, int width, int height, int target_value);

    // ========== GRID UTILITIES ==========
    // Find all cells with specific value
    static PackedInt32Array find_value(const PackedInt32Array& grid, int value);
    // Find all cells not equal to value
    static PackedInt32Array find_not_value(const PackedInt32Array& grid, int value);
    // Count cells with specific value
    static int count_value(const PackedInt32Array& grid, int value);
    // Get rectangular region as flat array
    static PackedInt32Array get_region(const PackedInt32Array& grid, int grid_width, int x, int y, int region_width, int region_height);
    // Set rectangular region from flat array
    static PackedInt32Array set_region(const PackedInt32Array& grid, int grid_width, int x, int y, int region_width, int region_height, const PackedInt32Array& values);

private:
    // Internal helper for shadowcasting octants
    static void cast_light(const PackedInt32Array& grid, int width, int height,
                          PackedInt32Array& visible, const Vector2i& origin, int radius,
                          int blocking_value, int row, float start_slope, float end_slope,
                          int xx, int xy, int yx, int yy);
};

}

#endif // AGENTITE_GRID_OPS_HPP
