using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using HouzLink.Controllers;

namespace HouzLink.Communication
{
    public class CommDriver : IDisposable
    {

        public string ComPort { get; private set; } = "COM3";
        public int BaudRate { get; set; } = 115200;

        public StatusEnm Status => _serialPort != null && _serialPort.IsOpen ? StatusEnm.Online : StatusEnm.Offline;

        public StatusEnm LinkStatus { get; private set; }

        public CommDriver()
        {
            LogController.LogAdd("CommController.ctr");
        }

        //events
        public event EventHandler<string> CommandReceived;
        public event EventHandler<StatusEnm> StatusChanged;
        private static readonly ConcurrentQueue<string> _sendQueue = new ConcurrentQueue<string>();

        //hardware stuff
        private static SerialPort _serialPort;
        private static string _buffer;
        private static bool _busy = false;
        private const string _newLine = "\n";

        private static DateTime _lastOk;
        private int _voidInterval = 10000;

        public bool Connect()
        {
            if (_serialPort != null)
            {
                if (_lastOk.AddSeconds(_voidInterval) > DateTime.Now && _serialPort.IsOpen) return true;
                if (_serialPort.IsOpen) _serialPort.Close();
            }

            LogController.LogAdd("CommController.Connect()");
            try
            {
                //scan comm ports
                string[] availablePorts = SerialPort.GetPortNames();
                if (availablePorts.Length == 1)
                    ComPort = availablePorts[0];
                _serialPort = new SerialPort(ComPort, BaudRate, Parity.None, 8, StopBits.One);
                _serialPort.ErrorReceived += Comm_ErrorReceived;
                _serialPort.DataReceived += Comm_DataReceivedBuffer;
                _serialPort.PinChanged += Comm_PinChanged;
                _serialPort.DtrEnable = true;
                _serialPort.Open();
                SetStatus(StatusEnm.Booting);
                LogController.LogAdd("CommController.Connect.ok");
                return true;
            }
            catch (UnauthorizedAccessException e)
            {
                LogController.LogAdd($"CommController.Connect.error: {e.GetBaseException().Message}");
                return false;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                LogController.LogAdd($"CommController.Connect.error: {e.GetBaseException().Message}");
                return false;
            }
        }

        private void Comm_PinChanged(object sender, SerialPinChangedEventArgs e)
        {
            LogController.LogAdd($"CommController.PinChanged: {e}");
        }

        public bool Disconnect()
        {
            LogController.LogAdd("CommController.Disconnect()");
            if (_serialPort == null || !_serialPort.IsOpen) return true;
            _serialPort.Close();
            return true;
        }

        private void Comm_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
        {
            Console.WriteLine($"error: {e.EventType}");
        }

        private void Comm_DataReceivedBuffer(object sender, SerialDataReceivedEventArgs e)
        {
            _busy = true;
            _buffer += _serialPort.ReadExisting();
            _lastOk = DateTime.Now;
            if (!_buffer.Contains(_newLine)) return;
            foreach (Match m in Regex.Matches(_buffer, @"\[(.*?)\]"))
            {
                LogController.LogAdd($"CommController.DataReceived> match: {m.Groups[1].ToString()}");
                CommandReceived?.Invoke(this, m.Groups[1].ToString());
            }
            _busy = false;

            //link goes online
            switch (_buffer)
            {
                case "[online]\r\n":
                    SetStatus(StatusEnm.Online);
                    if (_sendQueue.Any()) Dispatch();
                    break;
                case "[offline]\r\n":
                    SetStatus(StatusEnm.Offline);
                    break;
            }

            _buffer = string.Empty;
        }

        public void Dispose()
        {
            _serialPort?.Dispose();
        }


        public enum StatusEnm
        {
            Booting,
            Online,
            Offline
        }

        private StatusEnm SetStatus(StatusEnm newStatus)
        {
            if (LinkStatus == newStatus) return newStatus;
            LinkStatus = newStatus;
            StatusChanged?.Invoke(this, LinkStatus);
            return newStatus;
        }

        public bool Send(string str)
        {
            if (!Connect()) return false;
            _sendQueue.Enqueue(str+"\n");
            LogController.LogAdd($"CommController.Send: {str} [{_sendQueue.Count}]");

            if (_deliveryLock) return true;
            new Thread(() => {
                Thread.CurrentThread.IsBackground = true;
                Dispatch();
            }).Start();
            return true;
        }

        private static bool _deliveryLock = false;

        private void Dispatch()
        {
            if(LinkStatus != StatusEnm.Online) return;
            _deliveryLock = true;

            while (_busy)
            {
                LogController.LogAdd($"CommController.Dispatch(): busy..");
                Thread.Sleep(20);
            }

            while (_sendQueue.Count > 0)
            {
                if (_sendQueue.TryDequeue(out string sendString))
                {
                    LogController.LogAdd($"CommController.Dispatch(): {sendString}");
                    _serialPort.Write(sendString);
                    Thread.Sleep(2000);
                }
                else
                    Thread.Sleep(20);
            }
            _deliveryLock = false;
        }

    }
}
