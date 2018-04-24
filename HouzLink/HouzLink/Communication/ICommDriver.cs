using System;

namespace HouzLink.Communication
{
    public interface ICommDriver
    {
        int BaudRate { get; set; }
        string ComPort { get; }
        CommDriver.StatusEnm LinkStatus { get; }
        CommDriver.StatusEnm Status { get; }

        event EventHandler<string> CommandReceived;
        event EventHandler<CommDriver.StatusEnm> StatusChanged;

        bool Connect();
        bool Disconnect();
        void Dispose();
        bool Send(string str);
    }
}