﻿using System;
using System.IO;
using System.Collections.Generic;
using System.Threading.Tasks;

using A2v10.Infrastructure;

namespace A2v10.Runtime
{

    //<add key = "appPath" value="c:/git/a2v10/apps" />
    //<add key = "appKey" value="develop" />

    public class DesktopApplicationHost : IApplicationHost
    {
        IProfiler _profiler;
        IDictionary<String, String> _cnnStrings = new Dictionary<String, String>();

        public DesktopApplicationHost(IProfiler profiler)
        {
            _profiler = profiler;
        }

        public IProfiler Profiler => _profiler;

        public String AppPath
        {
            get
            {
                // TODO: CONFIG
                return "c:/git/a2v10/apps";
                /*
                String path = ConfigurationManager.AppSettings["appPath"];
                if (path == null)
                    throw new ConfigurationErrorsException("Configuration parameter 'appSettings/appPath' not defined");
                return path;
                */
            }
        }

        public string AppKey
        {
            get
            {
                // TODO: CONFIG
                return "develop"; //ConfigurationManager.AppSettings["appKey"];

            }
        }


        public bool IsDebugConfiguration
        {
            get
            {
                return true;
            }
        }

        public String AppVersion => AppInfo.MainAssembly.Version;

        public String ConnectionString(String source)
        {
            if (String.IsNullOrEmpty(source))
                source = "Default";

            if (source == "Default")
                return "Data Source=localhost\\SQLEXPRESS2014;Initial Catalog=a2v10git;Integrated Security=True";
            else if (source == "a2start")
                return "Data Source=localhost\\SQLEXPRESS2014;Initial Catalog=a2start;Integrated Security=True";
            else if (source == "frontis")
                return "Data Source=localhost\\SQLEXPRESS2014;Initial Catalog=a2frontis_wt;Integrated Security=True";

            throw new ArgumentOutOfRangeException($"Connection string '{source}' not found");
            /*
            String cnnStr = null;
            if (_cnnStrings.TryGetValue(source, out cnnStr))
                return cnnStr;
            var strSet = ConfigurationManager.ConnectionStrings[source];
            if (strSet == null)
                throw new ConfigurationErrorsException($"Connection string '{source}' not found");
            cnnStr = strSet.ConnectionString;
            _cnnStrings.Add(source, strSet.ConnectionString);
            return cnnStr;
            */
        }

        public string MakeFullPath(bool bAdmin, string path, string fileName)
        {
            String appKey = bAdmin ? "admin" : AppKey;
            String fullPath = Path.Combine($"{AppPath}/{appKey}", path, fileName);
            return Path.GetFullPath(fullPath);
        }

        public async Task<String> ReadTextFile(bool bAdmin, string path, string fileName)
        {
            String fullPath = MakeFullPath(bAdmin, path, fileName);
            using (var tr = new StreamReader(fullPath))
            {
                return await tr.ReadToEndAsync();
            }
        }
    }
}