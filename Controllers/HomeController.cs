using System.Diagnostics;
using Microsoft.AspNetCore.Mvc;
using IOTProject.Models;
using Newtonsoft.Json;
using IOTProject.Data;
using Microsoft.EntityFrameworkCore;

namespace IOTProject.Controllers;

public class HomeController : Controller
{
    private readonly AppDbContext _context;
    public HomeController( AppDbContext context)
    {
        _context = context;
    }

    public IActionResult Index()
    {
        return View();
    }

    public ContentResult JSON()
    {

        var dataPoints = _context.MoistureDatas.Select(x => new DataPoint(x.Timestamp.Ticks, x.Moisture)).ToList();

        JsonSerializerSettings _jsonSetting = new JsonSerializerSettings() { NullValueHandling = NullValueHandling.Ignore };
        return Content(JsonConvert.SerializeObject(dataPoints, _jsonSetting), "application/json");
    }

    public IActionResult Privacy()
    {
        return View();
    }

    [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
    public IActionResult Error()
    {
        return View(new ErrorViewModel { RequestId = Activity.Current?.Id ?? HttpContext.TraceIdentifier });
    }
}
