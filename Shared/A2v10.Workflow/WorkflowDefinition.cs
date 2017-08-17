﻿
using System;
using System.Activities;
using System.Activities.XamlIntegration;
using System.IO;
using System.Security.Cryptography;
using System.Text;

using A2v10.Infrastructure;

namespace A2v10.Workflow
{
    public enum WorkflowType
    {
        Unknown,
        ClrType,
        File
    }

    internal class WorkflowDefinition
    {
        public String Source { get; private set; }
        public String Name { get; private set; }
        public String Path { get; private set; }
        public WorkflowType Type { get; private set; }
        public String Assembly { get; private set; }

        public String Definition { get; private set; }
        public WorkflowIdentity Identity { get; private set; }

        Int32 Version
        {
            get
            {
                if (String.IsNullOrEmpty(Name))
                    return 1;
                int pos = Name.LastIndexOf("_v");
                if (pos == -1)
                    return 1;
                return Int32.Parse(Name.Substring(pos + 2));
            }
        }


        private WorkflowDefinition(String source)
        {
            // clr:A2.Workflows
            // file:fullName
            Source = source.Trim();
            if (Source.StartsWith("clr:"))
            {
                Type = WorkflowType.ClrType;
                String[] clrTypes = Source.Substring(4).Split(';');
                Name = clrTypes[0].Trim();
                Assembly = clrTypes[1].Trim().Replace("assembly", String.Empty).Replace("=", String.Empty);
            }
            else if (Source.StartsWith("file:"))
            {
                Type = WorkflowType.File;
                Path = Source.Substring(5).Trim();
                Name = System.IO.Path.GetFileNameWithoutExtension(Path);
            }
            else
            {
                throw new WorkflowException($"Invalid workflow source ('{Source}')");
            }
            Identity = new WorkflowIdentity(Name, new Version(Version, 0), null);
        }

        public static WorkflowDefinition Create(String source)
        {
            var def = new WorkflowDefinition(source);
            return def;
        }

        String GetWorkflowFullPath(IApplicationHost host)
        {
            String fullPath = System.IO.Path.Combine(host.AppPath, host.AppKey, Path);
            fullPath = System.IO.Path.ChangeExtension(fullPath, "xaml");
            fullPath = System.IO.Path.GetFullPath(fullPath);
            return fullPath;
        }

        String GetHashedName()
        {
            HashAlgorithm algorithm = MD5.Create();
            var hash = algorithm.ComputeHash(Encoding.UTF8.GetBytes(Definition));
            StringBuilder sb = new StringBuilder(Name);
            sb.Append("_");
            foreach (byte b in hash)
            {
                sb.Append(b.ToString("x2"));
            }
            return sb.ToString();
        }


        public Activity LoadFromSource(IApplicationHost host)
        {
            if (Type == WorkflowType.File)
            {
                String fullPath = GetWorkflowFullPath(host);
                using (var sr = new StreamReader(fullPath))
                {
                    Definition = sr.ReadToEnd();
                    sr.BaseStream.Seek(0, SeekOrigin.Begin);
                    using (var xr = ActivityXamlServices.CreateReader(sr.BaseStream))
                    {
                        var root = ActivityXamlServices.Load(xr);
                        RuntimeActivity.Compile(GetHashedName(), root);
                        return root;
                    }
                }
            }
            else if (Type == WorkflowType.ClrType)
            {
                return System.Activator.CreateInstance(Assembly, Name).Unwrap() as Activity;
            }
            else
            {
                throw new NotImplementedException($"WorkflowDefinition.LoadFromFile. Type={Type.ToString()}");
            }
        }

        public Activity LoadFromString()
        {
            if (Type == WorkflowType.File)
            {
                using (var sr = new StringReader(Definition))
                {
                    Activity root = ActivityXamlServices.Load(sr) as Activity;
                    RuntimeActivity.Compile(GetHashedName(), root);
                    return root;
                }
            }
            else if (Type == WorkflowType.ClrType)
            {
                return System.Activator.CreateInstance(Assembly, Name).Unwrap() as Activity;
            }
            else
            {
                throw new NotImplementedException($"WorkflowDefinition.LoadFromString. Type={Type.ToString()}");
            }
        }
    }
}