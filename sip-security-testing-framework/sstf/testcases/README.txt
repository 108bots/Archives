This directory contains two types of files:

1) The XML scenario files corresponding to the messages and their ordering you included for a given test case.
2) If the original input message templates contained fuzzed fields (fuzz keywords) then a .csv file will be generated corresponding to that message. The first line in the this file contains the phrase "SEQUENTIAL"; this implies
that values from the file will be chosen sequentially at runtime. You can replace this with "RANDOM" for values
to be chosen at random.

All scenario (.xml) and related csv files MUST be present in this directory,
in order to load correctly in the GUI.

DONt DELETE template.xml,  it is the base file required to generate the xml scenario file.

Dont DELETE the following directories

The directory 'savedTestData' stores your saved test event settings data. The files here are used by the GUI to load and
store the events in a stored test case.

The directory 'savedTestSettings' stores your saved test event settings config. The files here are used by the GUI to load and store the events in a stored test case.

