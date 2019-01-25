#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "stdint.h"

typedef struct _bbox
{
    double x_min;
    double y_min;
    double z_min;

    double x_max;
    double y_max;
    double z_max;
} BBox;

typedef struct _bbox_uint
{
    uint32_t x_min;
    uint32_t y_min;
    uint32_t z_min;

    uint32_t x_max;
    uint32_t y_max;
    uint32_t z_max;
} BBox_uint;

BBox*
bbox_init(double x_min, double y_min, double z_min, double x_max, double y_max,
          double z_max);

void
bbox_destroy(BBox* bbox);

BBox_uint*
bbox_uint_init(uint32_t x_min, uint32_t y_min, uint32_t z_min, uint32_t x_max,
               uint32_t y_max, uint32_t z_max);

void
bbox_uint_destroy(BBox_uint* bbox_uint);

inline char
bbox_check_intersect(BBox* bbox, double x, double y, double z);

#endif // !BOUNDING_BOX_H