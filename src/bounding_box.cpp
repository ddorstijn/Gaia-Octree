#include "bounding_box.h"

#include <stdlib.h>

BBox*
bbox_init(double x_min, double y_min, double z_min, double x_max, double y_max,
          double z_max)
{
    BBox* bbox = (BBox*)malloc(sizeof *bbox);
    bbox->x_min = x_min;
    bbox->y_min = y_min;
    bbox->z_min = z_min;
    bbox->x_max = x_max;
    bbox->y_max = y_max;
    bbox->z_max = z_max;

    return bbox;
}

void
bbox_destroy(BBox* bbox)
{
    free(bbox);
}

BBox_uint*
bbox_uint_init(uint32_t x_min, uint32_t y_min, uint32_t z_min, uint32_t x_max,
               uint32_t y_max, uint32_t z_max)
{
    BBox_uint* bbox = (BBox_uint*)malloc(sizeof *bbox);
    bbox->x_min = x_min;
    bbox->y_min = y_min;
    bbox->z_min = z_min;
    bbox->x_max = x_max;
    bbox->y_max = y_max;
    bbox->z_max = z_max;

    return bbox;
}

void
bbox_uint_destroy(BBox_uint* bbox_uint)
{
    free(bbox_uint);
}

inline char
bbox_check_intersect(BBox* bbox, double x, double y, double z)
{
    if (x > bbox->x_max || x < bbox->x_min || y > bbox->y_max ||
        z < bbox->z_min || z > bbox->z_max || z < bbox->z_min) {
        return 0;
    }

    return 1;
}