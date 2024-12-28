using IOTProject.Data;
using IOTProject.Models;
using Microsoft.AspNetCore.Mvc;

namespace IOTProject.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class SoilController : ControllerBase
    {
        private readonly AppDbContext _context;
        private static string _pumpStatus = "off";

        public SoilController(AppDbContext context)
        {
            _context = context;
        }

        [HttpPost("soil-data")]
        public async Task<IActionResult> StoreMoisture([FromBody] MoistureData data)
        {
            if (data.Moisture < 0)
                return BadRequest("Invalid moisture value.");

            _context.MoistureDatas.Add(data);
            await _context.SaveChangesAsync();

            //// تصمیم‌گیری برای وضعیت پمپ
            //_pumpStatus = data.Moisture < 300 ? "on" : "off";

            return Created("", new { message = "Moisture level recorded successfully.", pump_status = _pumpStatus });
        }

        [HttpGet("soil-status")]
        public IActionResult GetSoilStatus()
        {
            var latestData = _context.MoistureDatas.OrderByDescending(m => m.Timestamp).FirstOrDefault();
            //if (latestData == null)
            //    return NotFound("No data found.");

            return Ok(new { moisture = latestData?.Moisture ?? 0, pump_status = _pumpStatus });
        }

        [HttpPost("pump-status")]
        public IActionResult UpdatePumpStatus([FromBody] PumpStatusDto dto)
        {
            if (dto.PumpStatus != "on" && dto.PumpStatus != "off")
                return BadRequest("Invalid pump status.");

            _pumpStatus = dto.PumpStatus;

            return Ok(new { message = "Pump status updated successfully.", pump_status = _pumpStatus });
        }

        [HttpPost("last-status")]
        public IActionResult GetMoistureHistory([FromQuery] int count = 10)
        {
            var history = _context.MoistureDatas
                .OrderByDescending(m => m.Timestamp)
                .Take(count)
                .Select(m => new { m.Moisture, m.Timestamp })
                .ToList();

            return Ok(new { message = "Moisture history retrieved successfully.", data = history });
        }
    }
}
