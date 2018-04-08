using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Communication;
using HouzLink.Logic;
using HouzLink.Repository;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace HouzLink
{
    public class Startup
    {
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddMvc();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IHostingEnvironment env)
        {
            ContextServiceLocator.Instance = app.ApplicationServices;
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }
            app.UseDefaultFiles();
            app.UseStaticFiles();
            app.UseMvc();

            //setup
            ContextServiceLocator.DeviceLogic.Setup();
        }
        public static class ContextServiceLocator
        {
            private static CommLogic _commLogic;
            public static CommLogic CommLogic => _commLogic ?? (_commLogic = new CommLogic());

            private static DeviceLogic _deviceLogic;
            public static DeviceLogic DeviceLogic => _deviceLogic ?? (_deviceLogic = new DeviceLogic());

            private static DeviceRepo _deviceRepo;
            public static DeviceRepo DeviceRepo => _deviceRepo ?? (_deviceRepo=new DeviceRepo());

            private static NodeRepo _nodeRepo;
            public static NodeRepo NodeRepo => _nodeRepo ?? (_nodeRepo = new NodeRepo());

            public static IServiceProvider Instance { get; set; }
        }

    }
}
