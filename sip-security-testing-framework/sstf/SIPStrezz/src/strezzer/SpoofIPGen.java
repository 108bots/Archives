/*SIP SECURITY TESTING FRAMEWORK - SSTF version 0.1
 */
/* SSTF WIZARD - IP address Range Generator
 * INPUT : [IP net] [mask] [count|all] [src|dst]
 *        ex: 10.10.10.0 24 all src
 *            11.11.0.0 16 500 dst
 * OUTPUT: All IP address in the range, written to the corresponding csv file 
 * 
 */
package strezzer;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.StringTokenizer;

//cmdStr = StrezzHeader.SPOOF_DIR+"spoof_ip_gen_sstf "+src_ip+" "+
//    src_msk+" dummy add "+src_cnt+" src";
public class SpoofIPGen {
	
	public SpoofIPGen (String net, String mask, String count, String type) {
		
		System.out.println("******************RUNNING SPOOF IP GEN*************************************");
		
		System.out.println("Net: "+net+" Mask: "+mask+" Count: "+count+" Type: "+type);
		
		if ((net == null) || (mask == null) || (count == null) || (type == null)) {
			System.out.println("NULL input");
			return;
		}
		
		if ((net == "") || (mask == "") || (count == "") || (type == "")) {
			System.out.println("EMPTY input");
			return;
		}
		
		String outCSVfname = new String();
        //form the CSV filename
		outCSVfname = StrezzHeader.SPOOF_DIR+"SIP_"+type+"_ip_spoof.csv";
		
		 StringTokenizer st1 = new StringTokenizer(net, ".");
		 //validate IP
		 if ((st1.countTokens() != 4) || (st1 == null)) {
			 System.out.println("Malformed IP net");
			 return;
		 }
		 
		 
		  String oct1Str = st1.nextToken(); 
		  String oct2Str = st1.nextToken();
		  String oct3Str = st1.nextToken();
		  String oct4Str = st1.nextToken();
		  
		  //validate net
		  if (Integer.parseInt(oct4Str) != 0) {
			  System.out.println("Net Address must end in 0 octet");
			  return;
		  }
		
		  //validate mask
		  int maskValue = Integer.parseInt(mask);
		  if (maskValue > 32) {
			  System.out.println("Wrong Mask");
			  return;
		  }
		  
		  if (maskValue > 0 && maskValue < 9) {
			  if ((Integer.parseInt(oct4Str) != 0)&&(Integer.parseInt(oct3Str) != 0)&&(Integer.parseInt(oct2Str) != 0)) {
				  System.out.println("Mask - Net Mismatch");
				  return;
			  }  
		  }
		  
		  if (maskValue > 8 && maskValue < 17) {
			  if ((Integer.parseInt(oct4Str) != 0)&&(Integer.parseInt(oct3Str) != 0)) {
				  System.out.println("Mask - Net Mismatch");
				  return;
			  }  
		  }
		  
		  if (maskValue > 16 && maskValue < 33) {
			  if (Integer.parseInt(oct4Str) != 0) {
				  System.out.println("Mask - Net Mismatch");
				  return;
			  }  
		  }
		  
		  int countValue =0;
		  
		  if (count.equalsIgnoreCase("all")) {
			  
			  countValue = (int)Math.pow(2, 32 - maskValue);
			  countValue -= 4;
			  
		  } else {
			countValue = Integer.parseInt(count);  
		  }
		  
		  if (countValue == 0) {
			  System.out.println("0 count Value");
			  return;
		  }
		  
		System.out.println("countValue: "+countValue);
		  
		try {
		    
		    FileWriter fstream = new FileWriter(outCSVfname);
		    BufferedWriter out = new BufferedWriter(fstream);
		   	    
		   	    	    
		    
		  //out.write("SEQUENTIAL\n"); //will be added in them merged CSV
		    int cnt = 0;
		    for (int i = Integer.parseInt(oct1Str); i < 255; i++) {
                if (cnt >= countValue) break;
                for (int j = Integer.parseInt(oct2Str); j < 255; j++) {
                	if (cnt >= countValue) break;
                  for (int k = Integer.parseInt(oct3Str); k < 255; k++) {
                	  if (cnt >= countValue) break;
                    for (int l = Integer.parseInt(oct4Str); l < 255; l++) {
                      if (cnt >= countValue) break;
                      if (l == 0) continue;
                      String addrStr = new String ();
                      addrStr = String.valueOf(i);
                      addrStr += ".";
                      addrStr += String.valueOf(j);
                      addrStr += ".";
                      addrStr += String.valueOf(k);
                      addrStr += ".";
                      addrStr += String.valueOf(l);
                      
                      //System.out.println(addrStr);
                      out.write(addrStr+"\n");
                
                      cnt++;

                    }
                  }
                }
              }
		    
		    out.close();
		    
		} catch (IOException e){
			
			System.out.println ( "Creating IP Spoof file IO Exception =: " + e );
		    e.printStackTrace();
		}
		
		System.out.println("******************SPOOF IP GEN  DONE*************************************");
	}

}
