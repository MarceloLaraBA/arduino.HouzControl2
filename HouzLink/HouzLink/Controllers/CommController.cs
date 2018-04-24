using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Threading.Tasks;
using HouzLink.Communication;
using Microsoft.AspNetCore.Mvc;

namespace HouzLink.Controllers
{
    [Route("api/[controller]")]
    public class CommController : Controller
    {
        private readonly ICommDriver _comm;

        public CommController(ICommDriver comm)
        {
            _comm = comm;
        }

        //private Communication.CommDriver _comm => Startup.ContextServiceLocator.CommDriver;

        // GET api/comm
        [HttpGet]
        public CommDto GetStatus() => new CommDto()
        {
            port = _comm.ComPort,
            bitrate = _comm.BaudRate,
            open = _comm.Status == Communication.CommDriver.StatusEnm.Online,
            link = _comm.LinkStatus
        };

        // GET api/comm
        [HttpPost]
        public bool Connect()
        {
            return _comm.Connect();
        }
        // GET api/comm
        [HttpDelete]
        public bool Disconnect()
        {
            _comm.Disconnect();
            return true;
        }

        // GET api/comm
        [HttpPost]
        [Route("send")]
        public bool Send([FromBody] string str)
        {
            return _comm.Send(str);
        }
    }
    public class CommDto
    {
        public CommDto()
        {
        }

        public CommDto(Communication.CommDriver commCtrl)
        {
            port = commCtrl.ComPort;
            bitrate = commCtrl.BaudRate;
            open = commCtrl.Status == Communication.CommDriver.StatusEnm.Online;
            link = commCtrl.LinkStatus;
        }

        public string port { get; set; }
        public int bitrate { get; set; }
        public bool open { get; set; }
        public Communication.CommDriver.StatusEnm link { get; set; }
    }

}
