package Gridder;
import java.awt.Dimension;
//import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;


public class RegionMatrix implements ActionListener, ItemListener {
	
	// Class variables
	final int max = 100;
	
	JPanel panel[] = new JPanel[max];
	JPanel panelMain;
	
	JTextField coordinates[] = new JTextField[max], numDivisions[] = new JTextField[max];
	JTextField geometricFactor[] = new JTextField[max], spacing[] = new JTextField[max];
	JTextField endCoord;
	
	JComboBox spacingType[] = new JComboBox[max];
	JCheckBox isSpacing[] = new JCheckBox[max];
	
	JScrollPane scroll;
	
	int Segments;
	int x, y, w, h;
	
	final String SPACING[] = {"Linear", "Geometric", "Logarithmic Expanding", "Logarithmic Contracting"};
	String newline = System.getProperty("line.separator");
	
	// Constructor
	public RegionMatrix(int x, int y, int w, int h, int Segments) {
    	
		this.x = x; this.y = y; this.w = w; this.h = h;
		this.Segments = Segments;
		
		// Panel that holds all input boxes
    	panelMain = new JPanel();
    	panelMain.setLayout(null);
    	panelMain.setLocation(0, 0);
    	panelMain.setSize(210 + (w + 10)*(max + 1), h*12);
    	//setup(panelMain, x, y, (w + 10)*(max + 1), h*10);
    	
    	for(int i = 0; i < max; i++) {
    		JLabel label = new JLabel("Segment " + Integer.toString(i+1));
    		setup(label, 0, 0, w, h, 4);
    		
    		// Input options for each segment
    		coordinates[i] = new JTextField();
    		if(i == 0)
    			coordinates[i].setToolTipText("Coordinates where segment " + (i + 1) + " should start" );
    		else
    			coordinates[i].setToolTipText("Coordinates where segment " + (i + 1) + " should start and segment " + i + " should end");
    		setup(coordinates[i], 0, (h + 5), w, h);
    		
    		isSpacing[i] = new JCheckBox();
    		setup(isSpacing[i], 0, (h + 5)*2, w, h, true);
    		numDivisions[i] = new JTextField();
    		setup(numDivisions[i], 0, (h + 5)*3, w, h);
    		spacing[i] = new JTextField();
    		setup(spacing[i], 0, (h + 5)*4, w, h);
    		
    		spacingType[i] = new JComboBox(SPACING);
    		spacingType[i].setToolTipText("<html>Linear - spacing between each division is the same<br>" +
    				"Geometric - each division is a certain factor of the previous division<br>" +
    				">>>> For example, a geometric factor of 1.2 and an initial spacing of 1 would have subsequent spacing 1.2, 1.44, 1.728, ...<br>" +
    				"Logarithmic Expanding - geometric with a factor of 10<br>" +
    				"Logarithmic Contracting - geometric with a factor of 0.1<br>");
    		setup(spacingType[i], 0, (h + 5)*5, w, h, 0);
    		
    		geometricFactor[i] = new JTextField();
    		geometricFactor[i].setToolTipText("<html>Each division is a certain factor of the previous division.<br>" +
    				"For example, a geometric factor of 1.2 and an initial spacing of 1 would have subsequent spacing 1.2, 1.44, 1.728, ...");
    		setup(geometricFactor[i], 0, (h + 5)*6, w, h);
    		
    		// Set the initial states
    		spacing[i].setVisible(false);
    		geometricFactor[i].setVisible(false);
    		isSpacing[i].setSelected(false);
    		
    		// Create the panel to hold this particular column
    		panel[i] = new JPanel();
    		//setup(panel[i], x + (w + 10)*i, y, w, h*10);
    		panel[i].setLayout(null);
    		panel[i].setLocation(200 + (w + 10)*i, 0);
        	panel[i].setSize(w, h*10);
        	
    		// Add the input options to the panel
    		panel[i].add(label);
    		panel[i].add(coordinates[i]);
    		panel[i].add(numDivisions[i]);
    		panel[i].add(spacing[i]);
    		panel[i].add(spacingType[i]);
    		panel[i].add(geometricFactor[i]);
    		panel[i].add(isSpacing[i]);
    		
    		// Initially, the panel will be invisible
    		panel[i].setVisible(false);
    		panelMain.add(panel[i]);
    	}
    	
    	// Show only the specified number of columns
    	for(int i = 0; i < Segments; i++)
    		panel[i].setVisible(true);
    	
    	//panelMain.setVisible(true);
    	//panelMain.setOpaque(true);
    	
    	// The end coordinate for the last segment
    	endCoord = new JTextField();
    	endCoord.setToolTipText("Coordinates where the last segment ends");
    	setup(endCoord, 200 + (w + 10)*Segments, h + 5, w, h);
    	panelMain.add(endCoord);
    	
    	String labels[] = {"Begin Coordinates", "Checked = Define Spacing", "Number of Divisions", "Spacing between divisions", "Type of Spacing", "Geometric Factor"};
		labelColumn(panelMain, 0, 25, 190, 20, labels);
		
		// Clumsy way to scroll through a JPanel
		JTextArea storyArea = new JTextArea("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" +
				"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\n");
		storyArea.setSize(5000, 300);
        storyArea.setEditable(true);
        storyArea.setLineWrap(false);
        storyArea.setWrapStyleWord(true);
		storyArea.add(panelMain);
        
		// Create the JScrollPane to contain the options
		scroll = new JScrollPane(storyArea, JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
                JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
    	scroll.setPreferredSize(new Dimension(770, 200));
    	//storyArea.setVisible(false);
    	scroll.setLocation(x, y);
    	//scroll.setBounds(new Rectangle(10, 60, 1000, 300));
    	scroll.setVisible(true);

    	
	}
	
	
	/******************
	 * Event Methods
	 ******************/

	public void actionPerformed(ActionEvent e) {
		// If "Geometric" is chosen for the spacing type, the JTextField for geometric factor is enabled
		for(int i = 0; i < Segments; i++) {
			if(e.getSource() == spacingType[i]) {
				if(spacingType[i].getSelectedIndex() == 1)
					geometricFactor[i].setVisible(true);
				else
					geometricFactor[i].setVisible(false);
			}
		}
	}
	
	public void itemStateChanged(ItemEvent e) {
		// Disables and enables JTextFields based on whether the user chooses to specify
		// the number of divisions or the length of the spacing between divisions
		for(int i = 0; i < Segments; i++) {
			if(e.getSource() == isSpacing[i]) {
				if(e.getStateChange() == ItemEvent.SELECTED) {
					numDivisions[i].setVisible(false);
					spacing[i].setVisible(true);
				}
				else {
					numDivisions[i].setVisible(true);
					spacing[i].setVisible(false);
				}
			}
		}
		
	}

	
	/**************************
	 * Getters and setters
	 **************************/
	
	public JPanel getPanel() { return panelMain; }
	public JScrollPane getScrollPane() { return scroll; }
	
	public void setSegments(int Segments) {
		if(this.Segments < Segments) {
			for(int i = this.Segments; i < Segments; i++) {
				panel[i].setVisible(true);
				
			}
		}
		else if(this.Segments > Segments) {
			for(int i = Segments; i < this.Segments; i++) {
				panel[i].setVisible(false);
			}
		}
		this.Segments = Segments;
		endCoord.setLocation(200 + (w + 10)*Segments, h + 5);
	}
	
	
	
	
	
	
	
	
	
	
	
	
	/**************************************
	 * Methods copied from UserInterface
	 ***************************************/
	
	
	/*
	
	public void visible(int index) {
		numDivisions[Segments].setVisible(true);
    	spacing[Segments].setVisible(true);
    	spacingType[Segments].setVisible(true);
    	geometricFactor[Segments].setVisible(true);
    	isSpacing[Segments].setVisible(true);
	}
	
	public void endCoord() {
		if(Segments != max) {
	    	panel[Segments].setVisible(true);
	    	numDivisions[Segments].setVisible(false);
	    	spacing[Segments].setVisible(false);
	    	spacingType[Segments].setVisible(false);
	    	geometricFactor[Segments].setVisible(false);
	    	isSpacing[Segments].setVisible(false);
    	}
    	else if (Segments == max)
    		panel[Segments].setVisible(true);
	}
	*/
	
	
	
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

    public void setup(JPanel panel, int x, int y, int w, int h, String titleName) {
    	panel.setLayout(null);
    	panel.setLocation(x, y);
    	panel.setSize(w, h);
    	JLabel title = new JLabel(titleName);
    	setup(title, 0, 0, w, 20, 0);
    	panel.add(title);
    }
    
    public void setup(JComboBox combo, int x, int y, int w, int h, int selectedIndex) {
    	combo.setLocation(x, y);
    	combo.setSize(w, h);
    	combo.addActionListener(this);
    	//combo.setSelectedIndex(selectedIndex);
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

    
    
    
    public void labelColumn(JPanel panel, int x, int y, int w, int h, String[] labelNames) {
    	JLabel labels[] = new JLabel[labelNames.length];
    	for(int i = 0; i < labels.length; i++) {
    		labels[i] = new JLabel(labelNames[i]);
    		setup(labels[i], x, y + (h + 5)*i, w, h, 2);
    		panel.add(labels[i]);
    	}
    }

	
	
}
