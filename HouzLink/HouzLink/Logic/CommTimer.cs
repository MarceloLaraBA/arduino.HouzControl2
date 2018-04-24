using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Model;
using HouzLink.Repository;
using HouzLink.WebSocketMiddleware;

namespace HouzLink.Logic
{
    public class CommTimer
    {
        private readonly IDeviceRepo _deviceRepo;
        private readonly IDeviceLogic _deviceLogic;
        private readonly IClientHandler _clientHandler;

        //private IDeviceRepo _deviceRepo => Startup.ContextServiceLocator.DeviceRepo;
        //private IDeviceLogic _deviceLogic => Startup.ContextServiceLocator.DeviceLogic;
        //private IClientHandler _client => Startup.ContextServiceLocator.ClientHandler;
        //public  CommTimer Instance => _instance ?? (_instance = new CommTimer());

        private System.Timers.Timer _timer;

        private static CommTimer _instance = null;

        public void Setup()
        {
            _timer = new System.Timers.Timer{Interval = 5000,AutoReset = false,Enabled = true};
            _timer.Elapsed += Update;
            _timer.Start();
        }

        private DateTime _lastStatus = DateTime.Now;

        public CommTimer(IDeviceRepo deviceRepo, IDeviceLogic deviceLogic, IClientHandler clientHandler)
        {
            _deviceRepo = deviceRepo;
            _deviceLogic = deviceLogic;
            _clientHandler = clientHandler;
        }

        private async void Update(object sender, EventArgs e)
        {
            _timer.Interval = 10000;
            // query all devices
            await _deviceLogic.QueryAllDevices();
            _timer.Start();
        }


    }
}