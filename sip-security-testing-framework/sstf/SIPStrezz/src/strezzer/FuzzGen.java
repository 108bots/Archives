/*SIP SECURITY TESTING FRAMEWORK - SSTF version 0.1
 *Uses a genetic algorithm to generate fuzz data
 * Input: Parent Population types eg: <parent1 parent2> "SIP_uri SIP_uri" "SIP_int SIP_string" etc
 *        Count: Number of offsprings (length of the offspring fz file)
 * Output: Offspring[xx].fz file
 * Pre-requisite: population files (.fz files in /fuzz directory) and corresponding 
 * fitness files (.fit files in /fuzz/fitness directory) should exist
 * Author: Hemanth Srinivasan
 * Year: 2008
 */
package strezzer; 

import javax.swing.*;

import java.util.*;
import java.io.*;

public class FuzzGen extends StrezzHeader {
	
	//fitness parameters
	public static int fitParamCount = 0;
	public static boolean NATIVE_TYPE_DEF = false;
	public static String NATIVE_TYPE_VAL = null;
	public static boolean NATIVE_SIZE_DEF = false;
	public static String NATIVE_SIZE_VAL = null;
	public static boolean PARENTS_FIT_DEF = false;
	public static String PARENTS_FIT_VAL = null;
	public static int PARENTS_FIT_VALUE = 0;  //is calculated based on PARENTS_FIT_VAL (AVG, MIN or MAX)
	public static boolean NATIVE_CHARACTER_DEF = false;
	public static String NATIVE_CHARACTER_VAL = null;
	public static boolean DAMAGE_COEFF_DEF = false;
	public static String DAMAGE_COEFF_VAL = null;
	public static String USER_MODE = "any";
	public static boolean USER_DEF = false;
	public static ArrayList USER_DEF_VALS = new ArrayList ();
	
	public static int parentIndex1 = -1;
	public static int parentIndex2 = -1; 
	public static byte [] parent1Data = null; //contains the entire parent .fz file
	public static byte [] parent2Data = null;
	public static Random rand = new Random(RAND_SEED);
	public static byte [] offspring1Data = null;
	public static byte [] offspring2Data = null;
	public static int offspring1fit = 0;
	public static int offspring2fit = 0;
	public static int parent1fit = 0;
	public static int parent2fit = 0;
	public static File off, offit;
	public static FileOutputStream offout;
	public static FileWriter offitout;
	public static String parent1Name = new String();
	public static String parent2Name = new String();
	public static String offSpringfname = new String ();
	public static String offSpringfitfname = new String ();
    

	//public static void main(String[] args) {
	
	public FuzzGen(String par1, String par2) {
		
		 System.out.println("\n**************Running FuzzGen*********************\n");
		 
		 //Reset Fitness Params
		 fitParamCount = 0;
		 NATIVE_TYPE_DEF = false;
		 NATIVE_TYPE_VAL = null;
		 NATIVE_SIZE_DEF = false;
		 NATIVE_SIZE_VAL = null;
		 PARENTS_FIT_DEF = false;
		 PARENTS_FIT_VAL = null;
		 PARENTS_FIT_VALUE = 0;
		 NATIVE_CHARACTER_DEF = false;
		 NATIVE_CHARACTER_VAL = null;
		 DAMAGE_COEFF_DEF = false;
		 DAMAGE_COEFF_VAL = null;
		 USER_MODE = "any";
		 USER_DEF = false;
			
		/*
		 //parent type 1
	       do {
                parent1Name = null;
 	    	    parent1Name = JOptionPane.showInputDialog(null, "Specify SIP Parent type (case sensitive)", "Choose Parent 1", JOptionPane.QUESTION_MESSAGE);	          
	    	    if (parent1Name == null) 
	    	    {
	    	    	System.out.println("\nGoodbye!!!");
	    	    	System.exit(0);
	 	        }
	           
	       } while ((parent1Name == "") || (parent1Name == JOptionPane.UNINITIALIZED_VALUE) || (parent1Name.length() == 0));
	       
	             
	       //parent type 2
	        parent2Name = null;
	        parent2Name = JOptionPane.showInputDialog(null, "Specify SIP Parent type  (case sensitive)", "Choose Parent 2", JOptionPane.QUESTION_MESSAGE);	          
	    	if ((parent2Name == "") || (parent2Name == null) || (parent2Name == JOptionPane.UNINITIALIZED_VALUE) || (parent2Name.length() == 0)) {
	    	  	parent2Name = parent1Name;
	    	  	System.out.println("\nUsing Same Parent type as 1!!!");
	 	    }
	    	
	    	
*/
		   //extract the file name from path name
		   String temp1Name = new String();
		   String temp2Name = new String();
		   
		   StringTokenizer st1 = new StringTokenizer(par1, slash);
		   while (st1.hasMoreTokens()) {
			   temp1Name = st1.nextToken(); 
			} 
		   
		   StringTokenizer st2 = new StringTokenizer(par2, slash);
		   while (st2.hasMoreTokens()) {
			   temp2Name = st2.nextToken(); 
		   }
           
		   System.out.println("\ntemp 1 type is: "+temp1Name);
		   System.out.println("\ntemp 2 type is: "+temp2Name);
		   
		   //lose the traling .fz 
		   parent1Name = temp1Name.substring(0, temp1Name.length()-3);
		   parent2Name = temp2Name.substring(0, temp2Name.length()-3);
		   
	       System.out.println("\nParent 1 type is: "+parent1Name);
		   System.out.println("\nParent 2 type is: "+parent2Name);
		   
		   //Process the fitness Parameter File
		   processFitnessParameters();
		   
		   //reads the parents file into a buffer
		   readParents(par1, par2);
		   
		   //create OffSpring file
		   
		   //extract the parent type (xx) from SIP_xx
		   //lose the SIP_
		   
		   String par1type = parent1Name.substring(4, parent1Name.length());
		   String par2type = parent2Name.substring(4, parent2Name.length());
		   
		   offSpringfname = OFFSPRING_DIR+"SIP_"+par1type+"-"+par2type+".fz";
		   offSpringfitfname = FITNESS_DIR+"SIP_"+par1type+"-"+par2type+".fit";
			    
	 	   try 
	 	   {
	 		    //offspring fz file
	 	    	off = new File(offSpringfname);
	 	 	    boolean s1 = off.createNewFile();
	 	 	    offout = new FileOutputStream (off, true);
				
	 		    //offspring fitness file
	 	    	offit = new File(offSpringfitfname);
	 	 	    boolean s2 = offit.createNewFile();
        	 	offitout = new FileWriter(offit, true);
	 	 	    
	 	 	   
	 	    }
	 	    catch (IOException e) {
	 	    	System.out.println ( "create offspring fz/fit file exception =: " + e );
			    e.printStackTrace();
	 	    	
	 	    }
		   
	 	   for (int i = 0; i < OFFSPRING_PAIRS; i++) { //2 offsprings generated per interation
	 		   //SELECTION
	 		   //pick one random value from each of the initial population (parent 1 and 2)
	 		   parentIndex1 = selectParentIndex(parent1Name, (short)1);
	 		   parentIndex2 = selectParentIndex(parent2Name, (short)2);
		   
	 		   //crossover
	 		   crossOverParents(parentIndex1, parentIndex2);
		   
	 		   //mutation
	 		   mutateOffsprings();
		   
	 		   //compute offspring fitness
	 		   PARENTS_FIT_VALUE = compFitnessFromParent (PARENTS_FIT_VAL);
  	 		   offspring1fit = computeFitness (offspring1Data);
  	 		   offspring2fit = computeFitness (offspring2Data);
	 			  
	 		   //Write the fz and fit values to Offspring files
		   
	 		   //convert fitness values to string before writing
	 		   String os1 = new String();
	 		   Integer oi1 = new Integer(offspring1fit);
	 		   os1 = oi1.toString()+"\n";
	 		   String os2 = new String();
	 		   Integer oi2 = new Integer(offspring2fit);
	 		   os2 = oi2.toString()+"\n";
		   
		   	   try {
			   
		   		   //write offspring fz values
		   		   offout.write(offspring1Data, 0, offspring1Data.length);
		   		   offout.write('\n');
		   		   offout.write(offspring2Data, 0, offspring2Data.length);
		   		   offout.write('\n');
			   
		   		   //write offspring fitness values
		   		   offitout.write(os1);
		   		   offitout.write(os2);
		   	   } catch (IOException e) {
		   	      	System.out.println ( "writing offspring streams exception =: " + e );
		   	      	e.printStackTrace();
	 	    	
		   	   }
		   	   
		   	   //reset all global parameters
		   	   resetGlobals();
	 	   }
		   
		 //close the offspring file streams 
		   try {
			   offout.close();
			   offitout.close();
		   }
		   catch (IOException e) {
	 	    	System.out.println ( "closing offspring streams exception =: " + e );
			    e.printStackTrace();
	 	    	
	 	    }
		   
		   System.out.println("\n**************FuzzGen DONE*********************\n");
	}

	public static void resetGlobals () {
		parentIndex1 = -1;
		parentIndex2 = -1; 
		offspring1Data = null;
		offspring2Data = null;
		offspring1fit = 0;
		offspring2fit = 0;
		parent1fit = 0;
		parent2fit = 0;
		
	}
	
	public static int compFitnessFromParent (String FitnessMode) {
		
		int fitness = 0;
		
		if (FitnessMode.equalsIgnoreCase("avg")) {
			fitness = (int)Math.ceil((parent1fit + parent2fit) / 2);	
		}
		
		if (FitnessMode.equalsIgnoreCase("min")) {
			fitness = Math.min(parent1fit, parent2fit);	
		}
		
		if (FitnessMode.equalsIgnoreCase("max")) {
			fitness = Math.max(parent1fit, parent2fit);	
		}
		
		
		return fitness;
	}
	
	public static void mutateOffsprings() {
		
		int mut1cnt = rand.nextInt(MAX_MUTATION_COUNT);
		int mut2cnt = rand.nextInt(MAX_MUTATION_COUNT);
		
		System.out.println("\nmut1cnt: "+mut1cnt+" mut2cnt "+mut2cnt);
		
		//randomly change mutcnt # of bytes in offspring 
		//the new bytes will be valid (partial ASCII range: 32 - 126)
		
		//offspring 1
		for (int i = 0; i < mut1cnt; i++) {
		    byte chr = (byte)0; //dummy
			
		    while (chr < 32) {   
		    	chr = (byte)rand.nextInt(127);  //ascii value to be written
		    	if ((chr == 59) || (chr == 10) || (chr == 13)) //';' is the csv delimiter, don't use it
		    	   chr = (byte)0;                              // dont use newline/carriage return chars
		    }
		    
			int pos = 0;
			if (offspring1Data.length > 0) {
				pos = rand.nextInt(offspring1Data.length); //position of change
				offspring1Data[pos] = chr;
			}
				
		}
		  
		//offspring 2
		for (int i = 0; i < mut2cnt; i++) {
		    byte chr = (byte)0; //dummy
		    
		    while (chr < 32) {   
		    	chr = (byte)rand.nextInt(127);  //ascii value to be written
		    	if (chr == 59) //';' is the csv delimiter, don't use it
		    		chr = (byte)0;
		    }
			
			int pos = 0;
			if (offspring2Data.length > 0) {
				pos = rand.nextInt(offspring2Data.length); //position of change
				offspring2Data[pos] = chr;
			}
			
		}

	}
	
	public static void readParents(String par1, String par2) {
		
		int fsize = 0;
		
		try {
			
			System.out.println ("Opening File: "+par1);
			File inFile = new File (par1);
			fsize = (int)inFile.length();
			if (inFile == null) {
				System.out.println ("Error: Opening File "+ par1);
			}
			parent1Data = new byte [fsize];
			
			// read file into buffer
			FileInputStream fin = new FileInputStream (inFile);
			fin.read(parent1Data, 0, fsize);    
			fin.close ();
	    } catch  (IOException e) {
	        System.out.println ( "Parent1Data IO Exception =: " + e );
	        e.printStackTrace();
	        System.exit(1);
	    } //stream exception
	    
	    if (par1.equals(par2)) {
	    	parent2Data = new byte [parent1Data.length];
	    	System.arraycopy(parent1Data, 0, parent2Data, 0, parent1Data.length);
	    	return;
	    }
	    
	    try {
			
			System.out.println ("Opening File: "+par2);
			File inFile = new File (par2);
			fsize = (int)inFile.length();
			if (inFile == null) {
				System.out.println ("Error: Opening File "+ par2);
			}
			parent2Data = new byte [fsize];
			
			// read file into buffer
			FileInputStream fin = new FileInputStream (inFile);
			fin.read(parent2Data, 0, fsize);    
			fin.close ();
	    } catch  (IOException e) {
	        System.out.println ( "Parent2Data IO Exception =: " + e );
	        e.printStackTrace();
	        System.exit(1);
	    } //stream exception
	    
		
	}
	
	public static void crossOverParents(int parDataIndex1, int parDataIndex2) {
		
		ArrayList child1List = new ArrayList();
		ArrayList child2List = new ArrayList();
		
		//extract child1data from parent1data based on parDataIndex1
		int i = 0;
		int lineCnt = 0;
		while (i < parent1Data.length) {
			if (parDataIndex1 == 0) {  //line 1
				while (parent1Data[i] != '\n') {
					child1List.add(new Byte(parent1Data[i]));
					i++;
				}
				break;
			}
			else {  //index greater that 0
				while (lineCnt != parDataIndex1) {  //traverse till the previous end of line
					if (parent1Data[i++] == '\n')
					   lineCnt++;
				}
				
				while (parent1Data[i] != '\n') {
					child1List.add(new Byte(parent1Data[i]));
					i++;
				}
				break;
				
			}
			
		}
		
		//extract child2data from parent1data based on parDataIndex2
		i = 0;
		lineCnt = 0;
		while (i < parent2Data.length) {
			if (parDataIndex2 == 0) {  //line 1
				while (parent2Data[i] != '\n') {
					child2List.add(new Byte(parent2Data[i]));
					i++;
				}
				break;
			}
			else {  //index greater that 0
				while (lineCnt != parDataIndex2) {  //traverse till the previous end of line
					if (parent2Data[i++] == '\n')
					   lineCnt++;
				}
				
				while (parent2Data[i] != '\n') {
					child2List.add(new Byte(parent2Data[i]));
					i++;
				}
				break;
		 		
			}
			
		}
		
		System.out.println("Full Parent 1 Length:"+parent1Data.length);
		System.out.println("Full Parent 2 Length:"+parent2Data.length);
		System.out.println("Extracted Parent 1 Length:"+child1List.size());
		System.out.println("Extraced Parent 2 Length:"+child2List.size());
	
		//pick random crossover points
		int crossPnt1 = 0;
		int crossPnt2 = 0;
		
		if (child1List.size() > 0)
			crossPnt1 = rand.nextInt(child1List.size());
		
		if (child2List.size() > 0)
			crossPnt2 = rand.nextInt(child2List.size());
			
		
		System.out.println("Cross over point 1:"+crossPnt1);
		System.out.println("Cross over point 2:"+crossPnt2);
	
		//allocate memory for offspringdata
		offspring1Data = new byte [crossPnt1 + child2List.size() - crossPnt2];
		offspring2Data = new byte [crossPnt2 + child1List.size() - crossPnt1];
		
		System.out.println("Offspring 1 Length:"+offspring1Data.length);
		System.out.println("Offspring 2 Length:"+offspring2Data.length);
	
		 
		//offspring1 = parent1a|parent2b
		//offspring2 = parent2a|parent1b
		
		int offcnt = 0;
		//offspring1a
		for (i = 0; i < crossPnt1; i++) {
			Byte b = (Byte)child1List.get(i);
			offspring1Data[offcnt++] = b.byteValue();
		}
   
		//offspring1b
		for (i = crossPnt2; i < child2List.size(); i++) {
			Byte b = (Byte)child2List.get(i);
			offspring1Data[offcnt++] = b.byteValue();
		}
		
		offcnt = 0;
		//offspring2a
		for (i = 0; i < crossPnt2; i++) {
			Byte b = (Byte)child2List.get(i);
			offspring2Data[offcnt++] = b.byteValue();
		}
		
		//offspring2b
		for (i = crossPnt1; i < child1List.size(); i++) {
			Byte b = (Byte)child1List.get(i);
			offspring2Data[offcnt++] = b.byteValue();
		}
		
		
		//print the offspring values
		System.out.println("OffSpring 1:");
		for (int j = 0; j < offspring1Data.length; j++) {
        	//System.out.print((char)offspring1Data[j]);
        }
		//print the offspring values
		System.out.println("\nOffSpring 2:");
		for (int j = 0; j < offspring2Data.length; j++) {
        	//System.out.print((char)offspring2Data[j]);
        }
		
	}
	
	public static int selectParentIndex(String parentName, short parentType) {
		
		String fileName = FITNESS_DIR+parentName+".fit";
		ArrayList fitValuesList = new ArrayList();
		int fsize = 0;
		
		//associated with each of the fitness values is a fitness probability, higher the
		//value, higher the probability of being chosen
		double [] fitProb = new double[FIT_HIGH - FIT_LOW + 1];
		//fitProbSum[i] contains the sum of i fitProb terms 
		double [] fitProbSum = new double[FIT_HIGH - FIT_LOW + 1];
		int chosenFitValue = -1;
		
		try {
			
			System.out.println ("Opening File "+fileName);
			File inFile = new File (fileName);
			fsize = (int)inFile.length();
			if (inFile == null) {
				System.out.println ("Error: Opening File "+ fileName);
			}
						
			// read file into buffer
			FileInputStream fin = new FileInputStream (inFile);
			DataInputStream in = new DataInputStream(fin);
	        BufferedReader br = new BufferedReader(new InputStreamReader(in));
	        String strLine;
	        //Read File Line By Line
	        while ((strLine = br.readLine()) != null)   {
	        	fitValuesList.add(Integer.valueOf(strLine));	
	        }
	        //Close the input stream
	        fin.close ();
	    } catch  (IOException e) {
	        System.out.println ( "Fitness file IO Exception =: " + e );
	        e.printStackTrace();
	        System.exit(1);
	    } //stream exception
	    
	    for (int i = 0; i < fitValuesList.size(); i++) {
	        Integer val = (Integer)fitValuesList.get(i);
	        System.out.println (val.intValue());
	    }
	
	    //generate uniform rand #s,  p[i] = q[i] / Summation(q)
	    double probsum = 0;
	    for (int i = 0; i < FIT_HIGH - FIT_LOW + 1; i++) {
			fitProb[i] = rand.nextDouble();
			probsum += fitProb[i];
		}
	    for (int i = 0; i < FIT_HIGH - FIT_LOW + 1; i++) {
	    	fitProb[i] = fitProb[i] / probsum;
		}
      		
	    //sort the random numbers
	    Arrays.sort(fitProb);
	    for (int i = 0; i < FIT_HIGH - FIT_LOW + 1; i++) {
	    	System.out.println (fitProb[i]);
	    }
		System.out.println (" ");
	    //fitProb[0] is the prob associated with fitness val FIT_LOW
	    //fitProb[FIT_HIGH - FIT_LOW + 1] is the prob associated with fitness val FIT_HIGH
	    double tsum = 0;
	    for (int i = 0; i < FIT_HIGH - FIT_LOW + 1; i++) {
	    	tsum += fitProb[i];
	    	fitProbSum[i] = tsum;
		}
	    for (int i = 0; i < FIT_HIGH - FIT_LOW + 1; i++) {
	    	System.out.println (fitProbSum[i]);
	    }
	    
	    boolean found = false;
	    while (!found) {   //make sure that the random fit value generated is present in the fitness file
	    	//generate rand u b/w 0 and 1
	    	double randFit = rand.nextDouble();
	    	double preval = 0, curval = 0;
	    	int i;
			//if fitProbsum[i-1]<randFit<=fitProbsum[i] then choose fitProbsum[i]
			for (i = 0; i < FIT_HIGH - FIT_LOW + 1; i++) {
				curval = fitProbSum[i];
				if (randFit > preval && randFit <= curval) {
					break;
				}
				preval=curval;
			}
			System.out.println ("RandFit is: "+randFit+" curval is: "+curval);
			// i gives the position of the curval=fitProbSum, fitProb[i] gives the chosen fitness Prob
			//and i + FIT_lOW gives the corresponding fitness value
			chosenFitValue = i + FIT_LOW;
			System.out.println ("Fitness Probability is: "+fitProb[i]+" Fitness Value is: "+chosenFitValue);
					    
			found = fitValuesList.contains(new Integer(chosenFitValue));
			System.out.println ("Found is: "+found);
			
		    	
	    }
	    
	    //store the fitness values of parents
	    if (parentType == 1) 
	    	parent1fit = chosenFitValue;
	    else if (parentType == 2)
	    	parent2fit = chosenFitValue;
	    
		//now find all occurrences of the chosenFitValue in the fitness file (fitValuesList)
		//track the index of the occurrences too. From this set pick on index at random
		//this index is the line number (value) in the fuzz file, which will serve as the parent.
		ArrayList fitLocations = new ArrayList();
		for (int j = 0; j < fitValuesList.size(); j++) {
	        Integer val = (Integer)fitValuesList.get(j);
	        if (val.intValue() == chosenFitValue)
	        	fitLocations.add(Integer.valueOf(j));
	    }
	
		int randfitIndex = -1;
		if (fitLocations.size() == 1) {
		   randfitIndex = 0;	
		} else {
			//choose a random index from fitlocations
			randfitIndex = rand.nextInt(fitLocations.size());

		}
		Integer parentLocationIndex = (Integer)fitLocations.get(randfitIndex);
		System.out.println ("FitLocationSize is: "+fitLocations.size()+" RandFitIndex is: "+randfitIndex+" ParentLocationIndex is: "+parentLocationIndex.intValue());
	    return parentLocationIndex.intValue();
	}
	
	public static void processFitnessParameters () {
		//process the parameters.fit file
		String fileName = FITNESS_DIR + "parameters.fit";
		
		try {
			
			System.out.println ("Opening File "+fileName);
			File inFile = new File (fileName);
			
			if (inFile == null) {
				System.out.println ("Error: Opening File "+ fileName);
			}
						
			// read file into buffer
			FileInputStream fin = new FileInputStream (inFile);
			DataInputStream in = new DataInputStream(fin);
	        BufferedReader br = new BufferedReader(new InputStreamReader(in));
	        String strLine;
	        //Read File Line By Line
	        while ((strLine = br.readLine()) != null)   {
	        	
	        	if ((strLine == null) || (strLine.equalsIgnoreCase("")) || (strLine.equalsIgnoreCase("\n")))
	        		continue;
	        	if (strLine.charAt(0) == '#')  //ignore comment lines
	        		continue;
	        	
	           //tokenize the line	
	           		    		   
	 		   StringTokenizer st1 = new StringTokenizer(strLine, " ");
	 		   if (st1.countTokens() != 3) {  //ignore malformed rule
	 			   System.out.println ("Ignoring Malformed Fitness Parameter");
	 			   continue;
	 		   }
	 		   
	 			String paramType = st1.nextToken(); 
	 			String paramValue = st1.nextToken();
	 			String paramFlag = st1.nextToken();
	 			
	 			System.out.println ("paramType: "+paramType+" paramValue: "+paramValue+" paramFlag: "+paramFlag);
	 			
	 			if (paramType.equalsIgnoreCase("native_type")) {
	 				if (paramValue != null)
	 				 NATIVE_TYPE_VAL = paramValue;
	 				
	 				if (paramFlag != null) {
	 					int flg = Integer.parseInt(paramFlag);
	 					if (flg == 1) {
	 						NATIVE_TYPE_DEF = true;
	 						fitParamCount++;
	 					}
		 			
	 				}
	 			}
	 			
	 			if (paramType.equalsIgnoreCase("native_size")) {
	 				if (paramValue != null)
	 				 NATIVE_SIZE_VAL = paramValue;
	 				
	 				if (paramFlag != null) {
	 					int flg = Integer.parseInt(paramFlag);
	 					if (flg == 1) {
	 						NATIVE_SIZE_DEF = true;
	 						fitParamCount++;
	 					}
		 			
	 				}
	 			}
	 			
	 			if (paramType.equalsIgnoreCase("parents_fitness")) {
	 				if (paramValue != null)
	 					PARENTS_FIT_VAL = paramValue;
	 				
	 				if (paramFlag != null) {
	 					int flg = Integer.parseInt(paramFlag);
	 					if (flg == 1) {
	 						PARENTS_FIT_DEF = true;
	 						fitParamCount++;
	 					}
		 			
	 				}
	 			}
	 			
	 			if (paramType.equalsIgnoreCase("native_character")) {
	 				if (paramValue != null)
	 					NATIVE_CHARACTER_VAL = paramValue;
	 				
	 				if (paramFlag != null) {
	 					int flg = Integer.parseInt(paramFlag);
	 					if (flg == 1) {
	 						NATIVE_CHARACTER_DEF = true;
	 						fitParamCount++;
	 					}
		 			
	 				}
	 			}
	 			
	 			if (paramType.equalsIgnoreCase("damage_co_eff")) {
	 				if (paramValue != null)
	 					DAMAGE_COEFF_VAL= paramValue;
	 				
	 				if (paramFlag != null) {
	 					int flg = Integer.parseInt(paramFlag);
	 					if (flg == 1) {
	 						DAMAGE_COEFF_DEF = true;
	 						fitParamCount++;
	 					}
		 			
	 				}
	 			}
	 			
	 			if (paramType.equalsIgnoreCase("user_mode")) {
	 				if (paramValue != null)
	 					USER_MODE= paramValue;
	 				 				
	 			}
	 			
	 			if (paramType.equalsIgnoreCase("user_defined")) {
	 				if (paramValue != null)
	 					USER_DEF_VALS.add(paramValue);
	 				
	 				if (paramFlag != null) {
	 					int flg = Integer.parseInt(paramFlag);
	 					if (flg == 1) {
	 						if (USER_DEF == false) {  //for ANY case increment only once
	 							fitParamCount++;
	 						}
	 						USER_DEF = true;
	 						if (USER_MODE.equalsIgnoreCase("all")) { //for ALL case increment for each user_defined
	 							fitParamCount++;
	 						}
	 					}
		 			
	 				}
	 			}
	 		
	 		 		   
	        }
	        //Close the input stream
	        fin.close ();
	    } catch  (IOException e) {
	        System.out.println ( "Fitness Parameter file IO Exception =: " + e );
	        e.printStackTrace();
	        
	    } //stream exception
	    
	}
	
	public static int computeFitness (byte [] fuzzValue) {
		
		int fitValue = 0;
		float deviationFactor = (float)1 / fitParamCount;
		float totalDeviationContribtion = 0;
		
		System.out.println("deviationFactor: "+deviationFactor);
		
		//Compute deviation Contributions for each parameter
		
		//NATIVE_TYPE
		if (NATIVE_TYPE_DEF == true) {
			float deviationCont = 0;
			float percentageCont = 0;
			
			if (NATIVE_TYPE_VAL.equalsIgnoreCase("string")) {
				int totalByteCount = fuzzValue.length;
				int deviousByteCount = 0;
				
				//valid ASCII range for string characters is 32D - 126D
				//increment deviousByteCount for all bytes that dont fall in this range
				for (int i = 0; i < totalByteCount; i++) {
					byte val = fuzzValue[i];
					if ((val < 32) && (val > 126))
						deviousByteCount++;
				}
				
				percentageCont = (float)deviousByteCount / totalByteCount;
				deviationCont = percentageCont * deviationFactor;
			}
			
			if (NATIVE_TYPE_VAL.equalsIgnoreCase("number")) {
				int totalByteCount = fuzzValue.length;
				int deviousByteCount = 0;
				
				//valid ASCII range for numbers is 48D - 57D (0 - 9)
				//46D for . 45D for - 43D for +
				//increment deviousByteCount for all bytes that dont fall in this range
				for (int i = 0; i < totalByteCount; i++) {
					byte val = fuzzValue[i];
					if ( (val !=  43) && (val != 45) && (val != 46) && ((val < 48) && (val > 57)) )
						deviousByteCount++;
				}
				
				percentageCont = (float)deviousByteCount / totalByteCount;
				deviationCont = percentageCont * deviationFactor;
				
			}

			System.out.println("percentageCont: "+percentageCont);
			System.out.println("deviationCont: "+deviationCont);
			
			totalDeviationContribtion += deviationCont;
		}
		
		//NATIVE_SIZE
		if (NATIVE_SIZE_DEF == true) {
			
			int nativeSize = Integer.parseInt(NATIVE_SIZE_VAL);
			float deviationCont = 0;
			float percentageCont = 0;
			int totalByteCount = fuzzValue.length;
			
			
			//check range for numbers
			if (NATIVE_TYPE_VAL.equalsIgnoreCase("number")) {
				String maxStr = null;
				int maxLen = 0;
				
				if (nativeSize > 0) {
					maxStr = Double.toString(Math.pow(2, nativeSize));
					maxLen = maxStr.length();
				}
				
				if (totalByteCount <= maxLen) {
					percentageCont = 0.5f;
					if (totalByteCount == maxLen) {
						if ((fuzzValue[0] > maxStr.charAt(0)) || (fuzzValue[0] < '0'))
						  percentageCont = 0.75f;	
					}
					
				} else if (totalByteCount > maxLen) {
					percentageCont = 1.0f;
										
				} 
				
				
			} else {
			
				if (totalByteCount <= nativeSize)
					percentageCont = 0.25f;
				else if ((totalByteCount > nativeSize) && (totalByteCount <= 10*nativeSize))
					percentageCont = 0.50f;
				else if ((totalByteCount > 10*nativeSize) && (totalByteCount <= 50*nativeSize))
					percentageCont = 0.75f;
				else if (totalByteCount > 50*nativeSize)
					percentageCont = 1.0f;
			
			}
			
			deviationCont = percentageCont * deviationFactor;
			totalDeviationContribtion += deviationCont;
			
			System.out.println("percentageCont: "+percentageCont);
			System.out.println("deviationCont: "+deviationCont);
			
		}
		
		//PARENTS_FITNESS
		if (PARENTS_FIT_DEF == true) {
			
			float deviationCont = 0;
			float percentageCont = (float)PARENTS_FIT_VALUE / 10;
			
			deviationCont = percentageCont * deviationFactor;
			totalDeviationContribtion += deviationCont;
			
			System.out.println("percentageCont: "+percentageCont);
			System.out.println("deviationCont: "+deviationCont);
			
		}
		
		//NATIVE CHARACTER
		if (NATIVE_CHARACTER_DEF == true) {
			
			float deviationCont = 0;
			float percentageCont = 0;
			
			int totalByteCount = fuzzValue.length;
			int deviousByteCount = 0;
			
			//valid ASCII range is 0D - 127D, beyond upto 255 is extended ASCII
			//increment deviousByteCount for all bytes that dont fall in this range
			for (int i = 0; i < totalByteCount; i++) {
				byte val = fuzzValue[i];
				if ((val < 0) || (val > 127))
					deviousByteCount++;
			}
			
			percentageCont = (float)deviousByteCount / totalByteCount;
			deviationCont = percentageCont * deviationFactor;
			totalDeviationContribtion += deviationCont;
			
			System.out.println("percentageCont: "+percentageCont);
			System.out.println("deviationCont: "+deviationCont);
			
		}
		
		//DAMAGE_CO_EFF
		if (DAMAGE_COEFF_DEF == true) {
			
			//currently DUMMY
			float deviationCont = 1 * deviationFactor;
			totalDeviationContribtion += deviationCont;
			
			
		}
		
		//USER_DEFINED
		if (USER_DEF == true) {
			
			float deviationCont = 0;
			float percentageCont = 0;
			int matchCount = 0;
			boolean matchFound = false;
			
			for (int i = 0; i < USER_DEF_VALS.size(); i++) {
				
				String matchStr = (String)USER_DEF_VALS.get(i);

				//Search for the string pattern in the byteArray
				byte [] matchByte = Strezz.strtoByteArr(matchStr);
				int found = Strezz.searchPattern(fuzzValue, matchByte);
				
				if (found >= 0) 
					matchCount++;
				
				if (USER_MODE.equalsIgnoreCase("any")) {
					if (matchCount > 0) {
						matchFound = true;
						break;
					}
						
				}
				
				if (USER_MODE.equalsIgnoreCase("all")) {
					if (matchCount >= USER_DEF_VALS.size()) {
						matchFound = true;
						break;
					}
				}
				
				
			}
			
			if (matchFound) {
				
				percentageCont = 1;
				
			}
			
			deviationCont = percentageCont * deviationFactor;
			totalDeviationContribtion += deviationCont;
			
			System.out.println("percentageCont: "+percentageCont);
			System.out.println("deviationCont: "+deviationCont);
			
		}
		
		fitValue = (int)Math.ceil(totalDeviationContribtion * 10);	
		
		System.out.println("FitParamCount: "+fitParamCount);
		System.out.println("totalDeviationContribtion: "+totalDeviationContribtion);
		System.out.println("fitValue: "+fitValue);
		
		return fitValue;
	}
	
	
}
