#ifndef PARTITIONER_H
#define PARTITIONER_H

#include "bounding_box.h"
#include "gaia_db.h"
#include "libmorton/morton.h"

typedef struct _partition_buffer
{
    BBox* bounding_box;
    FILE* file;
    const char* filename;
} PartitionBuffer;

PartitionBuffer
partition_buffer_init(const char* filename, BBox bbox);

void
buffer_add_star(PartitionBuffer* buffer, SStar* star);

void
buffer_flush(PartitionBuffer* buffer);

void
buffer_destroy(PartitionBuffer* buffer);

#endif // !PARTITIONER_H