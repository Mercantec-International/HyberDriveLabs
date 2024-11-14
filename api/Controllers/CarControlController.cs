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
                await HandleCarControl(webSocket);
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
                    string response = "";

                    switch (command.ToLower())
                    {
                        case "forward":
                            response = "Bilen kører fremad";
                            _logger.LogInformation("Udfører: Kør fremad");
                            var forwardMsg = Encoding.UTF8.GetBytes("forward");
                            await webSocket.SendAsync(
                                new ArraySegment<byte>(forwardMsg),
                                WebSocketMessageType.Text,
                                true,
                                CancellationToken.None
                            );
                            break;
                        case "backward":
                            response = "Bilen bakker";
                            _logger.LogInformation("Udfører: Bak");
                            break;
                        case "stop":
                            response = "Bilen stopper";
                            _logger.LogInformation("Udfører: Stop");
                            break;
                        default:
                            response = "Ukendt kommando";
                            _logger.LogWarning($"Ukendt kommando modtaget: {command}");
                            break;
                    }

                    var serverMsg = Encoding.UTF8.GetBytes(response);
                    await webSocket.SendAsync(
                        new ArraySegment<byte>(serverMsg, 0, serverMsg.Length),
                        receiveResult.MessageType,
                        receiveResult.EndOfMessage,
                        CancellationToken.None
                    );
                    _logger.LogInformation($"Sendt svar: {response}");

                    receiveResult = await webSocket.ReceiveAsync(
                        new ArraySegment<byte>(buffer),
                        CancellationToken.None
                    );
                }

                _logger.LogInformation("WebSocket forbindelse lukkes normalt");
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
