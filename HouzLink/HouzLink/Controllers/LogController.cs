using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;

namespace HouzLink.Controllers
{
    [Route("api/[controller]")]
    public class LogController : Controller
    {
        private static List<string> _houzLog { get; } = new List<string>();

        public static void LogAdd(string logTxt)
        {
            _houzLog.Add($"{_houzLog.Count}.{logTxt}");
        }

        // GET api/values
        [HttpGet]
        public IEnumerable<string> Get()
        {
            return _houzLog;
        }
    }
}
