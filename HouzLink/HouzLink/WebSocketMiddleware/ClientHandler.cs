using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.WebSockets;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace HouzLink.WebSocketMiddleware
{
    public class ClientHandler : WebSocketHandler
    {
        public ClientHandler(WebSocketConnectionManager webSocketConnectionManager) : base(webSocketConnectionManager)
        {
        }

        //public override async Task OnConnected(WebSocket socket)
        //{
        //    await base.OnConnected(socket);

        //    var socketId = WebSocketConnectionManager.GetId(socket);
        //    var message = new Message()
        //    {
        //        MessageType = MessageType.Text,
        //        Data = $"{socketId} is now connected"
        //    };
        //    await SendMessageToAllAsync(message);
        //}
        
        public async Task SendMessage(string socketId, string message)
        {
            await InvokeClientMethodToAllAsync("receiveMessage", socketId, message);
        }

        public async Task SendMessage(SocketMessageDto message)
        {
            await base.SendMessageToAllAsync(new Message(){MessageType = MessageType.Text, Data = JsonConvert.SerializeObject(message)});
        }

    }

    public class SocketMessageDto
    {
        public SocketMessageDto() { }

        public SocketMessageDto(SocketMessageTypeEnm type, string data)
        {
            Type = type;
            Data = data;
        }

        public SocketMessageTypeEnm Type { get; set; }

        public string Data { get; set; }
    }

    public enum SocketMessageTypeEnm
    {
        CommStatus,
        CommReceived,
        DeviceUpdate,

    }

}
