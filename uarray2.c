/*
 * Authors: Frances Hughes
 * Date Created: 2/5/2017
 */
#include "uarray2.h"
#include "uarray.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define T UArray2_T

struct T {
        int width, height, size;
        UArray_T elems;
};



/*#########################FUNCTION DECLARATIONS####################*/
void UArray2_init(T array, int width, int height, int size, void *elems);
int UArray2_get_index(T array, int width, int height);

/*
 * Creates a new UArray2_T with specified width, height, and size
 */
T UArray2_new(int width, int height, int size)
{
        T array = malloc(sizeof(struct T));
        
        array->elems = UArray_new((width * height), size);
        array->width = width;
        array->height = height;
        array->size = size;
        
        return array;
}

/*
 * Frees every elems in UArray2_T and then frees UArray2_T
 */ 
void UArray2_free(T *array) 
{
        assert(array != NULL && *array != NULL);
        UArray_free(&((*array)->elems));
        free(*array);
}
        
/*
 * Returns the width of the UArray2_T
 */
int UArray2_width(T array) 
{
        assert(array != NULL);
        return array->width;
}

/*
 * Returns the height of the UArray2_T
 */
int UArray2_height(T array)
{
        assert(array != NULL);
        return array->height;
}

/*
 * Returns the size element of the UArray2_T
 */
int UArray2_size(T array) 
{
        assert(array != NULL);
        return array->size;
}

/*
 * Returns a pointer to the element of the UArray2_T at the specified width 
 * and height
 */
void *UArray2_at(T array, int width, int height)
{       
        assert(array != NULL);
        return UArray_at(array->elems, UArray2_get_index(array, width, height));
}

/*
 * Returns the index of the UArray2_T at the specified width and height
 */
int UArray2_get_index(T array, int width, int height)
{
        assert(width >= 0 && width < UArray2_width(array));
        assert(height >= 0 && height < UArray2_height(array));
        return (height * UArray2_width(array)) + width;
}

/*
 * Calls the function pointed to by apply for every element in column major
 * order
 */
void UArray2_map_col_major(T array,
        void apply(int width, int height, T array, void *p1, void *p2),
        void *cl)
{
        for (int i = 0; i < UArray2_width(array); i++) {
                for (int j = 0; j < UArray2_height(array); j++) {
                        apply(i, j, array, UArray2_at(array, i, j), cl);
                }
        }
}

/*
 * Calls the function pointed to be apply for every element in row major order
 */
void UArray2_map_row_major(T array,
        void apply(int width, int height, T array, void *p1, void *p2),
        void *cl) 
{
        for (int i = 0; i < UArray2_height(array); i++) {
                for (int j = 0; j < UArray2_width(array); j++) {
                        apply(j, i, array, UArray2_at(array, j, i), cl);
                }
        }
}
