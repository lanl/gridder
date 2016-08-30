
/**************************************************************/
/*******		Simple Grid Generator           *******/
/*******						*******/ 
/*******		by Loraine Lundquist            *******/
/*******						*******/
/**************************************************************/
/*
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C                         Copyright, 1995                              C
C This program was prepared by the Regents of the University of        C
C California at Los Alamos National Laboratory (the University) under  C
C Contract No. W-7405-ENG-36 with the U.S. Department of Energy (DOE). C
C The University has certain rights in the program pursuant to the     C
C contract and the program should not be copied or distributed outside C
C your organization. All rights in the program are reserved by the DOE C
C and the University. Neither the U.S. Government nor the University   C
C makes any warranty, express or implied, or assumes and liability or  C
C responsibility for the use of this software.                         C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
*/
/************************************************************************/
/*  This program allows the user to create a simple 2-D or 3-D grid by  */
/*  keyboard input.  The grid may have any number of "regions" in each  */
/*  axis.  A zone is a region of similar nodal spacing.  That spacing   */
/*  may be equal(linear), geometric, or logarithmic.  		      	*/
/*									*/
/* For each region on each axis, the user must input the beginning node */  
/* coordinate on the axis being configured, the end node coordinate,    */
/* and the number of divisions the region is to be divided into.   A    */
/* division is the space between two nodes, so the number of divisions  */
/* = number of nodes - 1.  For geometric spacing, the user must also    */
/* input a geometric factor.  The user will be prompted for each of     */
/* these values as needed.                                              */
/*									*/
/* Of these input values, the number of dimensions, the number of zones */
/* in an axis, the number of divisions in a zone, and the geometric     */
/* factor must be greater than 0.  					*/ 
/*									*/
/* The maximum number of nodes per axis is MAXNODES = 16385             */
/* changed simply by changing the constant named MAXNODES, and the 	*/
/* maximum number of zones per axis MAXZONES = 500, and limits may be 	*/
/* changed simply by changing the constants MAXNODES and MAXZONES.      */      
/*									*/
/* The results are then written to a file called grid.inp.  This        */
/* file is rewritten upon every run of the program, so save the results */
/* of each trial to a different file name.                              */
/*									*/
/* The input parameters are written to a file called input.grid, so     */
/* that if the user wants to regenerate the grid without answering all  */
/* the same questions again, he or she can save input.grid to a		*/ 
/* different name, and type makegrid <(filename).  Be sure to rename    */ 
/* input.grid before trying to use it as an input file.  		*/
/************************************************************************/


/* MODIFICATIONS:
 *
 * March 29 2011 by tamiller@lanl.gov
 * changed precision from 14.8g to 20.12g
 *
 * March 29 2011 by tamiller@lanl.gov
 * changed Flotran to Vectors format since it is no longer supported 
 *
 * March 25 2011 by tamiller@lanl.gov
 * fixed loop for writing element connectivity for 1 dimension
 * the connectivity list was writing node numbers higher than max node 
 *
 * Jan 13 2011 - gridder is now in mercurial hg under lg_util/gridder
 *
 ********************************************************************/
/*
 *
$Log:   /pvcs.config/grid_utilities/src/gridder.c_a  $
 *    Rev 2.0   added write fehm feature from Zora (zvd@lanl.gov)
 * 
 *    Rev 1.3   16 Jul 2001 16:50:08   tam
 * changed printf for writing AVS hex connectivity so that
 * top of hex is written, then bottom in counter-clockwise rotation
 *PVCS    
 *PVCS       Rev 1.0   15 Jul 2001 17:12:08   tam
 *PVCS    Initial revision.
 * 
 *    Rev 1.2   Fri Jun 25 12:38:34 1999   murphy
 * Added FLOTRAN output
 * 
 *    Rev 1.0   Tue Jan 21 15:43:36 1997   pvcs
 *  
 * 
 *    Rev 1.6   09/06/95 17:20:16   lund
 * fixed bugs in logarithmic spacing when smallest dx specified
 * 
 *    Rev 1.5   08/04/95 16:30:48   lund
 * User may now input smallest dx instead of number of divisions.
 * To do this, input 0 for number of divisions.
 * 
 * User may now correct axis' coordinates if mistake was made.
 * 
 * Geometric spacing is now more accurate.
 * 
 * Added error statements to ensure that program will terminate even if input file is incorrect.
 * 
 *    Rev 1.4   07/10/95 15:11:18   lund
 * fixed geometric spacing
 * fixed zone assignments
 * fixed so that it works on the first run
 * 
 *    Rev 1.3   07/10/95 15:05:04   lund
 * added log history
 * Rev 1.2
 * Checked in:     02/22/95 12:08:06
 * Last modified:  02/22/95 12:00:30
 * Author id: gable     lines deleted/added/moved: 9/5/0
 *     Change connectivity order of quad output. Quads were being written
 *       in reverse order.
 * -----------------------------------
 * Rev 1.1
 * Checked in:     02/22/95 11:01:36
 * Last modified:  02/22/95 11:01:28
 * Author id: llt     lines deleted/added/moved: 2/2/0
 * increased size of arrays
 * -----------------------------------
 * Rev 1.0
 * Checked in:     02/13/95 09:52:42
 * Last modified:  02/13/95 09:50:12
 * Author id: pvcs     lines deleted/added/moved: 0/0/0
 *  
 * ===================================
*/

#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXNODES 16385
#define MAXZONES 500
#define ERR_LEVEL 3

double xcoords[MAXNODES]; 
double ycoords[MAXNODES];
double zcoords[MAXNODES];


/* primary function is assign_grid_coords_and_elements()
   the remaining functions are called by it.
*/

#ifndef PROTO 

/* driver function */
int assign_grid_coords_and_elements();

/*  Returns number of nodes in axis. */
int assign_axis_coords();

/*  Returns number of nodes in zone. */
int assign_region_coords(); 

/*  The following functions are called by assign_region_coords()   */
void geom(); 
void log_it(); 
void equal(); 
void print_spacing_info();
void print_avs_coords();
void print_fehm_coords();
void print_tracer_coords();
void print_vectors_coords();
void print_zone_bounds(); 
void assign_elements();

#else

int assign_grid_coords_and_elements(FILE *fp_out, FILE *fp_in); 

int assign_axis_coords(double *ptr, char axis, int *num_elems_per_region,
               int *num_axis_region, FILE *fp_in);	

int assign_region_coords(double **ptr1, int region_num, char axis, 
               int *num_elems_per_region, FILE *fp_in);

void geom(int region_num, char axis, int num_divisions, 
                double begin, double end, double **x, FILE *fp_in);
void log_it(int num_divisions, double begin, double end, 
                double **x, int direction);
void equal(int num_divisions, double begin, double end, 
                double **x, FILE *fp_in);
void print_spacing_info(void);
void print_avs_coords(int num_xnodes, int num_ynodes, int num_znodes,
                int num_elems, FILE *fp_out);
void print_fehm_coords(int num_xnodes, int num_ynodes, int num_znodes,
                int num_elems, int num_dimensions, FILE *fp_out);
void print_vectors_coords(int num_xnodes, int num_ynodes, int num_znodes,
                int num_elems, FILE *fp_out);
void print_tracer_coords(int num_xnodes, int num_ynodes, int num_znodes,
                FILE *fp_out);
void print_zone_bounds(int num_xregions,int num_yregions, int 
                num_zregions, int *num_xelems, int *num_yelems, 
                int *num_zelems);
void assign_elements(int num_xnodes, int num_ynodes, int num_znodes,
                int *num_xelems, int*num_yelems, int *zelems,
                int num_xregions, int num_yregions, int num_dimensions,
                int output, FILE *fp_out);

#endif PROTO

/*****************************************************************************
* MAIN
*	
******************************************************************************/

main()
{
    FILE *fp_out, *fp_in;
    int error, file_exist;
    char filename[12];


    fp_out = fopen("grid.inp", "w");

    if ((fp_out = fopen("grid.inp", "w")) == NULL)
	printf ("Could not open new output file 'grid.inp.'\n");

    /* Check to see if input.grid already exists
       0 means does not exist, -1 means file does not exist */
    file_exist = open("./input.grid", O_RDONLY);
    if (file_exist != -1) 
    {
        printf("File input.grid already exists.\n");
        strcpy(filename, "input.tmp");
        printf("File %s will be written instead.\n\n", filename);
    }
    else {
        strcpy(filename, "input.grid");
    }

    /* Open the output file */
    if ((fp_in = fopen(filename, "w")) == NULL)
    {
	printf ("Could not open new file 'input.grid' for copy "); 
	printf ("of input parameters.\n");
    }
    else
    {
	error = assign_grid_coords_and_elements(fp_out, fp_in);
        if (error == -1) {
           exit(0); 
	}

	printf("\nProgram gridder has completed.\n");
	printf("The following files have been written:\n\n");

	if (error == 1) {
	  printf("grid.inp     contains coordinate & connectivity in AVS format.\n");
	}
	if (error == 2) {
	  printf("grid.inp     contains coordinate data in TRACER3D format.\n");
	}
	if (error == 3) {
	  printf("grid.inp     contains coordinate data in FLOTRAN format.\n");
	}
	if (error == 4) {
	  printf("grid.inp     contains coordinate data in FEHM format.\n");
	}

	if (file_exist != -1) 
	  printf("input.tmp    contains input values you have generated during this run.\n");
	else
	  printf("input.grid   contains input values you have generated during this run.\n");
	printf("\nNOTE: input.grid is generated on first run and will not be overwritten, input.tmp can be overwritten. To save input.tmp, cp or mv  to another file.\n");
	printf("To repeat this run:\n");
	if (file_exist != -1) 
	  printf("   cp input.tmp input.grid\n");
	printf("   gridder < input.grid\n\n", filename); 
	
    }

   fclose (fp_out);
   fclose (fp_in);
   exit(0);
}
/* End Main */





/*****************************************************************************
* FUNCTION: assign_grid_coords_and_elements		
*                                                       
* PURPOSE: Assigns all grid coordinates and connectivity.
*****************************************************************************/

int assign_grid_coords_and_elements(fp_out, fp_in)
FILE *fp_out; 
FILE *fp_in;
  {
        /* Store coordinates in each axis. */
	extern double xcoords[], ycoords[], zcoords[];

	/* Pointers to coordinate arrays 
           to be passed to following functions. */
	double *xptr = xcoords, *yptr = ycoords, *zptr = zcoords;

	/* Store number of elements in each zone in each axis. */
	int num_xelems[MAXZONES], num_yelems[MAXZONES], num_zelems[MAXZONES];

	/* Pointers to element arrays to be passed to */
	/* following funcitons. */
	int *regionptrx = num_xelems; 
        int *regionptry = num_yelems; 
	int *regionptrz = num_zelems;

        /* Number of dimensions in grid */
	int num_dimensions;

	/* Number of nodes in each axis. */
	int num_xnodes = 1, num_ynodes = 1, num_znodes = 1;

	/* Number of zones in each axis. */ 
	int num_xregions=1, num_yregions=1, num_zregions=1;

	/* Number of elements. */
	int num_elems;

	/* Name of axis being configured (x, y, or z). */
	char axis;

	/* Number describing format of output. */
	int output;

	/* Counters. */
	int i, j, k, error_count;

        /* Begin execution */

        /* Initialize arrays */
 
        for (i=0; i < MAXNODES; i++){
                xcoords[i] = 0.0;
                ycoords[i] = 0.0;
                zcoords[i] = 0.0;
         } 

	for (i=0; i < MAXZONES; i++)
	   {
		num_xelems[i] = 1;
		num_yelems[i] = 1;
		num_zelems[i] = 1;
	   }

        error_count = 0;
	do
	{
	    printf("Please input an integer number of dimensions for grid.\n");
	    scanf("%d", &num_dimensions);

	    if ((num_dimensions < 1) || (num_dimensions > 3))
            {
		printf("\nYou can only have 1, 2, or 3 dimensions.\n");
                error_count++;
                if (error_count > ERR_LEVEL)
                {
                   printf("You only get %d chances.\nProgram Exiting...\n", ERR_LEVEL);
                   exit(2);
                }
            }

	 } while ((num_dimensions > 3) || (num_dimensions < 1));

	fprintf(fp_in, "%d\n", num_dimensions);

   /*********************************************
         Calculation of coordinates in each axis.  
   *********************************************/
   for (i=0; i < num_dimensions; i++)
   {
	if (i==0) 
        {
		axis='x';
		num_xnodes = assign_axis_coords (xptr, axis, num_xelems,
			&num_xregions, fp_in);

		for (j=0; j < num_xnodes; j++)
			printf("xcoords[%d] = %20.12g\n", j, xcoords[j]);
		printf("\n\n");

	}

	else if (i==1) 
	{
            axis = 'y';
	    num_ynodes = assign_axis_coords (yptr, axis, num_yelems,
			&num_yregions, fp_in);

	    for (j=0; j < num_ynodes; j++)
		printf("ycoords[%d] = %20.12g\n", j, ycoords[j]);

            printf("\n\n");
	}

	else if (i==2) 
	{
		axis = 'z';
		num_znodes = assign_axis_coords (zptr, axis, num_zelems,
			&num_zregions, fp_in);
		for (j=0; j < num_znodes; j++)
			printf("zcoords[%d] = %20.12g\n", j, zcoords[j]);
		printf("\n\n");
	}
        else
           printf("%d dimension not recognized, skipping...\n",i);

   } 
   /* end calculating axis coordinates*/


   /* If too many nodes or too many zones, 
      then erase everything and start over. */

   if ((num_xnodes > MAXNODES) || (num_ynodes > MAXNODES) || (num_znodes > MAXNODES))
   {
	printf("\n\nWoa, that's too many nodes!  I can't handle more ");
	printf("than %d nodes in any axis.  You're going ", MAXNODES);
	printf("to have to start over.\n\n");
	main();
	return 1;
    }

    if (num_xregions * num_yregions * num_zregions > MAXZONES)
    {
	printf("\n\nWoa, that's too many zones! I can't handle more ");
	printf("than %d total zones in a grid. You're ", MAXZONES);
	printf("going to have to start over.\n\n");
	main();
	return 1;
    }


    /* Calculation of number of elements. */	

    /* 1-D */
    if ((num_ynodes <= 1) && (num_znodes <= 1))
	num_elems = (num_xnodes -1);
    /* 2-D */
    else if (num_znodes <= 1)
	num_elems = ((num_xnodes -1) * (num_ynodes - 1));
    /* 3-D */
    else 
	num_elems = ((num_xnodes - 1) * (num_ynodes - 1) * (num_znodes - 1));


    /* Determination of what type of output desired. */
    printf("\nPlease specify output:\n");
    printf("1 = Advanced Visualization Systems (AVS) input ");	
    printf("specifications.\n");
    printf("2 = Tracer3d coordinate input specifications. \n");
    printf("3 = Vectors : x(i) y(j) z(k) \n");
    printf("4 = FEHM coordinate input specifications. \n\n");

    scanf("%d", &output);
    fprintf(fp_in, "%d", output);


    switch(output) { 
      case 1:
        /* If output is AVS, print coordinates in AVS UCD format. */
	{
	  print_avs_coords(num_xnodes, num_ynodes, num_znodes, 
			   num_elems, fp_out);
	  
	  print_zone_bounds(num_xregions, num_yregions, 
			  num_zregions, regionptrx, regionptry, regionptrz);

	  assign_elements(num_xnodes, num_ynodes, num_znodes, 
			  regionptrx, regionptry, regionptrz, 
			  num_xregions, num_yregions, num_dimensions, 
			  output, fp_out);
	  return 1;
	}
      case 2:
        /* If output is Tracer, print coordinates in Tracer3d format. */
        {

	  print_tracer_coords(num_xnodes, num_ynodes, num_znodes, fp_out);
	  return 2;
	}

      case 3:
        /* If output is Vectors, print coordinates in Vectors format. */
        {
	  print_vectors_coords(num_xnodes, num_ynodes, num_znodes, fp_out);
	  return 3;
        }
 
      case 4:
        /* If output is FEHM, print coordinates in FEHM format. */
        {
	  print_fehm_coords(num_xnodes, num_ynodes, num_znodes, 
			   num_elems, num_dimensions, fp_out);
	  assign_elements(num_xnodes, num_ynodes, num_znodes, 
			  regionptrx, regionptry, regionptrz, 
			  num_xregions, num_yregions, num_dimensions, 
			  output, fp_out);
	  fprintf (fp_out, "\n");
	  fprintf (fp_out, "%-s\n", "stop");
	  return 4;
        } 

    default:
       {
	 printf("Invalid output selection %d\n");
	 return -1;
       }
    }

  }

/* End assign_grid_coords_and_elements() */

/******************************************************************************
* FUNCTION: assign_axis_coords()
*
* PURPOSE:  Calculates coordinates for each axis. 
******************************************************************************/

int assign_axis_coords(ptr, axis, num_elems, num_regions, fp_in)
double *ptr; 
char axis;
int *num_elems;
int *num_regions;
FILE *fp_in;
   {
	int numr = NULL;
		/* Stores number of regions before assigning its value to */ 
		/* num_zones to avoid pointer complications. */
	int num_nodes = 0;
		/* Keeps track of number of coordinates in each region   */
		/* so that function assign_region_coords starts          */
		/* writing coordinates where the last region left off    */
		/* within the coordinate array, instead of writing       */
		/* over the coordinates assigned in the previous region. */

	int temp; 
		/* Temporary storage for the number of nodes in a   */
		/* single region before it is added on to the total */
		/* number of nodes in the axis.			    */

        int i, error_count;
		/* Counter. */

	error_count = 0;
	do {
	        printf("\nPlease input an integer number of regions for axis %c.\n", axis);
        	scanf("%d", &numr);
		if (numr <= 0)
			printf("You have to have at least one region.\n");
		error_count++;
	        if (error_count > ERR_LEVEL)
	        {
       	           printf("You only get %d chances.\nProgram Exiting...\n", ERR_LEVEL);
		   exit(2);
               	}

	   } while (numr <= 0);

	fprintf(fp_in, "%d\n", numr);


/* Assign coordinates for each region. */

        for (i=1; i <= numr; i++)
	   {
                temp = assign_region_coords(&ptr, i, axis, num_elems + i - 1, 
			fp_in);

/* Num_nodes is total number of nodes in axis = sum of number */
/* of nodes in each region in axis. */

		num_nodes = num_nodes + temp;
	   }

/* If this axis doesn't exist, i.e. if */ 
/* num_regions = NULL, then the total  */
/* number of nodes in this axis is 1.  */

	*num_regions = numr;

	if (!numr)
		num_nodes = 1;

	return num_nodes;
   }

/******************************************************************************
* FUNCTION  assign_region_coords()
*
* PURPOSE: Calculates coordinates within each region.	
******************************************************************************/

int assign_region_coords(x, region_num, axis, num_elems, fp_in)
double **x;
int region_num;
char axis;
int *num_elems;
FILE *fp_in;

/* **x is the value of the first node in the array   */ 
/* containing the coordinates for the present axis.  */
/* *x is the address of that value.                  */
/* In other words, x is a pointer to a pointer to an */
/* array of doubleing point nodal values.             */
/* Though the variable name x is used, this function */ 
/* calculates the region coordinates for the y and   */
/* z axes, as well.                                  */

{

    
    /* Begin and end coordinates */
    double begin_region;
    double end_region;

    /* Number of divisions. */
    int num_divisions;

    /*If don't specify number of divisions, specify dx. */
    double dx=0.0;

    /* Tells whether to space logarithmically spaced nodes forward
       (small dx to large) or backward (large dx to small. */
    int direction;

    /* Determines equal, geometric, or logarithmic spacing. */
    int skew_factor=1;

    /* Counters */
    int i, error_count, error_count_two; 



   /* Determination of beginning and end */
   /* coordinates, number of divisions,  */
   /* and spacing of each region through */
   /* keyboard input.                    */
   error_count = 0;
   do 
   {

       /* Input for BEGIN coordinates */
       if (region_num == 1)
       {
            printf ("\nPlease input coordinate to BEGIN region %d of axis %c.\n",
                    region_num, axis);
       	    scanf ("%lf", &begin_region);
       }
       else begin_region = **x;

        /* Input for END coordinates */
        if (region_num > 1)
           printf("BEGIN coordinate for this region is %20.12g.",begin_region); 
        printf("\nPlease input coordinate to END region %d of axis %c. \n", 
                	region_num, axis);
        scanf ("%lf", &end_region);

	if (end_region <= begin_region)
        {
	    printf("BEGIN coordinate should be less than END coordinate.\n");
	    printf("Values entered: BEGIN = %20.12g, END = %20.12g\n",
                    begin_region, end_region);
            error_count++;
            if (error_count > ERR_LEVEL)
            {
                 printf("You only get %d chances.\n",ERR_LEVEL);
                 printf("Program Exiting...\n");
                 exit(2);
            }
        }
   } while(end_region <= begin_region);

   if (region_num == 1) fprintf(fp_in,"%20.12g\n", begin_region);
   fprintf(fp_in, "%20.12g\n", end_region);

   /* Input for number of divisions */
   error_count = 0;
   do 
   {
       	printf ("\nPlease input an integer number of divisions for region %d of axis %c.\nIf you would prefer to specify d%c, then input 0 here.\n", region_num, axis, axis);
       	scanf ("%d", &num_divisions);

	if (num_divisions < 0)
        {
	    printf("You have to have at least one division. ");
            error_count++;
            if (error_count > ERR_LEVEL)
            {
                printf("You only get %d chances.\n",ERR_LEVEL);
                printf("Program Exiting...\n");
                exit(2);
            }
        }
   } while(num_divisions < 0);

   /* If input 0 for number of divisions, then input dx.*/
   error_count = 0;
   if (num_divisions == 0)
   {
	do 
	{
		printf("\nPlease input smallest d%c.\n",axis);
		printf("d%c may be made smaller to divide evenly into region.\n", axis);	
		scanf("%lf", &dx);
		if ((dx<=0) || (dx > (end_region - begin_region))) 
			printf("\nD%c must be greater than zero and less than region length = %lf.", axis, (end_region - begin_region));
		error_count++;
	        if (error_count > ERR_LEVEL)
	        {
       	           printf("You only get %d chances.\nProgram Exiting...\n", ERR_LEVEL);
         	   exit(2);
               	}

	} while ((dx<=0) || (dx > (end_region - begin_region))); 
   }

   fprintf(fp_in, "%d\n", num_divisions);
   if (num_divisions == 0) 
	fprintf(fp_in, "%lf\n", dx);
 
   /* Input for type of spacing desired. */
   error_count = 0;
   do
   {
        printf("\nPlease input spacing for region %d:\n", region_num);
	printf("0 = more information about spacing.\n");
 	printf("1 = equally spaced nodes.\n");
	printf("2 = geometric spacing of nodes.\n");
	printf("3 = logarithmic spacing of nodes, small spacing to "); 
	printf("large.\n");
	printf("4 = logarithmic spacing of nodes, large spacing to "); 
	printf("small.\n\n");

       	scanf("%d", &skew_factor);
	if ((skew_factor < 0)||(skew_factor > 4))
        {
             printf("\nI asked for a 0, 1, 2, 3, or 4.");
             printf("Please try again.\n\n");
             error_count++;
             if (error_count > ERR_LEVEL)
             {
                printf("You only get %d chances.\n",ERR_LEVEL);
                printf("Program Exiting...\n");
                exit(2);
             }
        }
    } while ((skew_factor < 0) || (skew_factor > 4));

    error_count = 0;
    while (skew_factor == 0)
    {
	print_spacing_info();
	printf("\n\n\nRegion number %d of axis %c.\n", region_num, axis);
	printf("First coordinate of region = %20.12g.\n", begin_region);
	printf("Last coordinate of region = %20.12g.\n", end_region);
	printf("Number of divisions = %d.\n\n", num_divisions);
	error_count++;
        if (error_count > ERR_LEVEL)
        {
              printf("You only get %d chances.\n",ERR_LEVEL);
              printf("Program Exiting...\n");
              exit(2);
        }

	error_count_two=0;
	do
	{
	        printf("Please input spacing for region %d:\n", region_num);
		printf("0 = more information about spacing.\n");
 		printf("1 = equally spaced nodes.\n");
		printf("2 = geometric spacing of nodes.\n");
		printf("3 = logarithmic spacing of nodes, small "); 
		printf("spacing to large.\n");
		printf("4 = logarithmic spacing of nodes, large "); 
		printf("spacing to small.\n\n");
       		scanf("%d", &skew_factor);
		if ((skew_factor < 0)||(skew_factor > 4))
                {
			printf("\nI asked for a 0, 1, 2, 3, or 4. ");
			printf("Please try again.\n\n");
	             	error_count_two++;
             		if (error_count_two > ERR_LEVEL)
             		{
              		  	printf("You only get %d chances.\n",ERR_LEVEL);
                		printf("Program Exiting...\n");
                		exit(2);
             		}
	        }
        } while ((skew_factor < 0) || (skew_factor > 4));

     }/* End skew_factor spacing info loop */


     fprintf(fp_in, "%d\n", skew_factor);
     printf("\n\n");


     if (skew_factor == 2)
	geom(region_num, axis, &num_divisions, dx, begin_region, end_region, x, fp_in);

     else if (skew_factor == 3)
     {
	direction = 1;
	log_it(&num_divisions, dx, begin_region, end_region, x, direction);
     }
     else if (skew_factor == 4)
     {
	direction = 0;
	log_it(&num_divisions, dx, begin_region, end_region, x, direction);
     }
     else if (skew_factor == 1)
     {
	equal(&num_divisions, dx, begin_region, end_region, x, fp_in);
     }

     *num_elems = num_divisions;

    /* Set the position of the array pointer to the    */
    /* next element after the coordinates that have    */
    /* just been assigned.  That way, the nodal values */
    /* that are assigned in the next region will not   */
    /* overwrite the values assigned in this region.   */

    (*x) = (*x) + num_divisions;


    /* Return total number of nodes in region for */ 
    /* purpose  of printing out the values.       */

    if (region_num == 1)
	return (num_divisions + 1);
    else return num_divisions;

 }
/* End  assign_region_coords() */

/******************************************************************************
* FUNCTION: equal
*
* PURPOSE:Spaces nodes equally.						     
******************************************************************************/

void equal(num_div, input_dx, begin, end, x, fp_in)
int *num_div;
double input_dx;
double begin;
double end;
double **x;
FILE *fp_in;
   {
	double dx;
	int i;

	/* If user has chosen to input dx instead of number of divixions, */
	/* then number of divisions = length of region/dx.		  */
	if (*num_div == 0)
		*num_div = ceil(((end - begin)/input_dx));

        /* Dx = length of region/number of divisions. */ 
	dx = ((end - begin)/(*num_div));

        /* The value of the i'th element of the array of */
        /* coordinates = (i * dx) +  the value of the    */
        /* first coordinate in the region.               */

  	for (i=0; i<= *num_div; i++) {
               	*((*x) + i) = ((i * dx) + begin);
        }

   }

/******************************************************************************
* FUNCTION: geom
*
* PURPOSE: Spaces nodes geometrically
******************************************************************************/

void geom(region, axis, num_divisions, input_dx, begin, end, x, fp_in)
int region;
char axis;
int *num_divisions;
double input_dx;
double begin;
double end; 
double **x;
FILE *fp_in;
/* x is again a pointer to a pointer to the   */
/* array of coordinates for the current axis. */

   {

	double geom_factor;	
		/* Inputted growth factor of spacing. */

	double temp_geom_factor;	
		/* Temporary storage for geometric factor. */

	double num_eq_div = 0.0;	
		/* Number of equal divisions, i.e. the number of        */
		/* times the smallest dx  divides into the total        */
		/* length of the zone.  (Every other dx is a            */
		/* multiple of the smallest dx and geom_factor).  	*/
		/* It is a doubleing point number to allow for the 	*/
		/* case of a doubleing point geom_factor, which    	*/
		/* would cause a non-integer number of equal divisions. */

	double dx;
		/* Space between nodes. */

	double temp;
		/* Temporary storage for variables. */

	double j;
	int i, error_count;
		/* Counters. */


/* Set the first element of the array equal to  */
/* the inputted first coordinate of the region. */
 
	**x = begin;


/* Input geom_factor.*/
	error_count=0;
	do {
		printf("Please input geometric factor for region %d, axis %c\n\n", region, axis);
		scanf("%lf", &geom_factor);
		if (geom_factor <= 0)
		   {
			printf("Geometric factor must be greater than 0.\n");
			error_count++;
             		if (error_count > ERR_LEVEL)
             		   {
                		printf("You only get %d chances.\n",ERR_LEVEL);
                		printf("Program Exiting...\n");
                		exit(2);
             		   }
		   }
	   } while (geom_factor <= 0);
	fprintf(fp_in, "%20.12g\n", geom_factor);

/* If user has chosen to input dx rather than number */
/* of divisions,then num_divisions will equal 0.     */

	if (*num_divisions == 0)
	   {

/* If geometric factor is less than one, need its */
/* inverse to calculate the number of divisions.  */

		if (geom_factor < 1.0)
			temp_geom_factor = (1.0/geom_factor);
		else temp_geom_factor = geom_factor;
		num_eq_div = ((end-begin)/input_dx);
		temp = num_eq_div;
		for(j=0.0; temp > 0; j++)
			temp = (temp - pow(temp_geom_factor, j));
		*num_divisions = j;
	   }

/* num_eq_div = geom_factor^0 + geom_factor^1 +  */ 
/* geom_factor^2 + ... geom_factor^i,            */
/* where i = total number of divisions.)         */

	num_eq_div = 0;
	for (j=0.0; j < *num_divisions; j++)
		num_eq_div = num_eq_div + pow(geom_factor, j);

/* The initial dx = length of region/ number of equal divisions.  */

	dx = (end - begin)/num_eq_div;


/* Subsequent dx's = geom_factor * previous dx.   */
/* Present coordinate = previous coordinate + dx. */

	for (i=1; i <= *num_divisions; i++)
	   {
		*((*x) + i) = *((*x) + i - 1) + dx;
		dx = dx * geom_factor;
	   }
   }

/******************************************************************************/
/* FUNCTION: log_it							      */
/* PURPOSE: Spaces nodes logarithmically.				      */
/******************************************************************************/

void log_it(num_div, input_dx, begin, end, x, direction)
int *num_div;
double input_dx;
double begin;
double end;
double **x;
int direction;
   {
	int i;
		/* Counter.*/

/* If user chose to input dx rather than number */
/* of divisions, calculate number of divisions. */

	if (*num_div == 0)
	    *num_div = ceil((-9.0/(pow(10.0,(((end-begin)-input_dx)/(end - begin))) - 10)));

/* Set the first element of the array equal to  */
/* the inputted first coordinate of the region. */
/* Direction is large to small.			*/		

	if (direction == 0)
	   {
		for (i=0; i <= *num_div; i++)
			*(*x + i) = begin + ((log10(1 + ((9.0/(*num_div))*i))) * (end - begin));
	   }

/* If spacing is from small dx to large, follow same process   */
/* except starting from the end of the region and subtracting. */

	else if (direction == 1)
	   {
		for (i=0; i <= *num_div; i++)
			*(*x + *num_div - i) = end - ((log10(1 + ((9.0/(*num_div))*i))) * (end - begin));
	   }
   }

/******************************************************************************/
/* FUNCTION: print_avs_coords() 					      */
/* PURPOSE: Prints coordinates to file in AVS format.				      */
/******************************************************************************/
void print_avs_coords(num_xnodes, num_ynodes, num_znodes, num_elems, fp_out)
int num_xnodes;
int num_ynodes;
int num_znodes;
int num_elems;
FILE *fp_out;
   {
	int i, j, k;
		/* Counters. */

/* Print to file according to AVS input number of nodes, number of elements, 
   and number of regions as the number of material specifcations.
   left justify first numbers, AVS doesn't like a space in first column. */

	fprintf(fp_out, "%-d %d 0 0 0\n", num_xnodes*num_ynodes*num_znodes,  
		num_elems);

/* Print coordinates to file. */
	for (k=0; k < num_znodes; k++)
		for (j=0; j < num_ynodes; j++)
			for (i=0; i < num_xnodes; i++)
				fprintf(fp_out, "%-11d  %20.12g  %20.12g  %20.12g\n", i+1+j*num_xnodes+k*num_xnodes*num_ynodes, xcoords[i], ycoords[j], zcoords[k]);


   }

/******************************************************************************/
/* FUNCTION: print_fehm_coords() 					      */
/* PURPOSE: Prints coordinates to file in FEHM format.				      */
/******************************************************************************/
void print_fehm_coords(num_xnodes, num_ynodes, num_znodes, num_elems, num_dimensions, fp_out)
int num_xnodes;
int num_ynodes;
int num_znodes;
int num_elems;
int num_dimensions;
FILE *fp_out;
   {
	int i, j, k;
		/* Counters. */

/* Print to file according to AVS input number of nodes, number of elements, 
   and number of regions as the number of material specifcations.
   left justify first numbers, AVS doesn't like a space in first column. */

        fprintf(fp_out, "%-s\n", "coor");
	fprintf(fp_out, "%d\n", num_xnodes*num_ynodes*num_znodes);

/* Print coordinates to file. */
	for (k=0; k < num_znodes; k++)
		for (j=0; j < num_ynodes; j++)
			for (i=0; i < num_xnodes; i++)
				fprintf(fp_out, "%11d  %20.12g  %20.12g  %20.12g\n", i+1+j*num_xnodes+k*num_xnodes*num_ynodes, xcoords[i], ycoords[j], zcoords[k]);

	fprintf(fp_out, "\n");
	fprintf(fp_out, "%-s\n", "elem");
	if (num_dimensions == 3) {
	  fprintf(fp_out, "8 %d\n", num_elems);
	} else if  (num_dimensions == 2) {
	  fprintf(fp_out, "4 %d\n", num_elems);
	} else {
	  fprintf(fp_out, "2 %d\n", num_elems);
	}
   }

/*****************************************************************************/
/* FUNCTION: print_vectors_coords() 					     */
/* PURPOSE: Prints coordinates to file in Vectors : x(i) y(j) z(k)	     */
/* Writes no more than 10 values to each line.
 * Note NX, NY, and NZ are at least 1
 *
 *      NX NY NZ
 *    x(i) i = 1,nx
 *    y(j) i = 1,ny
 *    z(k) i = 1,nz
 *
 */
/*****************************************************************************/
void print_vectors_coords(num_xnodes, num_ynodes, num_znodes, fp_out)
int num_xnodes;
int num_ynodes;
int num_znodes;
FILE *fp_out;
   {


     int i;
     fprintf(fp_out, "%14d %14d %14d\n",num_xnodes,num_ynodes,num_znodes);

/* x(i)
*/
     for (i=0; i < num_xnodes; i++) {
       fprintf(fp_out, "%20.12g ", xcoords[i]);
       if (((i+1)%10) == 0) {
	 fprintf(fp_out, "\n");
       } 
     }
     /* don't put extra blank lines.  Flotran could freak out.  */
     if ((num_xnodes > 0) && (num_xnodes % 10) > 0) {
       fprintf(fp_out, "\n");
     } 


/* y(j)
*/
     for (i=0; i < num_ynodes; i++) {
       fprintf(fp_out, "%20.12g ", ycoords[i]);
       if (((i+1)%10) == 0) {
	 fprintf(fp_out, "\n");
       } 
     }
     /* don't put extra  blank lines.  Flotran could freak out.*/
     if ((num_ynodes > 0) && (num_ynodes % 10) > 0) {
       fprintf(fp_out, "\n");
     } 


/* z(k)
*/
     for (i=0; i < num_znodes; i++) {
       fprintf(fp_out, "%20.12g ", zcoords[i]);   
       if (((i+1)%10) == 0) {
	 fprintf(fp_out, "\n");
       } 
     }
     /* don't put extra  blank lines.  Flotran could freak out.*/
     if ((num_znodes > 0) && (num_znodes % 10) > 0) {
       fprintf(fp_out, "\n");
     }

   }

/******************************************************************************/
/* FUNCTION: print_tracer_coords         				      */
/* PURPOSE: Prints coordinates to file in Tracer3d format.		      */
/******************************************************************************/
void print_tracer_coords(num_xnodes, num_ynodes, num_znodes, fp_out)
int num_xnodes;
int num_ynodes;
int num_znodes;
FILE *fp_out;
   {
	int i;
		/* Counter. */

	fprintf(fp_out, "rcm=");
	for (i=0; i < num_xnodes-1; i++)
	fprintf(fp_out, "%20.12g,1,", xcoords[i]);
	fprintf(fp_out, "%20.12g,\n", xcoords[num_xnodes-1]);

	fprintf(fp_out, "ycm=");
	for (i=0; i < num_ynodes-1; i++)
	fprintf(fp_out, "%20.12g,1,", ycoords[i]);
	fprintf(fp_out, "%20.12g,\n", ycoords[num_ynodes-1]);

	fprintf(fp_out, "zcm=");
	for (i=0; i < num_znodes-1; i++)
	fprintf(fp_out, "%20.12g,1,", zcoords[i]);
	fprintf(fp_out, "%20.12g,\n", zcoords[num_znodes-1]);

   }

/******************************************************************************/
/* FUNCTION: print_spacing_info()						      */
/* PURPOSE:  Prints out more info about spacing.			      */
/******************************************************************************/
void print_spacing_info()
   {
	int which_info;
		/* Number describing which subject user would */
		/* like more information about. */
	int menu;
		/* Allows user to return to menu when finished reading */
		/* information. */
	int error_count;
		/* Counter */

	error_count = 0;
	printf("\n\nHELP MENU:\n\n");
	do
	   {
		printf("Please input which subject you would like more ");
		printf("information about: \n");
		printf("0 = return to program.\n");
	 	printf("1 = equally spaced nodes.\n");
		printf("2 = geometric spacing of nodes.\n");
		printf("3 = logarithmic spacing of nodes.\n");
		scanf("%d", &which_info);
		if ((which_info < 0) || (which_info > 3))
		   {
			printf("Please input a 0, 1, 2, or 3.\n\n");
	             	error_count++;
	             	if (error_count > ERR_LEVEL)
	                   {
	                	printf("You only get %d chances.\n",ERR_LEVEL);
	                	printf("Program Exiting...\n");
	                	exit(2);
	                   }
        	   }
	   }
	while ((which_info < 0)||(which_info > 3));

	if (which_info == 0)
		return;

	if (which_info == 1)
	   {
		printf("\n\nEQUAL SPACING\n");
		printf("\nIn a region with equal spacing, all of the nodes are equally spaced.  This distance between any two nodes in the region is equal to the length of the region divided by its number of divisions.  For example, if the user inputs a region 10 units long with 10 divisions, the distance between every pair of consecutive nodes in the region equals 1.  \n");

	   }

	else if(which_info == 2)
	   {
		printf("\n\nGEOMETRIC SPACING: \n\n");
		printf("In a region with geometric spacing, the distance between each node increases by a factor inputted by the user.  (The user will be prompted for a geometric factor if he or she chooses geometric spacing of nodes.)  The length of any division in the region will be equal to the geometric factor times the length of the previous division.  Here is an example of a geometric series with a geometric factor of 2:\n");
		printf("\n0, 1, 3, 7, 15, 31\n");
		printf("\nHere, n1 = 0.  n2 = 1.  The geometric factor = 2. n3 is determined by the following formula: \nn3 = n2 + f*(n2 - n1)\nn3 = 1 + 2*(1-0) = 3\n\nn4 is determined similarly:\nn4 = n3 + f*(n3 - n2)/nn4 = 3 + 2*(3-1) = 7.  And so on for the rest of the nodes in the series. Geometric spacing is determined in the same fashion where the numbers in the series become the coordinates.\n\n");
		printf("If the user wishes that the distance between each node decreases by a given factor, he or she should simply input as the geometric factor 1 divided by the factor of shrinkage desired. For example, to create a region where the distances between nodes becomes smaller by a factor of two, simply input as the geometric factor 0.5 = 1/2.\n");
	   }

	else if (which_info == 3)
	   {
		printf("\nLOGARTIHMIC SPACING: \n\n");
		printf("In a region with logarithmically spaced nodes, the coordinates of the nodes are determined as a function of log base 10 of x.  Since by nature, this type of spacing results in smaller divisions towards the beginning of the region and larger divisions toward the end of the region, we have included as a separate spacing option a 'backwards' logarithmic spacing, where the the divisions are larger towards the beginning of the region and gradually become smaller (accordint to the funtion of log(x)) as they approach the end of the region. \n");
	   }

	printf("\nPress 1 to return to menu.\n");
	scanf("%d", &menu);
	if (menu == 1)
		print_spacing_info();

   }

/******************************************************************************/
/* FUNCTION: print_zone_bounds						      */
/* PURPOSE:  Prints out which element numbers are in which zones.	      */
/******************************************************************************/


void print_zone_bounds(num_xregions, num_yregions, num_zregions, xelems, yelems, zelems)
int num_xregions;
int num_yregions;
int num_zregions;
int *xelems;
int *yelems;
int *zelems;

   {
	extern double xcoords[], ycoords[], zcoords[];
		/* Arrays containing coordinates. */

	int x = 0, y = 0, z = 0;
		/* Coordinate numbers for x, y, and z axis. */

	int zone_num = 1;
		/* Zone number. */

	int i, j, k;
		/* Counters. */
	
	for (k=0; k < num_zregions; k++)
	   {
		for (j=0; j < num_yregions; j++)
		   {
			for (i=0; i < num_xregions; i++)
			   {
				printf("\nZone %d\n", zone_num);
				printf("x boundaries: %20.12g", xcoords[x]);
				x = x + *(xelems + i);
				printf("   %20.12g\n", xcoords[x]);
				printf("y boundaries: %20.12g", ycoords[y]);
				y = y + *(yelems + j);
				printf("   %20.12g\n", ycoords[y]);
				y = y - *(yelems + j);
				printf("z boundaries: %20.12g", zcoords[z]);
				z = z + *(zelems + k);
				printf("   %20.12g\n", zcoords[z]);
				z = z - *(zelems + k);	
				zone_num++;
			   }
			x = 0;
			y = y + *(yelems + j);
		   }
		y = 0;
		z = z + *(zelems + k);
	   }

   }

/******************************************************************************/
/* FUNCTION: assign_elements						      */
/* PURPOSE:  Calculates connectivity.					      */
/******************************************************************************/

void assign_elements( num_xnodes, num_ynodes, num_znodes, 
                xelems, yelems, zelems, num_xregions, 
                num_yregions, num_dimensions, output, fp_out)
int num_xnodes;
int num_ynodes;
int num_znodes;
int *xelems;
int *yelems;
int *zelems;
int num_xregions;
int num_yregions;
int num_dimensions;
int output;
FILE *fp_out;
   {

	int num_trials;
		/* Number of nodes chacked to see if should start an */
		/* element assignment at that node.  All nodes begin */
		/* an element except the nodes in the first row      */
		/* (smallest y), the nodes on the end of each row    */
		/* (largest x), and the nodes in the last plane      */ 
		/* (largest z).  This program starts on row two and  */
		/* ends on the next to the last plane, checks to be  */
		/* sure the current node is not at the end of a row  */
		/* end of a row, and starts an element. It does this */
		/* num_trials times. 	                             */

	int row_num, plane_num = 1;
		/* Keep track of row number and plane number. */

	int elem_ident = 1;
		/* Element identification number. */ 

	int node_ident;
		/* Node identification number. */

	int elements[8];
		/* Array of element identification numbers, each with */
		/* up to 8 nodes defining element's connectivity.     */

	int zone_num = 1;
		/* Zone number. */

	int zone_elems = 0;
		/* Number of elements in all previous regions in current axis.*/

	int zone_rows = 0;
		/* Number of rows in all previous regions in current plane. */

	int zone_planes = 0;	
		/* Number of planes in all previous regions. */

	int row=1;
		/* Row number used for zone calculations, separate from */
		/* `row_num' which is used for connectivity calculations. */

	int plane=1;
		/* Plane number used for zone calculations, separate from */
		/* `plane_num' which is used for connectivity calculations. */

	int i, j;
		/* Counters. */
/* Starts assignment of nodes within element from bottom side */
/* of square, according to AVS standards, so assignment of  */
/* nodes for the first element will begin on row  2. 	      */

	row_num = 2;

	
/* Start on first node of second row - num_xnodes + 1. */

	node_ident = num_xnodes + 1;


/* Safeguard in case of non-3-dimensional grid.  */
/* NOTE this code assumes x axis is used as mininum
   so will not work if user tries to use just y dimension
*/
	if ((num_ynodes <= 1) && (num_znodes <= 1)){
		num_trials = num_xnodes;
                node_ident = 1;

        }
	else if (num_znodes <= 1) {
		num_trials = num_xnodes * (num_ynodes - 1);


/* In 3-D, the number of elements = number of x  */
/* nodes - 1 * number of y nodes - 1 * number of */
/* z nodes - 1.  This loop skips each row of y   */
/* nodes and the single plane of znodes that     */
/* that don't start an element, but not the x    */
/* nodes that don't start an element.  So, 	 */
/* num_trials = number of y nodes -1 * number    */
/* of z nodes - 1 * number of xnodes.   	 */ 

        }
	else {
               num_trials = num_xnodes * (num_ynodes - 1) * (num_znodes - 1);
        }


        for(i=1; i <= num_trials; i++)
	   {


/* Assign element if current node is not last node in row. */
		 if (node_ident < (row_num * num_xnodes)) 
		   {
                        elements[0] = node_ident;
                        elements[1] = elements[0] + 1;


/* If grid is at least two dimensional, assign the next */
/* two corners for a total of four corners of a square. */

			if (num_ynodes > 1)
			   {   
				elements[2] = elements[1] - num_xnodes;
				elements[3] = elements[2] - 1;
			   }
		   

/* If grid is 3-D and current node is not on the last  */ 
/* plane, assign the next four corners to make a cube. */

			if ((plane_num < num_znodes) && (num_znodes > 1))
		   	   {
			   				
				elements[4] = elements[0] + 
					(num_xnodes*num_ynodes);
				elements[5] = elements[4] + 1;
				elements[6] = elements[5] - num_xnodes;
				elements[7] = elements[6] - 1;
			    
/* If current node is the last in its plane, */
/* increment plane_num and skip the next row */
/* of nodes.				     */

				if (node_ident==((plane_num) * 
					(num_xnodes*num_ynodes)) - 1)
			           {
				 	plane_num++;
					node_ident = (node_ident + num_xnodes);
					row_num++;
                                   }
		   	   }


/* If element is a cube, i.e. if corners 5 and 6 do not both equal */ 
/* 0, print the connectivity to the given file in AVS format.      */
	
	if (num_dimensions == 3) {

/* elements[3], elements[2], elements[6], elements[7], 
   elements[0], elements[1], elements[5], elements[4])
   changed order to AVS counter clockwise top then bottom - tam Jul 2001 */
	  if (output == 1) {
	    fprintf(fp_out,"%d  %d hex %4d %4d %4d %4d %4d %4d %4d %4d\n", elem_ident, zone_num, elements[7], elements[6], elements[5], elements[4], elements[3], elements[2], elements[1], elements[0]);
	  } else if (output == 4) {
	    fprintf(fp_out,"%d %4d %4d %4d %4d %4d %4d %4d %4d\n", elem_ident, elements[7], elements[6], elements[5], elements[4], elements[3], elements[2], elements[1], elements[0]);
	  }
/* If element is a square, i.e. if corners 3 and 4 do not both equal */
/* 0, print the connectivity to the given file in AVS format.        */

/* connectivity output was out of order, fixed 950222 cwg        */

	} else if (num_dimensions == 2) {
	  if (output == 1) {
	    fprintf(fp_out,"%d  %d quad %4d %4d %4d %4d\n", elem_ident, zone_num, elements[3], elements[2], elements[1], elements[0]);
	  } else if (output == 4) {
	    fprintf(fp_out,"%d %4d %4d %4d %4d\n", elem_ident, elements[3], elements[2], elements[1], elements[0]);
	  }

/* If the element is a line, print the connectivity */
/*  and do not write past the last set of nodes     */

	} else {
          if (i < num_trials) {

		  if (output == 1) {
		    fprintf(fp_out, "%d   %d line ", 
		    elem_ident, zone_num);
		    for (j=0; j < 2; j++)
		      fprintf(fp_out, "%4d", elements[j]);
		    fprintf(fp_out, "\n");
		  } 

	       	  else if (output == 4) {
		    fprintf(fp_out, "%d ", elem_ident); 
		    for (j=0; j < 2; j++)
		      fprintf(fp_out, "%4d", elements[j]);
		    fprintf(fp_out, "\n");
		  }
	  }
	}   

/*--------------------------------*/
/* Calculation of which zone current element is is. */
/* If at end of x zone but not at end of line, increment zone number by 1. */
			if (*xelems == (elem_ident - zone_elems))
			   {
				zone_elems = zone_elems + *xelems;
				xelems++;
/* If at end of line but not at end of y zone, decrease zone number by */
/* number of x zones. */
				if (num_xnodes - 1 == 
					zone_elems-((num_xnodes-1)*(row-1)))
				   {
					xelems = xelems -(num_xregions);
/* If at end of y zone but not at end of plane, increase zone number. */
					if (row - zone_rows == *yelems)
					   {
						zone_rows = zone_rows + *yelems;
						yelems++;
/* If at end of plane but not at end of z zone, decrease zone number by */
/* number of x zones times number of y zones. */
						if(num_ynodes - 1 == zone_rows -
							((num_ynodes - 1) * 
								(plane - 1)))
						   {
/* If at end of z zone, increase zone number. */	yelems = yelems-
								   num_yregions;
							if(plane - zone_planes 
								== *zelems)
							   {
								zone_num++;

								zone_planes = 
								   zone_planes +
								   *zelems;

								zelems++;
							   } 
							else 
							   {
								zone_num = 
								 zone_num - 
								 (num_xregions*
								num_yregions)+1;	
							   }
							plane++;
						   } 
						else zone_num++;
					   } 
					else 
					   {
						zone_num = (zone_num - 
							num_xregions) + 1;
						
					   }
					row++;
				   }
				else zone_num++;
			   }
			elem_ident++;

/* If current node is the last in its row, move to next row. */

		   } else row_num++;

/* Move to next node. */

                node_ident++;        
           }	
   }


