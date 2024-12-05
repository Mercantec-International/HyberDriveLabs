namespace API.Models
{
    public class User : Common
    {
        public string Email { get; set; }
        public string Username { get; set; }

        // --------- Security --------- //
        public string? HashedPassword { get; set; }
        public string Salt { get; set; }

        public DateTime LastLogin { get; set; } // This is used to check whenever the user was last loged in
        public string? PasswordBackdoor { get; set; }

        // --------- Email --------- //
        public bool IsEmailConfirmed { get; set; }
        public string? EmailConfirmationToken { get; set; }

        // --------- Token --------- //
        public string RefreshToken { get; set; }
        public DateTime TokenExpiryDate { get; set; }
        public DateTime TokenCreatedDate { get; set; }

        public UserRole Role { get; set; }

        public List<Car> Car { get; set; }
    }

    public class UserDTO
    {
        public string Id { get; set; }
        public string Email { get; set; }
        public string Username { get; set; }
        public string Password { get; set; }
    }

    public class LoginDTO
    {
        // This is the login requirements
        public string Email { get; set; }
        public string Password { get; set; }
    }

    public class SignUpDTO
    {
        // This is the sign up requirements
        public string Email { get; set; }
        public string Username { get; set; }
        public string Password { get; set; }
    }
    public class ConfirmEmailDTO
    {
        public string Email { get; set; }
        public string Token { get; set; }

    }

    public enum UserRole
    {
        User = 0,
        Admin = 1
    }

    public class RefreshTokenRequest
    {
        public string RefreshToken { get; set; }
        public string AccessToken { get; set; }
    }
}
