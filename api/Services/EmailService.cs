namespace API.Services
{

    public class EmailService
    {
        private readonly IConfiguration _configuration;
        private readonly string _templatePath;

        public EmailService(IConfiguration configuration, IWebHostEnvironment environment)
        {
            _configuration = configuration;
            _templatePath = Path.Combine(
                environment.ContentRootPath,
                "API",
                "Templates",
                "EmailConfirmation.html"
            );
        }

        private async Task<string> GetEmailTemplate(string confirmationUrl)
        {
            var assembly = typeof(EmailService).Assembly;
            var resourceName = "API.Templates.EmailConfirmation.html";

            using var stream = assembly.GetManifestResourceStream(resourceName);
            using var reader = new StreamReader(stream);

            string template = await reader.ReadToEndAsync();
            return template.Replace("{confirmationUrl}", confirmationUrl);
        }

        public async Task SendConfirmationEmail(string email)
        {
            try
            {
                string confirmationToken = Guid.NewGuid().ToString();
                var confirmationUrl = $"https://hyberdrivelabs.onrender.com/API/Users/confirm-email?token={confirmationToken}&email={email}";
                var emailBody = await GetEmailTemplate(confirmationUrl);

                var smtpClient = new SmtpClient
                {
                    Host = "smtp.gmail.com",
                    Port = 587,
                    UseDefaultCredentials = false,
                    EnableSsl = true,
                    DeliveryMethod = SmtpDeliveryMethod.Network,
                    Credentials = new NetworkCredential(_configuration["EmailService:Email"] ?? Environment.GetEnvironmentVariable("Email"), _configuration["EmailService:Password"] ?? Environment.GetEnvironmentVariable("Password"))
                };

                var mailMessage = new MailMessage
                {
                    From = new MailAddress(_configuration["EmailService:Email"] ?? Environment.GetEnvironmentVariable("Email")),
                    Subject = "HyperDrive Labs - Confirm your email address",
                    Body = emailBody,
                    IsBodyHtml = true
                };
                mailMessage.To.Add(email);

                await smtpClient.SendMailAsync(mailMessage);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Email sending failed: {ex.Message}");
                throw;
            }
        }
    }
}
