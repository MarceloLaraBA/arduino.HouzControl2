using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Communication;
using HouzLink.Logic;
using HouzLink.Model;
using HouzLink.Repository;
using Microsoft.AspNetCore.Mvc;

namespace HouzLink.Controllers
{
    [Route("api/[controller]")]
    public class DeviceController : Controller
    {
        private readonly IDeviceRepo _deviceRepo;
        private readonly IDeviceLogic _deviceLogic;

        public DeviceController(IDeviceRepo deviceRepo, IDeviceLogic deviceLogic)
        {
            _deviceRepo = deviceRepo;
            _deviceLogic = deviceLogic;
        }

        //private DeviceRepo _deviceRepo => Startup.ContextServiceLocator.DeviceRepo;
        //private DeviceLogic _deviceLogic => Startup.ContextServiceLocator.DeviceLogic;

        [HttpGet]
        public IEnumerable<Device> GetAll() => _deviceRepo.GetAll();

        [HttpGet]
        [Route("{id}")]
        public Device GetById(int id) => _deviceRepo.GetById(id);

        // GET api/comm
        [HttpGet]
        [Route("{id}/query")]
        public bool QueryDevice(int id) => _deviceLogic.QueryDevice(id);
        // GET api/comm
        [HttpGet]
        [Route("{id}/set/{payload}")]
        public bool SendCommand(int id, int payload) => _deviceLogic.SendCommand(id, Device.Command.SET, payload);

    }
}
