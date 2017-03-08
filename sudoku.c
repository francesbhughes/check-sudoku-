/*
 * Names: Alex Johnson (ajohns16) and Frances Hughes (fhughe01)
 * Date: 2/9/2017
 *
 * sudoku.c
 */

#include "pnmrdr.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "uarray2.h"

/*#################FUNCTION DECLARATIONS#######################*/
UArray2_T get_sudoku(int argc, char *argv[]);
UArray2_T check_format(FILE *fp);
void check_mapdata(Pnmrdr_mapdata *map);
UArray2_T get_uarray(Pnmrdr_T rdr);
bool check_sudoku(UArray2_T array);
void check_zero_intensity(int width, int height, UArray2_T array, void *p1,
                void *p2);
void check_rows(int width, int height, UArray2_T array, void *p1,
                void *p2);
void set_to_zero(int width, int height, UArray2_T array, void *p1, void *p2);
void check_cols(int width, int height, UArray2_T array, void *p1, void *p2);
bool check_groups(UArray2_T array); 
bool test_group(int width, int height, UArray2_T array);
bool check_error_report(UArray2_T array);

int main(int argc, char *argv[]) 
{
        bool OK = true;
        UArray2_T array = get_sudoku(argc, argv);

        OK = check_sudoku(array);

        UArray2_free(&array);

        if (OK != true) {
                return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
}

/* 
 * Opens up file given as arguments or reads from standard input, 
 * and converts file to UArray2_T
 */ 
UArray2_T get_sudoku(int argc, char *argv[])
{ 
        UArray2_T array;
        if (argc == 1) {
                if (!feof(stdin)){
                        array = check_format(stdin);
                }                       
        } else if (argc > 2){ 
                fprintf( stderr, "too many arguments\n");
                exit(EXIT_FAILURE);     
        } else {
                FILE *fp = fopen(argv[1], "rb");
                if (fp == NULL) {
                        fprintf( stderr, "unable to open file\n");
                        exit(EXIT_FAILURE);
                }
                array = check_format(fp);
                fclose(fp);
        }
        return array; 
}

/*
 * Checks file to confirm it is a portable graymap
 */
UArray2_T check_format(FILE *fp) 
{
        Pnmrdr_T rdr = Pnmrdr_new(fp);

        Pnmrdr_mapdata *map =  malloc(sizeof(Pnmrdr_mapdata));
        *map = Pnmrdr_data(rdr);
        check_mapdata(map); 

        UArray2_T array = get_uarray(rdr);

        free(map);
        Pnmrdr_free(&rdr);
        return array;
}

/*
 * Checks mapdata for unexpected document types
 */
void check_mapdata(Pnmrdr_mapdata *map)
{
        if (map->type != 2){
                RAISE(Pnmrdr_Badformat);
        }
        if (map->width != 9 || map->height != 9) {
                RAISE(Pnmrdr_Badformat);
        }
        if (map->denominator != 9) {
                RAISE(Pnmrdr_Badformat);
        }
}        

/*
 * Creates a UArray2_T for the Pnmrdr_T data
 */
UArray2_T get_uarray(Pnmrdr_T rdr) 
{
        UArray2_T array = UArray2_new(9, 9, sizeof(int)); 

        for (int height = 0; height < 9; height++) {
                for (int width = 0; width < 9; width++) {
                        *((int *)UArray2_at(array, width, height)) 
                                = Pnmrdr_get(rdr);
                }
        }
        return array;
}

/*
 * Checks correctness of sudoku
 */
 bool check_sudoku(UArray2_T array){
        bool OK = true;
        UArray2_T temp_array = UArray2_new(9, 1, sizeof(int));
    
        UArray2_map_row_major(array, check_zero_intensity, &OK);
        UArray2_map_row_major(array, check_rows, &temp_array); 
        UArray2_map_col_major(array, check_cols, &temp_array);
        OK &= check_error_report(temp_array);
        
        UArray2_free(&temp_array); 
        
        OK &= check_groups(array);

        return OK;
}

/*
 * Confirms the first index in the array is not -1
 */
bool check_error_report(UArray2_T array) {
        if (*(int *)UArray2_at(array, 0, 0) == -1) {
                return false;
        }
        return true;
}

/*
 * Checks that no entry is 0
 */
void check_zero_intensity(int width, int height, UArray2_T array, void *p1,
                void *p2){
        (void)width;
        (void)height;
        (void)array;

        int *entry_p = p1;
        *((bool *)p2) &= *entry_p != 0;        
}

/*
 * Checks row by row that no two entries have the same intensity
 */
void check_rows(int width, int height, UArray2_T array, void *p1,
                void *p2){
        (void) height; 
        (void) array;
        int *entry_p = p1;

        if (*((int *)UArray2_at(*(UArray2_T *)p2, 0, 0)) == -1) {
                return;
        }

        if (width == 0){
                *((int *)UArray2_at(*(UArray2_T *)p2, 0, 0)) = *entry_p;
        } else {
                for (int i = 0; i < width; i++) {
                        /*Found duplicate*/
                        if (*((int *)UArray2_at(*(UArray2_T *)p2, i, 0))
                                        == *entry_p) {
                                *((int *)UArray2_at(*(UArray2_T *)p2, 0, 0)) 
                                        = -1;
                                return;
                        }
                }
                *((int *)UArray2_at(*(UArray2_T *)p2, width, 0)) = *entry_p;
        }
        /*At end of row*/
        if (width == 8){
                UArray2_map_row_major(*(UArray2_T *)p2, set_to_zero, NULL);  
        }
}

/*
 * Checks column by column that no two entries have the saem intensity
 */
void check_cols(int width, int height, UArray2_T array, void *p1,
                void *p2)
{
        (void) width;
        (void) array;
        int *entry_p = p1;

        if (*((int *)UArray2_at(*(UArray2_T *)p2, 0, 0)) == -1) {
                return;
        }

        if (height == 0){
                *((int *)UArray2_at(*(UArray2_T *)p2, 0, 0)) = *entry_p;
        } else {
                for (int i = 0; i < height; i++) {
                        /*Found duplicates*/
                        if (*((int *)UArray2_at(*(UArray2_T *)p2, i, 0))
                                        == *entry_p) {
                                *((int *)UArray2_at(*(UArray2_T *)p2, 0, 0)) 
                                        = -1;
                                return;
                        }
                }
                *((int *)UArray2_at(*(UArray2_T *)p2, height, 0)) = *entry_p;
        }
        /*At end of column*/
        if (height == 8){
                UArray2_map_row_major(*(UArray2_T *)p2, set_to_zero, NULL);  
        }
}

/*
 * Checks 3 x 3 groups every 3 indices
 */
bool check_groups(UArray2_T array) 
{
        int i, j;
        for (i = 0; i < 9; i += 3) {
                for (j = 0; j < 9; j += 3) {
                        if (test_group(i, j, array) == false) {
                                return false;
                        }
                }
        }
        return true;
}        

/*
 * Checks that no two entries in the same 3 x 3 group have the same 
 * intensity
 */
bool test_group(int width, int height, UArray2_T array) 
{
        UArray2_T group_array = UArray2_new(9, 1, sizeof(int));
        UArray2_T temp_array = UArray2_new(9, 1, sizeof(int));
        int i, j;
        int k = 0; /*Index for group_array*/
        bool errorVal;

        for (i = 0; i < 3; i++) {
                for (j = 0; j < 3; j++) {
                        /*Put 3 x 3 groups in group_array*/
                        *((int *)UArray2_at(group_array, k, 0)) = 
                                *((int *)UArray2_at(array, 
                                                        width + i, height + j));
                        k++;
                }
        }
        UArray2_map_row_major(group_array, check_rows, &temp_array);
        UArray2_free(&group_array);
        errorVal = check_error_report(temp_array);
        UArray2_free(&temp_array);
        return errorVal;
}

/*
 * Sets all indices in array to 0
 */
void set_to_zero(int width, int height, UArray2_T array, void *p1, void *p2)
{
        (void) p1;
        (void) p2;

        *((int *)UArray2_at(array, width, height)) = 0;
}
