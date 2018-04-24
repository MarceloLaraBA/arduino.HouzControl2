using System.Threading.Tasks;
using HouzLink.Model;

namespace HouzLink.Logic
{
    public interface IDeviceLogic
    {
        Task QueryAllDevices();
        bool QueryDevice(int id);
        bool SendCommand(Device device, Device.Command command, long payload);
        bool SendCommand(int deviceId, Device.Command command, long payload);
        void UpdateDevice(CommandResult command);
        void UpdateNodes();
    }
}