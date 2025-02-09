#include "pixel/controller.h"

#include <errno.h>

int new_pixel_multiupdate_request(
    struct pixel_multiupdate_request **self,
    size_t size)
{
    // Check arguments
    if (*self != NULL)
        return -EINVAL;

    // Create the structure
    *self = (struct pixel_multiupdate_request *)malloc(sizeof(struct pixel_multiupdate_request));
    if (self == NULL)
        return -ECANCELED;

    // Allocate parts of the structure
    (*self)->data = (struct pixel_update_data *)malloc(size);
    (*self)->size = size;

    // Check allocation
    if ((*self)->data == NULL)
    {
        free(*self);
        return -ECANCELED;
    }

    return 0;
};

void dispose_pixel_multiupdate_request(
    struct pixel_multiupdate_request **self)
{
    // Deallocate internal struct
    free((*self)->data);

    // Deallocate object
    free(*self);

    // Set to null (for safety)
    *self = NULL;
}