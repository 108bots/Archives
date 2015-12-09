/*SIP SECURITY TESTING FRAMEWORK - SSTF version 0.1
 */
/* SSTF WIZARD - User Range Generator  
 * INPUT : [userRange] [src|dst] ex: 123xx src
 * OUTPUT: 12300 - 12399 written to the corresponding csv file
 */
package strezzer;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.text.*;

public class CallExtensionGen {
	
	public CallExtensionGen (String range, String type) {
		
		System.out.println("******************RUNNING CALL EXTENSION GEN*************************************");
		
		System.out.println("Range: "+range+" Type: "+type);
		
		if ((range == null) || (type == null) || (range == "") || (type == "")) {
			System.out.println("EMPTY/NULL input");
			return;
		}
		
		String outCSVfname = new String();
		
		
		if ((range.charAt(range.length()-1) != 'x') && (range.charAt(range.length()-1) != 'X')) {  //no range specified
			System.out.println("NO RANGE specified (use: x or X to specify range)");
		    return;
		}
        //form the CSV filename
		outCSVfname = StrezzHeader.SPOOF_DIR+"SIP_"+type+"_user_spoof.csv";
		
		try {
		    
		    FileWriter fstream = new FileWriter(outCSVfname);
		    BufferedWriter out = new BufferedWriter(fstream);
		    
		    //out.write("SEQUENTIAL\n"); //will be added in them merged CSV
		    
		    //find the number of x's
		    int i = range.length()-1;
		    int rangeCount = 0;
		    while ((range.charAt(i)== 'x')|| (range.charAt(i)== 'X')) {
		    	i--;
		    	rangeCount++;
		    }
		    String rangePrefix = range.substring(0, i+1);
		    String maxStr = new String ();
		    for (int j = 0; j < rangeCount; j++)
		    	maxStr += "9";
		    
		    int maxValue = Integer.parseInt(maxStr);
		    
		    System.out.println("RangeCount: "+rangeCount+" maxValue: "+maxValue);
		    
		    NumberFormat nf = NumberFormat.getInstance();
		    nf.setMinimumIntegerDigits(rangeCount);
		    
		    
		 
		    for (int k = 0; k <= maxValue; k++) {
		    	
		       String str = rangePrefix+nf.format(k);  
		       out.write(str+"\n");
		      // System.out.println(str);
		    }
		    
		    out.close();
		    
		} catch (IOException e){
			
			System.out.println ( "Creating User Spoof file IO Exception =: " + e );
		    e.printStackTrace();
		}
		
		System.out.println("******************CALL EXTENSION GEN  DONE*************************************");
	}

}
