using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Model;

namespace HouzLink.Repository
{
    public class NodeRepo
    {
        private static DeviceRepo _deviceRepo => Startup.ContextServiceLocator.DeviceRepo;
        //public IEnumerable<Node> GetAll() => _deviceRepo.GetAll().Where(x => x.DeviceType == Device.DeviceTypeEnm.Node);

    }
}
