using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Autofac;
using Autofac.Extensions.DependencyInjection;
using HouzLink.Communication;
using HouzLink.Logic;
using HouzLink.Repository;
using HouzLink.WebSocketMiddleware;
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
                .ConfigureServices(services => services.AddAutofac())
                .UseContentRoot(pathToContentRoot)
                .UseUrls("http://0.0.0.0:5000")
                .UseStartup<Startup>()
                .Build();
        }
    }
    public class AutofacModule : Module
    {
        protected override void Load(ContainerBuilder builder)
        {
            builder.RegisterType<DeviceLogic>().As<IDeviceLogic>().SingleInstance();

            builder.RegisterType<CommDriver>().As<ICommDriver>().SingleInstance();
            builder.RegisterType<CommLogic>().As<ICommLogic>().SingleInstance();

            builder.RegisterType<DeviceRepo>().As<IDeviceRepo>().SingleInstance();

            builder.RegisterType<WebSocketManagerMiddleware>().As<IWebSocketManagerMiddleware>().SingleInstance();
            builder.RegisterType<WebSocketConnectionManager>().As<IWebSocketConnectionManager>().SingleInstance();
            builder.RegisterType<WebSocketHandler>().As<IWebSocketHandler>().SingleInstance();
            builder.RegisterType<ClientHandler>().As<IClientHandler>().SingleInstance();

        }
    }

}
