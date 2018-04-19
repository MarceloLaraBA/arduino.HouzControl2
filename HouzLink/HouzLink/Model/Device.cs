using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace HouzLink.Model
{
    public class CommandResult
    {
        public ResultEnm Result { get; set; }
        public Device Device { get; set; }

        public override string ToString()
        {
            return $"{Result}: {Device}";
        }
    }

    public enum ResultEnm
    {
        SentOk = 1,
        SentRetry = 2,
        SentFail = 3,
        Received = 4
    }



    public class Device
    {
        public Device()
        {
        }

        //MSG to be sent: N1DA12ABCD
        //0123456789

        //N1DA12ABCD < query temp
        //N1DC330001 < offlice light on
        //N1DC330000 < offlice light off

        //[id:0x12|cmd:0xA|payload:0xABCD|raw:0xda120000]

        public Device(string name, Int32 address, Int32 rfNode, DeviceTypeEnm deviceType)
        {
            Name = name;
            Id = address;
            Node = rfNode;
            DeviceType = deviceType;
        }
        public String Name { get; set; }
        public Int32 Id { get; set; }
        public Command Cmd { get; set; }
        public MediaEnm Media { get; set; }
        public Int32 Payload { get; set; }
        public Int32 Value { get; set; }
        public DateTime ValueDate { get; set; }
        public DeviceStatus Status { get; set; }
        public DeviceTypeEnm DeviceType { get; set; }
        public String ValueStr { get; set; }
        public Int32 Node { get; set; }

        //logic related props
        public bool IncludeInUpdate { get; set; }
        public bool IncludeInEnviroment { get; set; }


        //Device inner logic
        public bool IsQueryable() => DeviceType == DeviceTypeEnm.Node ||
                                   DeviceType == DeviceTypeEnm.Light ||
                                   DeviceType == DeviceTypeEnm.LightArray ||
                                   DeviceType == DeviceTypeEnm.AcSwitch ||
                                   DeviceType == DeviceTypeEnm.AcTemp;

        public String GetCommand(Command command, Int32 value = 0)
        {
            string cmd = string.Empty;
            cmd += $"N{Node:X}";                    // node selection
            cmd += "D";                             // device prefix
            cmd += ((int)command).ToString("X");    // command
            cmd += Id.ToString("X2");               // device
            cmd += value.ToString("X4");            // value (only on set)
            return cmd;
        }

        public override string ToString() => $"[{Id:X2}:{Name}|{Value:X4}]";

        public enum DeviceStatus
        {
            Unknown = 0,
            Ok = 1,
            Awaiting = 2,
            Error = 3,
            NodeDown = 4
        }
        public enum DeviceTypeEnm
        {
            Unknown,
            Node,
            Light,
            LightArray,
            Temp,
            LightSensor,
            IrAppliance,
            AcSwitch,
            AcTemp,
            Analog,
            Humidity,
            Pressure,
            LedIndicator,
            Switch,
            IrReceiver
        }
        public enum MediaEnm
        {
            Serial = 0,
            Rf = 1,
            Ir = 2
        }
        public enum Command
        {
            QUERY = 0xA,
            VALUE = 0xB,
            SET = 0xC,
            EVENT = 0xD,
            STATUS = 0xE
        }


    }
}
