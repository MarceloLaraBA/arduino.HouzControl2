using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

namespace HouzLink
{
    public class Program
    {
        public static bool RunningAsService { get; set; }

        public static void Main(string[] args)
        {
            RunningAsService = !(Debugger.IsAttached || args.Contains("--debug"));
            IWebHost host =  BuildWebHost(args);
            host.Run();
        }

        public static IWebHost BuildWebHost(string[] args)
        {
            var pathToContentRoot = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName);
            if (!RunningAsService)
            {
                pathToContentRoot = Directory.GetCurrentDirectory();
            }


            return WebHost.CreateDefaultBuilder(args)
                .UseContentRoot(pathToContentRoot)
                .UseUrls("http://0.0.0.0:5000")
                .UseStartup<Startup>()
                .Build();
        }
    }
}
