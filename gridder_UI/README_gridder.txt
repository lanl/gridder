Welcome to Gridder! 
   Online Manual and examples for command line version of gridder
   at http://meshing.lanl.gov/gridder/gridder.html

   Gridder generates orthogonal grids as 1-D lines, 2-D rectangles or 3-D 
   rectangular cuboids. The grids can be spaced linearly, logarithmically, 
   or geometrically, and the user can specify different spacing options for 
   different segments along each axis. Gridder breaks each of these segments 
   into divisions based on these spacing options.
   Two files are written, the resulting grid in chosen format and a 
   recording of the user choices so gridder can be rerun again.
   The file "grid.inp" is the grid file, "input.grid" are the gridder inputs.


**************************************************************************************************
* 
* Gridder GUI Java Code, written by Michael Cheung at Los Alamos National Laboratory
* Released: March 31st, 2011 (31/03/2011)
* E-mail: mcheung@lanl.gov
* 
* This program provides a Graphical User Interface for using gridder. The program needs to be in the
* same directory as the gridder executables as well as the LaGriT and GMV executables if the user
* intends on using them. Your operating system will automatically be detected, and if you are not
* running Windows, you have the option of launching GMV. You can find out more by simply hovering
* your mouse over various components of the GUI and reading the tool tips.
* 
* The program also checks for errors such as putting letters where a number should be or putting a double
* where an int should be by using try and catch blocks. It also checks for empty text fields, whether
* positive input fields have positive numbers in them, whether the starting coordinates of one segment
* is greater than the previous, and other potential user mistakes. To further minimize mistakes, the GUI
* automatically enables and disables components based on what is relevant. For each dimension tab there is
* a corresponding RegionMatrix object that removes and adds segments and text fields within each segment 
* based on user input.
* 
***************************************************************************************************/


GUI interface to call gridder: GridderGUI.jar

   The java interface calls gridder with a gridder input file
   that is created with the user interface. There are tool tips
   on the interface. Hoovering the mouse over any item will display
   help on using the interface to create the grid. 

   Additionally (linux only), the interface can be used to call 
   LaGriT to convert an AVS grid file GMV binary format, and display
   the mesh with GMV mesh viewer. User must select AVS format 1. 

   The result are 2 files called grid.inp and input.grid.
   The file grid.inp is the resulting mesh with coordinates
   and element connectivity written in chosen format. In this
   case the format is AVS with coordinates first, elements last.
   The file input.grid records the user choices and can be redirected
   back into gridder to repeat the run and mesh file.

--------------------------------------------------------
Demo 1 Session 
   creates a 1 Dimensional grid with 1 segment
   This segment is from 0 to 10 with 6 points on X axis 
   The Y axis and Z axis have 1 value at 0
   The grid points are connected as 5 line elements
--------------------------------------------------------
File grid.inp in AVS format:

6 5 0 0 0
1      0      0      0
2      2      0      0
3      4      0      0
4      6      0      0
5      8      0      0
6     10      0      0
1   1 line    1   2
2   1 line    2   3
3   1 line    3   4
4   1 line    4   5
5   1 line    5   6


--------------------------------------------------------
Demo 1 Session with GUI interface to write grid.inp 
User entries are in paranthesis
--------------------------------------------------------
% GridderGUI.jar

User selects 1-D Mesh
User selects tab to X Dimension
User enters Begin and End values for this segment
     (0) (10)
User enters number of Divisions for this segment
     (5)
User can select type of spacing, leave as linear

Return to General tab

Select Update All

If your OS is Linux Select "Launch GMV"

Enter File name (test.inp)

Select Launch (Note Errors will appear in Red Box)

Message will report success and test.inp will be written
and look like grid.inp in this Demo 1 example.
If selected GMV will show the grid elements, select
GMV Display / Nodes / Apply to see the node points.

Non GUI Usage:
   gridder (prompt user for values)
   gridder < input.grid  (input file for gridder)

   Command Line prompts will depend on choices made by
   user but will look something like the following display.
   The user entries are placed here in paranthesis.

   The result are 2 files called grid.inp and input.grid.

   The file grid.inp is the resulting mesh with coordinates
   and element connectivity written in chosen format. In this
   case the format is AVS with coordinates first, elements last.
 
--------------------------------------------------------
Demo 1 Session for command line version of gridder to write grid.inp
--------------------------------------------------------
% gridder 

Please input an integer number of dimensions for grid.
(1)

Please input an integer number of regions for axis x.
(1)

Please input coordinate to BEGIN region 1 of axis x.
(0.)

Please input coordinate to END region 1 of axis x. 
(50.)

Please input an integer number of divisions for region 1 of axis x.
If you would prefer to specify dx, then input 0 here.
(5)

Please input spacing for region 1:
0 = more information about spacing.
1 = equally spaced nodes.
2 = geometric spacing of nodes.
3 = logarithmic spacing of nodes, small spacing to large.
4 = logarithmic spacing of nodes, large spacing to small.
(1)

xcoords[0] =                    0
xcoords[1] =                   10
xcoords[2] =                   20
xcoords[3] =                   30
xcoords[4] =                   40
xcoords[5] =                   50

Please specify output:
1 = Advanced Visualization Systems (AVS) input specifications.
2 = Tracer3d coordinate input specifications. 
3 = Vectors : x(i) y(j) z(k) 
4 = FEHM coordinate input specifications. 
(1)

Program gridder has completed.
The following files have been written:

grid.inp     contains coordinate data in VECTORS format.
input.tmp    contains input values you have generated during this run.

NOTE: input.grid is generated on first run and will not be overwritten, 
input.tmp can be overwritten. To save input.tmp, cp or mv  to another file.
To repeat this run:
   gridder < input.grid


/**************************************************************/
/*******		Simple Grid Generator           *******/
/*******	        C Code   			*******/ 
/*******		by Loraine Lundquist            *******/
/*******						*******/
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
/* different name, and type gridder < new_filename                      */
/* Be sure to rename input.grid before trying to use it as input file.  */
/*									*/
/************************************************************************/


