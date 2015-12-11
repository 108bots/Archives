/*
Topology Designer - GUI class
Author: Hemanth Srinivasan
Year: 2006
*/
package topology_design_tool;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;


public class Topology_Designer_gui {
	static int flag = 0;
	static TopoFrame frame;
	static TopoPanel panel;
	
	 public static void main (String[] args) {
	     	
	    frame = new TopoFrame();
	    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	      
	    panel = new TopoPanel();
  		Container contentPane = frame.getContentPane();
  		contentPane.add(panel);//, BorderLayout.WEST);
  		panel.drawCanvas();
  		
	    frame.show();
         		
	    	
	 }
	      
	
	
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
class TopoFrame extends JFrame {
  
  public static final int DEFAULT_WIDTH = 1400;
  public static final int DEFAULT_HEIGHT = 1000;
 // static int MAX_x = 700;
 // static int MAX_y = 500;
  
  public TopoFrame() {
	//get screen dimensions
	Toolkit kit = Toolkit.getDefaultToolkit();
	Dimension screenSize = kit.getScreenSize();
	
    int screenHeight = screenSize.height;
	int screenWidth = screenSize.width;
		  
	  
	setTitle("Network Topology Designer");
	setSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	

   

	
  }
  
		
}

class CanvasPane extends JPanel

{
	public Graphics2D g2;
	
	public void paintComponent(Graphics g) {
	  Node temp, temp1;	
	  super.paintComponent(g);
	  g2 = (Graphics2D)g;
    
	  if(Topology_Designer_gui.flag == 4) {
		 g2.setColor(Color.white);
		 g2.fillRect(0, 0, 1200, 1000);
	  }
	  if ((Topology_Designer_gui.flag == 1) || (Topology_Designer_gui.flag == 2) || (Topology_Designer_gui.flag == 3)) {
  	   
  	    int i, j;	  
  	   
  	    //plot the points
 	    for(i = 0; i < Topology_Designer_prim.num_nodes; i++) {
 		  temp = Topology_Designer_prim.nodelist.get(i);
 		  this.plotPoints(temp.getX(), temp.getY());
  
 		 

 	    }

 	    //draw the lines
   	    // this.drawLines(50, 50, 200, 200);
 	    
 	    for(i = 0; i < Topology_Designer_prim.num_nodes; i++) {
  		  temp = Topology_Designer_prim.nodelist.get(i);
  		  for(j = 0; j < Topology_Designer_prim.num_nodes; j++) {
    		temp1 = Topology_Designer_prim.nodelist.get(j);
    		if(Topology_Designer_prim.link_matrix[i][j] == 1) {
  		      this.drawLines(temp.getX(), temp.getY(), temp1.getX(), temp1.getY());
  		     
    		}
  		  }
           
        }
 	    
 		   
 	   }//end if flag	  
	   
	  
  	  
    }
  	  
  
    public void plotPoints(int x, int y) {
    
    	g2.setColor(Color.BLUE);
    	g2.fillOval(x, y, 10, 10);
    	
    
    
    }
    
    public void drawLines(int x1, int y1, int x2, int y2) {
    	
     	g2.setColor(Color.BLACK);
    	g2.drawLine(x1, y1, x2, y2);

    	
    }
    
}

class TopoPanel extends JPanel {
	
  public TopoPanel () {
	  JButton button1 = new JButton("Random Example");	
	  JButton button2 = new JButton("Specify topology Size and Constraints");
	  JButton button3 = new JButton("Specify topology Size , Location and Constraints");
	  JButton button4 = new JButton("Reset");
	  JButton button5 = new JButton("View Statistics");
	  JButton button6 = new JButton("Help");
	  JButton button7 = new JButton("About");
	  JButton button8 = new JButton("Exit");
	  
	  //add buttons to panel
	  add(button1);
	  add(button2);  
	  add(button3);  
	  add(button4);  
	  add(button5);  
	  add(button6);
	  add(button7);
	  add(button8);
	  
	  //create button actions
	  TopoAction buttonAction1 = new TopoAction(1);
	  TopoAction buttonAction2 = new TopoAction(2);
	  TopoAction buttonAction3 = new TopoAction(3);
	  TopoAction buttonAction4 = new TopoAction(4);
	  TopoAction buttonAction5 = new TopoAction(5);
	  TopoAction buttonAction6 = new TopoAction(6);
	  TopoAction buttonAction7 = new TopoAction(7);
	  TopoAction buttonAction8 = new TopoAction(8);
	  
	  //associate actions with buttons
	  button1.addActionListener(buttonAction1);
	  button2.addActionListener(buttonAction2);
	  button3.addActionListener(buttonAction3);
	  button4.addActionListener(buttonAction4);
	  button5.addActionListener(buttonAction5);
	  button6.addActionListener(buttonAction6);
	  button7.addActionListener(buttonAction7);
	  button8.addActionListener(buttonAction8);
	  
 
	  
	  	
		  
  }
  
  public void drawCanvas() {
  //add a canvas to the frame
	  CanvasPane canvas = new CanvasPane();
	  canvas.setBackground(Color.white);
	  add(canvas, BorderLayout.CENTER);
	  canvas.setPreferredSize(new Dimension(1200, 1000));
	  canvas.setVisible(true);
  }
  
  	
}

class TopoAction implements ActionListener {
	private int option;
	 
	 
	public TopoAction (int opt) {
		option = opt;
	}
	
	public void actionPerformed(ActionEvent event) {
		
		//Topology_Designer_prim.read_input();
		String message = ""; 
		switch(option) {
		  case 1:   System.out.println("Captured Button1 Event");
		            Topology_Designer_gui.flag = 1;
		            Topology_Designer_prim.read_and_compute(1);
		            Topology_Designer_gui.panel.drawCanvas();
		            Topology_Designer_gui.frame.show();
		          
		            break;
		            
		  case 2:   System.out.println("Captured Button Event");
					Topology_Designer_gui.flag = 2;
			        Topology_Designer_prim.read_and_compute(2);
			        Topology_Designer_gui.panel.drawCanvas();
			        Topology_Designer_gui.frame.show();
			    
              		break;
              		
		  case 3:   System.out.println("Captured Button3 Event");
					Topology_Designer_gui.flag = 3;
			        Topology_Designer_prim.read_and_compute(3);
			        Topology_Designer_gui.panel.drawCanvas();
			        Topology_Designer_gui.frame.show();
			        
          			break;
          			
		  case 4:   System.out.println("Captured Button4 Event");
		            Topology_Designer_gui.flag = 4;
		            Topology_Designer_prim.num_nodes = 0;
		            Topology_Designer_prim.degree_const = 2;
		            Topology_Designer_prim.diameter_const = 4;
		            Topology_Designer_prim.elapsedTime = 0;
		            Topology_Designer_prim.topo_cost = 0;
		            Topology_Designer_gui.panel.drawCanvas();
		            Topology_Designer_gui.frame.show();
		        
          			break;
          			
		  case 5:   System.out.println("Captured Button5 Event");
		  			message = message + "Total Nodes: ";
		            message = message + String.valueOf(Topology_Designer_prim.num_nodes);
		            message = message + "\n";
		            message = message + "Degree Constraint: ";
		            message = message + String.valueOf(Topology_Designer_prim.degree_const);
		            message = message + "\n";
		            message = message + "Diameter Constraint: ";
		            message = message + String.valueOf(Topology_Designer_prim.diameter_const);
		            message = message + "\n";
		            message = message + "Total Topology Cost(in pxls): ";
		            message = message + String.valueOf(Topology_Designer_prim.topo_cost);
		            message = message + "\n";
		            message = message + "Program Execution Time(approx): ";
		            message = message + String.valueOf(Topology_Designer_prim.elapsedTime);
		            message = message + "ms";
		            JOptionPane.showMessageDialog(null, message, "Statistics", JOptionPane.INFORMATION_MESSAGE);
		            message = "";
          			break;
          			
		  case 6:   System.out.println("Captured Button6 Event");
					message = message + "NETWORK TOPOLOGY DESIGNER\n";
					message = message + "===========================\n\n";
					message = message + "This software provides the following options:\n\n";
			    	message = message + "1) The first option <Random Example> generates a graph of random\n";
					message = message + "   number of nodes (less than 100). It uses a default degree\n";
					message = message + "   constraint of 2 and diameter constraint of 4.\n\n";
					message = message + "2) The second option takes the number of nodes and the constraints\n";
					message = message + "   as an user input, but uses a random number of nodes to generate\n";
					message = message + "   the topology.\n\n";
					message = message + "3) The third option takes all values, nodes, constraints and node\n";
					message = message + "   co-ordinates as user input.\n\n";
					message = message + "4) The <Reset> Option clears the canvas and is ready for the next run.\n\n";
					message = message + "5) The statistics option shows some of the statistics such as, the\n";
					message = message + "   topology cost, the execution time etc. for that run.\n\n";
					message = message + "6) Help-prints this information.\n\n";
					message = message + "7) About-displays about this project.\n\n";
					message = message + "8) Exit- Exits this program.\n\n";
					JOptionPane.showMessageDialog(null, message, "Help", JOptionPane.INFORMATION_MESSAGE);
				    message = "";
		 
		  
          			break;
          			
		  case 7:   System.out.println("Captured Button7 Event");
					message = message + "Developed By: ";
			        message = message + "\n";
			        message = message + "Hemanth Srinivasan ";
			        message = message + "\n";
			        message = message + "Year 2006";
			        JOptionPane.showMessageDialog(null, message, "About", JOptionPane.INFORMATION_MESSAGE);
			        message = "";
					break;
		  
		  case 8:   System.out.println("Captured Button8 Event");
		            message = message + "        Exit the Program ?";
		            if ((JOptionPane.showConfirmDialog(null, message, "Confirm", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION ))
		              System.exit(0);        		
		            break;
		            
		  default:  System.out.println("Invalid Option");
					 
          			break;
		}
		
	}
}




