/**************************************************************************************************
 * 
 * Gridder GUI, written by Michael Cheung at Los Alamos National Laboratory
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




package Gridder;

import java.awt.Color;
import java.awt.Component;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.io.Writer;
import java.util.Scanner;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.JTextArea;
import javax.swing.JTabbedPane;
import javax.swing.JTextField;
import javax.swing.UnsupportedLookAndFeelException;

public class UserInterface implements ActionListener, ItemListener  {
	
	/*******************
	 *  Class variables
	 ********************/
	static JFrame frame;
	
	JPanel general, xPanel, yPanel, zPanel;
	JPanel xSegment, ySegment, zSegment;
	JPanel xLabels, yLabels, zLabels;
	JPanel test;
	
	JLabel info[] = new JLabel[6];
	JTextArea status, regions, errors, notifications;
	
	JButton update, clearAll;

	JTextField numX, numY, numZ;
	JTextField fileName;
	JButton launch;

	JScrollPane xScroll, yScroll, zScroll;
	JComboBox dimensions, format;
	
	int OS;

	JTabbedPane GUI;
	JCheckBox GMV;
	
	RegionMatrix x, y, z;
	int xR = 1, yR = 1, zR = 1;

	String extension = "";
	String newline = System.getProperty("line.separator");
	
	/*****************************
	 *  Create the X Direction tab
	 *****************************/
	public void xPanel() {
		// Setup X Tab
		xPanel = new JPanel();
		
		xPanel.setSize(800, 500);
		xPanel.setLocation(0,0);
		xPanel.setLayout(new FlowLayout(FlowLayout.LEFT));
	
		// Create the dynamically adjusting input columns
		x = new RegionMatrix(40, 50, 150, 20, 1);
		
		xScroll = x.getScrollPane();
		xScroll.setOpaque(true);

		// Create inputs in the general tab
		numX = new JTextField();
		setup(numX, 10, 80, 70, 20, 230, "Number of X Segments (integer)", general);
		numX.setText("1");
		numX.setToolTipText("Each segment has the same type of spacing in its corresponding direction (x, y, z)");
		
		general.add(numX);
		
	}
	
	// Analogous to xPanel()
	public void yPanel() {
		yPanel = new JPanel();
		
		yPanel.setSize(800, 500);
		yPanel.setLocation(0,0);
		yPanel.setLayout(new FlowLayout(FlowLayout.LEFT));
	
		y = new RegionMatrix(40, 50, 150, 20, 1);
		
		yScroll = y.getScrollPane();
		yScroll.setOpaque(true);
		
		numY = new JTextField();
		setup(numY, 10, 110, 70, 20, 230, "Number of Y Segments (integer)", general);
		numY.setText("1");
		numY.setToolTipText("Each segment has the same type of spacing in its corresponding direction (x, y, z)");
		
		general.add(numY);
	}
	
	// Analogous to yPanel()
	public void zPanel() {
		zPanel = new JPanel();
		
		zPanel.setSize(800, 500);
		zPanel.setLocation(0,0);
		zPanel.setLayout(new FlowLayout(FlowLayout.LEFT));
	
		z = new RegionMatrix(40, 50, 150, 20, 1);
		
		zScroll = z.getScrollPane();
		zScroll.setOpaque(true);

		numZ = new JTextField();
		setup(numZ, 10, 140, 70, 20, 230, "Number of Z Segments (integer)", general);
		numZ.setText("1");
		numZ.setToolTipText("Each segment has the same type of spacing in its corresponding direction (x, y, z)");
		
		general.add(numZ);
	}
	
	/*********************************************************
	 *  Creates the full GUI that will be added to the JFrame
	 *********************************************************/
	public JTabbedPane createContentPane() {
		
		general = new JPanel();
		general.setLayout(null);
		
		// Options for dimension
		String options[] = {"1-D Mesh", "2-D Mesh", "3-D Mesh"};
		dimensions = new JComboBox(options);
		setup(dimensions, 10, 20, 100, 20, 130, "Dimensions", general);
		
		String options2[] = {"AVS", "Tracer 3D", "Vector", "FEHM"};
		format = new JComboBox(options2);
		setup(format, 10, 50, 100, 20, 130, "Output Format", general);
		
		notifications = new JTextArea();
		setup(notifications, 350, 170, 350, 20, Color.black);
		
		// Detect OS
		String s = System.getProperty("os.name").toLowerCase();
		if(s.indexOf("win") >= 0) {
			notifications.setText("Your OS has been detected as Windows.");
			OS = 0;
		}
		if(s.indexOf("mac") >= 0) {
			notifications.setText("Your OS has been detected as Mac OS.");
			OS = 1;
		}
		if(s.indexOf("nux") >= 0) {
			notifications.setText("Your OS has been detected as Linux.");
			OS = 2;
		}
		if(s.indexOf("sun") >= 0) {
			notifications.setText("Your OS has been detected as Sun Solaris.");
			OS = 3;
		}
		
		// File name input
		fileName = new JTextField();
		fileName.setToolTipText("Extensions will be appended to the filename (.inp, .fehm, .gridder_input, etc.)");
		setup(fileName, 460, 200, 180, 20, 140, "Output File Name:", general);
		
		// Only allow GMV launches if the OS is not Windows
		if(OS != 0) {
			GMV = new JCheckBox("Launch GMV");
			GMV.setLocation(10, 200);
			GMV.setSize(270, 20);
	        general.add(GMV);
		}
		
		// Description of Gridder
		String text = "Welcome to Gridder! Gridder generates orthogonal grids as 1-D lines, 2-D rectangles " +
				"or 3-D rectangular cuboids. The grids can be spaced linearly, logarithmically, or geometrically, " +
				"and the user can specify different spacing options for different segments along each axis. Gridder " +
				"breaks each of these segments into divisions based on these spacing options.";
		status = new JTextArea(text);
		setup(status, 650, 20, 320, 140, Color.black);
		status.setToolTipText("Description of Gridder");
		
		// Region information
		regions = new JTextArea("1 x 1 x 1 = 1 rectangular cuboid region(s)");
		setup(regions, 340, 20, 290, 20, Color.blue);
		regions.setToolTipText("Overview of the grid");
		
		// Error information
		errors = new JTextArea("");
		setup(errors, 340, 60, 290, 60, Color.red);
		errors.setToolTipText("Error Notifications");
		
		// Update and Reset buttons
		update = new JButton("Update All");
		update.setToolTipText("Update the number of segments for each dimension");
		setup(update, 20, 170, 100, 20);
		clearAll = new JButton("Clear All");
		clearAll.setToolTipText("Resets everything back to defaults");
		setup(clearAll, 150, 170, 100, 20);
		
		// Add components
		general.add(regions);
		general.add(errors);
		general.add(status);
		general.add(notifications);
        general.add(dimensions);
        general.add(format);
        //general.add(OSInput);
        general.add(fileName);
        general.add(update);
        general.add(clearAll);
		general.setOpaque(true);
		
		// Create the dimension panels
		xPanel();
        yPanel();
        zPanel();
        
        launch = new JButton("Launch");
        setup(launch, 350, 200, 100, 20);
        general.add(launch);
		
        // Add the tabs
		GUI = new JTabbedPane();
		GUI.addTab("General", null, general, "General parameters.");
		GUI.addTab("X Dimension", null, xScroll, "Specify parameters in the x direction.");
		GUI.addTab("Y Dimension", null, yScroll, "Specify parameters in the y direction.");
		GUI.addTab("Z Dimension", null, zScroll, "Specify parameters in the z direction.");
		
		dimensions.setSelectedIndex(2);
		return GUI;
	}
	
	// Create the frame
	private static void createAndShowGUI() {

        JFrame.setDefaultLookAndFeelDecorated(false);
        frame = new JFrame("Gridder - http://meshing.lanl.gov/gridder/gridder.html");

        UserInterface inputs = new UserInterface();
        frame.setContentPane(inputs.createContentPane());
        
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(1000, 300);
        
        frame.setVisible(true);
    }

	/*******************
	 * Main method
	 ******************/
    public static void main(String[] args) {
    	// Can also just be createAndShowGUI();
    	SwingUtilities.invokeLater(new Runnable() {
    		public void run() {
    			createAndShowGUI();
    		}
    	});
    	
    }
    
    
    
    
    /************************
     * Event methods
     ************************/


	public void itemStateChanged(ItemEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	public void actionPerformed(ActionEvent e) {

		int dim = dimensions.getSelectedIndex() + 1;
		boolean proceed = true;
		
		if(e.getSource() == format && OS != 0) {
			if(format.getSelectedIndex() == 0)
				GMV.setVisible(true);
			else
				GMV.setVisible(false);
		}
		
		// Resets everything to defaults
		if(e.getSource() == clearAll) {
			GUI.removeTabAt(1);
			GUI.removeTabAt(1);
			GUI.removeTabAt(1);
			
			x = new RegionMatrix(40, 50, 150, 20, 1);
			y = new RegionMatrix(40, 50, 150, 20, 1);
			z = new RegionMatrix(40, 50, 150, 20, 1);
			
			xScroll = x.getScrollPane();
			xScroll.setOpaque(true);
			yScroll = y.getScrollPane();
			yScroll.setOpaque(true);
			zScroll = z.getScrollPane();
			zScroll.setOpaque(true);
			
			GUI.addTab("X Dimension", null, xScroll, "Specify parameters in the x direction.");
			GUI.addTab("Y Dimension", null, yScroll, "Specify parameters in the y direction.");
			GUI.addTab("Z Dimension", null, zScroll, "Specify parameters in the z direction.");
			
			dimensions.setSelectedIndex(2);
			numX.setText("1");
			numY.setText("1");
			numZ.setText("1");
			errors.setText("");
			format.setSelectedIndex(0);
			
			fileName.setText("");
		}
		
		// Updates the number of segments in each dimension
		if(e.getSource() == update) {
			errors.setText("");
			proceed = updateRegionInfo(dim);
		}
		
		// Launches Button
		if(e.getSource() == launch) {
			
			errors.setText("");
			
			// Check for errors
			proceed = updateRegionInfo(dim);
			if(proceed)
				proceed = checkAll(frame, x, "X", xR);
			if(proceed && dim > 1)
				proceed = checkAll(frame, y, "Y", yR);
			if(proceed && dim > 2)
				proceed = checkAll(frame, z, "Z", zR);
			
			if(proceed && fileName.getText().length() == 0) {
				errors.setText("Please specify a file name.");
				proceed = false;
			}
				
				
			// Launch the program(s) in the event of no errors
			if(proceed) {
				try {
					Writer writer = new OutputStreamWriter(new FileOutputStream(new File(fileName.getText()) + ".gridder_input"), "UTF-8");
					writer.write(dim +  " ");
					
					write(writer, x, xR);
					if(dim > 1)
						write(writer, y, yR);
					if(dim > 2)
						write(writer, z, zR);
					
					writer.write(format.getSelectedIndex() + 1 + " ");
					writer.close();
					
					switch(format.getSelectedIndex()) {
					case 0: extension = ".inp"; break;
					case 1: extension = ".tracer3d"; break;
					case 2: 
						switch(dim) {
						case 1: extension = ".x";
						case 2: extension = ".xy";
						case 3: extension = ".xyz";
						}
						break;	
					case 3: extension = ".fehm"; break;
					}
					// Create bat file for windows or LaGrit input file for Unix-based
					if(OS == 0) {
						writer = new OutputStreamWriter(new FileOutputStream(new File("temp.bat")), "UTF-8");
						writer.write("gridder.exe < " + fileName.getText() + ".gridder_input" + newline + "rename grid.inp " + fileName.getText()
								+ extension + newline + "del input.tmp" + newline + "del temp.bat");
						writer.close();
					}
					else {
						writer = new OutputStreamWriter(new FileOutputStream(new File(fileName.getText() + ".lagrit")), "UTF-8");
						writer.write("read / avs / " + fileName.getText() + extension + " / mo" + newline);
						writer.write("dump / gmv / " + fileName.getText() + ".gmv / mo" + newline);
						writer.write("finish" + newline);
						writer.close();	
					}
					
					if(launch()) {
						JOptionPane.showMessageDialog(frame, "Gridder run successful.");
					}
					
				}
				catch (UnsupportedEncodingException e1) { e1.printStackTrace(); }
				catch (FileNotFoundException e1) { e1.printStackTrace(); }
				catch (IOException e1) { e1.printStackTrace(); }
			}
			
		}
		
		// Disables and enables dimensions based on user selection
		if(e.getSource() == dimensions) {
			if(dimensions.getSelectedIndex() > 0)
				GUI.setEnabledAt(2, true);
			else
				GUI.setEnabledAt(2, false);
			if(dimensions.getSelectedIndex() == 2)
				GUI.setEnabledAt(3, true);
			else
				GUI.setEnabledAt(3, false);
			regions.setText("");
			proceed = updateRegionInfo(dim);
		}
	}
    
	/**************************************************************************************
	 * After writing the necessary files, the programs are launched through a terminal.
	 *************************************************************************************/
	public boolean launch() {
		boolean success = true;
		OutputStream stdin = null;
		
	    // Launch program for PCs
		if(OS == 0) {
			try {
			  Runtime.getRuntime().exec("cmd /c start; temp.bat");
			  
			}
			catch(Exception e) {
				success = false;
				JOptionPane.showMessageDialog(frame, "Could not launch Gridder!.\n- Make sure you have selected the appropriate " +
						"operating system (PC or Unix-based)\n- Make sure gridder.exe is in the same folder as this .jar");
			}
		}
		
		// Launch program(s) for Unix-based
		else {
			
			String appendOS = "";
			switch(OS) {
			case 1: appendOS = "_maci"; break;
			case 2: appendOS = "_linux"; break;
			case 3: appendOS = "_sun"; break;
			}
			
			// Try launching gridder
			try {
				Process process = Runtime.getRuntime().exec("bash");
				stdin = process.getOutputStream();
				
				// Give the input
				stdin.write(("gridder" + appendOS + " < " + fileName.getText() + ".gridder_input; ").getBytes());
				stdin.write(("mv grid.inp " + fileName.getText() + extension + "; ").getBytes());
			    
				// Remove gridder's input file
				stdin.write("rm input.tmp; ".getBytes());
				
				if(GMV.isSelected() && format.getSelectedIndex() == 0) {
					// Try launching LaGriT
					try  {
						stdin.write(("lagrit" + appendOS + " < " + fileName.getText() + ".lagrit; ").getBytes());
						
						if(checkLaGriT()) {

							// Remove outx3dgen, logx3dgen, and .lagrit file 
							try {
								//stdin.write("rm outx3dgen; ".getBytes());
								//stdin.write("rm logx3dgen; ".getBytes());
								stdin.write(("rm " + fileName.getText() + ".lagrit; ").getBytes());
							}
							catch(Exception e) {}
							
							// Try launching GMV
							try {
								stdin.write(("gmv" + appendOS + " -i " + fileName.getText() + ".gmv & ").getBytes());
							}
							catch(Exception e) {
								success = false;
								JOptionPane.showMessageDialog(frame, "Could not launch GMV!\n- Make sure you have GMV in the same directory as this .jar file" +
										"\n- Make sure that you are on a Unix-based operating system");
							}
						}
						else {
							success = false;
							JOptionPane.showMessageDialog(frame, "LaGriT run failed!\n- Make sure you have LaGriT in the same directory as this .jar file" +
									"\n- Make sure that you are on a Unix-based operating system\n- Make sure you can use bash");
						}
					}
					catch(Exception e) {
						success = false;
						JOptionPane.showMessageDialog(frame, "Could not launch LaGriT!\n- Make sure you have LaGriT in the same directory as this .jar file" +
								"\n- Make sure that you are on a Unix-based operating system\n- Make sure you can use bash");
					}
				}
				stdin.flush();
				stdin.close();
			}
			catch(Exception e) {
				success = false;
				JOptionPane.showMessageDialog(frame, "Could not launch Gridder!\n- Make sure gridder is in the same directory as this .jar file" +
						"\n- Make sure you can read/write files");
			}
			
			
		}
		
		
		
		
		return success;
	    
	}
	
	/*********************************************************************
	 * Checks outx3dgen to determine if LaGriT completed successfully
	 *********************************************************************/
	public boolean checkLaGriT() throws FileNotFoundException {
		Scanner in = new Scanner(new File("outx3dgen"));
		int count = 0;
		String s;
		boolean success = false;
		
		while(in.hasNext()) {
			s = in.next();
			if(s.equals("LaGriT") && count == 0)
				count++;
			if(s.equals("successfully") && count == 1)
				count++;
			if(s.equals("completed") && count == 2)
				success = true;
		}
		return success;
	}
	

	
	/**********************************************************************
	 * Updates number of segments for each dimension, catching user errors
	 **********************************************************************/
	public boolean updateRegionInfo(int dim) {
	
		boolean proceed = true;
		
		try {
			xR = Integer.parseInt(numX.getText());
			if(xR <= 0) {
				errors.setText("Number of X segments must be positive.");
				//JOptionPane.showMessageDialog(frame, "Number of X segments must be positive.");
				proceed = false;
			}
			else if(xR > 100) {
				errors.setText("No more than 100 X segments can be specified.");
				proceed = false;
			}
				
			if(dim > 1) {
				yR = Integer.parseInt(numY.getText());
				if(yR <= 0) {
					errors.setText("Number of Y segments must be positive.");
					//JOptionPane.showMessageDialog(frame, "Number of Y segments must be positive.");
					proceed = false;
				}
				else if(yR > 100) {
					errors.setText("No more than 100 Y segments can be specified.");
					proceed = false;
				}
				if(dim > 2) {
					zR = Integer.parseInt(numZ.getText());
					if(zR <= 0) {
						errors.setText("Number of Z segments must be positive.");
						//JOptionPane.showMessageDialog(frame, "Number of Z segments must be positive.");
						proceed = false;
					}
					else if(zR > 100) {
						errors.setText("No more than 100 Z segments can be specified.");
						proceed = false;
					}
				}
			}
		} 
		catch (NumberFormatException e1) {
			errors.setText("Number of segments must be an integer.");
			proceed = false;
		}
		
		// Update number of segments
		if(proceed) {
			numX.setVisible(false);
			numY.setVisible(false);
			numZ.setVisible(false);
			try {
				switch(dim) {
				case 3: z.setSegments(zR); numZ.setVisible(true);
				case 2: y.setSegments(yR); numY.setVisible(true);
				case 1: x.setSegments(xR); numX.setVisible(true);
				}
			}
			catch (ArrayIndexOutOfBoundsException e) {
				errors.setText("No more than 100 segments per direction can be specified.");
				proceed = false;
			}
			
			if(proceed) {
				switch(dim) {
				case 1: regions.setText(xR + " line segment region(s)"); break;
				case 2: regions.setText(xR + " x " + yR + " = " + xR*yR + " rectangular region(s)"); break;
				case 3: regions.setText(xR + " x " + yR + " x " + zR + " = " + xR*yR*zR + " rectangular cuboid region(s)"); break;
				}
			}
		}
		return proceed;
	}
    
	/*****************************
	 * Writes gridder input
	 *****************************/
	public void write(Writer writer, RegionMatrix RM, int xR) throws IOException {
		writer.write(xR + " ");
		writer.write(RM.coordinates[0].getText() + newline);
		for(int i = 0; i < xR; i++) {
			if(i + 1 == xR)
				writer.write(RM.endCoord.getText() + newline);
			else
				writer.write(RM.coordinates[i + 1].getText() + newline);
			
			if(RM.isSpacing[i].isSelected())
				writer.write("0 " + RM.spacing[i].getText() + newline);
			else
				writer.write(RM.numDivisions[i].getText() + newline);
			
			writer.write((RM.spacingType[i].getSelectedIndex() + 1) + newline);
			if(RM.spacingType[i].getSelectedIndex() == 1)
				writer.write(RM.geometricFactor[i].getText() + newline);
		}
	}
	
    double coordPrev;
    
    /************************************************
     * Check all the parameters in a RegionMatrix
     **********************************************/
	public boolean checkAll(Frame frame, RegionMatrix RM, String dim, int xR) {
		String current;
		boolean proceed = true;
		
		coordPrev = -2e31;
		for(int i = 0; i <= xR && proceed; i++) {
			if(i != xR) {
				current = RM.coordinates[i].getText();
				proceed = check(frame, current, dim + " Dimension, Segment " + (i + 1) + ". Coordinate input", i, xR, true, true, false);
			}
			else {
				current = RM.endCoord.getText();
				proceed = check(frame, current, dim + " Dimension, end coordinate. Coordinate input", i, xR, true, true, false);
			}
		}
		
		for(int i = 0; i < xR && proceed; i++) {
			if(RM.isSpacing[i].isSelected()) {
				current = RM.spacing[i].getText();
				proceed = check(frame, current, dim + " Dimension, Segment " + (i + 1) + ". Spacing input", i, xR, false, true, true);
			}
			else {
				current = RM.numDivisions[i].getText();
				proceed = check(frame, current, dim + " Dimension, Segment " + (i + 1) + ". Divisions input", i, xR, false, false, true);
			}
		}
		
		for(int i = 0; i < xR && proceed; i++) {
			if(RM.spacingType[i].getSelectedIndex() == 1) {
				current = RM.geometricFactor[i].getText();
				proceed = check(frame, current, dim + " Dimension, Segment " + (i + 1) + ". Geometric factor input", i, xR, false, true, true);
			}
		}
		
		return proceed;
	}
	
    
    
    /*************************************************************************************
     * Checks a certain parameter of a certain segment of a certain RegionMatrix
     *************************************************************************************/
	public boolean check(Frame frame, String current, String msg, int i, int xR, boolean isCoord, boolean isDouble, boolean isPositive) {
		boolean proceed = true;
		double coord = 0;
		
		if(proceed) {
			
			// Exception handling for coordinates
			if(current.length() == 0) {
				errors.setText("Error in " + msg + " is empty.");
				//JOptionPane.showMessageDialog(frame, "Error in " + msg + " is empty.");
				proceed = false;
			}
			
			if(proceed) {
				try{
					if(isDouble)
						coord = Double.parseDouble(current);
					else
						coord = Integer.parseInt(current);
					if(i != 0 && coord <= coordPrev && isCoord) {
						if(i != xR) {
							errors.setText("Error in " + msg.substring(0, msg.length() - 18) +
								". Coordinate must be greater than that of Segment " + i + ".");
							//JOptionPane.showMessageDialog(frame, "Error in X Dimension, Segment " + i +
								//". Coordinate must be greater than that of Segment " + i + ".");
						}
						else {
							errors.setText("Error in " + msg.substring(0, msg.length() - 18) +
								". Coordinate must be greater than that of Segment " + i + ".");
							//JOptionPane.showMessageDialog(frame, "Error in X Dimension, end coordinate. " +
								//" Coordinate must be greater than that of Segment " + i + ".");
						}		
						proceed = false;
					}
					coordPrev = coord;
				}
				catch(NumberFormatException e1) {
					errors.setText("Error in " + msg + " must be a number.");
					//JOptionPane.showMessageDialog(frame, "Error in " + msg + " must be a number.");
					
					proceed = false;
				}
			}
			
			if(proceed && isPositive && coord <= 0) {
				errors.setText("Error in " + msg + " must be positive.");
				proceed = false;
			}
		}
		return proceed;

	}
	
    
    
    
    
    
    
    /**********************************************************
     * Methods for creating a GUI interface
     *********************************************************/
    
    public void normalDistr(JPanel panel, String title, JTextField mean, JTextField stdDev,
    		JTextField NTG) {
    	JLabel titleLabel = new JLabel(title);
    	setup(titleLabel, 0, 0, 250, 20, 0);
    	
    	JLabel meanLabel = new JLabel("Mean");
    	setup(meanLabel, 0, 20, 130, 20, 2);
    	setup(mean, 150, 20, 100, 20);
    	
    	JLabel stdDevLabel = new JLabel("Standard Deviation");
    	setup(stdDevLabel, 0, 40, 130, 20, 2);
    	setup(stdDev, 150, 40, 100, 20);
    	
    	JLabel NTGLabel = new JLabel("Net to Gross");
    	setup(NTGLabel, 0, 60, 130, 20, 2);
    	setup(NTG, 150, 60, 100, 20);
    	
    	Component temp[] = {titleLabel, meanLabel, stdDevLabel, NTGLabel,
    			mean, stdDev, NTG};
    	add(panel, temp);
    }
    
    public void normalDistr(JPanel panel, String title, JTextField mean, JTextField stdDev) {
    	JLabel titleLabel = new JLabel(title);
    	setup(titleLabel, 0, 0, 250, 20, 0);
    	
    	JLabel meanLabel = new JLabel("Mean");
    	setup(meanLabel, 0, 20, 130, 20, 2);
    	setup(mean, 150, 20, 100, 20);
    	
    	JLabel stdDevLabel = new JLabel("Standard Deviation");
    	setup(stdDevLabel, 0, 40, 130, 20, 2);
    	setup(stdDev, 150, 40, 100, 20);
    	
    	Component temp[] = {titleLabel, meanLabel, stdDevLabel,
    			mean, stdDev};
    	add(panel, temp);
    }
    
    // Adds an array of components
    public void add(JPanel panel, Component[] component) {
    	for(int i = 0; i < component.length; i++)
    		panel.add(component[i]);
    }
    
    // Changes the visiblity for an array of components
    public void visible(Component[] component, boolean choice) {
    	for(int i = 0; i < component.length; i++)
    		component[i].setVisible(choice);
    }
    
    // Generates a column of labels
    public void labelColumn(JPanel panel, int x, int y, int w, int h, String[] labelNames) {
    	JLabel labels[] = new JLabel[labelNames.length];
    	for(int i = 0; i < labels.length; i++) {
    		labels[i] = new JLabel(labelNames[i]);
    		setup(labels[i], x, y + h*i, w, h, 2);
    		panel.add(labels[i]);
    	}
    }
    
    
    
    
    /***************************
     * Overloaded setup methods
     **************************/
    
    public void setup(JTextField field, int x, int y, int w, int h, int dx, String labelName,
    		JPanel panel) {
    	field.setLocation(x + dx, y);
    	field.setSize(w, h);
    	field.addActionListener(this);
    	JLabel label = new JLabel(labelName);
    	label.setLocation(x, y);
    	label.setSize(dx - 10, h);
    	label.setHorizontalAlignment(2);
    	panel.add(label);
    }
    
    public void setup(JLabel label, int x, int y, int w, int h, int alignment) {
    	label.setLocation(x, y);
    	label.setSize(w, h);
    	label.setHorizontalAlignment(alignment);
    }
    
    public void setup(JTextField field, int x, int y, int w, int h) {
    	field.setLocation(x, y);
    	field.setSize(w, h);
    	field.addActionListener(this);
    }
    
    public void setup(JPanel panel, int x, int y, int w, int h) {
    	panel.setLayout(null);
    	panel.setLocation(x, y);
    	panel.setSize(w, h);
    }
    
    // Generates a JPanel with a JLabel to the left of it
    public void setup(JPanel panel, int x, int y, int w, int h, String titleName) {
    	panel.setLayout(null);
    	panel.setLocation(x, y);
    	panel.setSize(w, h);
    	JLabel title = new JLabel(titleName);
    	setup(title, 0, 0, w, 20, 0);
    	panel.add(title);
    }
    
    public void setup(JComboBox combo, int x, int y, int w, int h) {
    	combo.setLocation(x, y);
    	combo.setSize(w, h);
    	combo.addActionListener(this);
    }
    
    public void setup(JComboBox combo, int x, int y, int w, int h, int dx, String label, JPanel panel) {
    	JLabel jLabel = new JLabel(label);
    	setup(jLabel, x, y, dx - 10, h, 2);
    	panel.add(jLabel);
    	combo.setLocation(x + dx, y);
    	combo.setSize(w, h);
    	combo.addActionListener(this);
    }
    
    public void setup(JButton button, int x, int y, int w, int h) {
    	button.setLocation(x, y);
    	button.setSize(w, h);
    	button.addActionListener(this);
    }
    
    public void setup(JCheckBox checkBox, int x, int y, int w, int h, boolean selected) {
    	checkBox.setLocation(x, y);
    	checkBox.setSize(w, h);
    	checkBox.setSelected(selected);
    	checkBox.addItemListener(this);
    }
    
    public void setup(JTextArea textArea, int x, int y, int w, int h, Color color) {
		textArea.setEditable(false);
	    textArea.setLineWrap(true);
	    textArea.setWrapStyleWord(true);
	    textArea.setLocation(x, y);
	    textArea.setSize(w, h);
	    textArea.setBorder(BorderFactory.createLineBorder(color));
    }
    
    
    
    
    
	
    
    
    
    
    
}
