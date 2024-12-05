namespace API.Models
{
    public class Car : Common
    {
        public DateTime LastEmergency { get; set; }
        public string UserId { get; set; }
        public User User { get; set; }
    }
    public class CarDTO
    {
        public DateTime LastEmergency { get; set; }
        public string UserId { get; set; }
    }
}
