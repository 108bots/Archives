/*SIP SECURITY TESTING FRAMEWORK - SSTF version 0.1
 *Uses the DOM to parse the fuzz Rules XML file and build the Rules objects.
 * Then depending on 1) rulename, 2) msg type, 3) testdevice type or 4) targetdevice type
 * processes the rules and generates the corresponding template (*.tpl) file
 * This .tpl file is a test case. It can be used with Strezz.java to generate the 
 * actual test XML scenario and CSV files, that will be used by SIPp.
 * Author: Hemanth Srinivasan
 * Year: 2008
 */

//Pending: Modify to take rulename, msgname, typename parameters

package strezzer;

import javax.swing.*;
import java.util.*;
import java.io.*;

import javax.xml.parsers.*;
import org.w3c.dom.*;
import org.xml.sax.*;

public class FuzzRulesParse extends StrezzHeader {
	
	public static ArrayList RulesList = new ArrayList();
	
	public static String allheaderfname = RULES_DIR+"allHeaders.tpl";
	public static String allheaderValue = new String();
	
	public static FileWriter tplout;
	public static File tpl;
     
	//the byteStream of the entire event data, including sends, recvs, pauses etc
	public static ByteArrayOutputStream byteOut = new ByteArrayOutputStream();
	
	//create arrayList to hold the Message byte data after rule processing
	public static ArrayList msglinesList = new ArrayList();
	

	//eventType = send, recv, pause -- for now
	public static String eventType = new String();
	
	//Syntax = A space delimted set of parameter strings
	//parameter string eg: response="100" optional="true" request="ACK" and so on
	public static String eventParameters = new String();
	
	public static String inRulesfname = new String ();
	
	//NOTE: Currently, if processing more that one rule, in the same file
	//these values will get overwritten to the most recently parsed rule
	//All these constitute output tpl file name 
	public static String outRuleName = new String ();
	public static String outRuleMsgType = new String ();
	public static String outSelfType = new String ();
	public static String outTargetType = new String ();
	
	//public static void main(String[] args) {
	public FuzzRulesParse (String rulesfname) {
		 
		System.out.println("\n**************Running FuzzRulesParse*********************\n");
		
/*     do {
 	   		inRulesfname = null;
 	   		inRulesfname = JOptionPane.showInputDialog(null, "Specify Name of xml Rules file:", "Fuzz Test Case Rules", JOptionPane.QUESTION_MESSAGE);	          
 	    	if (inRulesfname == null) 
 	    	{
 	    		System.out.println("\nGoodbye!!!");
 	    		System.exit(0);
	        }
        
  		} while ((inRulesfname == "") || (inRulesfname == JOptionPane.UNINITIALIZED_VALUE) || (inRulesfname.length() == 0));
  
*/
		
		
		//read rules filename
		inRulesfname = rulesfname;
		
		
		//Parse the Rules file
		parseRulesXML(inRulesfname);
	     
         //load the AllHeaders template into a local buffer
		 try {
		        BufferedReader in = new BufferedReader(new FileReader(allheaderfname));
		        String str;
		        while ((str = in.readLine()) != null) {
		            allheaderValue += str.toLowerCase();
		        }
		        in.close();
		 } catch (IOException e) {
		    	System.out.println ( "Reading AllHeaders file exception =: " + e );
			    e.printStackTrace();
		 }
		
		 // System.out.println ( "AllHeaders file Data: "+allheaderValue);
		 // System.out.println ( "AllHeaders file length: "+allheaderValue.length());
		 
		 
		 //Output template file 
         String outTPLfname = RULES_TPL_DIR+outRuleName+"_"+outRuleMsgType+"_"+
                              outSelfType+"_"+outTargetType+".tpl";
        
         try {
        	    tpl = new File(outTPLfname);
	 	 	    boolean s2 = tpl.createNewFile();
	 	 	    tplout = new FileWriter(tpl, true);
	 	 	    
	 	 	   
	 	 } catch (IOException e) {
	 	    	System.out.println ( "create rules template file exception =: " + e );
			    e.printStackTrace();
	 	    	
	 	 }
	 	 
	     for (int i = 0; i < RulesList.size(); i++) {
		     //display the rules from rules List
	    	 printRules((Rules)RulesList.get(i));
	    	 
	    	 //process each rule based create the message
	    	 processRule((Rules)RulesList.get(i));

	    	 //write msglineList to outtpl file 
	    	 try {
	    		 
	    		  /*  useful when processing multiple rules in a single file
				   String ruleheading = "Rule #"+i;
				   tplout.write(ruleheading, 0, ruleheading.length());
				   tplout.write('\n');
				   */
	    		 
	    		   for (int j = 0; j < msglinesList.size(); j++) {
	    			   String mline = (String)msglinesList.get(j);
	    			   tplout.write(mline, 0, mline.length());
	    			   if (!mline.equals("\n"))
	    				   tplout.write('\n');
	    		   }
	    		   tplout.write('\n');
		   		  
		   	   } catch (IOException e) {
		   	      	System.out.println ( "writing rules template streams exception =: " + e );
		   	      	e.printStackTrace();
	 	    	
		   	   }
		   	   
	    	 //clear msglist
             msglinesList.clear();
	     }
	     
	     //close the template file streams 
		   try {
			   tplout.close();
		   }
		   catch (IOException e) {
	 	    	System.out.println ( "closing rules template streams exception =: " + e );
			    e.printStackTrace();
	 	    	
	 	    }

	    
		   System.out.println("\n**************FuzzRulesParse DONE*********************\n");
	     
	}
	
	public static void processRule (Rules r) {
		
		//read the build Order and process each header in that order
		StringTokenizer st1 = new StringTokenizer(r.buildValue, ",");
		int tokenCount = st1.countTokens();
		
		for (int i = 0; i < tokenCount; i++) {
			String hdr = st1.nextToken();
			System.out.println ( "Token "+i+": "+hdr);
			
			//if "start" keyword -> first line -> messagetype
			if (hdr.equalsIgnoreCase("start"))  
				hdr = r.msgType;
			
			//if "lb" keyword -> line break, add directly to msglinesList
			if (hdr.equalsIgnoreCase("lb")) {
				msglinesList.add(new String("\n"));
				continue;
			}	
			
			//find the original hdr value in the allheaderValue
			String bsrchdr = "<"+hdr+">";
			
			int bpos = allheaderValue.indexOf(bsrchdr.toLowerCase());
			if (bpos < 0) {
				System.out.println ( "Warning: Definition for Header '"+hdr+"' not found in"+allheaderfname);
				continue;
			}
			String esrchdr = "</"+hdr+">";
			int epos = allheaderValue.indexOf(esrchdr.toLowerCase());
			if (epos < 0) {
				System.out.println ( "Warning: Closing tag for Header '"+hdr+"' not found in"+allheaderfname);
				continue;
			}
			String orghdrvalue = allheaderValue.substring(bpos+bsrchdr.length(), epos);
			System.out.println ( "Allheader Value for Token "+i+": "+orghdrvalue);
			
			//processes each header as per the rule, adds the new header to msglinesList
			processHeader ((String)r.mapHeader.get(hdr), orghdrvalue);
			
			
		}
		  
		//print msglinesList
		System.out.println ("MsglinesList Contains:");
		for (int j = 0; j < msglinesList.size(); j++)
			System.out.println(msglinesList.get(j));
		
		
	}
	
	public static void addHeader2MsgLine(String hdr) {
		//the original header from Allheaders.tpl might span multiple
		//lines delimite by ### , add each line as list member
		StringTokenizer st1 = new StringTokenizer(hdr, "###");
		
		while (st1.hasMoreTokens()) {
			msglinesList.add(st1.nextToken());
		}
		
		
	}
	
	public static void processHeader (String hdrule, String orghdr) {
	
		//no rule defined for this, just add orghdr to msglinesList and return
		if ((hdrule == null) || (hdrule.length() == 0)) {
			addHeader2MsgLine(orghdr);
		    return;
		}
        
		//process and interpret the rule
	    //if spanning mutiple lines, split the rule
		//process each rule: find occurence[arg1 of rule] of pattern[arg2 of rule] and replace it with [arg3 of rule] 
		
		StringTokenizer st1 = new StringTokenizer(hdrule, "\n");
		
		while (st1.hasMoreTokens()) {
			
			String srule = st1.nextToken().trim();
			System.out.println ( "Trimmed rule: "+srule);
			//interpret the rule
			StringTokenizer st2 = new StringTokenizer(srule, " ");
			int tokenCount = st2.countTokens();
			if (tokenCount != 3) {
				System.out.println ( "Ignoring Malformed rule: "+srule);
				continue;
			}
			
			int loc = 0;
			String arg1 = st2.nextToken();
			String arg2 = st2.nextToken();
			String arg3 = st2.nextToken();
			
			try {
				
				if (!arg1.equalsIgnoreCase("all"))
					loc = Integer.parseInt(arg1);
			
				
			} catch (NumberFormatException e) {
	 	    	System.out.println ( "Integer value expected in place of: " + arg1 );
			    e.printStackTrace();
			    System.out.println ( "Ignoring malfomed rule: "+srule);
			    continue;
	 	    }
			
			//count the no: of occurences of the pattern (arg2) in orghdr
			int firstindx = orghdr.indexOf(arg2);
			if (firstindx < 0) {
				System.out.println ( "Pattern not found in Orignal header: "+arg2);
				continue;
			}
			int lastindx = orghdr.lastIndexOf(arg2);
			int indxrange = -1;
			int patcount = 0;
			while (indxrange < lastindx) {
				indxrange = orghdr.indexOf(arg2, indxrange+arg2.length());
			    if (indxrange > -1)
			    	patcount++;
			}
			
			if (((loc <= 0) || (loc > patcount)) && (!arg1.equalsIgnoreCase("all"))) {
				System.out.println ( "Occurence value doenst match with Original header: "+arg1);
				continue;
			}
			
			//create array to store the occurence positions
			int [] posIndx = new int [patcount];
			indxrange = -1;
			for (int i = 0; i < patcount; i++) {
				indxrange = orghdr.indexOf(arg2, indxrange+arg2.length());
			    if (indxrange > -1)
			    	posIndx[i] = indxrange;
			}
			
			for (int i = 0; i < patcount; i++) 
				System.out.println ("Pattern Pos:"+posIndx[i]);
			
					
			String rephdr = new String();
			
			if (arg1.equalsIgnoreCase("all")) { //replace everywhere
				int startpos = 0, endpos = 0;
				for (int i = 0; i < patcount; i++) {
					
					endpos = posIndx[i];
					
					rephdr += orghdr.substring(startpos, endpos);
					rephdr += arg3;
					
					startpos = endpos + arg2.length();
								
				}
				//copy to the end of orghdr
				rephdr += orghdr.substring(startpos, orghdr.length());
				
			} else {  //replace only at position loc
						
				rephdr += orghdr.substring(0, posIndx[loc-1]);
				rephdr += arg3;
				rephdr += orghdr.substring(posIndx[loc-1]+arg2.length(), orghdr.length());
			
				//System.exit(0);
			}
		
			System.out.println ("Orghdr: "+orghdr);
			System.out.println ("Rephdr: "+rephdr);
			
			//after processing one rule, use the processes hdr as the orghdr for next rule
			orghdr = rephdr; 
		} 
		
		//add the final processed hdr to the msglist
		addHeader2MsgLine(orghdr);
		
	}
	
	
	public static void printRules (Rules r) {

		System.out.println("\nRule Name: "+r.ruleName);
		System.out.println("\nMessage Type: "+r.msgType);
		System.out.println("\nSelf Type: "+r.selfType);
		System.out.println("\nTarget Type: "+r.targetType);
		System.out.println("\nBuild Order String: "+r.buildValue);
		System.out.println("\nHeaders:");
		
		Set set = r.mapHeader.entrySet();
		
		Iterator i = set.iterator();
        
	    while(i.hasNext()){
	      Map.Entry me = (Map.Entry)i.next();
	      String key = (String)me.getKey();
	      String value = (String)me.getValue();
	      System.out.println("Header TYPE: "+key+ " VALUE: "+value.trim());
	    }
	    
	}
	
	public static void parseRulesXML (String fname) {
		
		Document doc = parseXmlFile(fname, false);
		parseDocumentObject(doc);
						
	}
	
	public static void parseDocumentObject(Document dom){
		//get the root elememt
		Element docEle = dom.getDocumentElement();
		
		//get a nodelist of ALL <rule> elements
		NodeList nl = docEle.getElementsByTagName("rule");
		if(nl != null && nl.getLength() > 0) {
			for(int i = 0 ; i < nl.getLength();i++) {
				//get the rule element
				Element el = (Element)nl.item(i);
				
				//Add Matching condition here. Rules with particular name, msgtype, selfType
				//or target type
				
				
				//get the Rules object
				Rules r  = getRule(el);
				//add it to Ruleslist
				RulesList.add(r);
				
			}
		}
	}
	
	//process the rule element, constructs a Rules object and returns it
	public static Rules getRule(Element el) {
		
		//Create a new rule with the value read from the xml nodes
		Rules rule = new Rules();
				
		//get rule name
		rule.ruleName = el.getAttribute("name");
		
		outRuleName = rule.ruleName;
		
		NodeList msgnl = el.getElementsByTagName("message");
		NodeList selfnl = el.getElementsByTagName("testdevice");
		NodeList targetnl = el.getElementsByTagName("targetdevice");
		NodeList headernl = el.getElementsByTagName("header");
		NodeList buildnl = el.getElementsByTagName("build");
		
		//except for header all other tags can appear only once
		//so process the first occurence and ignore rest
		//Can be expanded later, if required
		
		//message tag
		if( msgnl != null && msgnl.getLength() > 0) {
			Node n = msgnl.item(0);
			// Get all the attributes of an element in a map
		    NamedNodeMap attrs = n.getAttributes();
		    
		    // Get number of attributes in the element
		    int numAttrs = attrs.getLength();
		    	    
		    // Process each attribute
		    for (int i=0; i<numAttrs; i++) {
		        Attr attr = (Attr)attrs.item(i);
		    
		        // Get attribute name and value
		        String attrName = attr.getNodeName();
		        String attrValue = attr.getNodeValue();
		        
		        //currently only one attribute named 'type'
		        if (attrName.equals("type")) {
		        	rule.msgType = attrValue;
		        	
		        	outRuleMsgType = rule.msgType;
		        }
		        
		    }
		}
		
		//testdevice tag
		if( selfnl != null && selfnl.getLength() > 0) {
			Node n = selfnl.item(0);
			// Get all the attributes of an element in a map
		    NamedNodeMap attrs = n.getAttributes();
		    
		    // Get number of attributes in the element
		    int numAttrs = attrs.getLength();
		    	    
		    // Process each attribute
		    for (int i=0; i<numAttrs; i++) {
		        Attr attr = (Attr)attrs.item(i);
		    
		        // Get attribute name and value
		        String attrName = attr.getNodeName();
		        String attrValue = attr.getNodeValue();
		        
		        //currently only one attribute named 'type'
		        if (attrName.equals("type")) {
		        	rule.selfType = attrValue;
		        	outSelfType = rule.selfType;
		        }
		        
		    }
		}
		
		//targetdevice tag
		if( targetnl != null && targetnl.getLength() > 0) {
			Node n = targetnl.item(0);
			// Get all the attributes of an element in a map
		    NamedNodeMap attrs = n.getAttributes();
		    
		    // Get number of attributes in the element
		    int numAttrs = attrs.getLength();
		    	    
		    // Process each attribute
		    for (int i=0; i<numAttrs; i++) {
		        Attr attr = (Attr)attrs.item(i);
		    
		        // Get attribute name and value
		        String attrName = attr.getNodeName();
		        String attrValue = attr.getNodeValue();
		        
		        //currently only one attribute named 'type'
		        if (attrName.equals("type")) {
		        	rule.targetType = attrValue;
		        	outTargetType = rule.targetType;
		        }
		        
		    }
		}
		
		//build tag
		if( buildnl != null && buildnl.getLength() > 0) {
			Node n = buildnl.item(0);
			//get the value
			rule.buildValue = n.getTextContent();
		}

		//header tags
		if( headernl != null && headernl.getLength() > 0) {
			for (int i = 0 ; i < headernl.getLength();i++) {
			    String hdrType = new String();
			    String hdrValue = new String();
			    
				Node n = headernl.item(i);
				// Get all the attributes of an element in a map
				NamedNodeMap attrs = n.getAttributes();
		    
				// Get number of attributes in the element
				int numAttrs = attrs.getLength();
		    	    
				// Process each attribute
				for (int j=0; j<numAttrs; j++) {
					Attr attr = (Attr)attrs.item(j);
		    
					// Get attribute name and value
					String attrName = attr.getNodeName();
					String attrValue = attr.getNodeValue();
		        
					//currently only one attribute named 'type'
					if (attrName.equals("type"))
						hdrType = attrValue;
		        					
				}
				hdrValue = n.getTextContent();
				//System.out.println("IN Header TYPE: "+hdrType+ " IN VALUE: "+hdrValue.trim());
				rule.mapHeader.put(hdrType, hdrValue);
				
				
			}
		}
		
		return rule;
	}
	
/*	
public static Rules getRule(Element el) {
		
		//Create a new rule with the value read from the xml nodes
		Rules rule = new Rules();
				
		//get rule name
		rule.ruleName = el.getAttribute("name");
		
		//get all child element
		NodeList nl = el.getChildNodes();
		if (nl != null && nl.getLength() > 0) {
		   for (int i = 0 ; i < nl.getLength();i++) {
		       Element ele = (Element)nl.item(i);
			   if (ele.getTagName().equals("header")) { //process header and add to header hash map
				  String hdrType = ele.getAttribute("type");
				  String hdrValue = ele.getNodeValue();
				  rule.mapHeader.put(hdrType, hdrValue);
			   }
			   else if (ele.getTagName().equals("build")) {
				   rule.buildValue = ele.getNodeValue();
			   }
			   else {  //all other tags - get their attributes
				   
				   if (ele.getTagName().equals("message"))
					   rule.msgType = ele.getAttribute("type");
				   if (ele.getTagName().equals("testdevice"))
					   rule.selfType = ele.getAttribute("type");
				   if (ele.getTagName().equals("targetdevice"))
					   rule.targetType = ele.getAttribute("type");
          				   
			   }
		   }
		}
		
		return rule;
	}*/
	
	public static Document parseXmlFile(String filename, boolean validating) {
        try {
            // Create a builder factory
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            factory.setValidating(validating);

           //Using factory get an instance of document builder
			DocumentBuilder db = factory.newDocumentBuilder();
			
            // Create the builder and parse the file
            Document doc = db.parse(filename);
            return doc;
        }catch(ParserConfigurationException pce) {
			pce.printStackTrace();
		}catch(SAXException se) {
			se.printStackTrace();
		}catch(IOException ioe) {
			ioe.printStackTrace();
		}
        return null;
    }

	

}

class Rules {
	String ruleName;
	String msgType;
	String selfType;
	String targetType;
	String buildValue;
	//Hashmap of header objects
	//key = header type
	//value = header value = CDATA section
	HashMap mapHeader =  new HashMap();
}

