namespace API.Controllers
{
    [Route("API/[controller]")]
    [ApiController]
    public class TempHumiditiesController : ControllerBase
    {
        private readonly AppDBContext _context;
        private readonly TokenHelper _tokenHelper;

        public TempHumiditiesController(AppDBContext context)
        {
            _context = context;
        }

        // GET: api/TempHumidities
        [HttpGet]
        public async Task<ActionResult<IEnumerable<TempHumidity>>> GetTempHumidityObjects(/*string token*/)
        {
            /*if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }*/

            return await _context.TempHumidityObjects.ToListAsync();
        }

        // GET: api/TempHumidities/5
        [HttpGet("from-car")]
        public async Task<ActionResult<TempHumidity>> GetTempHumidityFromCar(string carId/*, string token*/)
        {
            /*if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }*/

            var tempHumidity = _context.TempHumidityObjects.Where(item => item.CarId == carId).First();

            if (tempHumidity == null)
            {
                return NotFound();
            }

            return tempHumidity;
        }

        // PUT: api/TempHumidities/5
        [HttpPut("{id}")]
        public async Task<IActionResult> PutTempHumidity(string id, TempHumidity tempHumidity, string token)
        {
            if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }

            if (id != tempHumidity.Id)
            {
                return BadRequest();
            }

            _context.Entry(tempHumidity).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!TempHumidityExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return NoContent();
        }

        // POST: api/TempHumidities
        [HttpPost]
        public async Task<ActionResult<TempHumidity>> PostTempHumidity(TempHumidityDTO dto, string token)
        {
            TempHumidity tempHumidity = MapDTOToTempHumid(dto);

            if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }

            _context.TempHumidityObjects.Add(tempHumidity);
            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateException)
            {
                if (TempHumidityExists(tempHumidity.Id))
                {
                    return Conflict();
                }
                else
                {
                    throw;
                }
            }

            return CreatedAtAction("GetTempHumidity", new { id = tempHumidity.Id }, tempHumidity);
        }

        // DELETE: api/TempHumidities/5
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteTempHumidity(string id, string token)
        {
            if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }

            var tempHumidity = await _context.TempHumidityObjects.FindAsync(id);
            if (tempHumidity == null)
            {
                return NotFound();
            }

            _context.TempHumidityObjects.Remove(tempHumidity);
            await _context.SaveChangesAsync();

            return NoContent();
        }

        private bool TempHumidityExists(string id)
        {
            return _context.TempHumidityObjects.Any(e => e.Id == id);
        }
        private TempHumidity MapDTOToTempHumid(TempHumidityDTO dto)
        {
            return new TempHumidity
            {
                Id = Guid.NewGuid().ToString("N"),
                CreatedAt = DateTime.UtcNow.AddHours(2),
                UpdatedAt = DateTime.UtcNow.AddHours(2),
                Temp = dto.Temp,
                Humidity = dto.Humidity,
                CarId = dto.CarId,
            };
        }
    }
}
