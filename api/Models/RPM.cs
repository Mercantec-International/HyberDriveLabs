namespace API.Models
{
    public class RPM : Common
    {
        public int Speed { get; set; }
        public string CarId { get; set; }
        public Car Car { get; set; }
    }

    public class RPMDTO
    {
        public int Speed { get; set; }
        public string CarId { get; set; }
    }
}
