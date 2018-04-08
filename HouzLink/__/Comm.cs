using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace HouzLink
{
    public class Comm : IDisposable
    {
        public string ComPort { get; set; } = "COM3";
        public int BaudRate { get; set; } = 115200;
        public StatusEnm Status { get; private set; } = StatusEnm.Offline;

        //events
        public event EventHandler<string> CommandReceived;
        private ConcurrentQueue<string> _sendQueue = new ConcurrentQueue<string>();
        private bool _busy = false;

        //hardware stuff
        private SerialPort _serialPort;
        private string _buffer;
        private const string _newLine = "\n";
        private DateTime _lastOk;
        private int _voidInterval = 10000;

        public void Connect()
        {
            try
            {
                //scan comm ports
                string[] availablePorts = SerialPort.GetPortNames();
                if (availablePorts.Length == 1)
                    ComPort = availablePorts[0];
                _serialPort = new SerialPort(ComPort, BaudRate, Parity.None, 8, StopBits.One);
                _serialPort.ErrorReceived += Comm_ErrorReceived;
                _serialPort.DataReceived += Comm_DataReceivedBuffer;
                _serialPort.DtrEnable = true;
                _serialPort.Open();
                Status = _serialPort.IsOpen ? StatusEnm.Online : StatusEnm.Offline;
            }
            catch (UnauthorizedAccessException e)
            {
                Status = StatusEnm.Offline;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                Status = StatusEnm.Offline;
            }
        }

        private void Comm_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
        {
            Console.WriteLine($"error: {e.EventType}");
        }

        private void Comm_DataReceivedBuffer(object sender, SerialDataReceivedEventArgs e)
        {
            _buffer += _serialPort.ReadExisting();
            if (!_buffer.Contains(_newLine)) return;
            string rec = _buffer;
            _buffer = string.Empty;
            foreach (Match m in Regex.Matches(rec, @"\[(.*?)\]"))
            {
                CommandReceived?.Invoke(this, m.Groups[1].ToString());
            }
        }

        public void Dispose()
        {
            _serialPort?.Dispose();
        }

        public enum StatusEnm
        {
            Online,
            Offline
        }

        public bool Send(string str)
        {
            if (Status != StatusEnm.Online) return false;
            _sendQueue.Enqueue(str);

            if(_deliveryLock) return true;
            new Thread(() => {
                Thread.CurrentThread.IsBackground = true;
                Deliver();
            }).Start();
            return true;
        }

        private bool _deliveryLock = false;

        private void Deliver()
        {
            _deliveryLock = true;
            while (this._busy)
                Thread.Sleep(20);

            while (this._sendQueue.Count > 0)
            {
                if(_sendQueue.TryDequeue(out string sendString))
                    _serialPort.Write(sendString);
                else
                    Thread.Sleep(20);
            }
            _deliveryLock = false;
        }

    }
}
