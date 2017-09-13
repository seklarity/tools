# Tools:
--------

A set of tools to extract the state of Windows Credentials

Licence:

CC BY 4.0 licence - https://creativecommons.org/licenses/by/4.0/

Architect and Developer: Devdatta Waghdhare, you can contact me at ( devwag [at] seklarity.com )

Developer: Joe Donahue, you can contact me at ( joed [at] seklarity.com )

Mimikatz code was written by Benjamin Delpy ( benjamin [at] gentilkiwi.com ) or visit his repo ( https://github.com/gentilkiwi/mimikatz )

More information at http://www.seklarity.com

#To run SeklarityC.exe:
------------------------

A built and signed version, including driver is located in the Release/1.1.0 folder.

Info on how to config to upload to Elasticsearch is on our website - http://seklarity.com/info.html

Note - This is a 64-bit executable to run on 64 bit Windows Machines.

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

#Build instructions:
-----------------
Visual Studio 2015

-Clone repo to local machine.

-Open SeklarityC_Sln.sln.

-In Solution explorer, highlight the solution and in the Properties windows at the bottom, make sure the Active Config is set to "debug/64"

-In Visual Studio, click Build->Build Solution

Visual Studio 2017

-Clone repo to local machine.

-Open SeklarityC_Sln.sln.

-If Visual Studio Prompts you to Retarget Solution, click Cancel

-In Solution explorer, highlight the solution and in the Properties windows at the bottom, make sure the Active Config is set to "debug/64"

-If running a different version of Visual Studio from 2015, you will need to change the Platform and SDK versions in the Project Properties

--For Visual Studio 2017, change platform to Visual Studio 2017 (v141) for both SeklarityAgent project Extractor and Dll_tester project

--For Visual Studio 2017 change SDK to version - 10.0.14393.0 for both SeklarityAgent project Extractor and Dll_tester project

-In Visual Studio, click Build->Build Solution

Note - This will run out of the box if Protected processes are not configured for Windows.  If your machine has Protected processes enabled, you will need to build the driver project (included in this repo) with the WinDDK build tools, as described in the mimikatz repo ReadMe file (https://github.com/gentilkiwi/mimikatz/blob/master/README.md).  And you will need to sign the driver.





