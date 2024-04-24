#include "noise.h"

/**
 * Interpolation function -> 3x^2 - 2x^3
 */
#define S0(x) ((3 - 2*x) * (x*x))

/**
 * Smooth interpolation function -> 6x^5 - 15x^4 + 10x^3
 */
#define S1(x) ((6 * x * x - 15 * x + 10) * (x * x * x))

/**
 * \brief Size for the permutation table
 */
const int permSize = 256;

/**
 * \brief Value to limit values between 0 and 255 via & operator
 */
const int maxValue = permSize - 1;

/**
 * \brief Size of the gradient table for 2 dimensional noise
 */
const int g2Size = 8;

/**
 * \brief Value to limit values between 0 and 7 via & operator
 */
const int g2MaxValue = g2Size - 1;

/**
 * \brief tau = 2*Pi for full circle
 */
const float tau = 6.2831853071f;

/**
 * \brief Span of noise values . From ~ -0.7 to 0.7
 */
const float n_span = sqrt(2.0f);

/**
 * \brief Random engine of the standard library.
 */
default_random_engine eng;

/**
 * \brief Distributon for the random engine with span from -1.0 to 1.0
 */
uniform_real_distribution<float> dist1(-1.0, 1.0);

/**
* Distributon for the random engine with span from 0 to tau. Currently not in use.
*/
uniform_real_distribution<float> dist2(0.0, tau);

Noise::Noise(int seed, int lC, float fS, float fF, float wD, float am)
	: m_seed(seed), m_layerCount(lC), m_startFrequency(fS), m_frequencyFactor(fF),
      m_weightDivisor(wD), m_amplitude(am), m_isSeamless(false), m_type(NoiseType::PerlinNoise)
{
	if (lC < 0)
		printCriticalError("Noise(..) 1", "Parameter layercount is less then 0");
	else if(fS < 0.0f)
		printCriticalError("Noise(..) 1", "Parameter frequencyStart is less then 0.0");
	else if(fF < 0.0f)
		printCriticalError("Noise(..) 1", "Parameter frequencyFactor is less then 0.0");
	else if(wD < 0.0f)
		printCriticalError("Noise(..) 1", "Parameter weightDivisor is less then 0.0");
	else if(am < 0.0f)
		printCriticalError("Noise(..) 1", "Parameter amplitude is less then 0.0");
	else{
		// Allocate memory
		m_gradients1D = new float[permSize];
		m_gradients2D = new vec2[g2Size];
		m_perm = new int[permSize];

		// Initialize permutation table and gradients
		eng.seed(m_seed);
		init();

		// Calculate weightStart
		float a = 1.0;
		float b = 0.0;
		for (int i = 1; i < lC; i++) {
			b += a;
			a /= wD;
		}
		b += a;
		m_startWeight = 1.0f / b;
	}
}

Noise::Noise(int seed, int lC, int lS, int lE, int tR, float wD, float am)
	: m_seed(seed), m_layerCount(lC), m_frequencyFactor(2.0f), m_weightDivisor(wD), m_amplitude(am), 
	  m_isSeamless(true), m_type(NoiseType::PerlinNoise), m_startLayer(lS), m_endLayer(lE)
{
	if (lC < 0)
		printCriticalError("Noise(..) 2", "Parameter layercount is less then 0");
	else if (lS < 0)
		printCriticalError("Noise(..) 2", "Parameter startLayer is less then 0");
	else if (lE < 0)
		printCriticalError("Noise(..) 2", "Parameter endLayer is less then 0");
	else if (tR < 0)
		printCriticalError("Noise(..) 2", "Parameter texResolution is less then 0");
	else if (wD < 0.0f)
		printCriticalError("Noise(..) 2", "Parameter weightDivisor is less then 0.0");
	else if (am < 0.0f)
		printCriticalError("Noise(..) 2", "Parameter amplitude is less then 0.0");
	else {
		// 1D gradients and the normal permutation table are not used so they are set to null.
		m_gradients1D = nullptr;
		m_perm = nullptr;

		// Allocate memory
		m_gradients2D = new vec2[g2Size];

		// Calculate frequency start
		m_startFrequency = tR / float((1 << m_startLayer));

		// Initialize permutation table and gradients
		eng.seed(seed);
		init_seamless();

		// Calculate weightStart
		float a = 1.0;
		float b = 0.0;
		for (int i = m_startLayer; i < lC; i++) {
			b += a;
			a /= wD;
		}
		b += a;
		m_startWeight = 1.0f / b;
	}
}

Noise::~Noise() {
	if(m_gradients1D != nullptr)
		delete [] m_gradients1D;
	if(m_perm != nullptr)
		delete [] m_perm;

	delete [] m_gradients2D;
}

void Noise::init(){
	int i;

	// Permutation table
	for (i = 0; i < permSize; i++)
		m_perm[i] = i;
	for (i = 0; i < permSize; i++)
		swap(m_perm[i], m_perm[eng() & maxValue]);

	// Gradients for 1D noise
	for (i = 0; i < permSize; i++)
		m_gradients1D[i] = dist1(eng);

	// Gradients for 2D noise
	float angle = 0;
	for (i = 0; i < g2Size; i++){
		m_gradients2D[i].x = cos(angle);
		m_gradients2D[i].y = sin(angle);
		angle += tau / float(g2Size);
	}
}

void Noise::init_seamless(){
	// Resize the permutation tables to the number of total layers.
	m_perms.resize(m_layerCount);

	// Inttialize the permutation tables
	for (int i = 0; i < m_layerCount; i++){

		// Size of the current permutation table is 2^(first layer + starting layer)
		int size = (1 << (i + m_startLayer));
		m_perms[i].resize(size);
		for (int j = 0; j < size; j++)
			m_perms[i][j] = j;
		for (int j = 0; j < size; j++)
			swap(m_perms[i][j], m_perms[i][eng() & (size - 1)]);
	}

	// Gradients for 2D noise
	float angle = 0;
	for (int i = 0; i < g2Size; i++){
		m_gradients2D[i].x = cos(angle);
		m_gradients2D[i].y = sin(angle);
		angle += tau / float(g2Size);
	}
}

float Noise::n1(float x)const{
	// Error checking
	if (m_isSeamless)
		printCriticalError("Noise::n1(..)", "Normal noise function on seamless noise object called.");
	else if (x < 0.0f) {
		x = 0.0f;
		printError("Noise::n1(..)", "For this Perlin Noise implementation there are no negative x values allowed. 'x' set to 0.0");
	}

	// X-Value rounded downwards. Used to determine the surrounding gradients
	int intX = int(x);

	// 'Normalize' noise value so that we only deal within a range of 0.0 and 1.0
	float nx = x - intX;

	// Calculate "dot-products" of the normalized position and the gradients.
	float dp0 = nx * m_gradients1D[m_perm[intX & maxValue] & maxValue];
	float dp1 = (nx - 1.0f) * m_gradients1D[m_perm[(intX + 1) & maxValue] & maxValue];

	// Calculate the smoothed x value
	float sx = S0(nx);

	// Return the weighted average
	return (dp0 * (1.0f - sx) + dp1 * sx);
}

float Noise::n2(float x, float y)const{
	// Error checking
	if (m_isSeamless)
		printCriticalError("Noise::n2(..)", "Normal noise function on seamless noise object called.");
	else if (x < 0.0f || y < 0.0f) {
		x = 0.0f; y = 0.0f;
		printError("Noise::n2(..)", "For this Perlin Noise implementation there are no negative x or y values allowed.\n 'x' and 'y' set to 0.0");
	}

	// TODO : CHECK ONLY POSITIVE FLOAT VALUES ALLOWED
	// Get position values rounded downwards for gradient determination
	int intX = int(x);
	int intY = int(y);

	// 'Normalize' position so that we only deal in a range of (0.0, 0.0) to (1.0, 1.0)
	float nx = x - intX;
	float ny = y - intY;

	// Calculate the vectors that point to given position (x, y)
	vec2 p0 = { nx, ny };
	vec2 p1 = { nx - 1.0f, ny };
	vec2 p2 = { nx, ny - 1.0f };
	vec2 p3 = { nx - 1.0f, ny - 1.0f };

	// Calculate the for surrounding gradients
	vec2 gradientXY = m_gradients2D[m_perm[(intX + (m_perm[intY & maxValue])) & maxValue] & g2MaxValue];
	vec2 gradientX1Y = m_gradients2D[m_perm[((intX + 1) + (m_perm[intY & maxValue])) & maxValue] & g2MaxValue];
	vec2 gradientXY1 = m_gradients2D[m_perm[(intX + (m_perm[(intY + 1) & maxValue])) & maxValue] & g2MaxValue];
	vec2 gradientX1Y1 = m_gradients2D[m_perm[((intX + 1) + (m_perm[(intY + 1) & maxValue])) & maxValue] & g2MaxValue];

	// Calculate the dot product between both
	float dp0 = p0.x * gradientXY.x + p0.y * gradientXY.y;
	float dp1 = p1.x * gradientX1Y.x + p1.y * gradientX1Y.y;
	float dp2 = p2.x * gradientXY1.x + p2.y * gradientXY1.y;
	float dp3 = p3.x * gradientX1Y1.x + p3.y * gradientX1Y1.y;

	// Smooth interpolation
	float sx = S1(nx);
	float sy = S1(ny);

	// Calculate weighted averages on x-axis
	float av1 = dp0 * (1.0f - sx) + dp1 * sx;
	float av2 = dp2 * (1.0f - sx) + dp3 * sx;

	// Calculate weighted average on y-axis
	return ((av1 * (1.0f - sy) + av2 * sy));
}

float Noise::noiseValue(float perlinValue)const {
	float result = 0.0;
	switch (m_type) {
		case NoiseType::PerlinNoise:
			result = perlinValue; break;
		case NoiseType::BillowyNoise:
			result = abs(perlinValue); break;
		case NoiseType::RidgidNoise:
			result = 1.0f - abs(perlinValue); break;
		case NoiseType::CosinusNoise:
			result = 1.0f - abs(cos(perlinValue)); break;
		default:
			result = perlinValue; break;
	}
	return result;
}
float Noise::n1_layered(float x)const{
	// Error checking
	if (m_isSeamless)
		printCriticalError("Noise::n1_layered(..)", "Normal noise function on seamless noise object called.");
	else if (x < 0.0f) {
		x = 0.0f;
		printError("Noise::n1_layered(..)", "For this Perlin Noise implementation there are no negative x values allowed. 'x' set to 0.0");
	}

	// Calculate noise value of the first layer
	float f = m_startFrequency;
	float w = m_startWeight;
	float n = noiseValue(n1(x / f)) * w;

	// Calculate noise values of all other layers and add them up considering their weight
	for (int i = 0; i < m_layerCount; i++) {
		f /= m_frequencyFactor;
		w /= m_weightDivisor;
		n += noiseValue(n1(x / f)) * w;
	}

	// Return the final noise value multiplied with the amplitude
	return n * m_amplitude;
}

float Noise::n2_layered(float x, float y)const{
	// Error checking
	if (m_isSeamless)
		printCriticalError("Noise::n2_layered(..)", "Normal noise function on seamless noise object called.");
	else if (x < 0.0f || y < 0.0f) {
		x = 0.0f; y = 0.0f;
		printError("Noise::n2_layered(..)", "For this Perlin Noise implementation there are no negative x or y values allowed.\n 'x' and 'y' set to 0.0");
	}

	// Calculate first noise value with offset to avoid directional artifacts.
	float offset = 7.19f;
	float f = m_startFrequency;
	float w = m_startWeight;
	float n = noiseValue((n2((x + offset) * (f / 1000.0f), (y + offset * 2) * (f / 1000.0f))))  * w;

	// Calculate all other noise values and add them up
	for (int i = 1; i < m_layerCount; i++) {
		f *= m_frequencyFactor;
		w /= m_weightDivisor;
		offset *= 1.73f;
		n += noiseValue((n2((x + offset) * (f / 1000.0f), (y + offset * 2) * (f / 1000.0f)))) * w;
	}

	// Return the final noise value multiplied with the amplitude
	return n * m_amplitude;
}

float Noise::n2_seamless(float x, float y, int layer, int limit)const{
	// Error checking
	if (!m_isSeamless)
		printCriticalError("Noise::n2_seamless(..)", "Seamless noise function on normal noise object called.");
	else if (x < 0.0f || y < 0.0f) {
		x = 0.0f; y = 0.0f;
		printError("Noise::n2_seamless(..)", "For this Perlin Noise implementation there are no negative x or y values allowed.\n 'x' and 'y' set to 0.0");
	}

	// See comments in function "n2" as reference
	int intX = int(x);
	int intY = int(y);

	float nx = x - intX;
	float ny = y - intY;

	vec2 p0 = { nx, ny };
	vec2 p1 = { nx - 1.0f, ny };
	vec2 p2 = { nx, ny - 1.0f };
	vec2 p3 = { nx - 1.0f, ny - 1.0f };

	// Calculate gradients based on the permutation table defined by the current layer number
	vec2 gradientXY = m_gradients2D[m_perms[layer][(intX + (m_perms[layer][intY & limit])) & limit] & g2MaxValue];
	vec2 gradientX1Y = m_gradients2D[m_perms[layer][((intX + 1) + (m_perms[layer][intY & limit])) & limit] & g2MaxValue];
	vec2 gradientXY1 = m_gradients2D[m_perms[layer][(intX + (m_perms[layer][(intY + 1) & limit])) & limit] & g2MaxValue];
	vec2 gradientX1Y1 = m_gradients2D[m_perms[layer][((intX + 1) + (m_perms[layer][(intY + 1) & limit])) & limit] & g2MaxValue];

	float dp0 = p0.x * gradientXY.x + p0.y * gradientXY.y;
	float dp1 = p1.x * gradientX1Y.x + p1.y * gradientX1Y.y;
	float dp2 = p2.x * gradientXY1.x + p2.y * gradientXY1.y;
	float dp3 = p3.x * gradientX1Y1.x + p3.y * gradientX1Y1.y;

	float sx = S1(nx);
	float sy = S1(ny);

	float av1 = dp0 * (1.0f - sx) + dp1 * sx;
	float av2 = dp2 * (1.0f - sx) + dp3 * sx;

	return (av1 * (1.0f - sy) + av2 * sy);
}

float Noise::n2_seamless_layered(float x, float y)const{
	// Error checking
	if (!m_isSeamless)
		printCriticalError("Noise::n2_seamless_layered(..)", "Seamless noise function on normal noise object called.");
	else if (x < 0.0f || y < 0.0f) {
		x = 0.0f; y = 0.0f;
		printError("Noise::n2_seamless_layered(..)", "For this Perlin Noise implementation there are no negative x or y values allowed.\n 'x' and 'y' set to 0.0");
	}

	// See comments in function "n2_layered" as reference
	float offset = 7.19f;
	float f = m_startFrequency;
	float w = m_startWeight;

	// Determine start layer
	int layer = m_startLayer - 1;

	// Determine limit for the start layer which is (2^(startlayer+1))-1
	int limit = (1 << (layer + 1)) - 1;

	float n = noiseValue(n2_seamless((x + offset) / f, (y + offset * 2) / f, layer, limit))  * w;

	for (int i = m_startLayer; i < m_endLayer; i++) {
		f /= m_frequencyFactor;
		w /= m_weightDivisor;

		// Determine current layer
		layer = i;

		// Determine current limit for the permutation table
		limit = (1 << (layer + 1)) - 1;

		offset *= 1.73f;

		n += noiseValue(n2_seamless((x + offset) / f, (y + offset * 2) / f, layer, limit))  * w;
	}

	return n * m_amplitude;
}

void Noise::setNewSeed(int seed){
	// Set new seed
    m_seed = seed;

	// Re initialize random engine
    eng.seed(m_seed);

	// Re initialize of gradient and permutation tables
    if(m_isSeamless)
        init_seamless();
    else
        init();
}
