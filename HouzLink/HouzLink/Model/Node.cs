using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using HouzLink.Repository;

namespace HouzLink.Model
{
    public class Node:Device
    {
        private ICollection<Device> Devices{get;set;}

    }
}
