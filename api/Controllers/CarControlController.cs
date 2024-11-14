using System.Net.WebSockets;
using System.Text;
using Microsoft.AspNetCore.Mvc;

namespace api.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class CarControlController : ControllerBase
    {
        private readonly ILogger<CarControlController> _logger;
        private static List<WebSocket> _connectedClients = new List<WebSocket>();

        public CarControlController(ILogger<CarControlController> logger)
        {
            _logger = logger;
        }

        [HttpGet("/ws")]
        public async Task HandleWebSocket()
        {
            if (HttpContext.WebSockets.IsWebSocketRequest)
            {
                _logger.LogInformation("WebSocket forbindelse anmodet");
                using var webSocket = await HttpContext.WebSockets.AcceptWebSocketAsync();
                _logger.LogInformation("WebSocket forbindelse accepteret");

                // Tilføj klienten til listen
                _connectedClients.Add(webSocket);

                try
                {
                    await HandleCarControl(webSocket);
                }
                finally
                {
                    // Fjern klienten fra listen når forbindelsen lukkes
                    _connectedClients.Remove(webSocket);
                }
            }
            else
            {
                _logger.LogWarning("Ugyldig WebSocket anmodning modtaget");
                HttpContext.Response.StatusCode = StatusCodes.Status400BadRequest;
            }
        }

        private async Task HandleCarControl(WebSocket webSocket)
        {
            var buffer = new byte[1024 * 4];
            try
            {
                var receiveResult = await webSocket.ReceiveAsync(
                    new ArraySegment<byte>(buffer),
                    CancellationToken.None
                );

                while (!receiveResult.CloseStatus.HasValue)
                {
                    string command = Encoding.UTF8.GetString(buffer, 0, receiveResult.Count);
                    _logger.LogInformation($"Modtaget kommando: {command}");

                    // Send kommandoen til alle tilsluttede klienter
                    var message = Encoding.UTF8.GetBytes(command);
                    var tasks = _connectedClients
                        .Where(client => client != webSocket && client.State == WebSocketState.Open)
                        .Select(client =>
                            client.SendAsync(
                                new ArraySegment<byte>(message),
                                WebSocketMessageType.Text,
                                true,
                                CancellationToken.None
                            )
                        );

                    await Task.WhenAll(tasks);

                    receiveResult = await webSocket.ReceiveAsync(
                        new ArraySegment<byte>(buffer),
                        CancellationToken.None
                    );
                }

                await webSocket.CloseAsync(
                    receiveResult.CloseStatus.Value,
                    receiveResult.CloseStatusDescription,
                    CancellationToken.None
                );
            }
            catch (Exception ex)
            {
                _logger.LogError($"Fejl i WebSocket håndtering: {ex.Message}");
                if (webSocket.State == WebSocketState.Open)
                {
                    await webSocket.CloseAsync(
                        WebSocketCloseStatus.InternalServerError,
                        "Der opstod en fejl",
                        CancellationToken.None
                    );
                }
            }
        }
    }
}
