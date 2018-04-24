using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using HouzLink.Communication;
using HouzLink.Controllers;
using HouzLink.Model;
using HouzLink.Repository;
using HouzLink.WebSocketMiddleware;
using Microsoft.AspNetCore.Mvc.Internal;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;

namespace HouzLink.Logic
{
    public class DeviceLogic : IDeviceLogic
    {
        private readonly IDeviceRepo _deviceRepo;

        private readonly IClientHandler _clientHandler;

        private readonly ICommDriver _commDriver;
        //todo: DI
        //private IDeviceRepo deviceRepo => Startup.ContextServiceLocator.DeviceRepo;
        //private IClientHandler client => Startup.ContextServiceLocator.ClientHandler;
        //private ICommDriver comm => Startup.ContextServiceLocator.CommDriver;

        private ICollection<Device> Nodes { get; set; }

        public DeviceLogic(IDeviceRepo deviceRepo, IClientHandler clientHandler, ICommDriver commDriver)
        {
            _deviceRepo = deviceRepo;
            _clientHandler = clientHandler;
            _commDriver = commDriver;
            LogController.LogAdd("DeviceLogic.ctr");
        }



        public async void UpdateDevice(CommandResult command)
        {
            Device device = _deviceRepo.GetById(command.Device.Id);
            if (device == null) return;

            //todo: update node status
            switch (command.Result)
            {
                case ResultEnm.SentOk:
                    device.Status = Device.DeviceStatus.Awaiting;
                    break;
                case ResultEnm.SentRetry:
                    device.Status = Device.DeviceStatus.Unknown;
                    break;
                case ResultEnm.SentFail:
                    device.Status = Device.DeviceStatus.Error;
                    break;
                case ResultEnm.Received:
                    device.Payload = command.Device.Payload;
                    device.ValueDate = DateTime.Now;
                    device.Status = Device.DeviceStatus.Ok;

                    switch (device.DeviceType)
                    {
                        case Device.DeviceTypeEnm.Humidity:
                        case Device.DeviceTypeEnm.Temp:
                            device.Value = (float)device.Payload / 100;
                            device.ValueStr = device.Value.ToString("F1");
                            break;
                        case Device.DeviceTypeEnm.Pressure:
                            device.Value = (float)device.Payload / 100;
                            device.ValueStr = device.Value.ToString("F1");
                            break;
                        default: device.Value = device.Payload; break;
                    }
                    break;
            }
            _deviceRepo.Update(device);
            await _clientHandler.SendMessage(MessageType.DeviceUpdate, device);
        }

        public void UpdateNodes()
        {
            foreach (Device node in _deviceRepo.GetAll().Where(x => x.DeviceType == Device.DeviceTypeEnm.Node))
            {
                SendCommand(node, Device.Command.QUERY, 0xF0F0);
            }
        }

        public bool SendCommand(Device device, Device.Command command, long payload)
        {
            string cmd = string.Empty;
            cmd += $"N{device.Node:X}";                    // node selection
            cmd += "D";                             // device prefix
            cmd += ((int)command).ToString("X");    // command
            cmd += device.Id.ToString("X2");               // device
            cmd += payload.ToString("X4");            // value (only on set)
            return _commDriver.Send(cmd);
        }

        public bool QueryDevice(int id)
        {
            Device device = _deviceRepo.GetById(id);
            return SendCommand(device, Device.Command.QUERY, 0);
        }

        public bool SendCommand(int deviceId, Device.Command command, long payload)
        {
            Device dev = _deviceRepo.GetById(deviceId);
            return dev != null && SendCommand(dev, command, payload);
        }

        public async Task QueryAllDevices()
        {
            DateTime voidDate = DateTime.Now.AddMinutes(-1);
            IEnumerable<Device> voidDevices = _deviceRepo.GetAll().Where(x => x.IncludeInUpdate & x.ValueDate < voidDate);

            foreach (Device node in voidDevices)
            {
                Console.WriteLine(node);
                //SendCommand(node, Device.Command.QUERY, 0xF0F0);
                await Task.Delay(1000);
            }
        }

        public void Start()
        {
            LogController.LogAdd("DeviceLogic.Setup()");
            UpdateNodes();
        }
    }
}
