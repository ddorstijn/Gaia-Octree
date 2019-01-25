#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "partitioner.h"

// Estimate the optimal amount of partitions we need, given the requested
// gridsize and the overall memory limit.
size_t
estimate_partitions(const size_t gridsize, const size_t memory_limit)
{
    printf("Estimating best partition count ...\n");
    uint64_t required = (gridsize * gridsize * gridsize) / 1024 / 1024;
    printf("To do this in-core I would need %lld Mb of system memory",
           required);
    if (required <= memory_limit) {
        printf("Memory limit of %lld Mb allows that", memory_limit);
        return 1;
    }

    size_t numpartitions = 1;
    size_t required_partition = required;
    while (required_partition > memory_limit) {
        required_partition = required_partition / 8;
        numpartitions = numpartitions * 8;
    }

    printf("Going to do it in %lld partitions of %lld Mb each.", numpartitions,
           required_partition);
    return numpartitions;
}

/**
 * @brief Partition the universe in smaller buffers and add the stars to their
 *        respective buffers.
 *
 * @param n_partitions - Number of estimated partitions
 * @param bbox_universe - The bounding box of the universe
 * @param unitlength - The length of one grid unit
 * @param gridsize - The total size of the universe in grid points
 * @param morton_partition - The amount of morton indices per partition
 */
void
partition_stars(const size_t n_partitions, BBox bbox_universe, float unitlength,
                size_t gridsize, uint_fast64_t morton_partition);
{
    // Generate buffers
    PartitionBuffer buffers[n_partitions];
    for (uint16_t i = 0; i < n_partitions; i++) {
        // Buffer + i + .buf + \0
        char* filename = (char*)malloc(12);
        snprintf(filename, sizeof filename, "%s%d%s", "buffer", i, ".buf");

        // Bounding box of the morton grid
        BBox_uint bbox_grid;
        // Bounding box of the grid in normal position
        BBox bbox_world;

        // Calculate minimum morton index of this partition
        morton3D_64_decode(
            morton_partition * i, (uint_fast32_t&)bbox_grid.x_min,
            (uint_fast32_t&)bbox_grid.y_min, (uint_fast32_t&)bbox_grid.z_min);

        // Calculate maximum morton index of this partition
        // -1, because z-curve skips to first block of next partition
        morton3D_64_decode(
            (morton_partition * (i + 1)) - 1, (uint_fast32_t&)bbox_grid.x_max,
            (uint_fast32_t&)bbox_grid.y_max, (uint_fast32_t&)bbox_grid.z_max);

        // Morton position * unitlength gives us the 3D - position
        bbox_world.x_min = bbox_grid.x_min * unitlength;
        bbox_world.y_min = bbox_grid.y_min * unitlength;
        bbox_world.z_min = bbox_grid.z_min * unitlength;
        // + 1, to include full last block
        bbox_world.x_max = (bbox_grid.x_max + 1) * unitlength;
        bbox_world.y_max = (bbox_grid.y_max + 1) * unitlength;
        bbox_world.z_max = (bbox_grid.z_max + 1) * unitlength;

        buffers[i] = partition_buffer_init(filename, bbox_world);
    }

    // Add the correct star to the correct buffer
    DB_CTX* context = gaia_setup_database("");
    DBC* cursor = gaia_open_cursor(context->dbp);
    while (gaia_cursor_has_next(cursor)) {
        SStar* star = gaia_cursor_get_star(cursor);
        for (uint16_t i = 0; i < n_partitions; i++) {
            char intersect = buffer_check_intersect(&buffers[i].bounding_box,
                                                    star.x, star.y, star.z);
            if (intersect) {
                // Convert world to absolute grid
                uint_fast32_t x = (star->x - bbox_universe->x_min) / unitlength;
                uint_fast32_t y = (star->y - bbox_universe->y_min) / unitlength;
                uint_fast32_t z = (star->z - bbox_universe->z_min) / unitlength;

                // Update the database so correct morton indices are stored
                uint_fast64_t morton_index = morton3D_64_encode(x, y, z);
                gaia_update_star_morton(context->dbp, star->id, morton_index);

                // Add the star to the buffer
                buffer_add_star(buffers[i], morton_index);

                break;
            }
        }
    }

    // Clean up
    for (uint16_t i = 0; i < n_partitions; i++) {
        // Write the full buffer to the disk
        buffer_flush(buffers[i]);
        // Destroy all the buffers
        buffer_destroy(buffers[i]);
    }
}