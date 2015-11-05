#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    return vec3f(1,1,1);
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double cofficient = m_nConstantAttenuationCoefficient + m_nLinearAttenuationCoefficient * sqrt((P - position).length_squared()) + m_nQuadraticAttenuationCoefficient * (P - position).length_squared();
	return 1.0 / max<double>(cofficient, 1.0);
	
	//return 1.0;
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
    return vec3f(1,1,1);
}

void PointLight::setAttenuationCoefficients(const double m_nConstantAttenuationCoeff,
	const double m_nLinearAttenuationCoeff,
	const double m_nQuadraticAttenuationCoeff)
{
	m_nConstantAttenuationCoefficient = m_nConstantAttenuationCoeff;
	m_nLinearAttenuationCoefficient = m_nLinearAttenuationCoeff;
	m_nQuadraticAttenuationCoefficient = m_nQuadraticAttenuationCoeff;
}

double AmbientLight::distanceAttenuation(const vec3f& P) const
{
	return 1.0;
}

vec3f AmbientLight::shadowAttenuation(const vec3f& P) const
{
	return vec3f(1, 1, 1);
}

vec3f AmbientLight::getColor(const vec3f& P) const
{
	return color;
}

vec3f AmbientLight::getDirection(const vec3f& P) const
{
	return vec3f(1,1,1);
}