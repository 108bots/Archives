This folder contians the template files (.tpl). Template files are the base files for the SIP message definitions. These message definitions contain the fuzz keywords (SIP_xxx) where ever fuzzing is desired.

Files withs extensions .tpl are not fuzzed/spoofed
Files withs extensions .fz.tpl are fuzzed
Files withs extensions .spf.tpl are spoofed
Files withs extensions .fz.spf.tpl are both fuzzed/spoofed

Folders with '-Test' as the suffix contain templates used in some of our Tests.
For 'rules' and 'rule_tpl' files, see README within those folders.
