/*SIP SECURITY TESTING FRAMEWORK - SSTF version 0.1
 * SSTF WIZARD - The GUI COMPONENT 
 * Author: Hemanth Srinivasan
 * Year: 2008 
 */
package strezzer;

import java.awt.*;
import java.awt.event.*;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.StringTokenizer;

import javax.swing.*;
import javax.swing.text.*;
import java.text.*;
import java.io.*;
import java.util.*;


//Main Class
public class SSTFwizard {
    
	
	public static void main(String[] args) {
		
		    
		//launch the starting thread
		SSTFthread startThread = new SSTFthread();
		startThread.start();
		
	}

}

/*
 * SSTFthread - Launches a new instance of the wizard
 */

class SSTFthread extends Thread {

	boolean stopRequested = false;
	
	public void run () {
		
		IntroductionFrame intFrame = new IntroductionFrame(this);
		intFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		intFrame.setVisible(true);
		
		
		while (!stopRequested) {
			//keep the thread running
			//do something if required
		}
		
		
	}
	
	public void stopThread () {
		
		stopRequested = true;
	}
	
}

/*
 * FrameListener - A frameListener for each frame
 */
class SSTFframeListener extends WindowAdapter {
   
	SSTFthread parentThread;
	
	
	public SSTFframeListener (SSTFthread parThr) {
		
		parentThread = parThr;
	}
	
	public void windowClosing(WindowEvent e) {
   
	   parentThread.stopThread();
	   
   }
   
}

/*First Frame - Introduction screen - Briefs the 4 step process of the wizard
 * 
 */
class IntroductionFrame extends JFrame {
	
	public String loadScenarioName = null;
	public JComboBox scenarioCombo = null;
	
	public IntroductionFrame(SSTFthread ownerThread) {
		    
		    setTitle("SSTF setup wizard - Introduction");
		    
		    addWindowListener(new SSTFframeListener(ownerThread));
		    
	        JPanel basic = new JPanel();
	        basic.setLayout(new BoxLayout(basic, BoxLayout.Y_AXIS));
	        add(basic);

	        JPanel topPanel = new JPanel(new BorderLayout(0, 0));
	        topPanel.setMaximumSize(new Dimension(800, 0));
	        JLabel intro = new JLabel("Welcome to the SIP Security Testing Framework (SSTF) setup wizard");
	        intro.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
	        intro.setPreferredSize(new Dimension(600, 0));
	        topPanel.add(intro, BorderLayout.WEST);

	        ImageIcon icon = new ImageIcon("sstf_icon.jpg");
	        //JLabel label = new JLabel(icon);
	        //label.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
	        JButton aboutButton = new JButton (icon);
	        aboutButton.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
	        topPanel.add(aboutButton, BorderLayout.EAST);
	        
	        aboutButton.addActionListener(new
	        		ActionListener() {
	        	        
	        	       public void actionPerformed (ActionEvent ent) {
	        	    	   String aboutText = "SIP Security Testing FrameWork (SSTF)"+
	        	    	   					  "\n"+
	        	    	   					  "(version 0.1)"+
	        	    	   					  "\n"+
	        	    	   					  " "+
	        	    	   					  "\n"+
	        	    		   				  "Developed by:"+
	        	    	                      "\n"+
	        	    	                      " "+
	        	    	                      "\n"+
	        	    	                      "Hemanth Srinivasan" +
	        	    	                      "\n"+
	        	    	                      "Year 2008";
	        	    	   
	        	    	   JOptionPane.showConfirmDialog(null, aboutText, "About SSTF", JOptionPane.PLAIN_MESSAGE);
	        	       }
	                }
	        		);


	        JButton helpButton = new JButton ("HELP");
	        helpButton.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
	        	        
	        helpButton.addActionListener(new
	        		ActionListener() {
	        	        
	        	       public void actionPerformed (ActionEvent ent) {
	        	    	   String note = "Read the 6 step process to get started.";
	        	    	   TestSetupFrame.showHelpWindow(note);
	        	       }
	                }
	        		);

	        topPanel.add(helpButton, BorderLayout.CENTER);
	        
	        JSeparator separator = new JSeparator();
	        separator.setForeground(Color.gray);

	        topPanel.add(separator, BorderLayout.SOUTH);

	        basic.add(topPanel);

	        //Create a Centeral panel 
	        JPanel centerPanel = new JPanel();
	        centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.X_AXIS));
	        centerPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	        basic.add(centerPanel);
	        
	        JPanel textPanel = new JPanel(new BorderLayout());
	        textPanel.setBorder(BorderFactory.createEmptyBorder(15, 25, 15, 25));
	        JTextPane pane = new JTextPane();

	        pane.setContentType("text/html");
	        String text = "<p><b><font size=\"4\" color=\"green\">Create a New Test Case in 4 Steps:</font></b></p> " +
	            "<p>&nbsp</p> " +
	            "<p><i>1) Build Events </i></p> " +
	            "<p>&nbsp</p> " +
	            "<p><i>2) Event Setup </i></p> " +
	            "<p>&nbsp</p> " +
	            "<p><i>3) Test Setup </i></p> " +
	            "<p>&nbsp</p> " +
	            "<p><i>4) Run </i></p> " ;
	        
	        pane.setText(text);
	        pane.setEditable(false);
	        textPanel.add(pane);

	        centerPanel.add(textPanel);
	        
	        
	        //add panel for load scenario
	        JPanel scnLoadPanel = new JPanel();
	        scnLoadPanel.setLayout(new BoxLayout(scnLoadPanel, BoxLayout.Y_AXIS));
	        scnLoadPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	        
	        centerPanel.add(scnLoadPanel);
	        
	        	        
	        JPanel scnLoadTopPanel = new JPanel();
	        scnLoadPanel.add(scnLoadTopPanel);
	        
	        JPanel scnLoadBottomPanel = new JPanel();
	        scnLoadPanel.add(scnLoadBottomPanel);
	        
	        JPanel scnLoadDummyPanel = new JPanel();
	        scnLoadDummyPanel.setPreferredSize(new Dimension(0, 300));
	        scnLoadPanel.add(scnLoadDummyPanel);
	        
	        JLabel scnLabel = new JLabel("Start Off with an Existing Test Scenario");
	        scnLabel.setBorder(BorderFactory.createEmptyBorder(15, 25, 15, 25));
	        scnLabel.setForeground(Color.RED);
	        scnLabel.setPreferredSize(new Dimension(300, 75));
	        scnLoadTopPanel.add(scnLabel);
	        
	        String [] scnFilesList = getTestScnearioFileNames ();
	        
	        if (scnFilesList == null)
	          scenarioCombo = new JComboBox();
	        else 
	          scenarioCombo = new JComboBox(scnFilesList);
	        
	        scnLoadBottomPanel.add(scenarioCombo);
	        
	        scenarioCombo.setSelectedIndex(-1);
		    
		    scenarioCombo.addActionListener(new
		    		  ActionListener() {
		    	  	
		    	  		public void actionPerformed (ActionEvent evt) {
		    	  			
		    	  		}	    	  
		      });
	        
	        //add action listeners for the cancel and next buttons
	        IntroNextAction nxtAction = new IntroNextAction(this, ownerThread);
	        IntroAbortAction abrtAction = new IntroAbortAction(this, ownerThread);
	       
	        
	        JPanel bottom = new JPanel(new FlowLayout(FlowLayout.RIGHT));

 
	        JButton newTestButton = new JButton("ANOTHER TEST INSTANCE");
			newTestButton.setMnemonic(KeyEvent.VK_A);
			newTestButton.addActionListener(new
			    		  ActionListener() {
			    	  	
		  	  		public void actionPerformed (ActionEvent evt) {
		  	  			
		  	  			//start new instance of the application
	  	  	  		    //relaunch the starting thread
		  	  		    SSTFthread newStartThread = new SSTFthread();
		  	  		    newStartThread.start();
		  	  			

		  	  		}
		  	  
			  });

			bottom.add(newTestButton);
			
	        JButton abortButton = new JButton("EXIT");
	        abortButton.setMnemonic(KeyEvent.VK_X);
	        abortButton.addActionListener(abrtAction);
	        JButton nextButton = new JButton("NEXT");
	        nextButton.setMnemonic(KeyEvent.VK_N);
	        nextButton.addActionListener(nxtAction);
	        
	        bottom.add(abortButton);
	        bottom.add(nextButton);
	        basic.add(bottom);

	        bottom.setMaximumSize(new Dimension(800, 0));

	        setSize(new Dimension(800, 600));
	        setResizable(false);
	        setLocationRelativeTo(null);
	        
	        
	}
	
	
	private String [] getTestScnearioFileNames () {
		
		File dir = new File(StrezzHeader.TESTCASE_DIR);
	    
	    String[] fileList = dir.list();
	    
	    if (fileList == null) {
                return null;
                
	    } 
	    
	    // get all xml files in the testcase directory
	    FilenameFilter filter = new FilenameFilter() {
	        public boolean accept(File dir, String name) {
	            if ((name.endsWith(".xml")||(name.endsWith(".XML"))) && !name.startsWith("template")) {
	            	return true;
	            }
	            else return false;
	        }
	    };
	    fileList = dir.list(filter);
	    
	    return fileList;
	}
	
	
}

class IntroNextAction implements ActionListener {
	
	IntroductionFrame parentFrame;
	SSTFthread parentThrd;
	
	public IntroNextAction (IntroductionFrame pFrame, SSTFthread ownThrd) {
	   	parentFrame = pFrame;
	   	parentThrd = ownThrd;
	}
	
	public void actionPerformed (ActionEvent event) {
		
		parentFrame.loadScenarioName = (String)parentFrame.scenarioCombo.getSelectedItem();
		
		if ((parentFrame.loadScenarioName == null) || (!parentFrame.loadScenarioName.endsWith(".xml"))) {
			
			BuildEventsFrame buildFrame = new BuildEventsFrame(parentThrd);
			buildFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
			buildFrame.setVisible(true);
			parentFrame.dispose();
			
		} else {
			
			//form the sipp command, first get all related csv files, if any
			String sippCmd = new String();
			//OS Check
			
			String osSTR = System.getProperty("os.name").toLowerCase();
			
			if (osSTR.contains("windows")) {
				sippCmd = StrezzHeader.SIPP_DIR+"sipp-w32";
			} else { //Unix based
				sippCmd = StrezzHeader.SIPP_DIR+"sipp";
			}
			
			sippCmd += " -sf "+StrezzHeader.TESTCASE_DIR+parentFrame.loadScenarioName;
			
			File dir = new File(StrezzHeader.TESTCASE_DIR);
		    String[] fileList = dir.list();
		    	     
		    
		    // get all csv files corresponding to the scenario file
		    FilenameFilter filter = new FilenameFilter() {
		        public boolean accept(File dir, String name) {
		        	//check if csv file and filename contains the scenario filename (excluding .xml extension)
		            if ((name.endsWith(".csv")||(name.endsWith(".CSV"))) && 
	            		name.startsWith(parentFrame.loadScenarioName.substring(0, parentFrame.loadScenarioName.length()-4))) {
		            	
		            	return true;
		            }
		            else return false;
		        }
		    };
		    fileList = dir.list(filter);
		    
			if ((fileList != null) || (fileList.length > 0)) {
				for (int i = 0; i < fileList.length; i++) {
					sippCmd = sippCmd+" -inf "+StrezzHeader.TESTCASE_DIR+fileList[i]; //injection files
				}
			}
			
			System.out.println("Loaded sipp command is: "+sippCmd);
			
			TestSetupFrame eventFrame = new TestSetupFrame(sippCmd, parentThrd);
			eventFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
			eventFrame.setVisible(true);
			parentFrame.dispose();
		}
		
	}
}

class IntroAbortAction implements ActionListener {
	
	IntroductionFrame parentFrame;
	SSTFthread parentThrd;
	
	public IntroAbortAction (IntroductionFrame pFrame, SSTFthread ownThrd) {
	   	
		parentFrame = pFrame;
		parentThrd = ownThrd;
	}
	
	public void actionPerformed (ActionEvent event) {
		
		parentFrame.dispose();
		parentThrd.stopThread();
		
	}

}



/*Second Frame - Allows to choose the events that will form the test case
 *               Current support only Send, Recv and Pause events. Later more
 *               events from SIPp can be added
 */
class BuildEventsFrame extends JFrame {
	
	public ArrayList eventList = new ArrayList();
	public String loadFileName = null;
	public JButton nextButton = new JButton("NEXT");
	
	public BuildEventsFrame (SSTFthread ownerThread) {
		
		    setTitle("SSTF setup wizard - Build Events");
		    
		    addWindowListener(new SSTFframeListener(ownerThread));
		    
	        JPanel basic = new JPanel();
	        basic.setLayout(new BoxLayout(basic, BoxLayout.Y_AXIS));
	        add(basic);

	        JPanel topPanel = new JPanel(new BorderLayout(0, 0));
	        topPanel.setMaximumSize(new Dimension(800, 0));
	        JLabel intro = new JLabel("Choose the Events from the Left and Add them to the Right OR Load Saved Settings");
	        intro.setPreferredSize (new Dimension(600, 50));
	        intro.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
	        
	        topPanel.add(intro);

	        JSeparator separator = new JSeparator();
	        separator.setForeground(Color.gray);

	        topPanel.add(separator, BorderLayout.SOUTH);

	        basic.add(topPanel);

	               
	        JPanel middlePanel = new JPanel();
	        middlePanel.setLayout(new BoxLayout(middlePanel, BoxLayout.X_AXIS));
	        middlePanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	        basic.add(middlePanel);
	        
	        //add two panels to the middle panel
	        JPanel middleLeftPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
	        middleLeftPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	        middlePanel.add(middleLeftPanel);
	        
	        JPanel middleRightPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
	        middleRightPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	        middlePanel.add(middleRightPanel);
	        
	        //add a list box and button to each of the middle panels
	        String eventsStr[] = {"Send", "Receive", "Pause"};
	        final JList givenList = new JList(eventsStr);
	        givenList.setPreferredSize(new Dimension(100, 470));
	        middleLeftPanel.add(givenList);
	        
	        JButton addButton = new JButton("Add");
	        addButton.setMnemonic(KeyEvent.VK_A);
	        middleLeftPanel.add(addButton);
	        
	        // Create a list that allows adds and removes - using Listmodels
	        final DefaultListModel selListModel = new DefaultListModel();
	        final JList selectedList = new JList(selListModel);
	        
	        selectedList.setPreferredSize(new Dimension(100, 470));
	        middleRightPanel.add(selectedList);
	        
	        
	        JButton removeButton = new JButton("Remove");
	        removeButton.setMnemonic(KeyEvent.VK_R);
	        middleRightPanel.add(removeButton);
	        
	        
	        //add action listeners for the add and remove buttons
	        addButton.addActionListener(new
	        		ActionListener() {
	        	        
	        	       public void actionPerformed (ActionEvent ent) {
	        	    	   //add chosen list to selectList and global event list
	        	    	   Object selObj[] = givenList.getSelectedValues();
	        	    	   for (int i = 0; i < selObj.length; i++) {
	        	    		   
        	    		       //to add rear of jlist(append)
	        	    		   int pos = selectedList.getModel().getSize();	      
	        	    		   selListModel.add(pos, selObj[i]); //by defaults adds front
	        	    		   
	        	    		   //add to global event list
	        	    		   eventList.add(selObj[i]);
	        	    	   }
	        	    	   System.out.println("Event List Contains:");
	        	    	   for (int i = 0; i < eventList.size(); i++) {
	        	   	         String evt = (String)eventList.get(i);
	        	   	         System.out.println(evt);
	        	    	   }
	        	       }
	        	
	                }
	        		);
	        
	        removeButton.addActionListener(new
	        		ActionListener() {
	        	        
	        	       public void actionPerformed (ActionEvent ent) {
	        	    	   //add chosen list to selectList and global event list
	        	    	   Object selObj[] = selectedList.getSelectedValues();
	        	    	   for (int i = 0; i < selObj.length; i++) {
	        	    		   selListModel.removeElement(selObj[i]);
	        	    		   //remove global event list
	        	    		   eventList.remove(selObj[i]);
	        	    		   
	        	    	   }
	        	    	   
	        	    	   System.out.println("Event List Contains:");
	        	    	   for (int i = 0; i < eventList.size(); i++) {
	        	   	         String evt = (String)eventList.get(i);
	        	   	         System.out.println(evt);
	        	    	   }
	        	    	   
	        	       }
	        	
	                }
	        		);
	        
	        
	        //add action listeners for the load, abort and next buttons
	        BuildNextAction nxtAction = new BuildNextAction(this, ownerThread);
	        BuildAbortAction abrtAction = new BuildAbortAction(this, ownerThread);
	        BuildLoadAction loadAction = new BuildLoadAction(this);
	       
		   
	        JPanel bottom = new JPanel(new FlowLayout(FlowLayout.RIGHT));

	        JButton abortButton = new JButton("ABORT");
	        abortButton.setMnemonic(KeyEvent.VK_T);
	        abortButton.addActionListener(abrtAction);
	       
	        //final JButton nextButton = new JButton("Next");
	        nextButton.setMnemonic(KeyEvent.VK_N);
	        nextButton.addActionListener(nxtAction);
	        
	        JButton loadButton = new JButton("Load Settings");
	        loadButton.setMnemonic(KeyEvent.VK_L);
	        loadButton.addActionListener(loadAction);
	        
		    bottom.add(loadButton);
		   
	        bottom.add(abortButton);
	        bottom.add(nextButton);
	        basic.add(bottom);

	        bottom.setMaximumSize(new Dimension(800, 0));

	        setSize(new Dimension(800, 600));
	        setResizable(false);
	        setLocationRelativeTo(null);
		        
	}
	
}

class BuildNextAction implements ActionListener {
	
	BuildEventsFrame parentFrame;
	SSTFthread ownerThrd;
	
	public BuildNextAction (BuildEventsFrame pFrame, SSTFthread ownerThread) {
	   	parentFrame = pFrame;
	   	ownerThrd = ownerThread;
	}
	
	public void actionPerformed (ActionEvent event) {
		
		parentFrame.dispose();
		
		EventSetupFrame eventFrame = new EventSetupFrame(parentFrame.eventList, parentFrame.loadFileName, ownerThrd);
		eventFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		eventFrame.setVisible(true);
				
	}
	
}

class BuildAbortAction implements ActionListener {
	
	BuildEventsFrame parentFrame;
	SSTFthread ownerThrd;
	
	public BuildAbortAction (BuildEventsFrame pFrame, SSTFthread ownerThread) {
	   	parentFrame = pFrame;
	   	ownerThrd = ownerThread;
	}
	
	public void actionPerformed (ActionEvent event) {
		
		parentFrame.dispose();
		ownerThrd.stopThread();
		
		//relaunch the starting thread
		SSTFthread newStartThread = new SSTFthread();
		newStartThread.start();
	}

}

class BuildLoadAction implements ActionListener {
	
	BuildEventsFrame parentFrame;
	
	
	public BuildLoadAction (BuildEventsFrame pFrame) {
	   	parentFrame = pFrame;
	   	
	}
	
	public void actionPerformed (ActionEvent event) {
		
		 JFileChooser chooser = new JFileChooser ();
		 chooser.setCurrentDirectory(new File (StrezzHeader.SAVED_SETTINGS_DIR));
		 int result = chooser.showDialog(parentFrame, "Select");
		  
		 if (result == JFileChooser.APPROVE_OPTION) {
		        
			   parentFrame.loadFileName = chooser.getSelectedFile().getPath(); //or getAbsolutePath()
		       
			   //trigger 'Next' button Click event, send the setting file to EventSetup Obj
			   parentFrame.nextButton.doClick();
		             
		   }
		   
	}

}

/*Third Frame - Allows to setup individually each of the above chosen events.
 *              Allows to chose messages, rules, templates. Also allows for configuring 
 *              the Fuzz and Spoof settings.
 */
class EventSetupFrame extends JFrame {
	
	HashMap fuzzKeywordFileMap = new HashMap(); //stores the fuzz keywords and the corresponding
														//.fz file as key,value pairs
	ArrayList eventList = new ArrayList();
    ArrayList eventMsgPassingList = new ArrayList();// contains a list of EventMsgPassing Objects
    													   // one object for each event

        
    ArrayList keywordList = new ArrayList(); //list of fuzzkeywords
    
    int curSelectedIndex = 0; //first item by default
    int prevSelectedIndex = 0; //first item by default
    
    //testcase name
    JTextField testNameField = new JTextField("", 25);
    
    JPanel middleRightPanel = new JPanel();
    
    //pause params
    //JFormattedTextField msecField = new JFormattedTextField(NumberFormat.getIntegerInstance());
    JTextField msecField = new JTextField("0", 10);
    JPanel paramPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
	JLabel msecLabel = new JLabel("Milliseconds:");
	 
	 
    //receive params
    ButtonGroup rcvTypeButtonGroup = new ButtonGroup();
    ButtonGroup optTypeButtonGroup = new ButtonGroup();;
    
    JRadioButton reqButton = new JRadioButton("Request");
    JRadioButton respButton = new JRadioButton("Response");
    	 
    JRadioButton optButton = new JRadioButton("Optional");
    JRadioButton mandButton = new JRadioButton("Mandatory");
    
    JTextField rcvValField = new JTextField("", 15);
    
    JPanel rcvTypePanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
	
	JPanel rcvOptPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
		 
	JPanel rcvValuePanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
	JLabel rcvValLabel = new JLabel("Request/Response Type:");
	
	
    
    //send params
    JTextField tplFilePathNameField = new JTextField("", 30);
    
    JButton fileSelectButton = new JButton("Select Message Template File");
    JButton fuzzSettingButton = new JButton("Fuzz Settings");
    JButton spoofSettingButton = new JButton("Spoof Settings");
    
    JPanel tplFilePanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
	 
	 
	 JPanel fuzzSettingPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
	 
	 JPanel spoofSettingPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
	 
	 //store this object
	 EventSetupFrame thisFrame;
	 
	public void updateEventSettings (String event, int indx) {
		
		
			if (event.equalsIgnoreCase(new String("send"))) {
			   //store the setting values in the msgPassingList object
				//the setting represents the values of the previous index
				
				String tplfname = tplFilePathNameField.getText();
			    //spoofFileName format: <testcase name>_spoof_<index>.csv
				//NOT USED now
				String spoofname = testNameField.getText()+"_"+"spoof_"+new String(Integer.toString(prevSelectedIndex))+".csv";
				
				//delete existing object if any and add new one at the pos
				if (eventMsgPassingList.size() > indx)
					eventMsgPassingList.remove(indx);
				
				EventMsgPassing emsgObj = new EventMsgPassing((short)0, tplfname, spoofname);
			    eventMsgPassingList.add(indx, emsgObj);
				
			
			} else if (event.equalsIgnoreCase(new String("receive"))) {
				//store the setting values in the msgPassingList object
				//the setting represents the values of the previous index
				
				short rcvType = -1;
				if (reqButton.isSelected())
					rcvType = 0;
				else if (respButton.isSelected())
					rcvType = 1;
				
				boolean optional = false;
				if (optButton.isSelected())
					optional = true;
				else if (mandButton.isSelected())
					optional = false;
				String rcval = rcvValField.getText();
				
			    //delete existing object if any and add new one at the pos
				if (eventMsgPassingList.size() > indx)
					eventMsgPassingList.remove(indx);
				
				EventMsgPassing emsgObj = new EventMsgPassing((short)1, rcvType, optional,rcval);
			    eventMsgPassingList.add(indx, emsgObj);
				
		
				
			} else if (event.equalsIgnoreCase(new String("pause"))) {
				//store the setting values in the msgPassingList object
				//the setting represents the values of the previous index
				int msecs = 0;
			    // Retrieve the value from the text field
				
		 	    //Integer msecValue = (Integer)msecField.getValue();
				String msecValue = msecField.getText();
				
			    if (msecValue != null)
				  msecs = Integer.parseInt(msecValue);
			   
			    //delete existing object if any and add new one at the pos
		 	    if (eventMsgPassingList.size() > indx)
		 		   eventMsgPassingList.remove(indx);
				
				EventMsgPassing emsgObj = new EventMsgPassing((short)2, msecs);
		 	    eventMsgPassingList.add(indx, emsgObj);
				
		
			}
			
		
	}
	
	
	public void loadTestEventSettings (String settingsFile) {
	
		//Read Settings file, load Objects, update eventList, update eventMsgPassingList
		int fLen = settingsFile.length();
		String fType = settingsFile.substring(fLen-3);
		
		try {
			
			if (fType.equalsIgnoreCase("set") == false) {
				throw new IOException();
			}
			
			System.out.println ("Opening File "+settingsFile);
			File inFile = new File (settingsFile);
			
			if (inFile == null) {
				System.out.println ("Error: Opening File "+ settingsFile);
				
			}
				
			
			// read file into buffer
			FileInputStream fin = new FileInputStream (inFile);
			DataInputStream in = new DataInputStream(fin);
	        BufferedReader br = new BufferedReader(new InputStreamReader(in));
	        String strLine;
	        //Read File Line By Line
	        int objCount = 0;
	        
	        while ((strLine = br.readLine()) != null)   {
	        	
	        	try {
	        	// Read from disk using FileInputStream.
	        	FileInputStream f_in = new FileInputStream (strLine);

	        	// Read object using ObjectInputStream.
	        	ObjectInputStream obj_in = new ObjectInputStream (f_in);

	        	// Read an object.
	        	Object obj = obj_in.readObject ();

	        	if (obj instanceof EventMsgPassing)
	        	{
	        	  
	        	  EventMsgPassing msgObj = (EventMsgPassing) obj;
	        	  eventMsgPassingList.add(objCount, msgObj);
	        	  if (msgObj.eventType == 0) {
	        		  eventList.add("send");
	        	  }
	        	  else if (msgObj.eventType == 1) {
	        		  eventList.add("receive");	        	  
	        	  }
	        	  else if (msgObj.eventType == 2) {
	        		  eventList.add("pause");
	        	  }
	        	  
	        	}
	        	
	        	obj_in.close();
	        	
	        	} catch  (Exception e) {
	    	        System.out.println ( "Read Settings/Data File Exception =: " + e );
	    	        e.printStackTrace();
	    	        
	    	    }
	        	
	        	objCount++;
	        }
	        //Close the input stream
	        fin.close ();
	    } catch  (IOException e) {
	        System.out.println ( "Opening Settings File IO Exception =: " + e );
	        e.printStackTrace();
	        
	    } //stream exception
	    
	    System.out.println ( "After loading Settings...");
	    printEventMsgObjects();
	    
	}
	
	
	
	public EventSetupFrame (ArrayList evtlst, String loadFile, SSTFthread ownerThread) {
				
		//first read the "keyword.lst" file to determine the keywords.
  	    String keywordFile = StrezzHeader.FUZZ_DIR+"keyword.lst";
		try {
	        BufferedReader in = new BufferedReader(new FileReader(keywordFile));
	        String str;
	        int i = 0;
	        while ((str = in.readLine()) != null) {
	        	 	keywordList.add(i, str);
	        	 	i++;
	        }
	        in.close();
	        
		} catch (IOException e) {
	    	System.out.println ( "Reading AllHeaders file exception =: " + e );
		    e.printStackTrace();
		}
	 
		//store the keywords and the default associated .fz files in the hashmap
		for (int i = 0; i < keywordList.size(); i++) {
			String str = (String)keywordList.get(i);
			fuzzKeywordFileMap.put(str, StrezzHeader.FUZZ_DIR+str+".fz");
		}
		

		thisFrame = this;
		addWindowListener(new SSTFframeListener(ownerThread));
		
		if (loadFile == null) {  //fresh config not loaded settings
			//copy the eventlist
			eventList = evtlst;
			for (int i = 0; i < eventList.size(); i++) {
		    	  //Dummy objects - Will be updated in updateEventSettings
		    	  EventMsgPassing emsgObj = new EventMsgPassing();
			 	  eventMsgPassingList.add(i, emsgObj);
			 	    
		      }
		}
		else { //load settings
			System.out.println("Loading Settings from: "+loadFile);
			
			//Read Settings file, load Objects, update eventList, update eventMsgPassingList
			
			loadTestEventSettings (loadFile);
		}
		
		
		System.out.println("Event List in EventSetup Contains:");
  	    for (int i = 0; i < eventList.size(); i++) {
 	         String evt = (String)eventList.get(i);
 	         System.out.println(evt);
  	    }
  	    
  	  //
  	  
				
  	  //create the main window frame components  	  
  	  setTitle("SSTF setup wizard - Events Setup");

      JPanel basic = new JPanel();
      basic.setLayout(new BoxLayout(basic, BoxLayout.Y_AXIS));
      add(basic);

      JPanel topPanel = new JPanel(new BorderLayout(0, 0));
      topPanel.setMaximumSize(new Dimension(800, 0));
      JLabel intro = new JLabel("Choose each Event on the Left and configure it on the right.   If Required \"Save Settings\" for later Use.");
      intro.setPreferredSize (new Dimension(600, 50));
      intro.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
      
      topPanel.add(intro, BorderLayout.NORTH);

      JLabel testNameLabel = new JLabel("Enter a name for the testcase:");
      testNameLabel.setPreferredSize (new Dimension(250, 50));
      testNameLabel.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
      topPanel.add(testNameLabel, BorderLayout.WEST);
      
      testNameField.setText("test");
      topPanel.add(testNameField, BorderLayout.CENTER);
            
      
      JSeparator separator = new JSeparator();
      separator.setForeground(Color.gray);

      topPanel.add(separator, BorderLayout.SOUTH);

      basic.add(topPanel);

             
      JPanel middlePanel = new JPanel();
      middlePanel.setLayout(new BoxLayout(middlePanel, BoxLayout.X_AXIS));
      middlePanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
      basic.add(middlePanel);
      
      //add two panels to the middle panel
      JPanel middleLeftPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
      middleLeftPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
      middleLeftPanel.setPreferredSize(new Dimension(110, 475));
      middlePanel.add(middleLeftPanel);
      
      middleRightPanel.setLayout(new BoxLayout(middleRightPanel, BoxLayout.Y_AXIS));
      middleRightPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
      middleRightPanel.setPreferredSize(new Dimension(480, 475));
      middlePanel.add(middleRightPanel);
      
      //add a list box to the middle left panel
    /*  final JList setupEventList = new JList(eventList.toArray());
      setupEventList.setPreferredSize(new Dimension(100, 450));
      middleLeftPanel.add(setupEventList);
    */  
      //using combo box instead  - BUT need a work around for duplicate elements 
      final JComboBox setupEventCombo = new JComboBox();
      //append 'index' to the event name ex: send-0, pause-1 etc

      for (int i = 0; i < eventList.size(); i++) {
    	  
    	  String evtStr = (String)eventList.get(i);
    	  evtStr = evtStr+"-"+Integer.toString(i);
    	  setupEventCombo.addItem(evtStr);
    	 
	 	    
      }
      middleLeftPanel.add(setupEventCombo);
      //add actionlistener for combo, which will store selected items index
      //in curSelectedIndex
      setupEventCombo.setSelectedIndex(-1);
      
      setupEventCombo.addActionListener(new
    		  ActionListener() {
    	  	
    	  		public void actionPerformed (ActionEvent evt) {
    	  			
    	  			prevSelectedIndex = curSelectedIndex;
    	  			
    	  			curSelectedIndex = setupEventCombo.getSelectedIndex();
    	  			
    	  			System.out.println("Prev Selected index is: "+prevSelectedIndex);
    	  			System.out.println("Cur Selected index is: "+curSelectedIndex);
    	  			
    	  			String evtName = (String)setupEventCombo.getSelectedItem();
    	  			
    	  			String prevEvtName = (String)setupEventCombo.getItemAt(prevSelectedIndex);
    	  			
    	  			//extract just event name (ignore index)
    	  			StringTokenizer st2 = new StringTokenizer(evtName, "-");
    	  			evtName = st2.nextToken(); 
    	  			
    	  			StringTokenizer st3 = new StringTokenizer(prevEvtName, "-");
    	  			prevEvtName = st3.nextToken(); 
    	  			
    	  			
    	  			//updateEventSettings (prevEvtName, prevSelectedIndex);
    	  		   	//Now updated by Confirm Button
    	  			
    	  			if (evtName.equalsIgnoreCase(new String("send"))) {
    	  				System.out.println("Selected event is: "+evtName);
    	  				showSendEventSetting (curSelectedIndex);
    	  			} else if (evtName.equalsIgnoreCase(new String("receive"))) {
    	  				System.out.println("Selected event is: "+evtName);
    	  			 	showReceiveEventSetting (curSelectedIndex);		
    	  			} else if (evtName.equalsIgnoreCase(new String("pause"))) {
    	  				System.out.println("Selected event is: "+evtName);
    	  			   	showPauseEventSetting (curSelectedIndex);
    	  			}

    	  		}
    	  
      });
      
      //add a confirm button - eventMsgList will be updated only on Clicking this button
      
      JButton confirmButton = new JButton("Confirm");
      confirmButton.setMnemonic(KeyEvent.VK_C);
      middleLeftPanel.add(confirmButton);
      confirmButton.addActionListener(new
    		  ActionListener() {
  	  	
	  			public void actionPerformed (ActionEvent evt) {
	  				
	  				String evtName = (String)setupEventCombo.getSelectedItem();
	  				if (evtName == null) return;
	  				
	  				StringTokenizer st2 = new StringTokenizer(evtName, "-");
    	  			evtName = st2.nextToken(); 
    	  			
	  			    int evtIndx = setupEventCombo.getSelectedIndex();
	  				
	  			    updateEventSettings (evtName, evtIndx);
	  			    
	  			}
	  
      		});
      
      
      
      //add action listeners for the abort and next buttons
      EventNextAction nxtAction = new EventNextAction(this, ownerThread);
      EventAbortAction abrtAction = new EventAbortAction(this, ownerThread);

      //save settings
      EventSaveAction saveAction = new EventSaveAction(this);
      
      JPanel bottom = new JPanel(new FlowLayout(FlowLayout.RIGHT));

      JButton abortButton = new JButton("ABORT");
      abortButton.setMnemonic(KeyEvent.VK_T);
      abortButton.addActionListener(abrtAction);
      JButton nextButton = new JButton("NEXT");
      nextButton.setMnemonic(KeyEvent.VK_N);
      nextButton.addActionListener(nxtAction);
      
      JButton saveButton = new JButton("Save Settings");
      saveButton.setMnemonic(KeyEvent.VK_S);
      saveButton.addActionListener(saveAction);
      
      bottom.add(saveButton);
      bottom.add(abortButton);
      bottom.add(nextButton);
      basic.add(bottom);

      bottom.setMaximumSize(new Dimension(800, 0));

      setSize(new Dimension(800, 600));
      setResizable(false);
      setLocationRelativeTo(null);
      
      //Add pause param components and hide them
      middleRightPanel.add(paramPanel);
 	  paramPanel.add(msecLabel);
 	  msecField.setColumns(10);
 	  
 	  paramPanel.add(msecField);
 	  paramPanel.setVisible(false);
   
      //Add receive param components and hide them
 	  middleRightPanel.add(rcvTypePanel);
	  rcvTypePanel.add(reqButton);
	  rcvTypePanel.add(respButton);
	  rcvTypeButtonGroup.add(reqButton);
      rcvTypeButtonGroup.add(respButton);
    
      middleRightPanel.add(rcvOptPanel);
 	  rcvOptPanel.add(optButton);
 	  rcvOptPanel.add(mandButton);
 	  optTypeButtonGroup.add(optButton);
 	  optTypeButtonGroup.add(mandButton);
 	
 	  middleRightPanel.add(rcvValuePanel);
 	  rcvValuePanel.add(rcvValLabel);
 	  rcvValuePanel.add(rcvValField);
 	  
 	  //hide the panels
 	  rcvTypePanel.setVisible(false);
 	  rcvOptPanel.setVisible(false);
 	  rcvValuePanel.setVisible(false);
 	
 	  //Add send param components and hide them
 	  middleRightPanel.add(tplFilePanel);
	  tplFilePanel.add(tplFilePathNameField);
	  tplFilePanel.add(fileSelectButton);
	 
	  middleRightPanel.add(fuzzSettingPanel);
	  fuzzSettingPanel.add(fuzzSettingButton);
	 
	  middleRightPanel.add(spoofSettingPanel);
	  spoofSettingPanel.add(spoofSettingButton);
     
	  //hide the panels
	  tplFilePanel.setVisible(false);
	  fuzzSettingPanel.setVisible(false);
	  spoofSettingPanel.setVisible(false);
	 
 	  ActionListener fileSelectListener = new 
	    ActionListener () {

		  public void actionPerformed (ActionEvent evt) {
       //create filechooser dialog
		   JFileChooser chooser = new JFileChooser ();
		   chooser.setCurrentDirectory(new File (StrezzHeader.TEMPLATE_DIR));
		   int result = chooser.showDialog(thisFrame, "Select");
		  
		   if (result == JFileChooser.APPROVE_OPTION)
		    tplFilePathNameField.setText(chooser.getSelectedFile().getPath()); //or getAbsolutePath()
		  
		  }

	   };
	  fileSelectButton.setMnemonic(KeyEvent.VK_F);
      fileSelectButton.addActionListener(fileSelectListener);
 
      ActionListener fuzzSettingListener = new 
	   ActionListener () {

		public void actionPerformed (ActionEvent evt) {
       //create the fuzz setting window
		  showFuzzSettingWindow();
		}

	   };
	  fuzzSettingButton.setMnemonic(KeyEvent.VK_Z);
	  fuzzSettingButton.addActionListener(fuzzSettingListener);
   
	  ActionListener spoofSettingListener = new 
	   ActionListener () {

		public void actionPerformed (ActionEvent evt) {
       //create the spoof setting window
			showSpoofSettingWindow();
		}

	  };
	  spoofSettingButton.setMnemonic(KeyEvent.VK_Z);
	  spoofSettingButton.addActionListener(spoofSettingListener);

      
	  
	  
	}
	
	public void showSpoofSettingWindow () {
		
		 //create the main window frame and other components
		 final JFrame fuzzFrame = new JFrame();
		 fuzzFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		 fuzzFrame.setTitle("SSTF setup wizard - Spoof Settings");

	      JPanel basic = new JPanel();
	      basic.setLayout(new BoxLayout(basic, BoxLayout.Y_AXIS));
	      fuzzFrame.add(basic);

	      JPanel topPanel = new JPanel(new BorderLayout(0, 0));
	      topPanel.setMaximumSize(new Dimension(800, 0));
	      String introText1 = "Supply the user and ip spoofing ranges below and click OK. ";
	      String introText2 = "NOTE: These settings will work only if the template file "+ 
	                          "defines appropriate Spoof keywords (Ex: SIP_src_ip_spoof).";
	      
	      JLabel intro1 = new JLabel(introText1);
	      intro1.setPreferredSize (new Dimension(600, 25));
	      intro1.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
	      
	      topPanel.add(intro1, BorderLayout.NORTH);

	      JLabel intro2 = new JLabel(introText2);
	      intro2.setPreferredSize (new Dimension(600, 25));
	      intro2.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
	      
	      topPanel.add(intro2, BorderLayout.CENTER);

	      
	      JSeparator separator = new JSeparator();
	      separator.setForeground(Color.gray);

	      topPanel.add(separator, BorderLayout.SOUTH);

	      basic.add(topPanel);

	             
	      JPanel middlePanel = new JPanel();
	      middlePanel.setLayout(new BoxLayout(middlePanel, BoxLayout.Y_AXIS));
	      middlePanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      basic.add(middlePanel);
	      
	      //use four panels for src usr, dst usr, src ip & dst ip settings
	      JPanel middlePanel1 = new JPanel();
	      middlePanel1.setLayout(new FlowLayout(FlowLayout.LEFT));
	      middlePanel1.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      middlePanel.add(middlePanel1);
	      
	      JPanel middlePanel2 = new JPanel();
	      middlePanel2.setLayout(new FlowLayout(FlowLayout.LEFT));
	      middlePanel2.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      middlePanel.add(middlePanel2);
	      
	      JPanel middlePanel3 = new JPanel();
	      middlePanel3.setLayout(new FlowLayout(FlowLayout.LEFT));
	      middlePanel3.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      middlePanel.add(middlePanel3);
	      
	      JPanel middlePanel4 = new JPanel();
	      middlePanel4.setLayout(new FlowLayout(FlowLayout.LEFT));
	      middlePanel4.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      middlePanel.add(middlePanel4);
	      
	      //src user
	      JLabel srcUsrLabel = new JLabel("From User (Caller) Range (ex: 1234xxx): ");
	      middlePanel1.add(srcUsrLabel);
	      final JTextField srcUsrField = new JTextField("", 15);
	      middlePanel1.add(srcUsrField);
	      
	     //dst user
	      JLabel dstUsrLabel = new JLabel("To User (Caller) Range (ex: 456xx): ");
	      middlePanel2.add(dstUsrLabel);
	      final JTextField dstUsrField = new JTextField("", 15);
	      middlePanel2.add(dstUsrField);
	      
	      //src ip
	      JLabel srcIPLabel = new JLabel("From IP address Range [net mask count] (ex: 192.168.0.0 16 100)");
	      middlePanel3.add(srcIPLabel);
	      final JTextField srcNetField = new JTextField("", 15);
	      middlePanel3.add(srcNetField);
	      final JTextField srcMaskField = new JTextField("", 2);
	      middlePanel3.add(srcMaskField);
	      final JTextField srcCountField = new JTextField("", 8);
	      middlePanel3.add(srcCountField);
	      srcCountField.setText("0");
	      
	      //dst ip
	      JLabel dstIPLabel = new JLabel("To IP address Range [net mask count] (Count value 0 generates full Range)");
	      middlePanel4.add(dstIPLabel);
	      final JTextField dstNetField = new JTextField("", 15);
	      middlePanel4.add(dstNetField);
	      final JTextField dstMaskField = new JTextField("", 2);
	      middlePanel4.add(dstMaskField);
	      final JTextField dstCountField = new JTextField("", 8);
	      middlePanel4.add(dstCountField);
	      dstCountField.setText("0");
	      
	      
	      JPanel bottom = new JPanel(new FlowLayout(FlowLayout.RIGHT));

	      JButton cancelButton = new JButton("Cancel");
	      cancelButton.setMnemonic(KeyEvent.VK_C);
	      cancelButton.addActionListener(new
	    		  ActionListener() {
	    	  	
  	  		public void actionPerformed (ActionEvent evt) {
  	  		  
  	  		  fuzzFrame.dispose();
  	  		}	    	  
  	  		
	      });
	      
	      JButton okButton = new JButton("OK");
	      okButton.setMnemonic(KeyEvent.VK_K);
	      okButton.addActionListener(new
	    		  ActionListener() {
	    	  	
  	  		public void actionPerformed (ActionEvent evt) {
  	  			
  	  		  //call spoof_ip_gen and call_extension_gen with chosen range values
  	  	      //Form the command and create a RunCommand Object to run it
  	  		
  	  			 //NO error checking done on the spoof setting field values
  	  			//only check for empty fields
  	  			
  	  			String src_usr = srcUsrField.getText();
  	  		    String dst_usr = dstUsrField.getText();
  	  		    String src_ip = srcNetField.getText();
  	  			String dst_ip = dstNetField.getText();
  	  			String src_msk = srcMaskField.getText();
  	  			String dst_msk = dstMaskField.getText();
  	  		    String src_cnt = srcCountField.getText();
  	  	        String dst_cnt = dstCountField.getText();

 	  	        /*
  	  			if (src_usr.equalsIgnoreCase("")) {
  	  	           JOptionPane.showConfirmDialog(fuzzFrame, "FROM User should be non-empty!!!", "Empty Spoof Value Notification", JOptionPane.PLAIN_MESSAGE);
  	  	           return;
  	  			}
  	  		    if (dst_usr.equalsIgnoreCase("")) {
	  	           JOptionPane.showConfirmDialog(fuzzFrame, "TO User should be non-empty!!!", "Empty Spoof Value Notification", JOptionPane.PLAIN_MESSAGE);
	  	           return;
	  			}
  	  		    if (src_ip.equalsIgnoreCase("")) {
	  	           JOptionPane.showConfirmDialog(fuzzFrame, "FROM IP net should be non-empty!!!", "Empty Spoof Value Notification", JOptionPane.PLAIN_MESSAGE);
	  	           return;
	  			}
  	  		    if (dst_ip.equalsIgnoreCase("")) {
	  	           JOptionPane.showConfirmDialog(fuzzFrame, "TO IP net should be non-empty!!!", "Empty Spoof Value Notification", JOptionPane.PLAIN_MESSAGE);
	  	           return;
	  			}
  	  		    if (src_msk.equalsIgnoreCase("")) {
	  	           JOptionPane.showConfirmDialog(fuzzFrame, "FROM IP mask should be non-empty!!!", "Empty Spoof Value Notification", JOptionPane.PLAIN_MESSAGE);
	  	           return;
	  			}
  	  		    if (dst_msk.equalsIgnoreCase("")) {
	  	           JOptionPane.showConfirmDialog(fuzzFrame, "TO IP mask should be non-empty!!!", "Empty Spoof Value Notification", JOptionPane.PLAIN_MESSAGE);
	  	           return;
	  			}
  	  		    if (src_cnt.equalsIgnoreCase("")) {
	  	           JOptionPane.showConfirmDialog(fuzzFrame, "FROM IP count should be non-empty!!!", "Empty Spoof Value Notification", JOptionPane.PLAIN_MESSAGE);
	  	           return;
	  			}
  	  		    if (dst_cnt.equalsIgnoreCase("")) {
	  	           JOptionPane.showConfirmDialog(fuzzFrame, "TO IP count should be non-empty!!!", "Empty Spoof Value Notification", JOptionPane.PLAIN_MESSAGE);
	  	           return;
	  			}
  	  		
  	  		    */
  	  	        
  	  		    if (src_cnt.equalsIgnoreCase("0") || src_cnt.equalsIgnoreCase("")) {
	  	           src_cnt = "all";
	  			}
  	  		    if (dst_cnt.equalsIgnoreCase("0") || dst_cnt.equalsIgnoreCase("")) {
	  	           dst_cnt = "all";
	  			}
	  		    
  	  		    //Create call extension gen object
  	  		    if (src_usr.equalsIgnoreCase("") == false) {
  	  		    	CallExtensionGen srcUObj = new CallExtensionGen (src_usr, "src");
  	  		    }
  	  		    if (dst_usr.equalsIgnoreCase("") == false) {
  	  		    	CallExtensionGen dstUObj = new CallExtensionGen (dst_usr, "dst");
  	  		    }
  	  		    
    	  	    //Create spoof ip gen object
  	  		    if ((src_ip.equalsIgnoreCase("") == false) && (src_msk.equalsIgnoreCase("") == false)) {
  	  		    	SpoofIPGen srcIPobj = new SpoofIPGen (src_ip, src_msk, src_cnt, "src");
  	  		    }
  	  		    if ((dst_ip.equalsIgnoreCase("") == false) && (dst_msk.equalsIgnoreCase("") == false)) {
  	  		    	SpoofIPGen dstIPobj = new SpoofIPGen (dst_ip, dst_msk, dst_cnt, "dst");
  	  		    }

  	  	        
  	  		    /*  //Using the Older C Programs
  	  		    String cmdStr="";
  	  		    //create RunCommand object
  	  		    RunCommand cmd = new RunCommand();
  	  		
  	  		    //from usr command
  	  		    cmdStr = StrezzHeader.SPOOF_DIR+"call_extension_gen_sstf "+src_usr+" src";
  	  		    System.out.println("FROM User Command: "+cmdStr);
  	  		    //execute the command
  	  		    cmd.runCommand(cmdStr);
  	  		    
  	  		    //to usr command
  	  		    cmdStr = StrezzHeader.SPOOF_DIR+"call_extension_gen_sstf "+dst_usr+" dst";
  	  		    System.out.println("TO User Command: "+cmdStr);
     	  		//execute the command
  	  		    cmd.runCommand(cmdStr);
  	  		    
  	  		    //from ip command
  	  		    cmdStr = StrezzHeader.SPOOF_DIR+"spoof_ip_gen_sstf "+src_ip+" "+
  	  		             src_msk+" dummy add "+src_cnt+" src";
	  		    System.out.println("FROM IP Command: "+cmdStr);
	  		    //execute the command
  	  		    cmd.runCommand(cmdStr);
  	  		    
  	  		    //to ip command
	  		    cmdStr = StrezzHeader.SPOOF_DIR+"spoof_ip_gen_sstf "+dst_ip+" "+
		             dst_msk+" dummy add "+dst_cnt+" dst";
	  		    System.out.println("TO IP Command: "+cmdStr);
	  		    //execute the command
  	  		    cmd.runCommand(cmdStr);
  	  		    
  	  		    */
  	  		    
  	  		    //close the frame
  	  		    fuzzFrame.dispose();
  	  		    
  	  		}	    	  
	      });
	      
	      bottom.add(cancelButton);
	      bottom.add(okButton);
	      basic.add(bottom);

	      bottom.setMaximumSize(new Dimension(800, 0));

	      fuzzFrame.setSize(new Dimension(800, 600));
	      fuzzFrame.setResizable(false);
	      fuzzFrame.setLocationRelativeTo(null);

	      fuzzFrame.setVisible(true);
	      
	}
	

	public void showFuzzSettingWindow() {
		
		
	    //print hashamp contents
		System.out.println("fuzzKeywordFileMap contents:");
		for (int i = 0; i < fuzzKeywordFileMap.size(); i++) {
			String key = (String)keywordList.get(i);
			String value = (String)fuzzKeywordFileMap.get(key);
			System.out.println(key+":"+value);
		}
		
		 //create the main window frame and other components
		 final JFrame fuzzFrame = new JFrame();
		 
		 fuzzFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		 fuzzFrame.setTitle("SSTF setup wizard - Fuzz Settings");

	      JPanel basic = new JPanel();
	      basic.setLayout(new BoxLayout(basic, BoxLayout.Y_AXIS));
	      fuzzFrame.add(basic);

	      JPanel topPanel = new JPanel(new BorderLayout(0, 0));
	      topPanel.setMaximumSize(new Dimension(800, 0));
	      JLabel intro = new JLabel("Select Fuzz keywords and choose a .fz file to use. For Fuzz Generation, use settings in the bottom half.");
	      intro.setPreferredSize (new Dimension(600, 50));
	      intro.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
	      
	      topPanel.add(intro, BorderLayout.NORTH);

	      JSeparator separator = new JSeparator();
	      separator.setForeground(Color.gray);

	      topPanel.add(separator, BorderLayout.SOUTH);

	      basic.add(topPanel);

	             
	      JPanel middlePanel = new JPanel();
	      middlePanel.setLayout(new BoxLayout(middlePanel, BoxLayout.Y_AXIS));
	      middlePanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      basic.add(middlePanel);
	      
	      //add two panels to the middle panel
	      JPanel middleTopPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
	      middleTopPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      middleTopPanel.setPreferredSize(new Dimension(700, 50));
	      middlePanel.add(middleTopPanel);
	      
	      JPanel middleBottomPanel = new JPanel();
	      //middleBottomPanel.setLayout(new BoxLayout(middleBottomPanel, BoxLayout.X_AXIS));
	      middleBottomPanel.setLayout(new FlowLayout(FlowLayout.CENTER));
	      middleBottomPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      middleBottomPanel.setPreferredSize(new Dimension(700, 450));
	      middlePanel.add(middleBottomPanel);
	      
	      //add two panels to the middle bottom panel
	      JPanel middleLeftBottomPanel = new JPanel();
	      middleLeftBottomPanel.setLayout(new BoxLayout(middleLeftBottomPanel, BoxLayout.Y_AXIS));
	      middleLeftBottomPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      middleLeftBottomPanel.setPreferredSize(new Dimension(350, 430));
	      middleBottomPanel.add(middleLeftBottomPanel);
	      
	      JPanel middleRightBottomPanel = new JPanel();
	      middleRightBottomPanel.setLayout(new BoxLayout(middleRightBottomPanel, BoxLayout.Y_AXIS));
	      middleRightBottomPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      middleRightBottomPanel.setPreferredSize(new Dimension(350, 430));
	      middleBottomPanel.add(middleRightBottomPanel);
	      
	      //add rules file selection for middleLeftBottomPanel
	      final JTextField fuzzRulesFilePathNameField = new JTextField("", 25);
	      middleLeftBottomPanel.add(fuzzRulesFilePathNameField);
	      
	      JButton chooseRulesFileButton = new JButton("Choose Rules File (.xml)");
	      chooseRulesFileButton.setMnemonic(KeyEvent.VK_C);
	      chooseRulesFileButton.addActionListener(new
	    		  ActionListener() {
	    	  	
  	  		public void actionPerformed (ActionEvent evt) {
  	  		 
  	  		  JFileChooser rulesChooser = new JFileChooser ();
  		      rulesChooser.setCurrentDirectory(new File (StrezzHeader.RULES_DIR));
  			  int result = rulesChooser.showDialog(fuzzFrame, "Select");
  			  
  			  if (result == JFileChooser.APPROVE_OPTION)
  			      fuzzRulesFilePathNameField.setText(rulesChooser.getSelectedFile().getPath()); //or getAbsolutePath()
  			  
  	  		}	    	  
  	  		
	      });
	      middleLeftBottomPanel.add(chooseRulesFileButton);
	   
	      //add buttons and text fields for parent file choosers
	            
	      
	      //parent1
	      final JTextField parent1FilePathNameField = new JTextField("", 25);
	      middleRightBottomPanel.add(parent1FilePathNameField);
	      
	      JButton parent1FileButton = new JButton("Choose Parent 1 File (.fz)");
	      parent1FileButton.setMnemonic(KeyEvent.VK_1);
	      parent1FileButton.addActionListener(new
	    		  ActionListener() {
	    	  	
  	  		public void actionPerformed (ActionEvent evt) {
  	  		 
  	  		  JFileChooser rulesChooser = new JFileChooser ();
  		      rulesChooser.setCurrentDirectory(new File (StrezzHeader.FUZZ_DIR));
  			  int result = rulesChooser.showDialog(fuzzFrame, "Select");
  			  
  			  if (result == JFileChooser.APPROVE_OPTION)
  				parent1FilePathNameField.setText(rulesChooser.getSelectedFile().getPath()); //or getAbsolutePath()
  			  
  	  		}	    	  
  	  		
	      });
	      middleRightBottomPanel.add(parent1FileButton);
	   
	      //parent2
	      final JTextField parent2FilePathNameField = new JTextField("", 25);
	      middleRightBottomPanel.add(parent2FilePathNameField);
	      
	      JButton parent2FileButton = new JButton("Choose Parent 2 File (.fz)");
	      parent2FileButton.setMnemonic(KeyEvent.VK_2);
	      parent2FileButton.addActionListener(new
	    		  ActionListener() {
	    	  	
  	  		public void actionPerformed (ActionEvent evt) {
  	  		 
  	  		  JFileChooser rulesChooser = new JFileChooser ();
  		      rulesChooser.setCurrentDirectory(new File (StrezzHeader.FUZZ_DIR));
  			  int result = rulesChooser.showDialog(fuzzFrame, "Select");
  			  
  			  if (result == JFileChooser.APPROVE_OPTION)
  				parent2FilePathNameField.setText(rulesChooser.getSelectedFile().getPath()); //or getAbsolutePath()
  			  
  	  		}	    	  
  	  		
	      });
	      middleRightBottomPanel.add(parent2FileButton);
	      
	      
	      final JComboBox keywordCombo = new JComboBox(keywordList.toArray());
	      middleTopPanel.add(keywordCombo);
	      
	      //add actionlistener for combo
	      //dont think anything to be done
	      keywordCombo.setSelectedIndex(-1);
	      keywordCombo.addActionListener(new
	    		  ActionListener() {
	    	  	
	    	  		public void actionPerformed (ActionEvent evt) {
	    	  			
	    	  		}	    	  
	      });
	      	      
	      final JTextField fuzzFilePathNameField = new JTextField("", 25);
	      middleTopPanel.add(fuzzFilePathNameField);
	      
	      JButton fuzzfileSelectButton = new JButton("Select fuzz file (.fz)"); 
	      fuzzfileSelectButton.setMnemonic(KeyEvent.VK_F);
	      middleTopPanel.add(fuzzfileSelectButton);
	 
	      JPanel bottom = new JPanel(new FlowLayout(FlowLayout.RIGHT));

	      JButton cancelButton = new JButton("Cancel");
	      cancelButton.setMnemonic(KeyEvent.VK_C);
	      cancelButton.addActionListener(new
	    		  ActionListener() {
	    	  	
  	  		public void actionPerformed (ActionEvent evt) {
  	  		  //set the hashmap to its default
  	  		   fuzzKeywordFileMap.clear();
  	  		  //store the keywords and the default associated .fz files in the hashmap
  	 		  for (int i = 0; i < keywordList.size(); i++) {
  	 			String str = (String)keywordList.get(i);
  	 			fuzzKeywordFileMap.put(str, StrezzHeader.FUZZ_DIR+str+".fz");
  	 		  }
  	  		  fuzzFrame.dispose();
  	  		}	    	  
  	  		
	      });
	      
	      JButton okButton = new JButton("OK");
	      okButton.setMnemonic(KeyEvent.VK_K);
	      okButton.addActionListener(new
	    		  ActionListener() {
	    	  	
  	  		public void actionPerformed (ActionEvent evt) {
  	  			
  	  		     fuzzFrame.dispose();
  	  		    
  	  		}	    	  
	      });
	      
	      bottom.add(cancelButton);
	      bottom.add(okButton);
	      basic.add(bottom);

	      bottom.setMaximumSize(new Dimension(800, 0));

	      fuzzFrame.setSize(new Dimension(800, 600));
	      fuzzFrame.setResizable(false);
	      fuzzFrame.setLocationRelativeTo(null);
	      
	      fuzzFrame.setVisible(true);
	      
	      ActionListener fuzzfileSelectListener = new 
		    ActionListener () {

			  public void actionPerformed (ActionEvent evt) {
	       //create filechooser dialog
			   JFileChooser chooser = new JFileChooser ();
			   chooser.setCurrentDirectory(new File (StrezzHeader.FUZZ_DIR));
			   int result = chooser.showDialog(fuzzFrame, "Select");
			  
			   if (result == JFileChooser.APPROVE_OPTION)
			    
				   fuzzFilePathNameField.setText(chooser.getSelectedFile().getPath()); //or getAbsolutePath()
			  
			    //need to update hashmap
			    String kwdName = (String)keywordCombo.getSelectedItem();
			    fuzzKeywordFileMap.put(kwdName, fuzzFilePathNameField.getText());
			    String newitem = (String)fuzzKeywordFileMap.get(kwdName);
			    System.out.println("Replaced item: "+newitem);
			  }

		   };
		 
		   fuzzfileSelectButton.addActionListener(fuzzfileSelectListener);
		
		   JButton genFuzzMsgTplButton = new JButton("Generate Fuzz Message Templates (.tpl's)"); 
		   genFuzzMsgTplButton.setMnemonic(KeyEvent.VK_T);
		   middleLeftBottomPanel.add(genFuzzMsgTplButton);
		   
		 //OFFSPRING_PAIRS count
		   
		   final JTextField offCountText = new JTextField("Enter Number of OffSprings Generated.Default is 50 pairs", 10);
		   
		   middleRightBottomPanel.add(offCountText);
		   		      
		   JButton genFuzzDataButton = new JButton("Generate Fuzz Data Values (.fz's)"); 
		   genFuzzDataButton.setMnemonic(KeyEvent.VK_D);
		   middleRightBottomPanel.add(genFuzzDataButton);
		   
		   ActionListener genFuzzMsgTplListener = new 
		    ActionListener () {

			  public void actionPerformed (ActionEvent evt) {
	          
			   //call FuzzRulesParse with the chosen rules file as input
			   String val = fuzzRulesFilePathNameField.getText(); 	  
			   if ((val.equalsIgnoreCase("") == true) || val == null) {
				   JOptionPane.showConfirmDialog(fuzzFrame, "Select a RULES File First!!!", "Fuzz Template Generation Notification", JOptionPane.PLAIN_MESSAGE);
				   return;
			   }
			   
			   //call fuzz rules parse
			   FuzzRulesParse frpObj = new FuzzRulesParse (val);
			   		   
			   
			   //show popup dialog for confirmation
			   JOptionPane.showConfirmDialog(fuzzFrame, "New Template Files (.tpl) generated will be found at: "+StrezzHeader.RULES_DIR, "Fuzz Template Generation Confirmation", JOptionPane.PLAIN_MESSAGE);

			  }

		   };
		   
		   ActionListener genFuzzDataListener = new 
		    ActionListener () {

			  public void actionPerformed (ActionEvent evt) {
				  
	           //call FuzzGen with the chosen parent files as input
				 String parent1 = parent1FilePathNameField.getText();
				 String parent2 = parent2FilePathNameField.getText();
				 				 
				 
				 if ((parent1 == null) || (parent2 == null) || (parent1.equalsIgnoreCase("") == true) || (parent1.equalsIgnoreCase("") == true)) {
					 JOptionPane.showConfirmDialog(fuzzFrame, "Select TWO Parent Files!!!", "Fuzz Data Generation Notification", JOptionPane.PLAIN_MESSAGE);
				   return;	 
				 }
				 
				 int offSpringCount = 0;
				 
				 if (offCountText.getText().toLowerCase().contains("enter"))	
                	 offSpringCount = 0;
				 else if ((offCountText.getText() != null) && (offCountText.getText().equalsIgnoreCase("")==false)) 
					 offSpringCount = Integer.parseInt(offCountText.getText());
                 
				 if (offSpringCount > 0)
					 StrezzHeader.OFFSPRING_PAIRS = offSpringCount;
			 
			    //call fuzzGen
	   		     FuzzGen fgObj = new FuzzGen (parent1, parent2);

	
				  
     		  //show popup dialog for confirmation
				 JOptionPane.showConfirmDialog(fuzzFrame, "New Fuzz Files (.fz) generated will be found at: "+StrezzHeader.OFFSPRING_DIR, "Fuzz Data Generation Confirmation", JOptionPane.PLAIN_MESSAGE);		  
  
			  }

		   };
		   
		   genFuzzMsgTplButton.addActionListener(genFuzzMsgTplListener);
		   genFuzzDataButton.addActionListener(genFuzzDataListener);
		   
	}
	
	/*These functions (showXXXEventSetting()) show the event configuration settings on the right middle panel
	 * Hide other panels, show relevant panels only
	 * evtmsgpassingList.size > index means we need to update the component values from the stored msgpassingList object
	 */
	private void showPauseEventSetting (int index) {
		
		if (eventMsgPassingList.size() > index) { //update components by reading from msgpassinglist
					
				EventMsgPassing evtmsgObj = (EventMsgPassing)eventMsgPassingList.get(index);
				
				//msecField.setValue(new Integer(evtmsgObj.pauseTime));
				msecField.setText(Integer.toString(evtmsgObj.pauseTime));
					
		} 
		//hide send and receive panels, show pause panel
		rcvTypePanel.setVisible(false);
	 	rcvOptPanel.setVisible(false);
	 	rcvValuePanel.setVisible(false);
	 	tplFilePanel.setVisible(false);
		fuzzSettingPanel.setVisible(false);
		spoofSettingPanel.setVisible(false);
		  
		paramPanel.setVisible(true);
	}
	
	private void showReceiveEventSetting (int index) {
		
		if (eventMsgPassingList.size() > index) {  //update components by reading from msgpassinglist
			
			EventMsgPassing evtmsgObj = (EventMsgPassing)eventMsgPassingList.get(index);
			
			if (evtmsgObj.recvType == 0) {
				reqButton.setSelected(true);
				respButton.setSelected(false);
			}
			else if (evtmsgObj.recvType == 1) {
				respButton.setSelected(true);
				reqButton.setSelected(false);
			}
			
			if (evtmsgObj.recvOptional == false) {
				mandButton.setSelected(true);
				optButton.setSelected(false);
			}
			else if (evtmsgObj.recvOptional == true) {
				optButton.setSelected(true);
				mandButton.setSelected(false);
			}
			
			rcvValField.setText(evtmsgObj.recvValue);
			 
		} 
		//hide send and pause panels, show receive panel
		paramPanel.setVisible(false);
	 	tplFilePanel.setVisible(false);
		fuzzSettingPanel.setVisible(false);
		spoofSettingPanel.setVisible(false);
		  
		
		rcvTypePanel.setVisible(true);
	 	rcvOptPanel.setVisible(true);
	 	rcvValuePanel.setVisible(true);
	}

	private void showSendEventSetting (int index) {
		
		if (eventMsgPassingList.size() > index) {  //update components by reading from msgpassinglist
			
			EventMsgPassing evtmsgObj = (EventMsgPassing)eventMsgPassingList.get(index);
			
			tplFilePathNameField.setText(evtmsgObj.sendTemplateFile);
			 
		} 
		//hide pause and receive panels, show send panel
		rcvTypePanel.setVisible(false);
	 	rcvOptPanel.setVisible(false);
	 	rcvValuePanel.setVisible(false);
	 	paramPanel.setVisible(false);
		
	 	tplFilePanel.setVisible(true);
		fuzzSettingPanel.setVisible(true);
		spoofSettingPanel.setVisible(true);
		
    }
	
	public void printEventMsgObjects() {
		
		System.out.println("EventMsgPassingList Size: "+eventMsgPassingList.size());
		for (int i = 0; i < eventMsgPassingList.size(); i++) {
			EventMsgPassing evtObj = (EventMsgPassing)eventMsgPassingList.get(i);
 	        System.out.println("******************Event: "+i);
 	        if (evtObj.eventType == 0) {
 	        	System.out.println("Event: Send");
 	        	System.out.println("Tpl file: "+evtObj.sendTemplateFile);
 	        	System.out.println("Spoof file: "+evtObj.spoofCSVFile);
 	        	
 	        }
 	        if (evtObj.eventType == 1) {
	        	System.out.println("Event: Receive");
	        	System.out.println("Optional: "+evtObj.recvOptional);
	        	String recvtype = (evtObj.recvType < 1) ? "Request" :"Response";
	        	System.out.println("Recv Type: "+recvtype);
	        	System.out.println("Recv Value: "+evtObj.recvValue);
	        	
	        }
 	        if (evtObj.eventType == 2) {
	        	System.out.println("Event: Pause");
	        	System.out.println("Pause Time: "+evtObj.pauseTime+" ms");
	        	
	        }
  	    }
	}
	
}

class EventNextAction implements ActionListener {
	
	EventSetupFrame parentFrame;
	SSTFthread ownerThrd;
	
	public EventNextAction (EventSetupFrame pFrame, SSTFthread ownerThread) {
	   	parentFrame = pFrame;
	   	ownerThrd = ownerThread;
	}
	
	public void actionPerformed (ActionEvent event) {
		
		//most recent selected event setting would have not been updated, update them now
		int lastEventIndex = parentFrame.curSelectedIndex;
		if (parentFrame.eventList.size() > 0) {
			String lastEvent = (String)parentFrame.eventList.get(lastEventIndex);
			parentFrame.updateEventSettings (lastEvent, lastEventIndex);
		}
		if (parentFrame.eventMsgPassingList.size() != parentFrame.eventList.size()) {
			JOptionPane.showConfirmDialog(parentFrame, "Not all events have been setup!!!!", "InComplete Event Setup Notification", JOptionPane.PLAIN_MESSAGE);
			System.out.println("Not all events have been setup!!!!");
			return;
		}
	
		parentFrame.printEventMsgObjects();
		 
	    //return if empty event list object 
		if (parentFrame.eventMsgPassingList.size() < 1 ) {
			JOptionPane.showConfirmDialog(parentFrame, "Event List Empty!!!!", "Empty Events Notification", JOptionPane.PLAIN_MESSAGE);
			System.out.println("Event List Empty!!!!");
			return; // UNCOMMENT LATER - Testing
		}
		
		String tstName = parentFrame.testNameField.getText();
		if (tstName.equalsIgnoreCase("") == true)
			tstName = "test";
	
    		//  UNCOMMENT LATER - Testing
		//need to call Strezz.java,
		Strezz strzObj = new Strezz (parentFrame.eventMsgPassingList, parentFrame.fuzzKeywordFileMap, tstName);
		//this returns an Arraylist of strings, 1st - xmlscenario filename
		// Then one csv filename per send event, and one spoof csv filename per send event
		ArrayList strezzResults = strzObj.getResults();
		
		
		//Form the SIPp command line with the above filenames and pass that to the Testsetup frame
		String sippCmd = new String();
		//OS Check
		
		String osSTR = System.getProperty("os.name").toLowerCase();
		
		if (osSTR.contains("windows")) {
			sippCmd = StrezzHeader.SIPP_DIR+"sipp-w32";
		} else { //Unix based
			sippCmd = StrezzHeader.SIPP_DIR+"sipp";
		}
		
		    	
		for (int i = 0; i < strezzResults.size(); i++) {
			
			System.out.println ((String)strezzResults.get(i));
			
			if (i == 0)
			   sippCmd = sippCmd+" -sf "+(String)strezzResults.get(i); //scenario file
			else
			   sippCmd = sippCmd+" -inf "+(String)strezzResults.get(i); //injection files
		}
		
		  //Testing
		
		//String sippCmd = StrezzHeader.SIPP_DIR+"sipp-w32 -sn uas";
		//String sippCmd = StrezzHeader.SIPP_DIR+"sipp -sn uac 127.0.0.1 -m 2";
		System.out.println ("SIPP cmd before Test setup: "+sippCmd);
		
		
		parentFrame.dispose();
		
		TestSetupFrame eventFrame = new TestSetupFrame(sippCmd, ownerThrd);
		eventFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		eventFrame.setVisible(true);
				
	}
	
}

class EventAbortAction implements ActionListener {
	
	EventSetupFrame parentFrame;
	SSTFthread ownerThrd;
	
	public EventAbortAction (EventSetupFrame pFrame, SSTFthread ownerThread) {
	   	parentFrame = pFrame;
	   	ownerThrd = ownerThread;
	}
	
	public void actionPerformed (ActionEvent event) {
		
		
		parentFrame.dispose();
		ownerThrd.stopThread();
		//relaunch the starting thread
		SSTFthread newStartThread = new SSTFthread();
		newStartThread.start();
		
	}

}

class EventSaveAction implements ActionListener {
	
	EventSetupFrame parentFrame;
		
	public EventSaveAction (EventSetupFrame pFrame) {
	   	
		parentFrame = pFrame;
	
	}
	
	public void actionPerformed (ActionEvent event) {
		
		
		//save (serialize) the eventMsgList Objects
		/* 1) write objects to savedTestData
		 * 2) write file containing object data file names to saveTestSettings
		 */
		
		Calendar cal = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat(StrezzHeader.DATE_FORMAT_NOW);
        String dateStr = sdf.format(cal.getTime());
        
		//String saveDataNamePrefix = StrezzHeader.SAVED_DATA_DIR+dateStr+"_"+parentFrame.testNameField.getText()+"_";
		//String saveSettingName = StrezzHeader.SAVED_SETTINGS_DIR+dateStr+"_"+parentFrame.testNameField.getText()+".set";
        
        String saveDataNamePrefix = StrezzHeader.SAVED_DATA_DIR+parentFrame.testNameField.getText()+"_"+dateStr+"_";
		String saveSettingName = StrezzHeader.SAVED_SETTINGS_DIR+parentFrame.testNameField.getText()+"_"+dateStr+".set";
		
		ArrayList dataFnameList = new ArrayList();

		//1) write object data files
		for (int i = 0; i < parentFrame.eventMsgPassingList.size(); i++) {
			
			EventMsgPassing obj = (EventMsgPassing)parentFrame.eventMsgPassingList.get(i);
			String fname = saveDataNamePrefix+Integer.toString(i)+".data";
			dataFnameList.add(fname);
			try {
				FileOutputStream f_out = new FileOutputStream (fname);

			// Use an ObjectOutputStream to send object data to the
			// FileOutputStream for writing to disk.
				ObjectOutputStream obj_out = new ObjectOutputStream (f_out);

			//Pass our object to the ObjectOutputStream's
			// writeObject() method to cause it to be written out
			// to disk.
				obj_out.writeObject (obj);
				
				obj_out.flush();
				obj_out.close();
				
			} catch (IOException e) {
				
				 System.out.println ( "Saving Test Data file IO Exception =: " + e );
			     e.printStackTrace();
			}
			
			
		}
		
		//2) write settings file
		try {
		    
		    FileWriter fstream = new FileWriter(saveSettingName);
		    BufferedWriter out = new BufferedWriter(fstream);
		    
		    
		    for (int i = 0; i < dataFnameList.size(); i++) {
		    	String str = (String)dataFnameList.get(i);
		    	out.write(str+"\n");
		    }
		    
		    out.close();
		} catch (IOException e){
			
			System.out.println ( "Saving Test Settings file IO Exception =: " + e );
		    e.printStackTrace();
		}
		    
		//confirmation dialog    
		JOptionPane.showConfirmDialog(parentFrame, "Your Settings Have Been Saved To "+StrezzHeader.SAVED_SETTINGS_DIR, "Save Event Settings", JOptionPane.PLAIN_MESSAGE);
		
	}

}


/*Fourth Frame - Allows for setting up other aspects (components) of SSTF, like performance, 
 *               other SIP, logging and external module settings
 * 
 */
class TestSetupFrame extends JFrame {
	
	int curSelectedIndex = 0; //first item by default
	
	String sippCommand = new String();
	
    JPanel middleRightPanel = new JPanel();
    
    //SIP params
    JPanel sipPanel = new JPanel();
    JPanel userPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel userLabel = new JLabel("Remote Username:**"); //-s
    JTextField userText = new JTextField("", 15);
    
    JPanel localIpPortPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel localIpPortLabel = new JLabel("Local IP and Port:"); //-i and -p
    JTextField localIpText = new JTextField("", 15);
    JTextField localPortText = new JTextField("", 5); 
	 
    JPanel remoteIpPortPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel remoteIpPortLabel = new JLabel("Remote IP and Port[ex- ipaddr:port]:**"); //remoteip:port
    JTextField remoteIpPortText = new JTextField("", 21);
    
    JPanel remoteSendAddrPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel remoteSendAddrLabel = new JLabel("Remote Sending Address[ex- host:port]:"); //-rsa
    JTextField remoteSendAddrText = new JTextField("", 21);
	
    JPanel transportModePanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel transportModeLabel = new JLabel("Transport Mode: TCP/UDP[ex- u1, un, t1, tn]:"); //-t
    JTextField transportModeText = new JTextField("", 5);
    
    JPanel lostCountPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel lostCountLabel = new JLabel("Lost Packets Count:"); //-lost
    JTextField lostCountText = new JTextField("", 5);
    
    
    //Performance params
    JPanel perfPanel = new JPanel();
    JPanel callRatePanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel callRateLabel = new JLabel("Call Rate:"); //-r
    JTextField callRateText = new JTextField("", 5);
    
    JPanel callRatePeriodPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel callRatePeriodLabel = new JLabel("Call Rate Period:"); //-rp
    JTextField callRatePeriodText = new JTextField("", 5);
    
    JPanel callDurationPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel callDurationLabel = new JLabel("Call Duration:"); //-d
    JTextField callDurationText = new JTextField("", 5);
    
    JPanel callMaxLimitPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel callMaxLimitLabel = new JLabel("Maximum Call Limit:"); //-m
    JTextField callMaxLimitText = new JTextField("", 5);
    
    JPanel maxSimulCallPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
    JLabel maxSimulCallLabel = new JLabel("Maximum Simultaneous Calls:"); //-l
    JTextField maxSimulCallText = new JTextField("", 5);
    
    
    //LOG params
    JPanel logPanel = new JPanel();
    JCheckBox traceMsgChkBox = new JCheckBox("Trace Message");
    JCheckBox traceErrChkBox = new JCheckBox("Trace Error");
    JCheckBox traceStatChkBox = new JCheckBox("Trace Stats");
    JCheckBox traceScreenChkBox = new JCheckBox("Trace Screen");
    
    // External Params
    JPanel externalPanel = new JPanel();
    JLabel extModuleLabel = new JLabel("External Module Settings....LATER"); 
	 
    public static void showHelpWindow(String helpNote) {
	    
    	final JFrame helpFrame = new JFrame();
		
		helpFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);		    
		helpFrame.setTitle("SSTF setup wizard - Help");

        JPanel basic = new JPanel();
        basic.setLayout(new BoxLayout(basic, BoxLayout.Y_AXIS));
        helpFrame.add(basic);

        JPanel topPanel = new JPanel(new BorderLayout(0, 0));
        topPanel.setMaximumSize(new Dimension(800, 0));
      
        
        JLabel intro = new JLabel(helpNote);
        intro.setPreferredSize (new Dimension(600, 100));
        intro.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
        topPanel.add(intro);
        
        
        JSeparator separator = new JSeparator();
        separator.setForeground(Color.gray);

        topPanel.add(separator, BorderLayout.SOUTH);

        basic.add(topPanel);

        JPanel textPanel = new JPanel(new BorderLayout());
        textPanel.setBorder(BorderFactory.createEmptyBorder(15, 25, 15, 25));
        
        JTextArea txtArea = new JTextArea();
        JScrollPane paneScrollPane = new JScrollPane(txtArea);
        paneScrollPane.setVerticalScrollBarPolicy(
                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        
        String areaText = new String();
        
        //read help file into a string
        try {
        	StringBuffer contents = new StringBuffer();
        	File inFile = new File ("sstfHelp.txt");
			BufferedReader input =  new BufferedReader(new FileReader(inFile));
			
			if (inFile == null) {
				System.out.println ("Error: Opening Help File ");
			}
			String line = null;			
			while (( line = input.readLine()) != null){
		          contents.append(line);
		          contents.append(System.getProperty("line.separator"));
		    }
			
			areaText = contents.toString();
			
	        //Close the input stream
	        input.close ();
	        
	    } catch  (IOException e) {
	        System.out.println ( "Help file IO Exception =: " + e );
	        e.printStackTrace();
	        areaText = "Could not find Data!!!";
	    } //stream exception
	    
        
	    txtArea.setText(areaText);
	    txtArea.setEditable(false);
        
        textPanel.add(paneScrollPane, BorderLayout.CENTER);
       

        basic.add(textPanel);
     
        
        JPanel bottom = new JPanel(new FlowLayout(FlowLayout.RIGHT));


		
        JButton okButton = new JButton("OK");
        okButton.setMnemonic(KeyEvent.VK_K);
        okButton.addActionListener(new
      		  ActionListener() {
      	  	
      	  		public void actionPerformed (ActionEvent evt) {
      	  		  //close the help window
      	  		  helpFrame.dispose();
      	  		}
      	  
        });
        
        
        bottom.add(okButton);
        
        basic.add(bottom);

        bottom.setMaximumSize(new Dimension(800, 0));

        helpFrame.setSize(new Dimension(800, 600));
        helpFrame.setResizable(false);
        helpFrame.setLocationRelativeTo(null);
        
        helpFrame.setVisible(true);
        
    }
 
	public TestSetupFrame (String sippCmd, SSTFthread ownerThread) {
		
	  addWindowListener(new SSTFframeListener(ownerThread));
		
      sippCommand = sippCmd;
		
      String[] testSettings = {"SIP","PERFORMANCE","LOGGING","EXTERNAL"};
	     
      //create the main window frame components  	  
  	  setTitle("SSTF setup wizard - Test Setup");

      JPanel basic = new JPanel();
      basic.setLayout(new BoxLayout(basic, BoxLayout.Y_AXIS));
      add(basic);

      JPanel topPanel = new JPanel(new BorderLayout(0, 0));
      topPanel.setMaximumSize(new Dimension(800, 0));
      JLabel intro = new JLabel("Choose a Test setting on the Left and configure it on the right. [Fields marked with ** are Mandatory]");
      intro.setPreferredSize (new Dimension(600, 50));
      intro.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
      
      JButton helpButton = new JButton("HELP");
      helpButton.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
      topPanel.add(helpButton, BorderLayout.EAST);
      
      helpButton.addActionListener(new
    		  ActionListener() {
    	  	
    	  		public void actionPerformed (ActionEvent evt) {
    	  		  //show the help window
    	  			String note = "NOTE: SIP settings in Test Setup will not override corresponding fuzzed fields";
    	  			showHelpWindow(note);
    	  		}
    	  
      });
      
      topPanel.add(intro, BorderLayout.NORTH);
      
      JSeparator separator = new JSeparator();
      separator.setForeground(Color.gray);

      topPanel.add(separator, BorderLayout.SOUTH);

      basic.add(topPanel);

             
      JPanel middlePanel = new JPanel();
      middlePanel.setLayout(new BoxLayout(middlePanel, BoxLayout.X_AXIS));
      middlePanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
      basic.add(middlePanel);
      
      //add two panels to the middle panel
      JPanel middleLeftPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
      middleLeftPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
      middleLeftPanel.setPreferredSize(new Dimension(110, 475));
      middlePanel.add(middleLeftPanel);
      
      middleRightPanel.setLayout(new BoxLayout(middleRightPanel, BoxLayout.Y_AXIS));
      middleRightPanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
      middleRightPanel.setPreferredSize(new Dimension(480, 475));
      middlePanel.add(middleRightPanel);
      
      //add a combo box to the middle left panel
      final JComboBox setupEventCombo = new JComboBox(testSettings);
      middleLeftPanel.add(setupEventCombo);
      //add actionlistener for combo, which will store selected items index
      //in curSelectedIndex
      setupEventCombo.setSelectedIndex(-1);
      setupEventCombo.addActionListener(new
    		  ActionListener() {
    	  	
    	  		public void actionPerformed (ActionEvent evt) {
    	  			
    	  					
    	  			curSelectedIndex = setupEventCombo.getSelectedIndex();
    	  			
    	  			System.out.println("Selected index is: "+curSelectedIndex);
    	  			
    	  			String evtName = (String)setupEventCombo.getSelectedItem();
    	  			  			
    	  			if (evtName.equalsIgnoreCase(new String("sip"))) {
    	  				System.out.println("Selected event is: "+evtName);
    	  				showSIPTestSetting ();
    	  			} else if (evtName.equalsIgnoreCase(new String("performance"))) {
    	  				System.out.println("Selected event is: "+evtName);
    	  			 	showPerfTestSetting ();		
    	  			} else if (evtName.equalsIgnoreCase(new String("logging"))) {
    	  				System.out.println("Selected event is: "+evtName);
    	  			   	showLogSetting ();
    	  			} else if (evtName.equalsIgnoreCase(new String("external"))) {
    	  				System.out.println("Selected event is: "+evtName);
    	  			   	showExternalSetting ();
    	  			}

    	  		}
    	  
      });
      
      
      
      //add action listeners for the abort and next buttons
      TestNextAction nxtAction = new TestNextAction(this, ownerThread);
      TestAbortAction abrtAction = new TestAbortAction(this, ownerThread);
      
      
      JPanel bottom = new JPanel(new FlowLayout(FlowLayout.RIGHT));

      JButton abortButton = new JButton("ABORT");
      abortButton.setMnemonic(KeyEvent.VK_T);
      abortButton.addActionListener(abrtAction);
      JButton nextButton = new JButton("NEXT");
      nextButton.setMnemonic(KeyEvent.VK_N);
      nextButton.addActionListener(nxtAction);
      
      bottom.add(abortButton);
      bottom.add(nextButton);
      basic.add(bottom);

      bottom.setMaximumSize(new Dimension(800, 0));

      setSize(new Dimension(800, 600));
      setResizable(false);
      setLocationRelativeTo(null);
      
      //Add sip param components and hide them
      sipPanel.setLayout(new BoxLayout(sipPanel, BoxLayout.Y_AXIS));
      middleRightPanel.add(sipPanel);
      
      sipPanel.add(userPanel);
      userPanel.add(userLabel);
      userPanel.add(userText);
         
      sipPanel.add(remoteIpPortPanel);
      remoteIpPortPanel.add(remoteIpPortLabel);
      remoteIpPortPanel.add(remoteIpPortText);
      
      sipPanel.add(localIpPortPanel);
      localIpPortPanel.add(localIpPortLabel);
      localIpPortPanel.add(localIpText);
      localIpPortPanel.add(localPortText);
                      
      sipPanel.add(remoteSendAddrPanel);
      remoteSendAddrPanel.add(remoteSendAddrLabel);
      remoteSendAddrPanel.add(remoteSendAddrText);
            
      sipPanel.add(transportModePanel);
      transportModePanel.add(transportModeLabel);
      transportModePanel.add(transportModeText);
            
      sipPanel.add(lostCountPanel);
      lostCountPanel.add(lostCountLabel);
      lostCountPanel.add(lostCountText);
      
      sipPanel.setVisible(false);
      
      //performance params
      perfPanel.setLayout(new BoxLayout(perfPanel, BoxLayout.Y_AXIS));
      middleRightPanel.add(perfPanel);
      
      perfPanel.add(callRatePanel);
      callRatePanel.add(callRateLabel);
      callRatePanel.add(callRateText);
           
      perfPanel.add(callRatePeriodPanel);
      callRatePeriodPanel.add(callRatePeriodLabel);
      callRatePeriodPanel.add(callRatePeriodText);
            
      perfPanel.add(callDurationPanel);
      callDurationPanel.add(callDurationLabel);
      callDurationPanel.add(callDurationText);
            
      perfPanel.add(callMaxLimitPanel);
      callMaxLimitPanel.add(callMaxLimitLabel);
      callMaxLimitPanel.add(callMaxLimitText);
            
      perfPanel.add(maxSimulCallPanel);
      maxSimulCallPanel.add(maxSimulCallLabel);
      maxSimulCallPanel.add(maxSimulCallText);
      
      perfPanel.setVisible(false);
      
      //Log Params
      logPanel.setLayout(new BoxLayout(logPanel, BoxLayout.Y_AXIS));
      middleRightPanel.add(logPanel);
      logPanel.add(traceMsgChkBox);
      logPanel.add(traceErrChkBox);
      logPanel.add(traceStatChkBox);
      logPanel.add(traceScreenChkBox);
      logPanel.setVisible(false);
      
      //External Params
      externalPanel.setLayout(new BoxLayout(externalPanel, BoxLayout.Y_AXIS));
      middleRightPanel.add(externalPanel);
      externalPanel.add(extModuleLabel);
      externalPanel.setVisible(false);

	}
	
	
	/*These functions (showXXXSetting()) show the event configuration settings on the right middle panel
	 * Hide other panels, show relevant panels only
	 */
	private void showSIPTestSetting () {
		//hide other panels, show sip settings panel
		perfPanel.setVisible(false);
		sipPanel.setVisible(true);
		logPanel.setVisible(false);
		externalPanel.setVisible(false);
				
	}
	
	private void showPerfTestSetting () {
		//hide other panels, show performance settings panel
		perfPanel.setVisible(true);
		sipPanel.setVisible(false);
		logPanel.setVisible(false);
		externalPanel.setVisible(false);
	}

	private void showLogSetting () {
		//hide other panels, show log settings panel		
		perfPanel.setVisible(false);
		sipPanel.setVisible(false);
		logPanel.setVisible(true);
		externalPanel.setVisible(false);
    }
	
	private void showExternalSetting () {
		//hide other panels, show external module settings panel
		perfPanel.setVisible(false);
		sipPanel.setVisible(false);
		logPanel.setVisible(false);
		externalPanel.setVisible(true);
		
    }

	public void processAllSettings () {
		
		//read all the settings (text feild and other components values) and
		//generate appropriate sipp options and append to the sipp cmd line
		System.out.println("In Testsetup: initial sipp cmd is: "+sippCommand);
		
		//SIP settings
		if (userText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -s "+userText.getText();
		    
		if (localIpText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -i "+localIpText.getText();
		    
		if (localPortText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -p "+localPortText.getText();
				    
		if (remoteSendAddrText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -rsa "+remoteSendAddrText.getText();
		    
		if (transportModeText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -t "+transportModeText.getText();
		     
		if (lostCountText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -lost "+lostCountText.getText();
		
		if (remoteIpPortText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" "+remoteIpPortText.getText();
		    
		
		//performance settings
		if (callRateText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -r "+callRateText.getText();
		if (callRatePeriodText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -rp "+callRatePeriodText.getText();
		if (callDurationText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -d "+callDurationText.getText();
		if (callMaxLimitText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -m "+callMaxLimitText.getText();
		if (maxSimulCallText.getText().equalsIgnoreCase("") == false)
			sippCommand = sippCommand+" -l "+maxSimulCallText.getText();
		
		//log settings
		if (traceMsgChkBox.isSelected())
			sippCommand = sippCommand+" -trace_msg ";
		if (traceErrChkBox.isSelected())
			sippCommand = sippCommand+" -trace_err ";
		if (traceStatChkBox.isSelected())
			sippCommand = sippCommand+" -trace_stat ";
		if (traceScreenChkBox.isSelected())
			sippCommand = sippCommand+" -trace_screen ";
		
		//external settings
		
		
	}
	
}

class TestNextAction implements ActionListener {
	
	TestSetupFrame parentFrame;
	SSTFthread ownerThrd;
	
	public TestNextAction (TestSetupFrame pFrame, SSTFthread ownerThread) {
	   	parentFrame = pFrame;
	   	ownerThrd = ownerThread;
	}
	
	public void actionPerformed (ActionEvent event) {
		

		if ((parentFrame.userText.getText().equalsIgnoreCase("")) || (parentFrame.remoteIpPortText.getText().equalsIgnoreCase(""))) {
			
			JOptionPane.showConfirmDialog(parentFrame, "Remote User Name or Remote IP:Port Cannot be Empty!!", "Mandatory Settings Notification", JOptionPane.PLAIN_MESSAGE);
			return;
		}

		parentFrame.dispose();

		//process all settings
		parentFrame.processAllSettings();
		System.out.println("In Testsetup: Final sipp cmd is: "+parentFrame.sippCommand);
		
		RunTestFrame eventFrame = new RunTestFrame(parentFrame.sippCommand, ownerThrd);
		eventFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		eventFrame.setVisible(true);
				
	}
	
}

class TestAbortAction implements ActionListener {
	
	TestSetupFrame parentFrame;
	SSTFthread ownerThrd;
	
	public TestAbortAction (TestSetupFrame pFrame,  SSTFthread ownerThread) {
	   	parentFrame = pFrame;
	   	ownerThrd = ownerThread;
	}
	
	public void actionPerformed (ActionEvent event) {
		
		
		parentFrame.dispose();
		ownerThrd.stopThread();
		//relaunch the starting thread
		SSTFthread newStartThread = new SSTFthread();
		newStartThread.start();

	}

}


/*Fifth/Last Frame - Displays the SIPp run window. Allows for RUNing and STOPping the test case
 *                   Is also the exit point for the wizard. 
 * 
 */
class RunTestFrame extends JFrame {
	
	String runCommand = new String ();
	
	JTextArea outputArea = new JTextArea();
	
	Process runProc = null;
	
	RunCommand cmdObj = new RunCommand();
	
	public RunTestFrame (String runCmd, final SSTFthread ownerThread) {
	
		  addWindowListener(new SSTFframeListener(ownerThread));
		
		  runCommand = runCmd;
		  
		  //extract scenario name and display
		  StringTokenizer st1 = new StringTokenizer(runCommand," ");
		  //get third token
		  String temp1Name = st1.nextToken(); 
		  temp1Name = st1.nextToken();
		  temp1Name = st1.nextToken();
		  
		  //get last token from that
		  StringTokenizer st2 = new StringTokenizer(temp1Name, StrezzHeader.slash);
		  String scenarioName = new String();
		  while (st2.hasMoreTokens()) {
			  scenarioName = st2.nextToken();
		  }
		  
	      //create the main window frame components  	  
	  	  setTitle("SSTF setup wizard - Run");

	      JPanel basic = new JPanel();
	      basic.setLayout(new BoxLayout(basic, BoxLayout.Y_AXIS));
	      add(basic);

	      JPanel topPanel = new JPanel(new BorderLayout(0, 0));
	      topPanel.setMaximumSize(new Dimension(800, 0));
	      JLabel intro = new JLabel("Click RUN to execute Test Case. STOP will forcefully terimate the test. EXIT will close this instance of the wizard. ");
	      intro.setPreferredSize (new Dimension(600, 50));
	      intro.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
	      
	      topPanel.add(intro, BorderLayout.NORTH);
	      
	      JLabel testName = new JLabel("\""+scenarioName.toUpperCase()+"\"", SwingConstants.CENTER);
	      testName.setForeground(Color.BLUE);
	      
	      testName.setPreferredSize (new Dimension(600, 50));
	      testName.setBorder(BorderFactory.createEmptyBorder(0, 25, 0, 0));
	      
	      topPanel.add(testName, BorderLayout.CENTER);
	      
	      JSeparator separator = new JSeparator();
	      separator.setForeground(Color.gray);

	      topPanel.add(separator, BorderLayout.SOUTH);

	      basic.add(topPanel);

	      JPanel textPanel = new JPanel(new BorderLayout());
	      textPanel.setBorder(BorderFactory.createEmptyBorder(15, 25, 15, 25));
	      

	      
	      //String text = "SIPp Output will go here..." ;
	      //outputArea.setText(text);
	      //textPanel.add(outputArea);
	      
	      JScrollPane paneScrollPane = new JScrollPane(outputArea);
	            
	      outputArea.setEditable(false);
	      	      
	      textPanel.add(paneScrollPane);

	      basic.add(textPanel);
	      
	      //add action listeners for the stop and run buttons
	      RunTestAction runAction = new RunTestAction(this);
	      StopTestAction stopAction = new StopTestAction(this);
	      RunExitAction exitAction = new RunExitAction(this, ownerThread);
	      
	      JPanel middlePanel = new JPanel();
	      middlePanel.setLayout(new BoxLayout(middlePanel, BoxLayout.X_AXIS));
	      middlePanel.setBorder(BorderFactory.createLineBorder(Color.BLACK));
	      basic.add(middlePanel);
	      	      
	      JPanel bottom = new JPanel(new FlowLayout(FlowLayout.RIGHT));
          
	      
		  JButton newTestButton = new JButton("ANOTHER TEST INSTANCE");
		  newTestButton.setMnemonic(KeyEvent.VK_N);
		  newTestButton.addActionListener(new
		    		  ActionListener() {
		    	  	
	  	  		public void actionPerformed (ActionEvent evt) {
	  	  			
	  	  			//start new instance of the application
  	  	  		    //relaunch the starting thread
	  	  		    SSTFthread newStartThread = new SSTFthread();
	  	  		    newStartThread.start();
	  	  			

	  	  		}
	  	  
		  });

		  bottom.add(newTestButton);
		  
		  
	      JButton exitButton = new JButton("EXIT");
	      exitButton.setMnemonic(KeyEvent.VK_X);
	      exitButton.addActionListener(exitAction);
	      
	      JButton stopButton = new JButton("STOP");
	      stopButton.setMnemonic(KeyEvent.VK_T);
	      stopButton.addActionListener(stopAction);
	      
	      JButton runButton = new JButton("RUN");
	      runButton.setMnemonic(KeyEvent.VK_R);
	      runButton.addActionListener(runAction);
	      
	      
	      bottom.add(exitButton);
	      bottom.add(stopButton);
	      bottom.add(runButton);
	      
	      basic.add(bottom);

	      bottom.setMaximumSize(new Dimension(800, 0));

	      setSize(new Dimension(800, 600));
	      setResizable(false);
	      setLocationRelativeTo(null);
	      
	    
		}

}

class RunExitAction implements ActionListener {
	
	RunTestFrame parentFrame;
	SSTFthread ownerThrd;
	
	public RunExitAction (RunTestFrame pFrame,  SSTFthread ownerThread) {
	   	parentFrame = pFrame;
	   	ownerThrd = ownerThread;
	}
	
	public void actionPerformed (ActionEvent event) {
		
		parentFrame.dispose();
		ownerThrd.stopThread();
		
	}

}

class RunTestAction implements ActionListener {
	
	RunTestFrame parentFrame;
	
	public RunTestAction (RunTestFrame pFrame) {
	   	parentFrame = pFrame;
	   	
	}
	
	public void actionPerformed (ActionEvent event) {
		
		//send interrupt to sipp to RUN	
		parentFrame.cmdObj = new RunCommand ();
		
	  	//run command and output	  	
		parentFrame.cmdObj.runProcCommand(parentFrame.runCommand, parentFrame);
 	  			
	}
	
}

class StopTestAction implements ActionListener {
	
	RunTestFrame parentFrame;
	
	public StopTestAction (RunTestFrame pFrame) {
	   	parentFrame = pFrame;
	}
	
	public void actionPerformed (ActionEvent event) {
		
		//send interrupt to sipp to STOP
		if (parentFrame.cmdObj.p != null)
			parentFrame.cmdObj.p.destroy();
		
		
	}

}

/*This class is used to the pass the event setup information to the Strezz class which actually
 * builds the test case (xml scenario file)
 */
class EventMsgPassing implements java.io.Serializable {
	
	short eventType; //0-send, 1-recv, 2-pause
	int pauseTime; //in milliseconds
	short recvType; //0-request, 1-response
	boolean recvOptional; //if recv message is optional or mandatory
	String recvValue; //eg: ACK for request, 200 for response
	String sendTemplateFile; //file name of the send message's template (.tpl file)
	
	/*NOT USED NOW*/
	String spoofCSVFile; //file name of the csv file to be used with user or ip spoofing
                         //Name format: <testcase name>_spoof_<index>.csv
	                     //index - gives the index of send message position in the event list
	
	public EventMsgPassing () {
	
	  //dummy
		
	}
	
	
	public EventMsgPassing (short type, int msecs) {
		
		try {
			if (type == 2) {
				eventType = 2;
				pauseTime = msecs;
			}
			else {
				
					throw new Exception();
			}
		} catch (Exception e) {
			System.out.println ( "Event Type should be 2 (Pause)");
			e.printStackTrace();
		}
	}
	
    public EventMsgPassing (short type, short rcvtype, boolean opt, String rcvVal) {
		
		try {
			if (type == 1) {
				eventType = 1;
				recvType = rcvtype;
				recvValue = rcvVal;
				recvOptional = opt;
			}
			else {
				
				  throw new Exception();
			}
		} catch (Exception e) {
			System.out.println ( "Event Type should be 1 (Recv)");
			e.printStackTrace();
		}
	}
	
    public EventMsgPassing (short type, String sendtplFile, String spfcsvFile) {
		
		try {
			if (type == 0) {
				eventType = 0;
				sendTemplateFile = sendtplFile;
				spoofCSVFile = spfcsvFile;
			}
			else {
				
					throw new Exception();
			}
		} catch (Exception e) {
			System.out.println ( "Event Type should be 0 (Send)");
			e.printStackTrace();
		}
	}
	
	
}

/*This class is used to run any Unix/Windows command given as the object argument. A new shell is
 * spawned that runs the user command.
 */

class RunCommand {
	
	public Process p;
	public String osSTR = new String ();
	
	public RunCommand () {
		
		p = null;
		//OS check
		osSTR = System.getProperty("os.name").toLowerCase();
		
	}
	
	//simply execute the command 
	public void runCommand (String cmd) {
		
    	//RUN COMMAND
    	//Process p = null;
    	String s;
    	String [] cmdArr = null;
    	
    	if (osSTR.contains("windows")) {
			//cmdArr = new String [] {cmd};
    		StringTokenizer st1 = new StringTokenizer(cmd, " ");
    		cmdArr = new String [st1.countTokens()];
    		int i = 0;
    		while (st1.hasMoreTokens()) {
    			   cmdArr[i++] = st1.nextToken(); 
    		}
    		
    	}else if (osSTR.contains("linux")) {
    		
    		cmdArr = new String [] {"/bin/sh", "-c", cmd};
    		
    	} else if (osSTR.contains("sunos")) {
    		
    		cmdArr = new String [] {"/usr/bin/sh", "-c", cmd};
    		
    	}
    	
    	try {
    		       		        		        		     
    		     p = Runtime.getRuntime().exec(cmdArr);
    		     
    		     BufferedReader stdInput = new BufferedReader(new 
    	         InputStreamReader(p.getInputStream()));

    	         BufferedReader stdError = new BufferedReader(new 
    	         InputStreamReader(p.getErrorStream()));

    	         // read the output from the command
    	         System.out.println("Here is the standard output of the command:");
    	         while ((s = stdInput.readLine()) != null) {
    	                System.out.println(s);
    	         }
    	            
    	        // read any errors from the attempted command
    	         System.out.println("Standard error of the command (if any):");
    	         while ((s = stdError.readLine()) != null) {
    	                System.out.println(s);
    	         }
    			 
    		} catch (IOException e)  {
    			System.out.println ( "cmd execution exception =: " + e );
    		    e.printStackTrace();
    		}
    
    }
	
	//execute the command and return process handle 
	public void runProcCommand (String cmd, final RunTestFrame parent) {
		
		parent.outputArea.setForeground(Color.BLUE);
		final Highlighter.HighlightPainter redPainter = new DefaultHighlighter.DefaultHighlightPainter( Color.red );
		
		parent.outputArea.getHighlighter().removeAllHighlights();
		
    	//RUN COMMAND
    	
		String [] cmdArr = null;
    	
		if (osSTR.contains("windows")) {
			//cmdArr = new String [] {cmd};
    		StringTokenizer st1 = new StringTokenizer(cmd, " ");
    		cmdArr = new String [st1.countTokens()];
    		int i = 0;
    		while (st1.hasMoreTokens()) {
    			   cmdArr[i++] = st1.nextToken(); 
    		}
    		
    	} else if (osSTR.contains("linux")) {
    		
    		cmdArr = new String [] {"/bin/sh", "-c", cmd};
    		
    	} else if (osSTR.contains("sunos")) {
    		
    		cmdArr = new String [] {"/usr/bin/sh", "-c", cmd};
    		
    	}
    	
    	//String[] cmdArr = new String [] {"/usr/bin/gnome-terminal", "-e", cmd};
    	//String[] cmdArr = new String [] {"/usr/bin/gnome-terminal"};
    	
    	
    	
    	try {
    		       		        		        		     
    		p = Runtime.getRuntime().exec(cmdArr);
    		    		                 			 
    		} catch (IOException e)  {
    			System.out.println ( "cmd execution exception =: " + e );
    		    e.printStackTrace();
    		}
    
    		   		
    		
    		new Thread () {
        		
        		public void run()
               {
        			    		
        			try {
        		  		
        				  				
        		  		BufferedReader stdInput = new BufferedReader(new 
        		  		InputStreamReader(p.getInputStream()));

        		  		BufferedReader stdError = new BufferedReader(new 
        		    	InputStreamReader(p.getErrorStream()));
        		  		
        		  		String s = new String();     
        		  		String accStr = new String();
        		  		
        		  		// read the output from the command
        		  		System.out.println("Here is the standard output of the command:");
        		  		 
        		  		while ((s = stdInput.readLine()) != null) {
        	   	           
        		  			
        	   	        	   accStr =  s + "\n";
        	   	        	   

          	   	        	   if ((accStr == null) || (accStr.equalsIgnoreCase("")) || (accStr.equalsIgnoreCase("\n")))
        	   	        		   continue;
        	   	        	   
        	   	        	   if (accStr.contains("Scenario Screen")) {
        	   	        		  parent.outputArea.setText("");
        	   	        		 
        	   	        	   }
        	   	        		
        	   	        	  // System.out.println(s);       	   	        	
        	   	           	   parent.outputArea.append(accStr);        	   
        	   	          
        	   	        	 
        		  		}
        		  		
        		  		 
        		  		
        		  		 // read any errors from the attempted command
        		         System.out.println("Standard error of the command (if any):");
        		         
        		         while ((s = stdError.readLine()) != null) {
        		        	
        		        	 System.out.println(s);
      		        	   
        		        	 accStr =  s + "\n";
        		        	 
        		        	 try {
        		        		 parent.outputArea.getHighlighter().addHighlight( parent.outputArea.getText().length() - 1, parent.outputArea.getText().length() + accStr.length() - 1, redPainter );
        		     		} catch(BadLocationException ble) {}
        		        	 
        		        	      
        		             parent.outputArea.append(accStr);
        		         }
        		         
        		         
        		         try {
     	                    // wait for spawned program to terminate.
     	    					p.waitFor();
     	                 } catch ( InterruptedException e ) {
     	                    	Thread.currentThread().interrupt();
     	                 }
        	            // child has terminated.
        	           
        	            
        		  	} catch (IOException e)  {
        				System.out.println ( "PROC CMD I/O execution exception =: " + e );
        			    e.printStackTrace();
        			}

              
        			   }
           	}.start();
        	
    	
    }
	
	//try look into the ProcessBuilder Class (java 1.5 and above)
	
	/*
    public void readFromProcess (final Process p, final RunTestFrame parent) {
    	
    					
    	new Thread () {
    		
    		public void run()
           {
    			try {
    		  		
    				  				
    		  		BufferedReader stdInput = new BufferedReader(new 
    		  		InputStreamReader(p.getInputStream()));

    		  		BufferedReader stdError = new BufferedReader(new 
    		    	InputStreamReader(p.getErrorStream()));
    		  		
    		  		String s = new String();     
    		  		String accStr = new String();
    		  		
    		  		// read the output from the command
    		  		System.out.println("Here is the standard output of the command:");
    		  		while ((s = stdInput.readLine()) != null) {
    	   	          // System.out.println(s);
    	   	          
    	   	        	   accStr =  s + "\n";
    	   	        	   if (s.contains("Scenario Screen"))
    	   	        		  parent.outputArea.setText("");
    	   	        	   parent.outputArea.append(accStr);        	   
    	   	          
    		  		}
    		  		//print output on text area
    		  		 
    		  		
    		  		 // read any errors from the attempted command
    		         System.out.println("Standard error of the command (if any):");
    		         while ((s = stdError.readLine()) != null) {
    		                System.out.println(s);
    		                accStr =  s + "\n";
    		                parent.outputArea.append(accStr);
    		                
    		         }
    	   	        
    		         
    		         try {
 	                    // wait for spawned program to terminate.
 	    					p.waitFor();
 	                 } catch ( InterruptedException e ) {
 	                    	Thread.currentThread().interrupt();
 	                 }
    	            // child has terminated.
    	           
    	            
    		  	} catch (IOException e)  {
    				System.out.println ( "PROC CMD I/O execution exception =: " + e );
    			    e.printStackTrace();
    			}

            // returning from run kills the thread.
            }
    	}.start();
    	
    	
    }
    
    */
	
	/*
    public void writeToProcess (final Process p, final String cmd) {
    	
    	
    			try {
	  		
    			//	BufferedWriter stdOutput = new BufferedWriter(new 
    			//	OutputStreamWriter(p.getOutputStream()));
    			//	stdOutput.write(cmd);

               	    		
    				//OutputStream out = p.getOutputStream();
     			    
     		        //out.write(cmd.getBytes());
     		        
    				PrintWriter pw = new PrintWriter(p.getOutputStream());
    				pw.println(cmd);
    				pw.flush();
    				
    				
    			
                
                
    			} catch (Exception e)  {
    				System.out.println ( "PROC CMD I/O execution exception =: " + e );
    				e.printStackTrace();
    			}
			
    		
    	
    }
	
	*/
    
}
