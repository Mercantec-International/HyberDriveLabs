namespace API.Models
{
    public class JWT : Common
    {
        public string RefreshToken { get; set; }
        public string UserId { get; set; }
        public User User { get; set; }
        public DateTime ExpiryDate { get; set; }
        public DateTime CreatedDate { get; set; }
    }
}
