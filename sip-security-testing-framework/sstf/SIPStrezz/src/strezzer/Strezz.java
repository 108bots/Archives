/*SIP SECURITY TESTING FRAMEWORK - SSTF version 0.1
 */
/* Generates XML scenario and CSV files to be used with SIPp to launch strezz tests
 * Input: SIP Message Type eg: INVITE
 * Output: XML scenario and CSV files
 * Pre-requisites: The directory structure should be correct, ../extract, ../fuzz, ../template, ../testcases 
 *    XX.fz.tpl files must indicate the fuzzed fields, YY.fz file contain the fuzzed values
 *    XX denotes message type, while YY denotes the fuzz field type indicated in .tpl file
 *    eg: SIP_ip, SIP_uri etc
 */

package strezzer;
import javax.swing.*;

import java.util.*;
import java.io.*;
import java.util.Calendar;
import java.text.SimpleDateFormat;


public class Strezz extends StrezzHeader {

	public static final String XML_TEMPLATE = ".."+slash+"testcases"+slash+"template.xml";
	public static boolean XML_BOOL = false; //flag to indicate if xml file was successfully generated
	public static boolean CSV_BOOL = false; //flag to indicate if csv file was successfully generated
	public static boolean SPOOF_CSV_BOOL = false; //flag to indicate if spoof csv file was successfully generated
	
	//the byteStream of the entire event data, including sends, recvs, pauses etc
	public static ByteArrayOutputStream byteOut = new ByteArrayOutputStream();
	
	//create arrayList to hold the Message objects   -- NOT USED NOW
	public static ArrayList msgList = new ArrayList();
	

	//eventType = send, recv, pause -- for now
	//public static String eventType = new String();
	
	//Syntax = A space delimted set of parameter strings
	//parameter string eg: response="100" optional="true" request="ACK" and so on
	public static String eventParameters = new String();
	
	public static String scenarioFileName = new String ();
	
	//one such file per send message
	public static String csvFileName = new String ();


	//All the spoof keyword csv files are merged into this 1 file and this is used in the 
	//scenairo file fields.
	//One such file per send message
	public static String spoofcsvFileName = new String ();
		
	//this list contains the scenarioFileName, csvFileName(s), spoofcsvFileName(s)
	//this is returned to the gui
	public static ArrayList retList = new ArrayList();
	
	//told the eventmsg Objects
	public static ArrayList eventMsgObjects = new ArrayList();
	
	//public static void main(String[] args) {
		
	public Strezz (ArrayList eventMsgObjs, HashMap fzKwdMap, String testCaseName) {
		
		System.out.println("\n**************Running Strezz*********************\n");	      	       
		
		//clear all the lists & buffers
		retList.clear();
		msgList.clear();
		eventMsgObjects.clear();
		byteOut.reset();
		
		
		eventMsgObjects = eventMsgObjs;
		
		
		

		//test message type
	 /*      do {
	    	   msgName = null;
	    	    msgName = JOptionPane.showInputDialog(null, "Specify SIP message type", "Fuzz Message", JOptionPane.QUESTION_MESSAGE);	          
	    	    if (msgName == null) {
	    	    	System.out.println("\nGoodbye!!!");
	    	    	System.exit(0);
	 	        }
	        
	           
	       } while ((msgName == "") || (msgName == JOptionPane.UNINITIALIZED_VALUE) || (msgName.length() == 0));
	       
	       //test case name
	       do {
	    	   testCaseName = null;	 
	    	   testCaseName = JOptionPane.showInputDialog(null, "Enter a name for the testcase", "Test Case Name", JOptionPane.QUESTION_MESSAGE);	          
	    	    if (testCaseName == null) {
	    	    	continue;
	 	        }
	        
	           
	       } while ((testCaseName == "") || (testCaseName == JOptionPane.UNINITIALIZED_VALUE) || (testCaseName.length() == 0));
	       
	    */
	     
		  
		   testCaseName = testCaseName.toLowerCase();
			
		   //System.out.println("\nMessage Name is: "+msgName);
		   System.out.println("\nTestcase Name is: "+testCaseName);
		   
			
	 	   Calendar cal = Calendar.getInstance();
           SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT_NOW);
           String dateStr = sdf.format(cal.getTime());
        
           System.out.println("\nCurrent Date_time is: "+dateStr);
           
           //prepare file name for scenario and csv files
           //Scenario file name: dateStr.testCaseName.xml
           //fuzz CSV file name: dateStr.testCaseName.sendCount.csv
           //spoof CSV file name: dateStr.testCaseName_spoof.sendCount.csv
           
           //scenarioFileName = TESTCASE_DIR+dateStr+"."+testCaseName+".xml";
           scenarioFileName = TESTCASE_DIR+testCaseName+"."+dateStr+".xml";
           retList.add(scenarioFileName);
           
           System.out.println("\nScenario XML file is: "+scenarioFileName);
           
        int sendCount = 0;
        //process each event in the eventMsgList		   
		for (int msgCount = 0; msgCount < eventMsgObjects.size(); msgCount++) {
		   
		   EventMsgPassing msgObj = (EventMsgPassing)eventMsgObjects.get(msgCount);
		   
		   //reset event parameters
		   eventParameters = "";
		   
		   switch (msgObj.eventType) {
		   
		   case (short)0:  //hand send event 
			   				{
				   			    sendCount++;
				   			    String sendCountStr = Integer.toString(sendCount);
				   			    
				   			    //create fuzz and spoof csv file names
				   			    
				   			    //csvFileName = TESTCASE_DIR+dateStr+"."+testCaseName+"_fuzz."+sendCountStr+".csv";
				   			    //spoofcsvFileName = TESTCASE_DIR+dateStr+"."+testCaseName+"_spoof."+sendCountStr+".csv";
				   			   
				   			    csvFileName = TESTCASE_DIR+testCaseName+"."+dateStr+"_fuzz."+sendCountStr+".csv";
				   			    spoofcsvFileName = TESTCASE_DIR+testCaseName+"."+dateStr+"_spoof."+sendCountStr+".csv";

				   			    System.out.println("\nInjection CSV file  is: "+csvFileName);
				                System.out.println("\nSpoof Injection CSV file  is: "+spoofcsvFileName);
				                             
				   			    //create new msg object
				   				//read the corresponding tpl file and create message object  
			   					Message msg = createMessage(msgObj.sendTemplateFile);
		 	
			   					//prepare the created message. Replace fuzz keywords with [fieldx file = csvFileName]
			   					//function returns the prepared msg as a byte array
			   					//function also creates the csv injection file from the keyword.fz files
			   					
			   					byte [] prepData = prepareData_CSV(msg, fzKwdMap);
		 	 
			   					System.out.println("Prepared Message is of length: "+prepData.length);
			   					for (int k = 0; k < prepData.length; k++) {
			   						System.out.print((char)prepData[k]);
			   					}
	       	
			   					//append to the event sequence - byteOut Stream
			   					//Send Section eg:   
			   					//<send [send parameters if any]>
			   					//    <![CDATA[
			   					//            preData
			   					//    ]]>
			   					//</send>
			   					byteOut.write('\n');
				   
			   					byteOut.write('<');
			   					byte [] bsend = strtoByteArr("send");
			   					byteOut.write(bsend, 0, bsend.length);
			   					if ((eventParameters.length() == 0) || (eventParameters == null))
			   						//no event parameters
			   						byteOut.write('>');
			   					else {
			   						byte [] bsendparam = strtoByteArr(eventParameters);
			   						byteOut.write(bsendparam, 0, bsendparam.length);
			   						byteOut.write('>');
			   					}
			   					byteOut.write('\n');
				   
			   					byte [] bcdata = strtoByteArr("<![CDATA[");
			   					byteOut.write(bcdata, 0, bcdata.length);
			   					byteOut.write('\n');
			   					byteOut.write('\n');
				   
			   					byteOut.write(prepData, 0, prepData.length);
			   					byteOut.write('\n');
				   
			   					byte [] bendcdata = strtoByteArr("]]>");
			   					byteOut.write(bendcdata, 0, bendcdata.length);
			   					byteOut.write('\n');
				   
			   					byte [] bendsend = strtoByteArr("</"+"send"+">");
			   					byteOut.write(bendsend, 0, bendsend.length);
			   					byteOut.write('\n');
			
			   				}
			   			  	break;
			   			  	
		   case (short)1:   //hand recieve
		   						{  //recv and pause events dont have data (CDATA) section
			   
			   					//append to the event sequence - byteOut Stream
			   					//Receive Section eg:   
			   					//<recv [recv parameters if any]>
			   					//</recv>
			   					
			   
			   					//process the receive parameters
								String recvType = new String();
								recvType = "";
								
								if (msgObj.recvType == 0)
									recvType="request";
								else if (msgObj.recvType == 1)
									recvType="response";
					
								String optStr  = new String();
								optStr = "";
								
								if (msgObj.recvOptional == true)
									optStr = "true";
								else
									optStr = "false";
					
								eventParameters = " "+recvType+"=\""+msgObj.recvValue+"\" optional=\""+optStr+"\"";
								System.out.println("Event Parameters: "+eventParameters);
								
								//if unknown Recv Type; ignore all the eventparameters
								if ((recvType.equalsIgnoreCase("")==true))
									eventParameters = "";
								
			   					byteOut.write('\n');
			   
			   					byteOut.write('<');
			   					byte [] brecv = strtoByteArr("recv");
			   					byteOut.write(brecv, 0, brecv.length);
			   					if ((eventParameters.equalsIgnoreCase("")==true) || (eventParameters.length() == 0) || (eventParameters == null))
			   						//no event parameters
			   						byteOut.write('>');
			   					else {
			   						   						
			   						byte [] brecvparam = strtoByteArr(eventParameters);
			   						byteOut.write(brecvparam, 0, brecvparam.length);
			   						byteOut.write('>');
			   					}
			   					byteOut.write('\n');
			   
			   					byte [] bendrecv = strtoByteArr("</"+"recv"+">");
			   					byteOut.write(bendrecv, 0, bendrecv.length);
			   					byteOut.write('\n');
			   
		   						}
   			   			  	break;
		   
		   case (short)2:  //hand pause event 
		   					{  //recv and pause events dont have data (CDATA) section
			   
			   				//append to the event sequence - byteOut Stream
 			   
			   				//Pause Section eg:   
			   				//<pause [pause parameters if any]>
			   				//</pause>
			   
			   				//process the pause parameters
							String msStr = new String();
							eventParameters="";
							if (msgObj.pauseTime > 0) {
								msStr = Integer.toString(msgObj.pauseTime);
								eventParameters = " milliseconds=\""+msStr+"\"";
							}
							
							System.out.println("Event Parameters: "+eventParameters);
				
				
							byteOut.write('\n');

							byteOut.write('<');
							byte [] brecv = strtoByteArr("pause");
							byteOut.write(brecv, 0, brecv.length);
							if ((eventParameters.equalsIgnoreCase("")==true) || (eventParameters.length() == 0) || (eventParameters == null))
								//no event parameters
								byteOut.write('>');
							else {
						   						
								byte [] brecvparam = strtoByteArr(eventParameters);
								byteOut.write(brecvparam, 0, brecvparam.length);
								byteOut.write('>');
							}
							byteOut.write('\n');

							byte [] bendrecv = strtoByteArr("</"+"pause"+">");
							byteOut.write(bendrecv, 0, bendrecv.length);
							byteOut.write('\n');
							
		   					}
		   
			  			  	break;
			 
		   } //end Switch - eventtype
 
		   
		}  //end for - msgObjList
		
		
		//create byte array for the final byteStream
		byte[] byteOutArray = byteOut.toByteArray();
		   
	 	//generate xml scenario file
		generateXmlScenario(byteOutArray, testCaseName);	
		  
		System.out.println("*****DONE******");
		   
		//Prompt user to run sipp command
		if (XML_BOOL == true) {
			   System.out.println("Run SIPP with Scenario file: "+scenarioFileName+" AND Injection files:");
			   for (int i = 1; i < retList.size(); i++) { //index 0 is scenarioFile
			 	   System.out.println((String)retList.get(i));
				   		
			   }
		       
		}
		
		   
		   
		System.out.println("\n**************Strezz DONE*********************\n");
		   
	} //end main

	public ArrayList getResults() {
	    
		return retList;
		
	}
	
	
	
	
	public static void generateXmlScenario(byte [] mData, String scName) {
		int fsize = 0, extra = 2; //2 newlines within the CDATA section
		byte [] fData = null;
		
		try {
			
			System.out.println ("Opening File "+XML_TEMPLATE);
			File inFile = new File (XML_TEMPLATE);
			fsize = (int)inFile.length();
			if (inFile == null) {
				System.out.println ("Error: Opening File "+ XML_TEMPLATE);
			}
			fData = new byte [fsize];
			
			// read file into buffer
			FileInputStream fin = new FileInputStream (inFile);
			fin.read(fData, 0, fsize);    
			fin.close ();
	    } catch  (IOException e) {
	        System.out.println ( "XML Template IO Exception =: " + e );
	        e.printStackTrace();
	    } //stream exception
	    
		//prepare the entire xml file buffer
	    byte [] outData =  new byte [fsize + mData.length + scName.length() + extra];
	    byte [] pat1 = {'n', 'a', 'm', 'e', '=', '"'}; //look for 'name=""'
	    int i = 0, j = 0;  //i tracks file data, j tracks output data
	    //first set scenario name to testcase name
	    int pos = searchPattern(fData, pat1);
	    //copy upto the pattern
	    System.arraycopy(fData, i, outData, j, pos + pat1.length );
	    i = i + pos + pat1.length;
	    j = j + pos + pat1.length;
	    byte [] bscn = strtoByteArr(scName);
		System.arraycopy(bscn, 0, outData, j, bscn.length);
		j = j + bscn.length;
		outData[j] = '"';
		i++;j++;
		outData[j] = '>';
		i++;j++;
		
		//find CDATA section
		/*byte [] pat2 = {'<', '!', '[', 'C', 'D', 'A', 'T', 'A', '['}; //<![CDATA[
		pos = searchPattern(fData, pat2);
		//copy upto the pattern
		int diff = pos + pat2.length + 1 - i;
		System.arraycopy(fData, i, outData, j, diff); 
		i = i + diff;
	    j = j + diff;*/
		
	    outData[j++] = '\n';
	    //copy the message data
	    System.arraycopy(mData, 0, outData, j, mData.length);
	    j = j + mData.length;
	    outData[j++] = '\n';
	    //copy rest of the template file data
	    System.arraycopy(fData, i, outData, j, fData.length - i);
	    
	    System.out.println("XML Data:");
        for (int x = 0; x < outData.length; x++) {
        	System.out.print((char)outData[x]);
        	//System.out.print(outData[x]+" ");
        }
        
	    //write outData to file
       //create xml scenario file 
 	    try 
 	    {
 	    	File f = new File(scenarioFileName);
 	 	    boolean success = f.createNewFile();
 	 	    
 	 	    FileOutputStream fout = new FileOutputStream (f);
			fout.write(outData, 0, outData.length);
			fout.close();
			XML_BOOL = true;
 	 	   
 	    }
 	    catch (IOException e) {
 	    	System.out.println ( "create xml file exception =: " + e );
		    e.printStackTrace();
 	    	
 	    }
 	    
	}

	
	//read and process the template(.tpl) file	
	public static Message createMessage (String fileName) {
		
		//System.out.println("Template file name "+fileName);

		int inSize, keywordCount = 0, pos = -1;
		byte [] tmpData, data, tmpFuzzvalue;
		//byte [] tmpFuzzfield;
		byte [] key = {'[', 'S', 'I', 'P', '_' }; //"[SIP_" - fuzz keyword pattern
		ArrayList posIndex = new ArrayList (); //keep track of the indices of the keywords
		
		
		//String fileName = TEMPLATE_DIR+msgType+".fz.tpl";
	
	    System.out.println ("Opening File "+fileName);
		File inFile = new File (fileName);
	    inSize = (int)inFile.length();
	    if (inFile == null) {
	      System.out.println ("Error: Opening File "+fileName);
	      return null;
	    }

	    tmpData = new byte [inSize]; //used for pattern matching;gets overwritten, so as to
	                                 //count the matches
	    data = new byte [inSize]; //original data from file
	    
	  //  tmpFuzzfield = new byte [MAX_KEYWORD_LENGTH];
	    tmpFuzzvalue = new byte [MAX_VALUE_LENGTH];
	    
	    try {
	      // read file into buffer
	      FileInputStream fin = new FileInputStream (inFile);
	      fin.read(tmpData, 0, inSize);    
	      fin.close ();
	    } catch  (IOException e) {
	       System.out.println ( "IO Exception =: " + e );
	       e.printStackTrace();
	       System.exit(0);
	    } //stream exception
	    
	    //copy tmpData to data
	    System.arraycopy(tmpData, 0, data, 0, inSize);
	    
	    //count the number of fuzz keywords - pattern = "[SIP_"
	    while (true) {  //search for all occurences
	       pos = searchPattern(tmpData, key);
	       if (pos > -1) {
        	 keywordCount++;
        	 posIndex.add(new Integer(pos));
        	 tmpData[pos] = 'x'; //replace '[' by 'x'
        	 tmpData[pos+1] = 'x'; //replace 'S' by 'x'
        	 tmpData[pos+2] = 'x'; //replace 'I' by 'x'
        	 tmpData[pos+3] = 'x'; //replace 'P' by 'x'
        	 tmpData[pos+4] = 'x'; //replace '_' by 'x'
	       }
	       else
	    	 break;
	    }
	    
	    //extract  template filename from full path name
	    String tempName = new String();
	    StringTokenizer st2 = new StringTokenizer(fileName, slash);
		while (st2.hasMoreTokens()) {
			   tempName = st2.nextToken(); 
		}
        
		System.out.println("\nTemplate Name is: "+tempName);
		//lose the traling .tpl 
		String msgType = tempName.substring(0, tempName.length()-4);
		   
		
		Message msg = new Message(msgType, keywordCount, inSize);
		//populate the msg object (fuzzFields, fuzzValues, rawData)
		
		//fuzzfields & fuzzValues
		int j = 0;
		while (j < keywordCount) {
			Integer indx = (Integer)(posIndex.get(j));
			pos = indx.intValue();
			msg.fuzzPos[j] = pos;
			int k = pos+1, keyWordLen = 0;   //dont extract '[' & ']' from [SIP_xxx]
					
			while (data[k] != ']') {
				if (keyWordLen > MAX_KEYWORD_LENGTH) { //Invalid keyword/value
					msg.fuzzField[j] = null;
					msg.fuzzValue[j] = null;
					break;
				}	
			//	tmpFuzzfield[keyWordLen] = data[k];
				msg.fuzzField[j] = 	msg.fuzzField[j] + (char)data[k];
				tmpFuzzvalue[keyWordLen] = data[k];
				keyWordLen++; k++;
			}
			//System.out.println("KeywordLength: "+keyWordLen);
						
			////msg.fuzzField[j] = new String(tmpFuzzfield); //store "SIP_xxx" as a string
			
			//System.out.println("fuzzField "+ msg.fuzzField[j]);
			
			//store the length of value (MORE useful when storing/loading a message)
			//convert integer to byte array
			//copy the field length value
			//fuzzvalue contains: <4B length><space><fuzzvalue>
		    System.arraycopy(intTo4ByteArray(keyWordLen), 0, msg.fuzzValue[j], 0, FIELD_WIDTH - 1);
	 
			msg.fuzzValue[j][FIELD_WIDTH - 1] = ' ';
		    
			//copy the field value
		    System.arraycopy(tmpFuzzvalue, 0, msg.fuzzValue[j], FIELD_WIDTH, keyWordLen);
		    
		 
		    j++;
		}
		
		//copy data to raw data
		System.arraycopy(data, 0, msg.rawData, 0, inSize);
	    
	    //add msg to msgList
	    msgList.add(msg);
	    
	    //display the message structure
	    printMessageList();
	    
	    return msg;
	} //end createMessage
	
    public static void executeUnxCmd (String cmd) {
    	//RUN UNIX COMMAND
    	Process p = null;
    	String s;
    	String[] cmdArr = new String [] {"/bin/sh", "-c", cmd};
    	
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
    
	public static byte [] prepareData_CSV(Message m, HashMap fzMap) {
		
		
		byte [] bField = {'f', 'i', 'e', 'l', 'd' };
		byte [] bFile = {'f', 'i', 'l', 'e', '='};
		byte [] bfullcsvName = new byte [csvFileName.length()];
		byte [] bfullspfcsvName = new byte [spoofcsvFileName.length()];
		
		int i = 0, j = 0, cnt = 0; //i: indexes these m.rawdata, j: indexes msgData
		
		//new - replacing the 'paste' command functionality
		ArrayList fzCSVFiles = new ArrayList ();
		ArrayList spfCSVFiles = new ArrayList ();
		
		//These have been deprecated by the mergeCSVFiles function
		String fzpasteCmd = "paste -d ';' ";
		String spfpasteCmd = "paste -d ';' ";
		
		
		int spoofCnt = 0;
		//need to handle fuzz kewords and spoof keywors seperately
		
		//find the number of spoof
		for (int k = 0; k < m.fuzzCount; k++) {
			if (m.fuzzField[k].contains("SIP_") && m.fuzzField[k].contains("spoof"))
				spoofCnt++;
		}
		
		byte [] msgData = new byte [m.dataSize + (bField.length + bFile.length + csvFileName.length() + FIELD_WIDTH) * m.fuzzCount-spoofCnt + 
 	                            (bField.length + bFile.length + spoofcsvFileName.length() + FIELD_WIDTH) * spoofCnt];
		
		spoofCnt = 0;
		int fzCnt = 0;
		
		while (cnt < m.fuzzCount) {
			
			int x = m.fuzzPos[cnt];
			String fzvalStr = byteArrtoStr(m.fuzzValue[cnt]);
			String fzfname = new String (); //FUZZ_DIR;
			String spfname = new String (SPOOF_DIR);	
			
			//fuzz field same as fuzz value = fuzz keyword
			if (m.fuzzField[cnt].equals(fzvalStr)) { //store [field...]
			//System.out.println("Oh Yeah!!!");
				
				int diff = x - i + 1;
				
				//spoof keyword
				if (m.fuzzField[cnt].contains("SIP_") && m.fuzzField[cnt].contains("spoof")) {
					
					spfname = spfname + m.fuzzField[cnt] + ".csv";
					spfpasteCmd = spfpasteCmd + spfname + " ";
					
					spfCSVFiles.add(spfname);
					
					System.arraycopy(m.rawData, i, msgData, j, diff);
					i = i + diff;
					j = j + diff;
					System.arraycopy(bField, 0, msgData, j, bField.length);
					j = j + bField.length;
					String cntStr = Integer.toString(spoofCnt);
					byte [] bcnt = strtoByteArr(cntStr);
					System.arraycopy(bcnt, 0, msgData, j, bcnt.length);
					j = j + bcnt.length;
					msgData[j++] = ' ';
					System.arraycopy(bFile, 0, msgData, j, bFile.length);
					j = j + bFile.length;
					msgData[j++] = '"';
					bfullspfcsvName = strtoByteArr(spoofcsvFileName);        	
					System.arraycopy(bfullspfcsvName, 0, msgData, j, bfullspfcsvName.length);
					j = j+ bfullspfcsvName.length;
					i = i + m.fuzzField[cnt].length();
					msgData[j++] = '"';
					
					spoofCnt++;
					
				} else {  //fuzz key word
					
					//Read the keyword-FuzzFile Mapping from the hashMap
					String mappedFzFile = null;
					
					mappedFzFile = (String)fzMap.get(m.fuzzField[cnt]);
					
					if (mappedFzFile != null)	
					  fzfname = fzfname + mappedFzFile;
					else     //if any error in mapping, just use default
					  fzfname = fzfname + m.fuzzField[cnt] + ".fz";
					
					fzpasteCmd = fzpasteCmd + fzfname + " ";
					
					fzCSVFiles.add(fzfname);
					
					System.arraycopy(m.rawData, i, msgData, j, diff);
					i = i + diff;
					j = j + diff;
					System.arraycopy(bField, 0, msgData, j, bField.length);
					j = j + bField.length;
					String cntStr = Integer.toString(fzCnt);
					byte [] bcnt = strtoByteArr(cntStr);
					System.arraycopy(bcnt, 0, msgData, j, bcnt.length);
					j = j + bcnt.length;
					msgData[j++] = ' ';
					System.arraycopy(bFile, 0, msgData, j, bFile.length);
					j = j + bFile.length;
					msgData[j++] = '"';
					bfullcsvName = strtoByteArr(csvFileName);        	
					System.arraycopy(bfullcsvName, 0, msgData, j, bfullcsvName.length);
					j = j+ bfullcsvName.length;
					i = i + m.fuzzField[cnt].length();
					msgData[j++] = '"';
					
					fzCnt++;
				}
				
				
				//System.out.println("i:"+i+" j: "+j+" x:"+x);
			}
			else { //user defined fuzz/spoof value, so store directly
					System.out.println("Oh No!!!"); //store fuzzvalue
					int fzLen = lenFuzzValue(m.fuzzValue[cnt]);
					System.arraycopy(m.fuzzValue[cnt], FIELD_WIDTH, msgData, j, fzLen);
					j = j + fzLen;
					i = i + m.fuzzField[cnt].length() + 1; //+1 to read past ']' in [SIP_xx]
                    
			}
			
			cnt++;
		}
		
		//prepare the fuzz paste command and append to created csvfile
		fzpasteCmd = fzpasteCmd + ">> " + csvFileName;
		
		System.out.println(fzpasteCmd);
	
		//prepare the spoof paste command and append to created csvfile
		spfpasteCmd = spfpasteCmd + ">> " + spoofcsvFileName;
		
		System.out.println(spfpasteCmd);
		
		//if pasteCmd doesnt contain "SIP_", means that no keywords defined so no need to create
		//the .csv file. 
		if (fzpasteCmd.indexOf("SIP_") != -1) {
			CSV_BOOL = true; 		      
		}
		else
			CSV_BOOL = false;
		
		if (spfpasteCmd.indexOf("SIP_") != -1) {
			SPOOF_CSV_BOOL = true; 		      
		}
		else
			SPOOF_CSV_BOOL = false;
		
		if (CSV_BOOL == true) {
			
			//choose the RANDOM or SEQUENTIAL execution of testcases
		/*	String exeStyle = JOptionPane.showInputDialog(null, "Specify SEQUENTIAL or RANDOM execution style", "Execution Style", JOptionPane.QUESTION_MESSAGE);	          
			if ((exeStyle == null) || (exeStyle.equals(""))) {
				System.out.println("Using Default Style: SEQUENTIAL");
				exeStyle = "SEQUENTIAL";
			} */
			
			//add the filename to returnList
		    retList.add(csvFileName);
		     
			String exeStyle = "SEQUENTIAL"; //or "RANDOM"
			
			//call mergeCSV files
			System.out.println("Generating Fuzz CSV file...");
			mergeCSVFiles(fzCSVFiles,csvFileName, exeStyle); //comment this if using 'paste'
			
			/* //remove this comment - if using unix 'paste' command
			//create csvfile with the execution style 
			try 
			{
				File f = new File(csvFileName);
				boolean success = f.createNewFile();
 	 	    
				BufferedWriter fout = new BufferedWriter(new FileWriter(csvFileName)); 
				fout.write(exeStyle);
				fout.write("\n");
				fout.close();
 	 	   
			}
			catch (IOException e) {
				System.out.println ( "create Fuzz csv file exception =: " + e );
				e.printStackTrace();
 	    	
			}
 	    
			System.out.println("Generating Fuzz CSV file...");
			executeUnxCmd(fzpasteCmd);
			*/
			
			
		}
		
		
		if (SPOOF_CSV_BOOL == true) {
			//choose the RANDOM or SEQUENTIAL execution of testcases

			//add the filename to returnList
			retList.add(spoofcsvFileName);
			 
			String exeStyle = "SEQUENTIAL"; //or "RANDOM"
			
			//call mergeCSV files
			System.out.println("Generating Spoof CSV file...");
			mergeCSVFiles(spfCSVFiles,spoofcsvFileName, exeStyle); //comment this if using 'paste'
			
			/*//remove this comment - if using unix 'paste' command 
			//create csvfile with the execution style 
			try 
			{
				File f = new File(spoofcsvFileName);
				boolean success = f.createNewFile();
 	 	    
				BufferedWriter fout = new BufferedWriter(new FileWriter(spoofcsvFileName)); 
				fout.write(exeStyle);
				fout.write("\n");
				fout.close();
 	 	   
			}
			catch (IOException e) {
				System.out.println ( "create Spoof csv file exception =: " + e );
				e.printStackTrace();
 	    	
			}
 	    
			System.out.println("Generating Spoof CSV file...");
			executeUnxCmd(spfpasteCmd);
			*/
			
			
		}
		
		
		//copy the rest till end of message
		System.arraycopy(m.rawData, i, msgData, j, m.dataSize - i);
		//System.out.println("i:"+i+" j: "+j+" datasize:"+m.dataSize);	
		
		j += m.dataSize - i;
		
		//System.out.println ( "j: " + j + "m.datasize: " + m.dataSize + "msgdatalen: "+msgData.length);
		
			
		//msgData contains trailing nulls. so copy it to another array of actual size
		//which is 'j' 
		byte [] tempRetData = new byte [j];
		System.arraycopy(msgData, 0, tempRetData, 0, j);
		
		return tempRetData;
	}
	
	/*This function does the functionality of the unix 'paste' command
	 * Added Later for portability
	 * 
	 * NOTE: This function is much slower than the Unix 'paste' command, so if are sure to 
	 * run on Unix system, dont use this function. Paste works faster.
	*/
	
	public static void mergeCSVFiles (ArrayList srcCSVFiles, String dstCSVFile, String csvStyle) {
		
		//Merge the different source csv files int into one destination csv file
		//Dst file length = length of the longest file.
		//if eof reached for smaller files. Restart at the beginning
		
		int maxLength = 0;
		
		try {

		    //create the WRITE streams
			FileWriter ofstream = new FileWriter(dstCSVFile);
		    BufferedWriter out = new BufferedWriter(ofstream);
		    out.write(csvStyle+"\n");
		    
		    //create the READ streams
		    RandomAccessFile [] inRAFarr = new RandomAccessFile[srcCSVFiles.size()];
		    
		    //open files, find max length file
		    for (int i = 0; i < srcCSVFiles.size(); i++) {

		    	inRAFarr[i] = new RandomAccessFile((String)srcCSVFiles.get(i), "r");
		    	long lastRec = inRAFarr[i].length();
		    	
		    	FileReader fileRead = new FileReader((String)srcCSVFiles.get(i));
		    	LineNumberReader lineRead = new LineNumberReader(fileRead);
		    	lineRead.skip(lastRec);
		    	
		    	int countRec = lineRead.getLineNumber()-1;
			    
		    	if (countRec > maxLength)
		    		maxLength = countRec;

		    	//rewind the RAF files
		    	inRAFarr[i].seek(0);
		    }
		    
		    //main write loop; runs maxLength times
		    for (int i = 0; i < maxLength; i++) {
		    	
		    	String outStr = new String ();
		    	//read from each input file
		    	for (int k = 0; k < inRAFarr.length; k++) {
		    		
		    		String str = inRAFarr[k].readLine();
		    		
		    		if (str == null) {
		    			inRAFarr[k].seek(0);		
		    			str = inRAFarr[k].readLine();
		    		}
		    		outStr += str+";";
		    		
		    	}
		    	out.write(outStr+"\n");
		    			    	
		    }
		    
		    //close all streams
			for (int j = 0; j < inRAFarr.length; j++) {
				inRAFarr[j].close();
			}
			out.close();
			
		} catch (IOException e) {
			
			System.out.println ( "Creating Master CSV file IO Exception =: " + e );
		    e.printStackTrace();
			
		}
		
	}
	
	
	public static int lenFuzzValue(byte [] fzVal) {
		
		int len = 0, i, j;
		
		len = (1000*fzVal[0]+100*fzVal[1]+10*fzVal[2]+1*fzVal[3]);
		System.out.println("   Length is: "+len);
		System.out.print("   Value is: ");
		i = 5; j = 0;
		while (j < len && i < MAX_VALUE_LENGTH) {
        	System.out.print((char)fzVal[i]);
        	//System.out.print(msg.rawData[i]);
        	i++; j++;
        }
		System.out.println("");
		return len;
	}
	
	public static void printMessageList() {
		
		Message msg;
		
		for (int i = 0; i < msgList.size(); i++) {
	        msg = (Message)msgList.get(i);
	        System.out.println("Message["+i+"]:");
	        System.out.println("TYPE: "+msg.fuzzMethod);
	        System.out.println("FuzzCount: "+msg.fuzzCount);
	        int x = 0;
	        while (x < msg.fuzzCount) {
                System.out.println("FuzzPos["+x+"]"+msg.fuzzPos[x]);
	        	System.out.println("FuzzField["+x+"]: "+msg.fuzzField[x]);
	        	//function to read and print the fuzzvalue
	        	System.out.println("FuzzValue["+x+"]");
	        	lenFuzzValue(msg.fuzzValue[x]);
	        	x++;
	        }
	        System.out.println("Message Data:");
	        for (int j = 0; j < msg.dataSize; j++) {
	        	System.out.print((char)msg.rawData[j]);
	        	//System.out.print(msg.rawData[j]);
	        }
	        
	    }
	}
	
	public static String byteArrtoStr (byte [] val) {
        int len = 0, i, j;
		String valStr = "";
		len = (1000*val[0]+100*val[1]+10*val[2]+1*val[3]);
		i = 5; j = 0;
		while (j < len && i < MAX_VALUE_LENGTH) {
        	//System.out.print((char)fzVal[i]);
            valStr = valStr+(char)val[i];
        	i++; j++;
        }
		//System.out.println("String Fuzz Value is: "+valStr);
		return valStr;
	}
	
	public static byte [] strtoByteArr (String str) {
        int i;
        byte [] strByte = new byte [str.length()];
       // System.out.println("CSV file name bytes:");
        for (i = 0; i < str.length(); i++) {
        	strByte[i] = (byte)str.charAt(i);
       // 	System.out.print((char)strByte[i]);
        }
        
		return strByte;
	}
	
	public static byte [] intTo4ByteArray(int num) {
		byte [] revlength = new byte[4];
		byte [] length = new byte[4];
		
		java.util.Arrays.fill(revlength, 0, 4, (byte)0);
		
		//extract every digit in the length value and store in byte array
		//digits extracted in reverse order, need to be reordered before storing
		
		int rem = 0, y = 0;
		
		do {
		  rem = num % 10;
		  revlength[y] = (byte)rem;
		  y++;
		  num = (num - rem) / 10 ;
		} while (num != 0);
		
		for (int i = 0, j = 3; j >= 0; j--, i++ )
		  length[i] = revlength[j];
		
		return length;	
	}
	
	// Knuth-Morris-Pratt Pattern Matching
	//searchPattern - searches for a pattern, returns starting index if found, else returns -1
	public static int searchPattern(byte[] data, byte[] pattern) {
	    int[] failure = computeFailure(pattern);
     	int j = 0;
	    if (data.length == 0) 
	    	return -1;

	    for (int i = 0; i < data.length; i++) {
	       while (j > 0 && pattern[j] != data[i]) {
	           j = failure[j - 1];
	       }
	       if (pattern[j] == data[i]) { j++; }
	       if (j == pattern.length) {
	    	   return i - pattern.length + 1;
	       }
	    }
	    
	    return -1;
	
	}//end searchPattern

	
	// Computes the failure function using a boot-strapping process,
	//where the pattern is matched against itself.
	private static int[] computeFailure(byte[] pattern) {
		int[] failure = new int[pattern.length];
		int j = 0;
	
		for (int i = 1; i < pattern.length; i++) {
			while (j > 0 && pattern[j] != pattern[i]) {
				j = failure[j - 1];
			}
			if (pattern[j] == pattern[i]) {
				j++;
			}
			failure[i] = j;
		}

		return failure;
	}//end computeFailure
	

	
	
} //Class Strezz

class Message implements Serializable {
	
	String fuzzMethod; //stores template filename (excluding the .tpl extension)
	int dataSize;
	int fuzzCount; //no: of fuzzed fields (fuzz keywords in the message)
	String [] fuzzField;
	int [] fuzzPos; //position of where the fuzz keywords are found in the raw message
	
	 //NOTE: Current SSTF wizard doesnt allow user defined fuzz values
    //hence fuzzValue:fuzzField logic is not useful

	byte [][] fuzzValue; //array of values of each corresponding fuzzField 
	                   //(either fuzz keyword or user defined value) - binary
	                  //The first 5 bytes(max) store the length of the value 4B for length +
                     //1B for the space. This followed by the actual field value
	
	                
	
	byte [] rawData; //entire SIP message in binary form
	
	Message(String mName, int fzCount, int dSize) {
	  	dataSize = dSize; 
		fuzzCount = fzCount;
		fuzzMethod = mName;
		fuzzField = new String [fuzzCount];
		fuzzPos = new int [fuzzCount];
		fuzzValue = new byte [fuzzCount][Strezz.MAX_VALUE_LENGTH + Strezz.FIELD_WIDTH];

		//initialize byte array to 0
		for (int i = 0; i < fuzzCount; i++) {
		   java.util.Arrays.fill(fuzzValue[i], 0, Strezz.MAX_VALUE_LENGTH + Strezz.FIELD_WIDTH, (byte)0);
		   fuzzField[i]="";
		}

		rawData = new byte [dSize];
	}
		 
}
