#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth, double prev_index );


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );

	bool sceneLoaded();
	void setAmbientLightRed(double d);
	void setAmbientLightGreen(double d);
	void setAmbientLightBlue(double d);

	void setDepth(int i);
	void setAntialiasing(int i);
	void			setAdaptiveThreshold(double d);
	void		setJitter(int i);
	void			setConstantAttenuationCoefficient(double d);
	void			setLinearAttenuationCoefficient(double d);
	void			setQuadraticAttenuationCoefficient(double d);

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;

	int m_nDepth;
	int m_nAntialiasing;
	int         m_nJitter;
	double      m_nAdaptiveThreshold;
	double      m_nConstantAttenuationCoefficient;
	double      m_nLinearAttenuationCoefficient;
	double      m_nQuadraticAttenuationCoefficient;

	bool m_bSceneLoaded;
};

#endif // __RAYTRACER_H__
