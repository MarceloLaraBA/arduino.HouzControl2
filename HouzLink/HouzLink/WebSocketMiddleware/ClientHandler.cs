using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.WebSockets;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Serialization;

namespace HouzLink.WebSocketMiddleware
{
    public class ClientHandler : WebSocketHandler, IClientHandler, IWebSocketHandler
    {
        private readonly JsonSerializerSettings _jsonSerializerSettings = new JsonSerializerSettings()
        {
            ContractResolver = new CamelCasePropertyNamesContractResolver()
        };

        public ClientHandler(IWebSocketConnectionManager wscm) : base(wscm)
        {
        }

        //public ClientHandler(WebSocketConnectionManager webSocketConnectionManager) : base(webSocketConnectionManager)
        //{
        //}
        
        //public async Task SendMessage(string socketId, string message)
        //{
        //    await InvokeClientMethodToAllAsync("receiveMessage", socketId, message);
        //}

        public async Task SendMessage(MessageType type, object data)
        {
            await base.SendMessageToAllAsync(new Message() { MessageType = type, Data = JsonConvert.SerializeObject(data, _jsonSerializerSettings) });
        }

    }


}
