/*
Topology Designer - Alogrithm class
Author: Hemanth Srinivasan
Year: 2006
*/
package topology_design_tool;
import java.util.ArrayList;
import java.util.StringTokenizer;
import javax.swing.*;
import java.util.Random;

public class Topology_Designer_prim {
	
	static int num_nodes = 0;	
    static int [][] link_matrix;
    static double [][] cost_matrix;
    static int [] degree;
    static int [] proximity;
    static int [] node_path;
    static int [] candidate_node;
    static int found_dst;
    static int org_source;
    public static ArrayList<Node> nodelist;// = new ArrayList<Node>();
    static int diameter_const = 4;
    static int degree_const = 2; 
    final static double MAX_COST = 99999999;
    public static double topo_cost;
    static long elapsedTime;
    
    public static void read_and_compute (int type) {
   	    	
      int i;
      String temp, input = "";
      
      
      if(type == 1) {
    	Random gen = new Random();
    	while (num_nodes == 0)
   	     num_nodes = gen.nextInt(25); //generate no: of nodes
    	
   	    System.out.println("num_nodes is " + num_nodes);
   	    nodelist = new ArrayList<Node>();
        nodelist.ensureCapacity(num_nodes);
     
   	    generate_random_coordinates();
      }
      
      if(type == 2 || type == 3 ) {
       while (num_nodes == 0) {      
	    input = JOptionPane.showInputDialog("Enter the number of nodes in the topology.");
      	  
	    System.out.println("Input is:" + input);
	  
	    if ( input == null) { 
		  System.out.println("Exiting the Program!!!Have good day!!!");
    	  System.exit(0);
        }
	   
	 
	    try {
		  num_nodes = Integer.parseInt(input);
	    } catch(NumberFormatException nfe) {
		  System.out.println("Enter some value....");
		
	    }
	  
      } 
      
      System.out.println("NO: of nodes is:" + num_nodes);
      
      
     
     
      
      //initializing the diameter constraint
      input = JOptionPane.showInputDialog("Enter the degree constraint");
  	  
	  System.out.println("Degree is:" + input);
	  
	  if  ( input == null) { 
		  System.out.println("Using default value of 2");
  	      
      }
	    
	  try {
		  degree_const = Integer.parseInt(input);
	  } catch(NumberFormatException nfe) {
	    	System.out.println("Using default value of 2");
		
	    }
	  
	  //initializing the diameter constraint
	  input = JOptionPane.showInputDialog("Enter the diameter constraint");
	  	  
	  System.out.println("Diameter is:" + input);
	  
	  if ( input == null) { 
		  System.out.println("Using default value of 4");
  	      
      }
	   	 
	  try {
		  diameter_const = Integer.parseInt(input);
	    } catch(NumberFormatException nfe) {
	    
	      System.out.println("Using default value of 4");
		
	    }
      
      }
      
      if(num_nodes < diameter_const)
    	  diameter_const = num_nodes;
      
      link_matrix = new int[num_nodes][num_nodes];
      cost_matrix = new double[num_nodes][num_nodes];
      degree = new int[num_nodes];
      proximity = new int[num_nodes];
      if(type == 2 || type == 3) {
        nodelist = new ArrayList<Node>();
        nodelist.ensureCapacity(num_nodes);
      }
      
      //generate the co-ordinates
      if (type == 2) {
    	  generate_random_coordinates();
      }
      //else take co-ordinates from user
      if (type == 3) {
       for (i = 0; i < num_nodes; i++) {
    	Node n = new Node();
    	temp = JOptionPane.showInputDialog("Enter (x y) coordinates for node " + (i+1));
    	
    	if (temp == null){
    		System.out.println("Exiting the program!! Have good day");
    		System.exit(0);
    	  }
    	
    	StringTokenizer t = new StringTokenizer(temp, " ");
    	if (t.countTokens() < 2) {
    	  i--;
    	  System.out.println("Enter (x y) coordinates for node. X <0-1200>, Y <0-1000>");
    	  continue;
    	}
    	int x, y;
    	x = Integer.parseInt(t.nextToken());
    	y = Integer.parseInt(t.nextToken());
    	
    	if(x > 1200)
    	  x = 1200;
    	
    	if(y > 1000)
      	  y = 1000;
    	n.setX(x);
    	n.setY(y);
    	//n.setX(Integer.parseInt(t.nextToken()));
    	//n.setY(Integer.parseInt(t.nextToken()));
    	
        nodelist.add(n);
        System.out.println("x,y is: " + n.getX() + " " + n.getY());
       }
      }
      
      long startTime = System.currentTimeMillis();
      
      initialize(); //initialize all matrices
      
      compute_cost_matrix(); //calucate distance between nodes and fill the cost matrix
      
      print_all_matrix(); //print the contents all the matrices
      
      System.out.println("Performing Prim's Algorithm");
      prims_algo(); //compute all pairs shortest path generate cost and path matrix
      
      print_all_matrix(); //print the contents all the matrices
      
      print_degree(); //print degree of all nodes
      
      print_topo_cost();// print the cost of the overall topology
      
      System.out.println("Performing Degree Optimization");
      degree_optimization(); //optmize links
      
      print_all_matrix(); //print the contents all the matrices
      
      print_degree(); //print degree of all nodes
      
      print_topo_cost();// print the cost of the overall topology
      
      System.out.println("Performing Diameter Optimization");
      diameter_optimization(); //reducing the diameter of the graph to 4
      System.out.println("Diameter Optimization Done");
      print_all_matrix(); //print the contents all the matrices
      
      print_degree(); //print degree of all nodes
      
      print_topo_cost();// print the cost of the overall topology
      
      long stopTime = System.currentTimeMillis();
      elapsedTime = stopTime - startTime;
      
      System.out.println("Approximate Program Execution Time is: " + (elapsedTime) + "ms");
   } 
    
   public static void initialize() {
	   int i,j;

	   for (i = 0; i < num_nodes; i++) {
		 for (j = 0; j < num_nodes; j++) {
	       link_matrix[i][j] = 0;
	   	   cost_matrix[i][j] = 0;
	   	 }
		 degree[i] = 0;
		 proximity[i] = -1;
	   }  
	   
   }
   
   public static void compute_cost_matrix() {
	   int i, j;
	   
	   for(i = 0; i < num_nodes; i++) {
		 Node cur = nodelist.get(i);
		 for(j = 0; j < num_nodes; j++) {
			 Node next = nodelist.get(j);
		   cost_matrix[i][j] = distance_x_y(cur.getX(), cur.getY(), next.getX(), next.getY()); 
		 }
		   
	   }
	   
   }
   
   public static void print_all_matrix() {
	   int i, j;
	   
	   System.out.print("\nLink matrix is\n");
	   for (i = 0; i < num_nodes; i++) {
	     for (j = 0; j < num_nodes; j++) {
	    	System.out.print(link_matrix[i][j] + " ");
	     }
	     System.out.print("\n");
	   }

	   System.out.print("\nCost matrix is\n");
	   for (i = 0; i < num_nodes; i++) {
	     for (j = 0; j < num_nodes; j++) {
	    	System.out.print(cost_matrix[i][j] + " ");
	     }
	     System.out.print("\n");
	   }

	/*   System.out.print("\nProximity values are:\n");
	   for (i = 0; i < num_nodes; i++) {
	     	    	System.out.println("Proximity of " + i + proximity[i]);
	   }*/
	   	   
   }
   
     
   public static void prims_algo() {
   
      int i, j, k, m = 0, n = 0, u = 0, v = 0;
      double min = MAX_COST;
   
      //final the lowest cost node pair
      for (i = 0; i < num_nodes; i++) {
	     for (j = 0; j < num_nodes; j++) {
	    	 if( min > cost_matrix[i][j])
				{
					min = cost_matrix[i][j];
					topo_cost = min;
					m = i;
					n = j;
				}
	     }
	     
      }
      //mark the first node pair
      link_matrix[m][n] = 1;
      link_matrix[n][m] = 1;
      
      if(m == n) {
     	 link_matrix[m][n] = 0;
     	}
      
      /*Initialize proximity[]. The value proximity[j] is a vertex in the tree
      such that cost_matrix[j][proximity[j]] is minimum among all choices for proximity[j]*/
          
      for(i = 0; i < num_nodes; i++) {
    	if(cost_matrix[i][n] < cost_matrix[i][m])
    		proximity[i] = n;
    	else
    		proximity[i] = m;
    	}
      proximity[n]= -1;
      proximity[m]= -1;
   
      /*Find another N-2 edges for minimum spanning tree*/
      j = 0;
      for(i = 0; i < num_nodes - 1; i++) {
    	j = find_min();
    	u = j;
    	v = proximity[j];
    	/*if(u == v) {
    	 link_matrix[u][v] = 0;
    	}*/
    	link_matrix[u][v] = 1;
    	link_matrix[v][u] = 1;
    	topo_cost = topo_cost + cost_matrix[j][proximity[j]];
    	proximity[j]= -1;
    	
    	/* update proximity[] */
    	for(k = 0; k < num_nodes; k++) {
     		if( (proximity[k]!= -1) && (cost_matrix[k][proximity[k]] > cost_matrix[k][j]))
     			proximity[k] = j;
    	}
      }
            
   }
   
   public static double distance_x_y(int x1, int y1, int x2, int y2) {
	  double res, x_diff, y_diff;
	  x_diff = Math.abs(x1 - x2);
	  y_diff = Math.abs(y1 - y2);
	  res = Math.pow(x_diff, 2) + Math.pow(y_diff, 2);
	  return Math.sqrt(res);
   }
   
   public static void diameter_optimization() {
	  //find path from every node to every other node
	    //this is done recursively by checking the link matrix and 
	    //jumping from one neighbor to his neighbor and so on till the destination
	  //verify paths from source to destination, via all first hop neighbors
	  //make a note of all the paths with length(node_path) > 4, also make a note of the the 3rd hop
	    //neighbor(candidate_nodes).
	  //among the chosen candidate nodes, find the one closest to the destination 
	   //and add a link between them
	   
	   int i, j, k, a = 0, b = 0, flag1 = 0;//, flag2 = 0;
	   double min_cost = MAX_COST;
	   
	   for (i = 0; i < num_nodes; i++) {
		 
		   /*node_path = new int[degree[i]];
		 candidate_node = new int[degree[i]];
		 
		 for (k = 0; k < degree[i]; k++) {
		   node_path[k] = 0;
		   candidate_node[k] = 0;
		 }*/
		 org_source = i;
		 
		 for (j = 0; j < num_nodes; j++) {
		   flag1 = 0;
		   found_dst = 0;
		   if((i == j) ||(link_matrix[i][j] == 1)) continue;
		   
		   node_path = new int[degree[i]];
		   candidate_node = new int[degree[i]];
			 
		   for (k = 0; k < degree[i]; k++) {
			 node_path[k] = 0;
			 candidate_node[k] = 0;
		   }
			 
		   
		   best_paths(i, i, j, 0); //recursively computes paths from i to j with a list of path  
		                     //path counts and candidate nodes, first argument is the parent node of i, which 
		                     //i itself, in this case
		   for(k = 0; k < degree[i]; k++) {
			 if ((node_path[k] != 0) && (node_path[k] <= diameter_const)) {
				flag1 = 1; 
			    break;
			 }   
		   }
		   
		   if(flag1 == 1) {
		//	 flag2 = 1;  
			 continue;
		   }
		   
		   for(k = 0; k < degree[i]; k++) {
			 if (node_path[k] > diameter_const) {
			  if(cost_matrix[candidate_node[k]][j] < min_cost) {
				 min_cost = cost_matrix[candidate_node[k]][j];
				 a = candidate_node[k];
				 b = j;
			  }
			 }   
		   }

           //add the selected link
	    	link_matrix[a][b] = 1;
			link_matrix[b][a] = 1;
			 
			//update total cost of the topology
			topo_cost = topo_cost + cost_matrix[a][b];
			
			System.out.println(" a is " + a + " b is " + b + " topo cost is " + topo_cost );
        } //end j
			  
		 
      }  //end i
	   
	   
   }
   
   public static void degree_optimization() {
	   //for each node with degree < 2, 
	   //1) check all other nodes with degree < 2, choose the closest
	   //such node and add a link to it. 
	   //2)If no such node exists then find the next closest node to this node, 
	   //  to which no link exists. add a link to it, 
	   //3)if no such node exists, just add a redundant link to the closest node 
	   
	   System.out.println("Degree is " + degree_const );
	   int i, j, k, a = 0, b = 0, deg_flag1 = 0, deg_flag2 =0;
	   double closest = MAX_COST; 
	   
	   for (i = 0; i < num_nodes; i++) {
		 while (degree[i] < degree_const) {  
		// if (degree[i] < degree_const) {
			 closest = MAX_COST;	 
			 deg_flag1 = 0;
			 deg_flag2 = 0;
			
		   for (j = 0; j < num_nodes; j++) {  //1
			   if (i == j) continue;
			   if ((degree[j] < degree_const) && (link_matrix[i][j]==0)) {
				   deg_flag1 = 1;
				   if (cost_matrix[i][j] < closest) {
					   closest = cost_matrix[i][j];
					   a = i;
					   b = j;
				   }
			   }
	       }
		   closest = MAX_COST;
		   
		   if (deg_flag1 == 0) {      //2
			 System.out.println("Deg_flag1 is:" + deg_flag1 );
		     for (k = 0; k < num_nodes; k++) {
			  if (i == k) continue;
			  if (link_matrix[i][k] == 0) {
				  deg_flag2 = 1;
				  if (cost_matrix[i][k] < closest) {
					   closest = cost_matrix[i][k];
					   a = i;
					   b = k;
				   }  
			  }
		     }
		   } 
		    closest = MAX_COST;
		     
		    if (deg_flag2 == 0 && deg_flag1 == 0) {    //3
		    	System.out.println("Deg_flag2 is:" + deg_flag2 );
			   for (k = 0; k < num_nodes; k++) {
			    if (i == k) continue;
				if (cost_matrix[i][k] < closest) {
				   closest = cost_matrix[i][k];
				   a = i;
				   b = k;
				 }  
			   }
		    }//end deg_flag2
		    
	   //   } //end deg_flag1
		
   	     //add the selected link
		 link_matrix[a][b] = 1;
		 link_matrix[b][a] = 1;
		 
		 //update degree matrix
		 compute_degree(a);
		 compute_degree(b);
		 //update total cost of the topology
		 topo_cost = topo_cost + cost_matrix[a][b];
	//	 System.out.println("Deg_flag1 is:" + deg_flag1 );
	//	 System.out.println("Deg_flag2 is:" + deg_flag2 );
	//	 System.out.println("I is:" + i );
		
	  //  }//end deg[i] < 2
		 
      } //end while        
		 
	 } //end for all i
	
	 
	 
   }
  
   
   public static void compute_degree(int node){
	  int j;
	  
	  for(j = 0; j < num_nodes; j++) {
		degree[node] += link_matrix[node][j];   
	  }
   }
   
   public static int find_min() {
     int x, y = 0;
     double mini = MAX_COST;
     for(x = 0; x < num_nodes; x++) {
      if((proximity[x]!= -1) && (cost_matrix[x][proximity[x]] < mini)) {
   		mini = cost_matrix[x][proximity[x]];
   		y = x;
   		}
   	 }
     return y;
   }
   
   public static void print_degree() {
	 int i;
	 for(i = 0; i < num_nodes; i++) {
			degree[i] = 0;   
	 }
	 for(i = 0; i < num_nodes; i++){
		 compute_degree(i);
		 System.out.println("Degree of Node" + (i+1) + ": " + degree[i]);
	 }
		 
   }
   
   public static void print_topo_cost() {
	   
	   System.out.println("Total Cost of the Topolgy is:" + topo_cost );
   }
		 
   public static void best_paths(int p, int s, int d, int node_path_num) {
	   
	  int i, j, m, k;
	  int [] sorted_neighbor;
	  int sorted_length;
	//  int ret_flag = 1;
	  	  
	  System.out.println(" parent is" + p + " source is" + s + " dest is" + d );
	  //k = node_path_num;
	  for (i = 0; i < num_nodes; i++) {
	    //k = 0; 
		  k = node_path_num;
 	    if(link_matrix[s][i] == 0) continue;	
	    if(i == p) continue;
	    
	    if(link_matrix[s][d] == 1) {
	      node_path[k]++; 	
	    //  if(node_path[k] <= diameter_const)
	      found_dst = 1;
	        return;
	    //  else
         //  continue;
	    }
	
	    if (p == s) 
	     sorted_length = degree[s];
	    else 
	     sorted_length = degree[s] - 1;
	    
	    /*for(m = 0; m < sorted_length; m++ ) {
	       if (node_path[m] <= 0 )	
	           ret_flag = 0;
	    }
	    if(ret_flag == 1)return; */
	    
	    sorted_neighbor = new int [sorted_length];
	    
	    
	    sorted_neighbor = sort_neighbors(p, s, d, sorted_neighbor, sorted_length);
	    
	    for(m = 0; m < sorted_length; m++ ) {
	    	System.out.println("sorted_neighbor " + sorted_neighbor[m] );
	     
	    }
	    
	    j = 0;
	    
	    while (j < sorted_length) {

	    	/*if(node_path[k] == diameter_const-1) {
	    		candidate_node[k] = s;
	    		System.out.println("S is " + s + "P is" + p + " selected_neighbor is " + sorted_neighbor[j]);
	    		System.out.println("Candidate 3rd hop num " + k + " is " + candidate_node[k]);
	    	}
	    	System.out.println(" parent is" + p + " source is" + s + " dest is" + d + " node_path " + k + "is" + node_path[k] );
	    	 */
	    	
	    	best_paths(s, sorted_neighbor[j], d, k );            	    	 	 
	    	node_path[k]++; 
	    	
	    	if( node_path[k]-1 == diameter_const) {
	    		candidate_node[k] = s;
	    		System.out.println("S is " + s + "P is" + p + " selected_neighbor is " + sorted_neighbor[j]);
	    		System.out.println("Candidate 3rd hop num " + k + " is " + candidate_node[k]);
	    	}
	    	System.out.println(" parent is" + p + " source is" + s + " dest is" + d + " node_path " + k + "is" + node_path[k] );
	    		
	    	
	    	      	    	  	

	    	if((found_dst == 0) && (node_path[k] > diameter_const)) return; //path length more the reqd diameter
	    	
	    	if ((node_path[k] != 0) && (found_dst == 1)&& (node_path[k] <= diameter_const)) return;
	    		    	
	    	if((found_dst == 1) && (node_path[k] > diameter_const) && (s != org_source)) {
	         return; 	
	        }
	        else if((found_dst == 1) && (node_path[k] > diameter_const) && (s == org_source)) {
	        	System.out.println(" k is " + k + " sorted length is " + sorted_length);
	        	if(k == (sorted_length - 1)) return;	
	          j++;
	          k++;
	          
	          
	        } 
	        
	    	
	    	
	    }
	    
	    
              
   
	    
	  } //end i
   
   }
   
   public static int [] sort_neighbors(int par, int src, int dst, int temp_sort[], int len ) {
	   
	   int i, j, k;
	   double [] temp;
	   double min = MAX_COST, tmp;
	   
	    
	   temp  = new double [num_nodes];
	   
	   //copy costs of neighbors
	   for(i = 0; i < num_nodes; i++ ) {
		
		if((i != par) && (link_matrix[src][i] == 1))
		  temp[i] = cost_matrix[src][i];
		else
		  temp[i] = 0;
		System.out.println("old temp " + temp[i] );
        
	   }
	   
	   //copy costs from neigbbors to the destination
	   for(i = 0; i < num_nodes; i++ ) {
		 if(temp[i] != 0) {
		
		   temp[i] = cost_matrix[i][dst];	 
		   System.out.println("new temp " + temp[i] );	 
		 }
		   
	   }
	      
	   
	   j = 0;
	   k =0;
	   
	   while (j < len) {
		 for(i = 0; i < num_nodes; i++ ) {
		   if((i == par) || (link_matrix[src][i] == 0) || (i == src)) continue;
		   
		   tmp = temp[i];
		   if (dst > src) {  //to break equicost tie, choose higher node id if going down
			 if(tmp <= min) {  
			   min = tmp;
			   k = i;
		     }   
		   }
		   else {  //if going up, that dest > source, choose lower node id
		     if(tmp < min) {
		       min = tmp;
			   k = i;
		     }
		     
		   }
   
		   
	     }
		
		temp_sort[j] = k;
		temp[k] = MAX_COST;
		min  = MAX_COST;
		j++;
		 
       }
	   
	 return temp_sort;  
	   
   }
   
   public static void generate_random_coordinates() {
	   int i;
	   Random generator = new Random();
	  
	   
	   
	   //generate X, co-ordinates
	   for (i = 0; i < num_nodes; i++) {
		 Node n = new Node();
		 n.setX(generator.nextInt(900));
	     n.setY(generator.nextInt(900));   
	     nodelist.add(n);
	   }
	   
	  
	   
   }
   
   
}

