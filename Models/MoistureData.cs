namespace IOTProject.Models
{
    public class MoistureData
    {
        public int Id { get; set; }
        public int Moisture { get; set; }
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;
    }
}
