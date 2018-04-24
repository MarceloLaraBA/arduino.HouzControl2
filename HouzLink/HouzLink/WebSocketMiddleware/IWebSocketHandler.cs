using System.Net.WebSockets;
using System.Threading.Tasks;

namespace HouzLink.WebSocketMiddleware
{
    public interface IWebSocketHandler
    {
        Task InvokeClientMethodAsync(string socketId, string methodName, object[] arguments);
        Task InvokeClientMethodToAllAsync(string methodName, params object[] arguments);
        Task InvokeClientMethodToGroupAsync(string groupID, string methodName, params object[] arguments);
        Task InvokeClientMethodToGroupAsync(string groupID, string methodName, string except, params object[] arguments);
        Task OnConnected(WebSocket socket);
        Task OnDisconnected(WebSocket socket);
        Task ReceiveAsync(WebSocket socket, WebSocketReceiveResult result, string serializedInvocationDescriptor);
        Task SendMessageAsync(string socketId, Message message);
        Task SendMessageAsync(WebSocket socket, Message message);
        Task SendMessageToAllAsync(Message message);
        Task SendMessageToGroupAsync(string groupID, Message message);
        Task SendMessageToGroupAsync(string groupID, Message message, string except);
    }
}