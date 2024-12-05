namespace API.Controllers
{
    [Route("API/[controller]")]
    [ApiController]
    public class RPMsController : ControllerBase
    {
        private readonly AppDBContext _context;
        private readonly TokenHelper _tokenHelper;

        public RPMsController(AppDBContext context)
        {
            _context = context;
        }

        // GET: api/RPMs
        [HttpGet]
        public async Task<ActionResult<IEnumerable<RPM>>> GetRPMs(/*string token*/)
        {
            /*if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }*/

            return await _context.RPMs.ToListAsync();
        }

        // GET: api/RPMs/5
        [HttpGet("from-car")]
        public async Task<ActionResult<RPM>> GetRPM(string carId/*, string token*/)
        {
            /*if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }*/

            var rPM = _context.RPMs.Where(item => item.CarId == carId).First();

            if (rPM == null)
            {
                return NotFound();
            }

            return rPM;
        }

        // PUT: api/RPMs/5
        [HttpPut("{id}")]
        public async Task<IActionResult> PutRPM(string id, RPM rPM, string token)
        {
            if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }

            if (id != rPM.Id)
            {
                return BadRequest();
            }

            _context.Entry(rPM).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!RPMExists(id))
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

        // POST: api/RPMs
        [HttpPost]
        public async Task<ActionResult<RPM>> PostRPM(RPMDTO dto, string token)
        {
            RPM rPM = MapDTOToRPM(dto);
            if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }

            _context.RPMs.Add(rPM);
            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateException)
            {
                if (RPMExists(rPM.Id))
                {
                    return Conflict();
                }
                else
                {
                    throw;
                }
            }

            return CreatedAtAction("GetRPM", new { id = rPM.Id }, rPM);
        }

        // DELETE: api/RPMs/5
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteRPM(string id, string token)
        {
            if (await _tokenHelper.ValidToken(token) != "Valid token")
            {
                return BadRequest("Invalid or expired refresh token");
            }

            var rPM = await _context.RPMs.FindAsync(id);
            if (rPM == null)
            {
                return NotFound();
            }

            _context.RPMs.Remove(rPM);
            await _context.SaveChangesAsync();

            return NoContent();
        }

        private bool RPMExists(string id)
        {
            return _context.RPMs.Any(e => e.Id == id);
        }
        private RPM MapDTOToRPM(RPMDTO dto)
        {
            return new RPM
            {
                Id = Guid.NewGuid().ToString("N"),
                CreatedAt = DateTime.UtcNow.AddHours(2),
                UpdatedAt = DateTime.UtcNow.AddHours(2),
                Speed = dto.Speed,
                CarId = dto.CarId,
            };
        }
    }
}
