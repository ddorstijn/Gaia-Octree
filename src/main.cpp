#include <stdint.h>
#include <stdio.h>

#include "OctreeBuilder.h"
#include "bounding_box.h"

int
main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WIN64)
    _setmaxstdio(1024); // increase file descriptor limit in Windows
#endif

    // THIS NEEDS TO BE REPLACED
    // Either by calculating the bounding box
    // Or by getting it from a file or from the user
    bbox bbox_universe = bbox_init(-20000, -20000, -20000, 20000, 20000, 20000);

    // Estimate the amount of partitions needed based on resolution of the grid
    // and its size
    size_t n_partitions = estimate_partitions(gridsize, voxel_memory_limit);

    // General information about the grid and the resolution
    // The length of one grid point
    float unitlength =
        (bbox_universe.x_max - bbox_universe.x_min) / (float)gridsize;

    // The amount of morton indices per partition
    uint64_t morton_part = (gridsize * gridsize * gridsize) / n_partitions;

    // Split world into smaller buffers
    partition_stars(n_partitions, bbox_universe, unitlength, gridsize,
                    morton_part);

    // Create Octreebuilder which will output our SVO
    OctreeBuilder builder = OctreeBuilder(base_filename, gridsize);

    for (size_t i = 0; i < n_partitions; i++) {
        // morton codes for this partition
        uint64_t start = i * morton_part;
        uint64_t end = (i + 1) * morton_part;

        // Read points
        char filename[15];
        snprintf(filename, sizeof filename, "%s%d%s", "buffer", i, ".buf");

        // build SVO
        printf("Building SVO for partition %lld...", i);

        while (fgets(line, sizeof line, handle)) {
            size_t len = strlen(line);
            if (len && (line[len - 1] != '\n')) {
                /* incomplete line */
            }

            builder.addVoxel(*it);
        }
    }
}