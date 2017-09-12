using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.IO;
using System.DirectoryServices;
using System.DirectoryServices.AccountManagement;
using System.Collections;
using Microsoft.Win32;
using System.Net.NetworkInformation;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Web.Script.Serialization;
using System.Globalization;

namespace SeklarityC
{
    public class LogWriter
    {
        public LogWriter(string logMessage)
        {
            LogWrite(logMessage);
        }
        public void LogWrite(string logMessage)
        {
            if (Program.doLog == "true")
            {
                //sekExePath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
                // At some point  I need to move from static directories and use above line
                var logDir = @"C:\Seklarity\SeklarityC\Logs\";

                Program.DirCheck(logDir);

                try
                {
                    using (StreamWriter w = File.AppendText(logDir + string.Format("\\{0}_log.txt", "SeklarityC")))
                    {
                        Log(logMessage, w);
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine("There was a problem writing to the log file.  Seklarity server will run, but logging is off" + ex);
                }
            }

        }

        public void Log(string logMessage, TextWriter txtWriter)
        {
            try
            {
                txtWriter.Write("\r\nLog Entry : ");
                txtWriter.WriteLine("{0} {1}", DateTime.Now.ToLongTimeString(), DateTime.Now.ToLongDateString());
                txtWriter.WriteLine(" :");
                txtWriter.WriteLine(" :{0}", logMessage);
                txtWriter.WriteLine("---------------------------------------------------");

            }
            catch (Exception ex)
            {
                Console.WriteLine("There was a problem writing to the log file.  Seklarity server will run, but logging might miss something" + ex);
            }
        }
    }

    internal sealed class Computer
    {
        public string name;
        public string dumpdate;
        public string osver;
        public bool protectedlsa = false;
        public string sekcversion;
        public string authpkg;
        public int credguardval;
        public List<string> admins;
        
        public static Computer Create(string mname, List<string> lalist, string tstamp, string osver, string sekcver, string authpkg, int credGuardVal, bool isprotected)
        {
            return new Computer
            {
                name = mname,

                admins = lalist,

                dumpdate = tstamp,

                osver = osver,

                sekcversion = sekcver,

                authpkg = authpkg,

                credguardval = credGuardVal,

                protectedlsa = isprotected,
            };
        }
    }

    internal sealed class Logon
    {
        public string machname;
        public string dumpdate;
        public string authId;
        public string session;
        public string uname;
        public string dname;
        public string logserv;
        public string logdate;
        //public string logtime;
        public string sid;
        public bool ntlmcred;
        public bool sha1cred;
        public bool netpwdcred;
        

        public static Logon Create(LogonStruct lStruct, string mname, string tstamp)
        {
            return new Logon
            {
                machname = mname,

                dumpdate = tstamp,

                authId = lStruct._authid,

                session = lStruct._session,

                uname = lStruct._username,

                dname = lStruct._domain,

                logserv = lStruct._logonserv,

                logdate = ConvertToDate(lStruct._logondate),

                //logtime = lStruct._logontime,

                sid = lStruct._sid,

                ntlmcred = lStruct._ntlmcred,

                sha1cred = lStruct._sha1cred,

                netpwdcred = lStruct._netpwdcred,

            };

        }

        private static string ConvertToDate(string logdate)
        {
            try
            {
                // Parse string from file to a date-time object
                DateTime dt = DateTime.Parse(logdate);
                // convert date-time to format as mapped in ES
                string esDateTime = dt.ToString("yyyy-MM-dd HH:mm:ss");
                return esDateTime;
            }
            catch { return null; }
        }

    }

    // Call to Extractor for list of logons
    [StructLayout(LayoutKind.Sequential)]
    public class LogonStruct
    {
        [MarshalAsAttribute(UnmanagedType.LPWStr)]
        public string _authid;
        [MarshalAsAttribute(UnmanagedType.LPWStr)]
        public string _session;
        [MarshalAsAttribute(UnmanagedType.LPWStr)]
        public string _username;
        [MarshalAsAttribute(UnmanagedType.LPWStr)]
        public string _domain;
        [MarshalAsAttribute(UnmanagedType.LPWStr)]
        public string _logonserv;
        [MarshalAsAttribute(UnmanagedType.LPWStr)]
        public string _logondate;
        //[MarshalAsAttribute(UnmanagedType.LPWStr)]
        //public string _logontime;
        [MarshalAsAttribute(UnmanagedType.LPWStr)]
        public string _sid;
        public bool _ntlmcred;
        public bool _sha1cred;
        public bool _netpwdcred;
        public int _tp;
        public int _val;
        public IntPtr next;
    }

    internal sealed class Window
    {
        public string scope { get; set; }
        public string append { get; set; }
    }

    internal sealed class Config
    {
        public string customer { get; set; }
        public string uploadUrl { get; set; }
        public string user { get; set; }
        public string pwd { get; set; }

    }

    internal sealed class WebSite
    {
        public static async Task Post(string siteURL, string json, string username, string pwd)
        {
            using (var client = new HttpClient())
            {
                if (username != null)
                {
                    Console.WriteLine("Using Basic Authentication.");
                    client.DefaultRequestHeaders.Authorization = new AuthenticationHeaderValue(
                        "Basic",
                        Convert.ToBase64String(
                        System.Text.ASCIIEncoding.ASCII.GetBytes(
                        string.Format("{0}:{1}", username, pwd))));
                }
                else
                {
                    Console.WriteLine("Username is blank; connecting without authentication.");
                }
                client.DefaultRequestHeaders.Add("Accept", "application/json, text/javascript, */*; q=0.01");
                client.DefaultRequestHeaders.Add("Accept-Encoding", "gzip, deflate, br");
                
                StringContent content = new StringContent(json,Encoding.UTF8, "application/x-www-form-urlencoded");
                Console.WriteLine("Posting to " + siteURL);
                try
                {
                    HttpResponseMessage response = await client.PostAsync(siteURL, content);
                    var contents = await response.Content.ReadAsStringAsync();

                    if (response.IsSuccessStatusCode)
                    {
                        Console.WriteLine("SUCCESS: Post success to " + siteURL);
                        Console.WriteLine("JSON posted...");
                        Console.WriteLine(json);
                        Console.WriteLine("Response body...");
                        Console.WriteLine(contents);
                        LogWriter lwPostNo = new LogWriter("SUCCESS: Post success to " + siteURL + ".\n Response body: " + contents);

                    }
                    else
                    {
                        Console.WriteLine("FAILURE: Post to " + siteURL + " failed.\n Response: " + response.ReasonPhrase);
                        Console.WriteLine("JSON posted...");
                        Console.WriteLine(json);
                        Console.WriteLine(contents);
                        LogWriter lwPostNo = new LogWriter("FAILURE: Post to " + siteURL + " failed.\n Response body: " + contents);
                    }
                }
                catch (Exception ex)
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("Posting to " + siteURL + " failed.");
                    Console.ResetColor();
                    Console.WriteLine("Cred data which we tried to post below: ");
                    Console.WriteLine(json);
                    Console.WriteLine("Exception:");
                    Console.WriteLine(ex);
                }
                
            }

        }


    }

    class Program
    {
        private static readonly JavaScriptSerializer s_jsonSerializer = new JavaScriptSerializer();

        public static string doLog = "false";

        public static void DirCheck(string dir)
        {
            if (!Directory.Exists(dir))
            {
                Directory.CreateDirectory(dir);
            }
        }

        static public int GetScopeNo(DateTime date, Window w)
        {
            var currentCulture = CultureInfo.CurrentCulture;
            var dateTimeInfo = DateTimeFormatInfo.GetInstance(currentCulture);
            var weekNo = currentCulture.Calendar.GetWeekOfYear(date, dateTimeInfo.CalendarWeekRule, dateTimeInfo.FirstDayOfWeek);
            var dayNo = currentCulture.Calendar.GetDayOfYear(date);
            var monthNo = currentCulture.Calendar.GetMonth(date);
            int scope = weekNo; //default

            switch (w.scope)
            {
                case "day":
                    scope = dayNo;
                    break;
                case "week":
                    scope = weekNo;
                    break;
                case "month":
                    scope = monthNo;
                    break;
                default:
                    Console.WriteLine("Invalid time windows scope in timewindow.json.  Check for lower case, no spaces: month, week, or day.");
                    Console.WriteLine("Setting scope to week");
                    break;
            }
            return scope;

        }

        //Functions to get computer admins
        public static PrincipalSearchResult<Principal> GetGroupMembers(string sGroupName, bool domainJoined)
        {
            GroupPrincipal oGroupPrincipal = GetGroup(sGroupName, domainJoined);

            return oGroupPrincipal.GetMembers(true);
        }

        public static GroupPrincipal GetGroup(string sGroupName, bool domainJoined)
        {
            PrincipalContext oPrincipalContext = GetPrincipalContext(domainJoined);

            GroupPrincipal oGroupPrincipal = GroupPrincipal.FindByIdentity(oPrincipalContext, sGroupName);
            return oGroupPrincipal;
        }

        public static PrincipalContext GetPrincipalContext(bool domainJoined)
        {
            PrincipalContext oPrincipalContext;
            if (domainJoined)
            {
                oPrincipalContext = new PrincipalContext(ContextType.Domain);
            }
            else
            {
                oPrincipalContext = new PrincipalContext(ContextType.Machine);
            }
            return oPrincipalContext;
        }

        [DllImport("SeklarityAgent_Extractor.dll")]
        private static extern IntPtr c_dllcall_for_logons();

        [DllImport("SeklarityAgent_Extractor.dll")]
        private static extern IntPtr c_dllcall_for_addDriver();

        [DllImport("SeklarityAgent_Extractor.dll")]
        private static extern IntPtr c_dllcall_for_removeDriver();

        private static readonly JavaScriptSerializer JsonSerializer = new JavaScriptSerializer();

        static public Config ReadConfigJson(string file)
        {
            Config scope = null;
            try
            {
                using (StreamReader r = new StreamReader(file))
                {
                    string json = r.ReadToEnd();
                    scope = s_jsonSerializer.Deserialize<Config>(json);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: " + ex);
            }
            return scope;
        }

        static int Main(string[] args)
        {
            //Parameters Parsing
            string runOnce = "true";

            try
            {
                //do {  TODO: Commented for 0.1 release. 
                //Param Class Init
                ConsoleCmdLine cmd = new ConsoleCmdLine();
                //register param for index server

                //register param for debug option to stop at readline and attach process
                CmdLineString pRunOnce = new CmdLineString("runonce", false, "Wether to run once or keep running. \n\t\tPossible values true or false. Not required. Default value is true.");
                cmd.RegisterParameter(pRunOnce);
                //log param for logging output
                CmdLineString pLog = new CmdLineString("debug", false, "Wether we want to log. \n\t\tPossible values true or false. Not required. Default value is false.");
                cmd.RegisterParameter(pLog);

                cmd.Parse(args);

                if (pRunOnce.Exists)
                {
                    runOnce = pRunOnce.Value;
                }
                if (pLog.Exists)
                {
                    Program.doLog = pLog.Value;
                }

                //Step1: Get computer properties to create Computer Object
                //machine name
                string mcName = Environment.MachineName;
            
                //os info:name + version + service pack
                RegistryKey registryKeyPr = Registry.LocalMachine.OpenSubKey("Software\\Microsoft\\Windows NT\\CurrentVersion");
                RegistryKey registryKeyLsa = Registry.LocalMachine.OpenSubKey("SYSTEM\\CurrentControlSet\\Control\\Lsa");

                string osVersion = (string.Format("{0} {1} {2} {3}", (string)registryKeyPr.GetValue("productName"), Environment.OSVersion.VersionString, Environment.Is64BitOperatingSystem?"64-bit" : "32-bit", Environment.OSVersion.ServicePack));
                
                //authpkg
                string[] arrAuthPkg = (string[])registryKeyLsa.GetValue("Authentication Packages");
                string authPkg = "";
                try
                {
                    if (null != arrAuthPkg && arrAuthPkg.Length > 0)
                    {
                        for (int i = 0; i < arrAuthPkg.Length; i++)
                        {
                            authPkg += arrAuthPkg[0];
                        }
                    }
                }
                catch { }//ignore auth pkg ex
            
                //exe info
                var assembly = System.Reflection.Assembly.GetExecutingAssembly();
                var fvi = FileVersionInfo.GetVersionInfo(assembly.Location);
                string sekcver = string.Format("{0} v{1}", fvi.FileDescription, fvi.ProductVersion) ; // or fvi.ProductVersion
                //lsa info
                bool isProtected = false;
                if ((int)registryKeyLsa.GetValue("RunAsPPL", 0) != 0)
                {
                    isProtected = true;
                }
                //Cred gaurd info. 
                //The value of this registry setting to 1 to enable Credential Guard with UEFI lock, set it to 2 to enable Credential Guard without lock, and set it to 0 to disable it
                int credGaurdVal = (int)registryKeyLsa.GetValue("LsaCfgFlags", 0);
                
                //domain
                string domain1 = "Not Domain joined";
                try
                {
                    domain1 = System.DirectoryServices.ActiveDirectory.Domain.GetComputerDomain().ToString();
                }
                catch (Exception) { domain1 = Environment.UserDomainName; }

                string timestamp = "1970-12-12T12:12:12";
                timestamp = DateTime.Now.ToString("yyyy-MM-dd'T'HH:mm:ss");

                var lalist = new List<string>();
                PrincipalSearchResult<Principal> princList = GetGroupMembers("Administrators", false);
                foreach (Principal admin in princList)
                {
                    string sid = admin.Sid.ToString();
                    string nbname = new System.Security.Principal.SecurityIdentifier(sid).Translate(typeof(System.Security.Principal.NTAccount)).ToString();
                    lalist.Add(nbname);
                }

                var computer = Computer.Create(mcName, lalist, timestamp, osVersion, sekcver, authPkg, credGaurdVal, isProtected);
            
                //Step2: Get List of Logon objects from extractor and generate a LogonsList

                int structSize = Marshal.SizeOf(typeof(LogonStruct));
            
                IntPtr pList = IntPtr.Zero;
                if(isProtected)
                {
                    try
                    {
                        c_dllcall_for_addDriver();
                    }
                    catch { }; //continue on error
                }
                
                pList = c_dllcall_for_logons(); //list of structs from extractor

                if (isProtected)
                {
                    try
                    {
                        c_dllcall_for_removeDriver();
                    }
                    catch { }; //continue on error
                }
                
                List<Logon> llist = new List<Logon>(); //new Logon list

                while (pList != IntPtr.Zero)
                {
                    LogonStruct ls = (LogonStruct)Marshal.PtrToStructure(pList, typeof(LogonStruct));
                    Logon l = Logon.Create(ls, mcName, timestamp);
                    llist.Add(l);
                    pList = ls.next;
                }

                //Step3: Post the computer and logons objects to API
                Window w = new Window();
                w.scope = "week";
                w.append = "";
                var today = DateTime.Today;
                int scopeWeek = GetScopeNo(today, w);
                int scopeYear = CultureInfo.CurrentCulture.Calendar.GetYear(today);

                Config config = ReadConfigJson("config.json"); ;
                string customer = config.customer;
                string esurl = String.Format(config.uploadUrl, customer); //String.Format(, customer);
                string indexName = scopeYear + "_" + scopeWeek + w.append;
                string esurlIndex = esurl + indexName;
 
                //3.1: Post Computer & logons in bulk operation (v1.1 release)
                string postUrl = esurlIndex + "/_bulk";
                var jsonc = JsonSerializer.Serialize(computer);

                string jsonBody = @"{ ""index"": {""_type"": ""computer""}}
                "  + jsonc + @"
                ";
                

                foreach (Logon l in llist)
                {
                    var ljson = JsonSerializer.Serialize(l);
                    try
                    {
                        jsonBody += @"{ ""index"": {""_type"": ""logons""}}
                        " + ljson + @"
                        ";
                    }
                    catch (Exception ex1)
                    {
                        Console.WriteLine(ex1);
                        LogWriter lwErr = new LogWriter(ex1.ToString());
                    }
                }

                try
                {
                    Random random = new Random();
                    int rand = random.Next(1, 60);
                    int mseconds = rand * 1000;
                    Console.WriteLine(string.Format("Waiting {0} secs before uploading", rand));
                    System.Threading.Thread.Sleep(mseconds);
                    WebSite.Post(postUrl, jsonBody, config.user, config.pwd).Wait();
                }
                catch (Exception ex1)
                {
                    Console.WriteLine(ex1);
                    LogWriter lwErr = new LogWriter(ex1.ToString());
                }
                //} while (runOnce == "false");

            } //main try
            catch(Exception ex)
            {
                Console.WriteLine(ex);
                LogWriter lwErr = new LogWriter(ex.ToString());
            }

            return 0;
        }
        
    }

}
