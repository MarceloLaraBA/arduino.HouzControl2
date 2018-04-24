using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;

namespace HouzLink.WebSocketMiddleware
{
    public interface IWebSocketManagerMiddleware
    {
        Task Invoke(HttpContext context);
    }
}