namespace API.Controllers
{
    [Route("API/[controller]")]
    [ApiController]
    public class CarsController : ControllerBase
    {
        private readonly AppDBContext _context;
        private readonly TokenHelper _tokenHelper;

        public CarsController(AppDBContext context)
        {
            _context = context;
        }

        // GET: api/Cars
        [HttpGet]
        public async Task<ActionResult<IEnumerable<Car>>> GetCars(/*string token*/)
        {
            /*if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }*/

            return await _context.Cars.ToListAsync();
        }

        // GET: api/Cars/5
        [HttpGet("from-user")]
        public async Task<ActionResult<Car>> GetCar(string userId/*, string token*/)
        {
            /*if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }*/

            var car = _context.Cars.Where(item => item.UserId == userId).First();

            if (car == null)
            {
                return NotFound();
            }

            return car;
        }

        // PUT: api/Cars/5
        [HttpPut("{id}")]
        public async Task<IActionResult> PutCar(string id, Car car, string token)
        {
            if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }

            if (id != car.Id)
            {
                return BadRequest();
            }

            _context.Entry(car).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!CarExists(id))
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

        // POST: api/Cars
        [HttpPost]
        public async Task<ActionResult<Car>> PostCar(CarDTO dto, string token)
        {
            Car car = MapDTOToCar(dto);
            if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }

            _context.Cars.Add(car);
            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateException)
            {
                if (CarExists(car.Id))
                {
                    return Conflict();
                }
                else
                {
                    throw;
                }
            }

            return CreatedAtAction("GetCar", new { id = car.Id }, car);
        }

        // DELETE: api/Cars/5
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteCar(string id, string token)
        {
            if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }

            var car = await _context.Cars.FindAsync(id);
            if (car == null)
            {
                return NotFound();
            }

            _context.Cars.Remove(car);
            await _context.SaveChangesAsync();

            return NoContent();
        }

        private bool CarExists(string id)
        {
            return _context.Cars.Any(e => e.Id == id);
        }
        private Car MapDTOToCar(CarDTO dto)
        {
            return new Car
            {
                Id = Guid.NewGuid().ToString("N"),
                CreatedAt = DateTime.UtcNow.AddHours(2),
                UpdatedAt = DateTime.UtcNow.AddHours(2),
                UserId = dto.UserId,
                LastEmergency = dto.LastEmergency,
            };
        }
    }
}
