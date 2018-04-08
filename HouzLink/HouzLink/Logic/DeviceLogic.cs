using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Communication;
using HouzLink.Controllers;
using HouzLink.Model;
using HouzLink.Repository;

namespace HouzLink.Logic
{
    public class DeviceLogic
    {
        //todo: DI
        private DeviceRepo _deviceRepo => Startup.ContextServiceLocator.DeviceRepo;
        private CommLogic _comm => Startup.ContextServiceLocator.CommLogic;


        private ICollection<Device> Nodes { get; set; }

        public DeviceLogic()
        {
            LogController.LogAdd("DeviceLogic.ctr");
        }

        public void Setup()
        {
            LogController.LogAdd("DeviceLogic.Setup()");

            if(!_comm.Connect()) return;

            UpdateNodes();
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



    }
}
