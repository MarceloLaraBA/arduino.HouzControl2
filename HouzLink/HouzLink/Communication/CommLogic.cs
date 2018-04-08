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
    public class CommLogic : IDisposable
    {

        public string ComPort { get; set; } = "COM3";
        public int BaudRate { get; set; } = 115200;

        public StatusEnm Status => _serialPort != null && _serialPort.IsOpen ? StatusEnm.Online : StatusEnm.Offline;

        public StatusEnm LinkStatus => _linkStatus;
        private static StatusEnm _linkStatus = StatusEnm.Booting;

        public CommLogic()
        {
            LogController.LogAdd("CommLogic.ctr");
        }

        //events
        public static event EventHandler<string> CommandReceived;
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

            LogController.LogAdd("CommLogic.Connect()");
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
                _linkStatus = StatusEnm.Booting;
                LogController.LogAdd("CommLogic.Connect.ok");
                return true;
            }
            catch (UnauthorizedAccessException e)
            {
                LogController.LogAdd($"CommLogic.Connect.error: {e.GetBaseException().Message}");
                return false;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                LogController.LogAdd($"CommLogic.Connect.error: {e.GetBaseException().Message}");
                return false;
            }
        }

        private void Comm_PinChanged(object sender, SerialPinChangedEventArgs e)
        {
            LogController.LogAdd($"CommLogic.PinChanged: {e}");
        }

        public bool Disconnect()
        {
            LogController.LogAdd("CommLogic.Disconnect()");
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
            LogController.LogAdd("CommLogic.DataReceived()");
            _buffer += _serialPort.ReadExisting();
            _lastOk = DateTime.Now;
            if (!_buffer.Contains(_newLine)) return;
            LogController.LogAdd($"CommLogic.DataReceived> hasNewLine: {_buffer}");

            foreach (Match m in Regex.Matches(_buffer, @"\[(.*?)\]"))
            {
                LogController.LogAdd($"CommLogic.DataReceived> match: {m.Groups[1].ToString()}");
                CommandReceived?.Invoke(this, m.Groups[1].ToString());
            }
            _busy = false;

            //link goes online
            if (_buffer == "[online]\r\n")
            {
                _linkStatus = StatusEnm.Online;
                if (_sendQueue.Any()) Deliver();
            }
            if (_buffer == "[offline]\r\n") _linkStatus = StatusEnm.Offline;

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

        public bool Send(string str)
        {
            if (!Connect()) return false;
            _sendQueue.Enqueue(str+"\n");
            LogController.LogAdd($"CommLogic.Send: {str} [{_sendQueue.Count}]");

            if (_deliveryLock) return true;
            new Thread(() => {
                Thread.CurrentThread.IsBackground = true;
                Deliver();
            }).Start();
            return true;
        }

        private static bool _deliveryLock = false;

        private void Deliver()
        {
            if(_linkStatus != StatusEnm.Online) return;
            _deliveryLock = true;

            while (_busy)
            {
                LogController.LogAdd($"CommLogic.Deliver(): busy..");
                Thread.Sleep(20);
            }

            while (_sendQueue.Count > 0)
            {
                if (_sendQueue.TryDequeue(out string sendString))
                {
                    LogController.LogAdd($"CommLogic.Deliver(): {sendString}");
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
