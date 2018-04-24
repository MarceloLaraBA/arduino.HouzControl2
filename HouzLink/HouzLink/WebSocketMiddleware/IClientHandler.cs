using System.Threading.Tasks;

namespace HouzLink.WebSocketMiddleware
{
    public interface IClientHandler
    {
        Task SendMessage(MessageType type, object data);
    }
}