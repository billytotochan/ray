// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), m_nDepth, 1.0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth, double prev_index)
{
	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
		vec3f incidentColor = m.shade(scene, r, i);
		if (depth <= 0) {
			return incidentColor;
		}
		
		vec3f incidentDirection = r.getDirection().normalize();
		
		if (!m.kr.iszero()) {
			vec3f reflectedPosition = r.at(i.t) + RAY_EPSILON * i.N.normalize();
			vec3f reflectedDirection = (incidentDirection + 2 * (-incidentDirection.dot(i.N.normalize()) * i.N.normalize())).normalize();
			ray reflectednRay(reflectedPosition, reflectedDirection);
			vec3f reflectedColor = traceRay(scene, reflectednRay, thresh, depth - 1, m.index);
			incidentColor += prod(m.kr, reflectedColor);
		}

		if (!m.kt.iszero()) {
			double n_i = (m.index == prev_index ? m.index : 1.0);
			double n_t = (m.index == prev_index ? 1.0 : m.index);
			double n_r = n_i / n_t;
			double c = -i.N.dot(incidentDirection) / (incidentDirection.length() * i.N.length());
			vec3f refractedPosition = r.at(i.t) - RAY_EPSILON * i.N.normalize();

			if (1 - pow(n_r, 2) * (1 - pow(c, 2)) > RAY_EPSILON) {
				vec3f refractedDirection = n_r * incidentDirection + (n_r * c - sqrt(1 - pow(n_r, 2) * (1 - pow(c, 2)))) * i.N;
				ray refractedRay(refractedPosition, refractedDirection);
				vec3f refractedColor = traceRay(scene, refractedRay, thresh, depth - 1, m.index);
				incidentColor += prod(m.kt, refractedColor);
			}
		}
		

		return incidentColor.clamp();
		
		//return m.shade(scene, r, i);
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}

void RayTracer::setDepth(int i)
{
	m_nDepth = i;
}

void RayTracer::setAntialiasing(int i)
{
	m_nAntialiasing = i;
}

void RayTracer::setAmbientLightRed(double d)
{
	this->scene->setAmbientLightRed(d);
}
void RayTracer::setAmbientLightGreen(double d)
{
	this->scene->setAmbientLightGreen(d);
}
void RayTracer::setAmbientLightBlue(double d)
{
	this->scene->setAmbientLightBlue(d);
}
void RayTracer::setJitter(int i)
{
	m_nJitter = i;
}
void RayTracer::setAdaptiveThreshold(double d)
{
	m_nAdaptiveThreshold = d;
}
void RayTracer::setConstantAttenuationCoefficient(double d)
{
	m_nConstantAttenuationCoefficient = d;
}
void RayTracer::setLinearAttenuationCoefficient(double d)
{
	m_nLinearAttenuationCoefficient = d;
}
void RayTracer::setQuadraticAttenuationCoefficient(double d)
{
	m_nQuadraticAttenuationCoefficient = d;
}