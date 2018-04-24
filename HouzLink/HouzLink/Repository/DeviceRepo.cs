using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Model;
using Microsoft.AspNetCore.Mvc.ViewComponents;

namespace HouzLink.Repository
{
    public class DeviceRepo : IDeviceRepo
    {



        public ICollection<Device> GetAll() => _devices;
        public Device GetById(int id) => _devices.FirstOrDefault(x=>x.Id==id);
        //public ICollection<Device> GetByFilter(IPredicate<Device> predicate) => _devices.Where(predicate);
        public Device Update(Device device)
        {
            _devices[_devices.FindIndex(x => x.Id == device.Id)] = device;
            return GetById(device.Id);
        }

        private static List<Device> _devices = new List<Device>();
        public DeviceRepo()
        {
            _devices = new List<Device>
            {
                new Device("office_node", 0x1, 1, Device.DeviceTypeEnm.Node),
                new Device("office_AC", 0x11, 1, Device.DeviceTypeEnm.AcSwitch){IncludeInUpdate=true},
                new Device("office_AC_temp", 0x12, 1, Device.DeviceTypeEnm.AcTemp){IncludeInUpdate=true},
                new Device("office_light", 0x13, 1, Device.DeviceTypeEnm.Light){IncludeInUpdate=true}, //N1DC130000
                new Device("office_switchLed", 0x14, 1, Device.DeviceTypeEnm.LedIndicator),
                new Device("office_switch  ", 0x15, 1, Device.DeviceTypeEnm.Switch),
                new Device("office_ir", 0x16, 1, Device.DeviceTypeEnm.IrReceiver),
                new Device("external_light", 0x17, 1, Device.DeviceTypeEnm.LightSensor),
                new Device("external_temp", 0x1A, 1, Device.DeviceTypeEnm.Temp){IncludeInEnviroment=true},
                new Device("external_humidity", 0x1B, 1, Device.DeviceTypeEnm.Humidity){IncludeInEnviroment=true},
                new Device("external_pressure", 0x1C, 1, Device.DeviceTypeEnm.Pressure){IncludeInEnviroment=true},

                new Device("bedroom_node", 0x2, 2, Device.DeviceTypeEnm.Node),
                new Device("bedroom_AC", 0x23, 2, Device.DeviceTypeEnm.AcSwitch){IncludeInUpdate=true},
                new Device("bedroom_AC_temp", 0x24, 2, Device.DeviceTypeEnm.AcTemp){IncludeInUpdate=true},
                new Device("bedroom_light", 0x25, 2, Device.DeviceTypeEnm.Light){IncludeInUpdate=true},
                new Device("bedroom_switchLed", 0x26, 2, Device.DeviceTypeEnm.LedIndicator),
                new Device("bedroom_switch", 0x27, 2, Device.DeviceTypeEnm.Switch),
                new Device("bedroom_ir", 0x28, 2, Device.DeviceTypeEnm.IrReceiver),

                new Device("living_node", 0x3, 3, Device.DeviceTypeEnm.Node),
                new Device("living_switch", 0x31, 3, Device.DeviceTypeEnm.Switch),
                new Device("living_switchLed", 0x32, 3, Device.DeviceTypeEnm.LedIndicator),
                new Device("living_mainLight", 0x33, 3, Device.DeviceTypeEnm.LightArray){IncludeInUpdate=true},
                new Device("living_dicroLight", 0x34, 3, Device.DeviceTypeEnm.LightArray){IncludeInUpdate=true},
                new Device("living_spotLight", 0x35, 3, Device.DeviceTypeEnm.LightArray){IncludeInUpdate=true},
                new Device("living_auxLight", 0x36, 3, Device.DeviceTypeEnm.LightArray){IncludeInUpdate=true},
                new Device("living_AC", 0x38, 3, Device.DeviceTypeEnm.AcSwitch),
                new Device("living_AC_temp", 0x39, 3, Device.DeviceTypeEnm.AcTemp)
            };
        }

    }
}
