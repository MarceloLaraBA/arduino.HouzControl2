using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Communication;
using HouzLink.Controllers;
using HouzLink.Model;
using HouzLink.Repository;
using HouzLink.WebSocketMiddleware;
using Newtonsoft.Json;

namespace HouzLink.Logic
{
    public class DeviceLogic
    {
        //todo: DI
        private DeviceRepo _deviceRepo => Startup.ContextServiceLocator.DeviceRepo;
        private ClientHandler _client => Startup.ContextServiceLocator.ClientHandler;
        private CommDriver _comm => Startup.ContextServiceLocator.CommDriver;

        private ICollection<Device> Nodes { get; set; }

        public DeviceLogic()
        {
            LogController.LogAdd("DeviceLogic.ctr");
        }

        public void Setup()
        {
            LogController.LogAdd("DeviceLogic.Setup()");
            UpdateNodes();
        }


        public async void UpdateDevice(CommandResult command)
        {
            Device device = _deviceRepo.GetById(command.Device.Id);
            if (device == null)
            {
                return;
            }

            switch (command.Result)
            {
                case ResultEnm.SentOk:
                    device.ValueDate = DateTime.Now;
                    device.Status = Device.DeviceStatus.Awaiting;
                    break;
                case ResultEnm.SentRetry:
                    device.ValueDate = DateTime.Now;
                    device.Status = Device.DeviceStatus.Unknown;
                    break;
                case ResultEnm.SentFail:
                    device.ValueDate = DateTime.Now;
                    device.Status = Device.DeviceStatus.Error;
                    break;
                case ResultEnm.Received:
                    device.Value = device.Payload = command.Device.Payload;
                    device.ValueDate = DateTime.Now;
                    device.Status = Device.DeviceStatus.Ok;
                    break;
            }
            _deviceRepo.Update(device);
            await _client.SendMessage(new SocketMessageDto(SocketMessageTypeEnm.DeviceUpdate,
                JsonConvert.SerializeObject(device)));
        }

        public void UpdateNodes()
        {
            foreach (Device node in _deviceRepo.GetAll().Where(x => x.DeviceType == Device.DeviceTypeEnm.Node))
            {
                SendCommand(node, Device.Command.QUERY, 0xF0F0);
            }
        }

        public bool SendCommand(Device device, Device.Command command, int payload)
        {
            string cmd = string.Empty;
            cmd += $"N{device.Node:X}";                    // node selection
            cmd += "D";                             // device prefix
            cmd += ((int)command).ToString("X");    // command
            cmd += device.Id.ToString("X2");               // device
            cmd += payload.ToString("X4");            // value (only on set)
            return _comm.Send(cmd);
        }


        public bool QueryDevice(int id)
        {
            Device device = _deviceRepo.GetById(id);
            return SendCommand(device, Device.Command.QUERY, 0);
        }
    }
}
