using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Communication;
using Microsoft.AspNetCore.Mvc;

namespace HouzLink.Controllers
{
    [Route("api/[controller]")]
    public class CommController : Controller
    {
        private CommLogic _comm => Startup.ContextServiceLocator.CommLogic;

        // GET api/comm
        [HttpGet]
        public CommDto GetStatus() => new CommDto()
        {
            port = _comm.ComPort,
            bitrate = _comm.BaudRate,
            open = _comm.Status == CommLogic.StatusEnm.Online,
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



        public class CommDto
        {
            public string port { get; set; }
            public int bitrate { get; set; }
            public bool open { get; set; }
            public CommLogic.StatusEnm link { get; set; }
        }

    }
}
