using IOTProject.Models;
using Microsoft.EntityFrameworkCore;

namespace IOTProject.Data
{
    public class AppDbContext : DbContext
    {
        public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }

        public DbSet<MoistureData> MoistureDatas { get; set; }
    }
}
