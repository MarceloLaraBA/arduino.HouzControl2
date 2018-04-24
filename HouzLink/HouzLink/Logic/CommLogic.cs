using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Communication;
using HouzLink.Controllers;
using HouzLink.Model;
using HouzLink.WebSocketMiddleware;
using Newtonsoft.Json;

namespace HouzLink.Logic
{
    public class CommLogic : ICommLogic
    {
        private readonly ICommDriver _comm;
        private readonly IDeviceLogic _deviceLogic;
        private readonly IClientHandler _clientHandler;
        //private CommDriver _comm => Startup.ContextServiceLocator.CommDriver;
        //private DeviceLogic _deviceLogic => Startup.ContextServiceLocator.DeviceLogic;
        //private ClientHandler _client => Startup.ContextServiceLocator.ClientHandler;

        public CommLogic(ICommDriver comm, IDeviceLogic deviceLogic, IClientHandler clientHandler )
        {
            _comm = comm;
            _deviceLogic = deviceLogic;
            _clientHandler = clientHandler;

            LogController.LogAdd("CommLogic.ctr");
        }

        public void Setup()
        {
            _comm.CommandReceived += CommOnCommandReceived;
            _comm.StatusChanged += CommOnStatusChanged;
            if (!_comm.Connect()) return;
        }

        private async void CommOnStatusChanged(object sender, CommDriver.StatusEnm statusEnm) =>
            await _clientHandler.SendMessage(MessageType.CommStatus, new CommDto((CommDriver) _comm));


        private async void CommOnCommandReceived(object sender, string s)
        {

            var result = ParseCommandResult(s);

            //send to debug client
            await _clientHandler.SendMessage(MessageType.CommLog, s);
        }
        private CommandResult ParseCommandResult(string inCommand)
        {
            if (inCommand.Length != 11) return null;

            CommandResult result = new CommandResult();
            Int32.TryParse(inCommand.Substring(0, 1), out int outResult);
            result.Result = (ResultEnm)outResult;

            Device device = new Device();
            if (Int32.TryParse(inCommand.Substring(2, 1), out int devId)) device.Node = devId;
            device.Cmd = (Device.Command)int.Parse(inCommand.Substring(4, 1), System.Globalization.NumberStyles.HexNumber);
            device.Id = int.Parse(inCommand.Substring(5, 2), System.Globalization.NumberStyles.HexNumber);
            device.Payload = long.Parse(inCommand.Substring(7, 4), System.Globalization.NumberStyles.HexNumber);

            result.Device = device;
            _deviceLogic.UpdateDevice(result);
            return result;
        }



    }
}
