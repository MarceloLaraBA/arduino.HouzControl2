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
    public class CommDriver : IDisposable, ICommDriver
    {

        public string ComPort { get; private set; } = "COM3";
        public int BaudRate { get; set; } = 115200;

        public StatusEnm Status => _serialPort != null && _serialPort.IsOpen ? StatusEnm.Online : StatusEnm.Offline;

        public StatusEnm LinkStatus { get; private set; }

        //events
        public event EventHandler<string> CommandReceived;
        public event EventHandler<StatusEnm> StatusChanged;
        private readonly ConcurrentQueue<string> _sendQueue = new ConcurrentQueue<string>();

        //hardware stuff
        private SerialPort _serialPort;
        private string _buffer;
        private bool _busy = false;
        private const string _newLine = "\n";

        private static DateTime _lastOk;
        private int _voidInterval = 10000;

        public bool Connect()
        {
            if (_busy) return _serialPort.IsOpen;
            _busy = true;

            if (_serialPort != null)
            {
                if (_lastOk.AddSeconds(_voidInterval) > DateTime.Now && _serialPort.IsOpen) return true;
                if (_serialPort.IsOpen) _serialPort.Close();
            }

            LogController.LogAdd("CommDriver.Connect()");
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
                LogController.LogAdd("CommDriver.Connect.ok");
                _busy = false;
                return true;
            }
            catch (UnauthorizedAccessException e)
            {
                _busy = false;
                LogController.LogAdd($"CommDriver.Connect.error: {e.GetBaseException().Message}");
                SetStatus(StatusEnm.ComClosed);
                return false;
            }
            catch (Exception e)
            {
                _busy = false;
                SetStatus(StatusEnm.ComClosed);
                LogController.LogAdd($"CommDriver.Connect.error: {e.GetBaseException().Message}");
                return false;
            }
        }

        private void Comm_PinChanged(object sender, SerialPinChangedEventArgs e)
        {
            LogController.LogAdd($"CommDriver.PinChanged: {e}");
        }

        public bool Disconnect()
        {
            LogController.LogAdd("CommDriver.Disconnect()");
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
            string rec = (sender as SerialPort)?.ReadExisting();
            LogController.LogAdd($"CommDriver.DataReceived> {rec}");
            _buffer += rec;
            _lastOk = DateTime.Now;
            if (!_buffer.Contains(_newLine)) return;
            foreach (Match m in Regex.Matches(_buffer, @"\[(.*?)\]"))
            {
                LogController.LogAdd($"CommDriver.DataReceived> match: {m.Groups[1].ToString()}");
                CommandReceived?.Invoke(this, m.Groups[1].ToString());
            }
            _busy = false;

            //link goes online
            switch (_buffer)
            {
                case "[online]\r\n":
                    SetStatus(StatusEnm.Online);
                    if (_sendQueue.Any()) DispatchAsync();
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
            ComClosed,
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
          //  if (!Connect()) return false;
            _sendQueue.Enqueue(str+"\n");
            LogController.LogAdd($"CommDriver.Send: {str} [{_sendQueue.Count}] [lock: {_deliveryLock}]");

            if (_deliveryLock) return true;
            _deliveryLock = true;
            new Thread(async () => {
                Thread.CurrentThread.IsBackground = true;
                await DispatchAsync();
            }).Start();
            return true;
        }

        private bool _deliveryLock;

        public CommDriver()
        {
            Connect();
        }

        private async Task DispatchAsync()
        {
            if(LinkStatus != StatusEnm.Online) return;
            while (_busy)
            {
                LogController.LogAdd($"CommDriver.Dispatch(): busy..");
                Thread.Sleep(20);
            }

            while (_sendQueue.Count > 0)
            {
                if (_sendQueue.TryDequeue(out string sendString))
                {
                    LogController.LogAdd($"CommDriver.Dispatch(): {sendString}");
                    _serialPort.Write(sendString);
                }
                else
                    await Task.Delay(20);
            }
            _deliveryLock = false;
        }

    }
}
