namespace API.Services
{
    public class TokenHelper
    {
        private readonly AppDBContext _context;
        
        public async Task<string> ValidToken(string token)
        {
            var refreshToken = await _context.Token.FirstOrDefaultAsync(rt => rt.RefreshToken == token);

            if (refreshToken == null || refreshToken.ExpiryDate <= DateTime.UtcNow)
            {
                return ("Invalid or expired refresh token");
            }

            return ("Valid token");
        }
    }
}
