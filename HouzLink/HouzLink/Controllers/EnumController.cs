using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Model;
using Microsoft.AspNetCore.Mvc;

namespace HouzLink.Controllers
{
    [Route("api/[controller]")]
    public class EnumController : Controller
    {

        // GET api/values
        [HttpGet]
        public EnumDto Get()
        {
            return new EnumDto();
        }


        public class EnumDto {
            public string[] DeviceStatus => Enum.GetNames(typeof(Device.DeviceStatus));
            public string[] Command => Enum.GetNames(typeof(Device.Command));
            public string[] DeviceType => Enum.GetNames(typeof(Device.DeviceTypeEnm));
        }
    }
}
