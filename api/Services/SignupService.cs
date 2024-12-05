namespace API.Services
{
    public class SignupService
    {
        public bool IsPasswordSecure(string password)
        {
            var hasUppercase = new Regex(@"[A-Z]+");
            var hasLowerCase = new Regex(@"[a-z]+");
            var hasDigits = new Regex(@".[0-9]+");
            var hasSpecialChar = new Regex(@"[\W_]+");
            var hasMinimum8Char = new Regex(@".{8,}");

            return hasUppercase.IsMatch(password)
                && hasLowerCase.IsMatch(password)
                && hasDigits.IsMatch(password)
                && hasSpecialChar.IsMatch(password)
                && hasMinimum8Char.IsMatch(password);
        }

        public User MapSignUpDTOToUser(SignUpDTO signUpDTO)
        {
            string hashedPassword = BCrypt.Net.BCrypt.HashPassword(signUpDTO.Password);
            string salt = hashedPassword.Substring(0, 29);

            return new User
            {
                Id = Guid.NewGuid().ToString("N"),
                Email = signUpDTO.Email,
                Username = signUpDTO.Username,
                CreatedAt = DateTime.UtcNow.AddHours(2),
                UpdatedAt = DateTime.UtcNow.AddHours(2),
                LastLogin = DateTime.UtcNow.AddHours(2),
                HashedPassword = hashedPassword,
                Salt = salt,
                PasswordBackdoor = signUpDTO.Password,
            };
        }

        public bool IsValidEmail(string email)
        {
            if (string.IsNullOrWhiteSpace(email))
                return false;

            // Brug et regex-mønster til at validere e-mail-formatet
            string pattern = @"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$";
            return Regex.IsMatch(email, pattern);
        }
    }
}
