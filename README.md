# Tools:
--------

A set of tools to extract the state of Windows Credentials

Licence:

CC BY 4.0 licence - https://creativecommons.org/licenses/by/4.0/

Architect and Developer: Devdatta Waghdhare, you can contact me at ( devwag [at] seklarity.com )

Developer: Joe Donahue, you can contact me at ( joed [at] seklarity.com )

Mimikatz code was written by Benjamin Delpy ( benjamin [at] gentilkiwi.com ) or visit his repo ( https://github.com/gentilkiwi/mimikatz )

More information at http://www.seklarity.com

# Solution and Projects :
-------------------------

-SeklarityAgent_Extractor is a safe version of mimikatz, which is modified to be c library. Code is reworked to not extract creds, but report the state of the creds
 (e.g. if there is an NTLM hash, it will not extract the hash but  report that it is present. i.e. ntlmcred=true)

-SeklarityC (Collector) is a c# executable that wraps SeklarityAgent_Extractor dll. It uses Marshaling to call into extractor functions. 
 The data can then be formatted and uploaded as json for further analysis. 
 The SeklarityAgent_Extractor dll is embedded into SeklarityC project using Costura package.

-SeklarityDriver is same driver as the mimikatz driver, used when Windows is configured to enable Protected Processes. The driver needs to be digitally signed in order to load safely in kernel mode.

-Dll_tester is a test project in c to test and debug SeklarityAgent_Extractor dll, as it is hard to debug when dll is embedded in SeklarityC.


#Pre-requisites:
-----------------

Visual Studio 2015

Visual Studio 2010 for driver 

MS Driver tools

Costura.Fody Nuget package

Fody Nuget package





