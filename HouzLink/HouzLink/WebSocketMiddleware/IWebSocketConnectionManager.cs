using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Net.WebSockets;
using System.Threading.Tasks;

namespace HouzLink.WebSocketMiddleware
{
    public interface IWebSocketConnectionManager
    {
        void AddSocket(WebSocket socket);
        void AddToGroup(string socketID, string groupID);
        ConcurrentDictionary<string, WebSocket> GetAll();
        List<string> GetAllFromGroup(string GroupID);
        string GetId(WebSocket socket);
        WebSocket GetSocketById(string id);
        void RemoveFromGroup(string socketID, string groupID);
        Task RemoveSocket(string id);
    }
}