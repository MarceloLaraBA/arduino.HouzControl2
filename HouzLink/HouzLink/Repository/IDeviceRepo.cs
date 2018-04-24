using System.Collections.Generic;
using HouzLink.Model;

namespace HouzLink.Repository
{
    public interface IDeviceRepo
    {
        ICollection<Device> GetAll();
        Device GetById(int id);
        Device Update(Device device);
    }
}