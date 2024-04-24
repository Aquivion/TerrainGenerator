#pragma once

#include "glm.h"
#include "error.h"
#include <random>
#include <functional>
#include <iostream>

/**
* \brief Enumeration class for the different noise types.
*/
enum class NoiseType { PerlinNoise, BillowyNoise, RidgidNoise, CosinusNoise };

/**
* \brief The Noise class, which implements the "Perlin Noise" algorithm (not the original one).
*
* A Noise object is used to get noise values that are generated based on the parameter given to the
* constructor. There are two ways implemented to generate noise values. The first one is based on
* the standard "Perlin Noise" algorithm. The second one uses a self created algorithm with which
* someone can get "seamless" noise values, which means a texture with that noise values would be
* tileable. 
*
* Furthermore there are 4 different ways to modify the "Perlin Noise" values to get other types
* of terrains or textures. Perlin Noise returns unmodified Perlin Noise values. Billowy Noise
* returns the absolute value of Perlin Noise and Ridgid Noise the (1.0 - absolute) value.
* Cosinus Noise just calculates the cosinus of Perlin Noise.
*/
class Noise
{
public:
	/**
	* \brief Allocates memory for the tables and initializes the noise parameter. Used for normal
	* "Perlin Noise" values.
	*
	* \param[in] seed This value initializes the random engine with which the random values are being generated.
	*
	* \param[in] layerCount Number of layers that are added together.
	*
	* \param[in] startFrequency Frequency of the first layer.
	*
	* \param[in] frequencyFactor Factor with which the frequency will be multiplied after each added layer.
	*
	* \param[in] weightDivisor Divisor with which the weight of each layer will be decreased.
	*
	* \param[in] amplitude Determines the overall 'height' of the noise values.
	*/
	Noise(int seed, int layerCount, float startFrequency, float frequencyFactor, float weightDivisor, float amplitude);

	/**
	* \brief Allocates memory for the tables and initializes the noise parameter. 
	*
	* Used for "Seamless Perlin Noise". This method workd with different sized permutation tables. It's base idea
	* is, that with each added layer the noise value will be generated with another "greater" permutation table.
	* This works because the size of the permutation table determines when the noise values will repeat.
	*
	* \param[in] seed This value initializes the random engine with which the random values are being generated.
	*
	* \param[in] layerCount Number of layers that are added together. Determines the start Frequency in this method.
	*
	* \param[in] startLayer First layer to start with. This is used because the really first layer has pretty
	* bad noise values because of the low size of the permutation table.
	*
	* \param[in] endLayer Last layer that will be added. 
	*
	* \param[in] texResolution Resolution of the texture that will be generated with this noise object. Used to
	* calculate the frequency for the first layer.
	*
	* \param[in] weightDivisor Divisor with which the weight of each layer will be decreased.
	*
	* \param[in] amplitude Determines the overall 'height' of the noise values.
	*/
	Noise(int seed, int layerCount, int startLayer, int endLayer, int texResolution, float weightDivisor, float amplitude);

	/**
	* \brief Destructor. Deletes gradient and permutation tables
	*/
	~Noise();

	/**
	* \brief Function for 1D-Perlin Noise.
	*
	* \param[in] x Position with which the noise value shall be generated.
	* 
	* \return Noise value at position x.
	*/
	float n1(float x)const;

	/**
	* \brief Function for 2D-Perlin Noise.
	*
	* \param[in] x X-Position with which the noise value shall be generated.
	*
	* \param[in] x Y-Position with which the noise value shall be generated.
	*
	* \return Noise value at position (x, y).
	*/
	float n2(float x, float y)const;

	/**
	* \brief Adds up all 1D-Noise values of the different layers, considerung their
	* frequency and weight.
	*
	* \param[in] x Position with which the noise value shall be generated.
	*
	* \return Noise value at position x after all layers were added up.
	*/
	float n1_layered(float x)const;

	/**
	* \brief Adds up all 2D-Noise values of the different layers, considerung their
	* frequency and weight.
	*
	* \param[in] x Position with which the noise value shall be generated.
	*
	* \return Noise value at position (x, y) after all layers were added up.
	*/
	float n2_layered(float x, float y)const;

	/**
	* \brief Function for 2D-Seamless Perlin Noise.
	*
	* \param[in] x X-Position with which the noise value shall be generated.
	*
	* \param[in] x Y-Position with which the noise value shall be generated.
	*
	* \param[in] layer Layer for which the noise value is generated. Determines
	* which permutation table is used.
	*
	* \param[in] limit Value to limit the values between 0 and the size of the
	* used permutation table.
	*
	* \return Noise value at position (x, y).
	*/
	float n2_seamless(float x, float y, int layer, int limit)const;

	/**
	* \brief Adds up all 2D-Noise values of the different layers, considerung their
	* frequency and weight.
	*
	* \param[in] x Position with which the noise value shall be generated.
	*
	* \return Noise value at position (x, y) after all layers were added up.
	*/
	float n2_seamless_layered(float x, float y)const;

	/**
	* \brief Getter for the seed.
	*/
	int getSeed()const { return m_seed; }

	/**
	* \brief Getter for the start layer for seamles perlin noise.
	*/
	int getStartLayer()const { return m_startLayer; }

	/**
	* \brief Getter for the end layer for seamles perlin noise.
	*/
	int getEndLayer()const { return m_endLayer; }

	/**
	* \brief Getter for the number of total layers.
	*/
	int getLayerCount()const { return m_layerCount; }

	/**
	* \brief Getter for the frequency of the first layer.
	*/
	float getStartFrequency()const { return m_startFrequency; }

	/**
	* \brief Getter for the frequency Factor.
	*/
	float getFrequencyFactor()const { return m_frequencyFactor; }

	/**
	* \brief Getter for the weight divisor.
	*/
	float getWeightDivisor()const { return m_weightDivisor; }

	/**
	* \brief Getter for the amplitude
	*/
	float getAmplitude()const { return m_amplitude; }

	/**
	* \brief Sets the start layer for seamless perlin noise.
	*
	* \param[in] new start layer.
	*/
	void setStartLayer(int startLayer) { m_startLayer = startLayer; }

	/**
	* \brief Sets the end layer for seamless perlin noise.
	*
	* \param[in] new end layer.
	*/
	void setEndLayer(int endLayer) { m_endLayer = endLayer; }

	/**
	* \brief Sets the number of layers.
	*
	* \param[in] new number of layers.
	*/
	void setLayerCount(int layerCount) { m_layerCount = layerCount; }

	/**
	* \brief Sets the frequency of the first layer.
	*
	* \param[in] new frequency of the first layer.
	*/
	void setStartFrequency(float startFrequency) { m_startFrequency = startFrequency; }

	/**
	* \brief Sets the frequency factor.
	*
	* \param[in] new frequency factor.
	*/
	void setFrequencyFactor(float frequencyFactor) { m_frequencyFactor = frequencyFactor; }

	/**
	* \brief Sets the weight divisor.
	*
	* \param[in] new weight divisor.
	*/
	void setWeightDivisor(float weigtDivisor) { m_weightDivisor = weigtDivisor; }

	/**
	* \brief Sets the amplitude.
	*
	* \param[in] new amplitude.
	*/
	void setAmplitude(float amplitude) { m_amplitude = amplitude; }

	/**
	* \brief Sets a new seed and re-initializes the random engine.
	*
	* \param[in] new seed.
	*/
	void setNewSeed(int seed);

	/**
	* \brief Sets another noise type.
	*
	* \param[in] new noise type.
	*/
	void setNoiseType(NoiseType type) { m_type = type; }

private:
	/**
	* \brief Initialize gradients and permutation-table.
	*/
	void init();

	/**
	* \brief Noise calculation for different noise types.
	*
	* \param[in] perlinValue Value that was returned by the pure perlin noise algorithm.
	*/
	float noiseValue(float perlinValue)const;

	/**
	* \brief Initialize gradients and permutation-tables for seamless noise.
	*/
	void init_seamless();


	/**
	* \brief Gradients for 1D-Perlin Noise.
	*/
	float* m_gradients1D;

	/**
	* \brief Gradients for 2D-Perlin Noise.
	*/
	vec2* m_gradients2D;

	/**
	* \brief Permutation table for normal Perlin Noise.
	*/
	int* m_perm;

	/**
	* \brief Permutation tables for seamless Perlin Noise.
	*/
	vector<vector<unsigned>> m_perms;

	/**
	* \brief Seed that determines the random values.
	*/
	int m_seed;

	/**
	* \brief Number of total layers.
	*/
	int m_layerCount;

	/**
	* \brief Frequency of the first layer
	*/
	float m_startFrequency;

	/**
	* \brief Factor with which the frequency for each added layer is mulitplied.
	*/
	float m_frequencyFactor;

	/**
	* \brief Weight of the first layer.
	*/
	float m_startWeight;

	/**
	* \brief Divisor with which the weight for a layer is decreased after each added layer
	*/
	float m_weightDivisor;

	/**
	* \brief Amplitude which determines the 'height' of the noise values.
	*/
	float m_amplitude;

	/**
	* \brief Boolean which tells whether the object is a seamless noise object or a normal one.
	*/
	bool m_isSeamless;

	/**
	* \brief Type of the noise.
	*/
	NoiseType m_type;

	/**
	* \brief Start layer for seamless Perlin Noise
	*/
	int m_startLayer;

	/**
	* \brief End layer for seamless Perlin Noise
	*/
	int m_endLayer;
};

