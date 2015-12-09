/*SIP SECURITY TESTING FRAMEWORK - SSTF version 0.1
 */
package strezzer;



public abstract class StrezzHeader {
	
	public static int MAX_KEYWORD_LENGTH = 256;
	public static int MAX_VALUE_LENGTH = 1024;  //user defined from GUI options
	public static int FIELD_WIDTH = 5; //4 byte to store "1024" + 1 byte for space  
	//public static final String DATE_FORMAT_NOW = "yyyy-MM-dd_HH:mm:ss";
	public static final String DATE_FORMAT_NOW = "yyyy-MM-dd_HH-mm-ss";

	//public static final String slash = java.io.File.separator;
	public static final String slash = "/";

	public static final String TESTCASE_DIR = ".."+slash+"testcases"+slash;
	public static final String TEMPLATE_DIR = ".."+slash+"template"+slash;
	public static final String FUZZ_DIR = ".."+slash+"fuzz"+slash;
	public static final String FITNESS_DIR = ".."+slash+"fuzz"+slash+"fitness"+slash;
	public static final String OFFSPRING_DIR = ".."+slash+"fuzz"+slash+"offspring"+slash;
	public static final String RULES_DIR = ".."+slash+"template"+slash+"rules"+slash;
	public static final String RULES_TPL_DIR = ".."+slash+"template"+slash+"rules_tpl"+slash;
	public static final String SPOOF_DIR = ".."+slash+"spoof"+slash;
	public static int FIT_LOW = 1; //lowest fitness value
	public static int FIT_HIGH = 10; //highest fitness value
	public static int RAND_SEED = 1;
	public static int MAX_MUTATION_COUNT = 5; //maximum # of bytes that will be mutated in an offspring
	public static int OFFSPRING_PAIRS = 50; //Number of offspring pairs generated for each Parent pair values (parent 1 and 2)
	public static final String EVENT_SEND = "send";
	public static final String EVENT_RECV = "recv";
	public static final String EVENT_PAUSE = "pause";
	
	public static final String SAVED_DATA_DIR = ".."+slash+"testcases"+slash+"savedTestData"+slash;
	public static final String SAVED_SETTINGS_DIR = ".."+slash+"testcases"+slash+"savedTestSettings"+slash;
	
	public static final String SIPP_DIR = ".."+slash+"testcases"+slash+"SIPP"+slash;
	/*
	public void getSystemInfo()
    {
        String nameOS = "os.name";        
        String versionOS = "os.version";        
        String architectureOS = "os.arch";
        System.out.println("\nThe information about OS");
        System.out.println("\nName of the OS: " + 
        System.getProperty(nameOS));
        System.out.println("Version of the OS: " + 
        System.getProperty(versionOS));
        System.out.println("Architecture of THe OS: " + 
        System.getProperty(architectureOS));
    } */
	
}
