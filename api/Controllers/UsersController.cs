namespace API.Controllers
{
    [Route("API/[controller]")]
    [ApiController]
    public class UsersController : ControllerBase
    {
        private readonly AppDBContext _context;
        private readonly TokenHelper _tokenHelper;
        private readonly JWTService _jwtService;
        private readonly SignupService _signupService;
        private readonly IConfiguration _configuration;
        private readonly EmailService _emailService;

        public UsersController(AppDBContext context, EmailService emailService, JWTService jwtService, SignupService signupService)
        {
            _context = context;
            _jwtService = jwtService;
            _signupService = signupService;
            _emailService = emailService;
        }

        // GET: api/User
        //[Authorize]
        [HttpGet]
        public async Task<ActionResult<IEnumerable<UserDTO>>> GetUser(/*string token*/)
        {
            /*if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }*/

            var User = await _context.User.Select(user => new UserDTO
            {
                Id = user.Id,
                Email = user.Email,
                Username = user.Username
            })
            .ToListAsync();

            return Ok(User);
        }

        // PUT: api/Users/5
        [HttpPut("{id}")]
        public async Task<IActionResult> PutUser(string id, User user, string token)
        {
            if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }

            if (id != user.Id)
            {
                return BadRequest();
            }

            _context.Entry(user).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!UserExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return NoContent();
        }

        // POST: api/User
        [HttpPost("signUp")]
        public async Task<ActionResult<User>> PostUser(SignUpDTO signUpDTO)
        {
            if (await _context.User.AnyAsync(u => u.Username == signUpDTO.Username))
            {
                return Conflict(new { message = "Username is already in use." });
            }
            if (await _context.User.AnyAsync(u => u.Email == signUpDTO.Email))
            {
                return Conflict(new { message = "Email is already in use." });
            }
            if (!_signupService.IsPasswordSecure(signUpDTO.Password))
            {
                return Conflict(new { message = "Password isnt secure." });
            }

            var user = _signupService.MapSignUpDTOToUser(signUpDTO);
            user.EmailConfirmationToken = Guid.NewGuid().ToString();
            user.IsEmailConfirmed = false;

            _context.User.Add(user);


            try
            {
                await _context.SaveChangesAsync();
                
                await _emailService.SendConfirmationEmail(user.Email);
            }
            catch (DbUpdateException)
            {

            }

            return Ok(new
            {
                user.Id,
                user.Email,
                message = "User created. Please check your email to confirm your account."
            });
            return Ok("User signup sucessful");
        }

        // POST: api/User
        [HttpPost("login")]
        public async Task<IActionResult> Login(LoginDTO loginDTO)
        {
            var user = await _context.User.SingleOrDefaultAsync(u => u.Email == loginDTO.Email);

            if (user == null || !BCrypt.Net.BCrypt.Verify(loginDTO.Password, user.HashedPassword))
            {
                return Unauthorized(new { message = "Ugyldig email eller adgangskode." });
            }

            if (!user.IsEmailConfirmed)
            {
                return Unauthorized(
                    new
                    {
                        message = "Email er ikke bekræftet. Tjek venligst din email for bekræftelses-link."
                    }
                );
            }

            var (accessToken, refreshToken) = _jwtService.GenerateTokens(user);

            return Ok(
                new
                {
                    accessToken,
                    refreshToken,
                    expiresIn = 30
                }
            );
        }

        [HttpPost("refresh-token")]
        public async Task<IActionResult> RefreshToken([FromBody] RefreshTokenRequest request)
        {
            try
            {
                var handler = new JwtSecurityTokenHandler();
                var jsonToken = handler.ReadToken(request.AccessToken) as JwtSecurityToken;
                var userId = jsonToken
                    ?.Claims.First(claim => claim.Type == JwtRegisteredClaimNames.Sub)
                    .Value;

                if (userId == null)
                {
                    return BadRequest(new { message = "Invalid token format" });
                }

                var user = await _context.User.SingleOrDefaultAsync(u => u.Id == userId);

                if (user == null)
                {
                    return NotFound(new { message = "User not found" });
                }

                if (!_jwtService.ValidateRefreshToken(request.RefreshToken))
                {
                    return Unauthorized(new { message = "Invalid refresh token" });
                }

                var (accessToken, newRefreshToken) = _jwtService.GenerateTokens(user);

                return Ok(
                    new
                    {
                        accessToken,
                        refreshToken = newRefreshToken,
                        expiresIn = 30
                    }
                );
            }
            catch (Exception)
            {
                return BadRequest(new { message = "Invalid token" });
            }
        }

        private string GenerateJWT(User user)
        {
            var keyString = _configuration["JwtSettings:Key"] ?? Environment.GetEnvironmentVariable("Key");
            var issuer = _configuration["JwtSettings:Issuer"] ?? Environment.GetEnvironmentVariable("Issuer");
            var audience = _configuration["JwtSettings:Audience"] ?? Environment.GetEnvironmentVariable("Audience");

            // Log the values
            Console.WriteLine($"Key: {keyString}");
            Console.WriteLine($"Issuer: {issuer}");
            Console.WriteLine($"Audience: {audience}");

            var claims = new[]
            {
                new Claim(JwtRegisteredClaimNames.Sub, user.Id),
                new Claim(JwtRegisteredClaimNames.Jti, Guid.NewGuid().ToString()),
                new Claim(ClaimTypes.Name, user.Username),
                new Claim(JwtRegisteredClaimNames.Sub, user.Id.ToString()), // Convert ID to string
            };

            var key = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(keyString));
            var creds = new SigningCredentials(key, SecurityAlgorithms.HmacSha256);

            var token = new JwtSecurityToken(
                issuer,
                audience,
                claims,
                expires: DateTime.Now.AddDays(1),
                signingCredentials: creds);

            return new JwtSecurityTokenHandler().WriteToken(token);
        }

        private string GenerateRefreshToken()
        {

            return "";
        }

        [HttpGet("confirm-email")]
        public async Task<IActionResult> ConfirmEmail(string token, string email)
        {
            // TODO: CHECK IF TOKEN IS THE SAME
            var user = await _context.User.SingleOrDefaultAsync(u =>
                u.Email == email
            );
            if (user == null)
            {
                // TODO: UPDATE URL
                return Redirect($"https://bing.com");
            }

            user.IsEmailConfirmed = true;
            user.EmailConfirmationToken = null;
            await _context.SaveChangesAsync();

            // TODO: UPDATE URL
            return Redirect($"https://google.com");
        }

        // DELETE: api/Users/5
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteUser(string id)
        {
            var user = await _context.User.FindAsync(id);
            if (user == null)
            {
                return NotFound();
            }

            _context.User.Remove(user);
            await _context.SaveChangesAsync();

            return NoContent();
        }

        private bool UserExists(string id)
        {
            return _context.User.Any(e => e.Id == id);
        }
    }
}
