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

-SeklarityAgent_Extractor is a safe version of mimikatz which is modified to be c library. Code is changed to not extract creds but mere report the presence of them. 

-SeklarityC (Collector) is a c# executable that wraps the SeklarityAgent_Extractor dll. It uses Marshaling to call into extractor functions. 
 The data can then be formatted and upload as json to webservice for further analysis. 
 The SeklarityAgent_Extractor dll is embedded into the SeklarityC projecet using Costura package.

-SeklarityDriver is same driver as mimikatz driver to be used in proptected process cases. The driver needs to be digitally signed in order to load safely in kernel mode.

-Dll_tester is merely a test project in c to test and debug SeklarityAgent_Extractor dll as its hard to debug when dll is emebedded in SeklarityC.


#Pre-requisites:
-----------------

Visual Studio 2015

Visual Studio 2010 for driver 

MS Driver tools

Costura.Fody Nuget package

Fody Nuget package





