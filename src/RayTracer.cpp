// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include <math.h>
#include <stdlib.h> 
#include <time.h> 

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
	int iteration[7] = { 0, 0, 0, 0, 0, 0, 0 };

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);
	double width = 1.0 / double(buffer_width);
	double height = 1.0 / double(buffer_height);

	col = (m_nSuperSampling ? 
		superTrace(x, y, width, height, m_nSuperSampling, iteration) :
		trace(scene, x, y) //simpleTrace(x,y,width,height)
	);

	//col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}

vec3f RayTracer::superTrace(double x, double y, double width, double height, int depth, int *iteration)
{
	if (depth>0) {
		iteration[depth] = 1;
		vec3f cornerA, cornerB, cornerC, cornerD;
		vec3f average;
		int flag = 1;
		cornerA = trace(scene, x - width / 2.0, y - height / 2.0);
		cornerB = trace(scene, x - width / 2.0, y + height / 2.0);
		cornerC = trace(scene, x + width / 2.0, y - height / 2.0);
		cornerD = trace(scene, x + width / 2.0, y + height / 2.0);
		average = (cornerA + cornerB + cornerC + cornerD) / 4;

		double dis[12] = {
			abs(average[0] - cornerA[0]), abs(average[1] - cornerA[1]), abs(average[2] - cornerA[2]),
			abs(average[0] - cornerB[0]), abs(average[1] - cornerB[1]), abs(average[2] - cornerB[2]),
			abs(average[0] - cornerC[0]), abs(average[1] - cornerC[1]), abs(average[2] - cornerC[2]),
			abs(average[0] - cornerD[0]), abs(average[0] - cornerD[1]), abs(average[0] - cornerD[2])
		};

		for (int i = 0; i<12; i++)
		{
			if (dis[i]>0.005) {
				flag = 0;
				break;
			}
		}
		if (flag == 1) {
			return average;
		}
		else {
			cornerA = superTrace(x - width / 4.0, y - height / 4.0, width / 2.0, height / 2.0, depth - 1, iteration);
			cornerB = superTrace(x - width / 4.0, y + height / 4.0, width / 2.0, height / 2.0, depth - 1, iteration);
			cornerC = superTrace(x + width / 4.0, y - height / 4.0, width / 2.0, height / 2.0, depth - 1, iteration);
			cornerD = superTrace(x + width / 4.0, y + height / 4.0, width / 2.0, height / 2.0, depth - 1, iteration);
			return ((cornerA + cornerB + cornerC + cornerD) / 4);
		}
	}
	else {
		return trace(scene, x, y);
	}
}

vec3f RayTracer::simpleTrace(double x, double y, double width, double height)
{
	vec3f col;
	vec3f a[25];
	double m[50];
	srand((unsigned)time(NULL)); 
	if(m_nAntialiasing==1)
	{
		if(m_nJitter==0)
		{
			col = trace( scene, x, y );
		}
		else
		{			
			m[0] = 2.0*(((double)((rand() + (int)(x*1000))%3))/3.0-0.5);
			m[1] = 2.0*(((double)((rand() + (int)(y*1000))%3))/3.0-0.5);
			col = trace(scene, x + m[0]*width/2.0, y+m[1]*height/2.0);
		}
	}
	else if(m_nAntialiasing==2)
	{
		if(m_nJitter==0)
		{
			a[0] = trace(scene,x-width/2.0,y-height/2.0);
			a[1] = trace(scene,x-width/2.0,y+height/2.0);
			a[2] = trace(scene,x+width/2.0,y-height/2.0);
			a[3] = trace(scene,x+width/2.0,y+height/2.0);
			col = (a[0]+a[1]+a[2]+a[3])/4;
		}
		else
		{
			for(int i=0;i<8;i++)
			{
				m[i] = 2.0*(((double)((rand() + i + (int)(y*1000))%5))/5.0-0.5);
			}
			a[0] = trace(scene,x-width/2.0+m[0]*width/2.0,y-height/2.0+m[4]*height/2.0);
			a[1] = trace(scene,x-width/2.0+m[1]*width/2.0,y+height/2.0+m[5]*height/2.0);
			a[2] = trace(scene,x+width/2.0+m[2]*width/2.0,y-height/2.0+m[6]*height/2.0);
			a[3] = trace(scene,x+width/2.0+m[3]*width/2.0,y+height/2.0+m[7]*height/2.0);
			col = (a[0]+a[1]+a[2]+a[3])/4;
		}
	}
	else if(m_nAntialiasing==3)
	{
		if(m_nJitter==0)
		{
			a[0] = trace(scene,x-width/2.0,y-height/2.0);
			a[1] = trace(scene,x-width/2.0,y);
			a[2] = trace(scene,x-width/2.0,y+height/2.0);
			a[3] = trace(scene,x,y-height/2.0);
			a[4] = trace(scene,x,y);
			a[5] = trace(scene,x,y+height/2.0);
			a[6] = trace(scene,x+width/2.0,y-height/2.0);
			a[7] = trace(scene,x+width/2.0,y);
			a[8] = trace(scene,x+width/2.0,y+height/2.0);
			col[0] = col[1] = col[2] = 0;
			for(int i=0;i<9;i++)
				col+=a[i];
			col/=9;
		}
		else
		{
			for(int i=0;i<18;i++)
			{
				m[i] = 2.0*(((double)((rand() + i + (int)(y*1000))%5))/5.0-0.5);
			}
			a[0] = trace(scene,x-width/2.0+m[0]*width/4.0,y-height/2.0+m[9]*height/4.0);
			a[1] = trace(scene,x-width/2.0+m[1]*width/4.0,y+m[10]*height/4.0);
			a[2] = trace(scene,x-width/2.0+m[2]*width/4.0,y+height/2.0+m[11]*height/4.0);
			a[3] = trace(scene,x+m[3]*width/4.0,y-height/2.0+m[12]*height/4.0);
			a[4] = trace(scene,x+m[4]*width/4.0,y+m[13]*height/4.0);
			a[5] = trace(scene,x+m[5]*width/4.0,y+height/2.0+m[14]*height/4.0);
			a[6] = trace(scene,x+width/2.0+m[6]*width/4.0,y-height/2.0+m[15]*height/4.0);
			a[7] = trace(scene,x+width/2.0+m[7]*width/4.0,y+m[16]*height/4.0);
			a[8] = trace(scene, x + width / 2.0 + m[8] * width / 4.0, y + height / 2.0 + m[17] * height / 4.0);
			col[0] = col[1] = col[2] = 0;
			for(int i=0;i<9;i++)
				col+=a[i];
			col/=9;
		}
	}
	else if(m_nAntialiasing==4)
	{
		if(m_nJitter==0)
		{
			a[0]  = trace(scene,x-width/2.0,y-height/2.0);
			a[1]  = trace(scene,x-width/2.0,y-height/6.0);
			a[2]  = trace(scene,x-width/2.0,y+height/6.0);
			a[3]  = trace(scene,x-width/2.0,y+height/2.0);
			a[4]  = trace(scene,x-width/6.0,y-height/2.0);
			a[5]  = trace(scene,x-width/6.0,y-height/6.0);
			a[6]  = trace(scene,x-width/6.0,y+height/6.0);
			a[7]  = trace(scene,x-width/6.0,y+height/2.0);
			a[8]  = trace(scene,x+width/6.0,y-height/2.0);
			a[9]  = trace(scene,x+width/6.0,y-height/6.0);
			a[10] = trace(scene,x+width/6.0,y+height/6.0);
			a[11] = trace(scene,x+width/6.0,y+height/2.0);
			a[12] = trace(scene,x+width/2.0,y-height/2.0);
			a[13] = trace(scene,x+width/2.0,y-height/6.0);
			a[14] = trace(scene,x+width/2.0,y+height/6.0);
			a[15] = trace(scene, x + width / 2.0, y + height / 2.0);
			col[0] = col[1] = col[2] = 0;
			for(int i=0;i<16;i++)
				col+=a[i];
			col/=16;
		}
		else
		{
			for(int i=0;i<32;i++)
			{
				m[i] = 2.0*(((double)((rand() + i + (int)(y*1000))%5))/5.0-0.5);
			}
			a[0]  = trace(scene,x-width/2.0+m[0]*width/6.0,y-height/2.0+m[16]*height/6.0);
			a[1]  = trace(scene,x-width/2.0+m[1]*width/6.0,y-height/6.0+m[17]*width/6.0);
			a[2]  = trace(scene,x-width/2.0+m[2]*width/6.0,y+height/6.0+m[18]*width/6.0);
			a[3]  = trace(scene,x-width/2.0+m[3]*width/6.0,y+height/2.0+m[19]*width/6.0);
			a[4]  = trace(scene,x-width/6.0+m[4]*width/6.0,y-height/2.0+m[20]*width/6.0);
			a[5]  = trace(scene,x-width/6.0+m[5]*width/6.0,y-height/6.0+m[21]*width/6.0);
			a[6]  = trace(scene,x-width/6.0+m[6]*width/6.0,y+height/6.0+m[22]*width/6.0);
			a[7]  = trace(scene,x-width/6.0+m[7]*width/6.0,y+height/2.0+m[23]*width/6.0);
			a[8]  = trace(scene,x+width/6.0+m[8]*width/6.0,y-height/2.0+m[24]*width/6.0);
			a[9]  = trace(scene,x+width/6.0+m[9]*width/6.0,y-height/6.0+m[25]*width/6.0);
			a[10] = trace(scene,x+width/6.0+m[10]*width/6.0,y+height/6.0+m[26]*width/6.0);
			a[11] = trace(scene,x+width/6.0+m[11]*width/6.0,y+height/2.0+m[27]*width/6.0);
			a[12] = trace(scene,x+width/2.0+m[12]*width/6.0,y-height/2.0+m[28]*width/6.0);
			a[13] = trace(scene,x+width/2.0+m[13]*width/6.0,y-height/6.0+m[29]*width/6.0);
			a[14] = trace(scene,x+width/2.0+m[14]*width/6.0,y+height/6.0+m[30]*width/6.0);
			a[15] = trace(scene, x + width / 2.0 + m[15] * width / 6.0, y + height / 2.0 + m[31] * width / 6.0);
			col[0] = col[1] = col[2] = 0;
			for(int i=0;i<16;i++)
				col+=a[i];
			col/=16;
		}
	}
	else 
	{
		if(m_nJitter==0)
		{
			a[0]  = trace(scene,x-width/2.0,y-height/2.0);
			a[1]  = trace(scene,x-width/2.0,y-height/4.0);
			a[2]  = trace(scene,x-width/2.0,y);
			a[3]  = trace(scene,x-width/2.0,y+height/4.0);
			a[4]  = trace(scene,x-width/2.0,y+height/2.0);

			a[5]  = trace(scene,x-width/4.0,y-height/2.0);
			a[6]  = trace(scene,x-width/4.0,y-height/4.0);
			a[7]  = trace(scene,x-width/4.0,y);
			a[8]  = trace(scene,x-width/4.0,y+height/4.0);
			a[9]  = trace(scene,x-width/4.0,y+height/2.0);

			a[10] = trace(scene,x,y-height/2.0);
			a[11] = trace(scene,x,y-height/4.0);
			a[12] = trace(scene,x,y);
			a[13] = trace(scene,x,y+height/4.0);
			a[14] = trace(scene,x,y+height/2.0);

			a[15] = trace(scene,x+width/4.0,y-height/2.0);
			a[16] = trace(scene,x+width/4.0,y-height/4.0);
			a[17] = trace(scene,x+width/4.0,y);
			a[18] = trace(scene,x+width/4.0,y+height/4.0);
			a[19] = trace(scene,x+width/4.0,y+height/2.0);

			a[20] = trace(scene,x+width/2.0,y-height/2.0);
			a[21] = trace(scene,x+width/2.0,y-height/4.0);
			a[22] = trace(scene,x+width/2.0,y);
			a[23] = trace(scene,x+width/2.0,y+height/4.0);
			a[24] = trace(scene,x+width/2.0,y+height/2.0);

			col[0] = col[1] = col[2] = 0;
			for(int i=0;i<25;i++)
				col+=a[i];
			col/=25;
		}
		else
		{
			for(int i=0;i<50;i++)
			{
				m[i] = 2.0*(((double)((rand() + i + (int)(y*1000))%5))/5.0-0.5);
			}
			a[0]  = trace(scene,x-width/2.0+m[0]*width/8.0,y-height/2.0+m[25]*height/8.0);
			a[1]  = trace(scene,x-width/2.0+m[1]*width/8.0,y-height/4.0+m[26]*height/8.0);
			a[2]  = trace(scene,x-width/2.0+m[2]*width/8.0,y+m[27]*height/8.0);
			a[3]  = trace(scene,x-width/2.0+m[3]*width/8.0,y+height/4.0+m[28]*height/8.0);
			a[4]  = trace(scene,x-width/2.0+m[4]*width/8.0,y+height/2.0+m[29]*height/8.0);

			a[5]  = trace(scene,x-width/4.0+m[5]*width/8.0,y-height/2.0+m[30]*height/8.0);
			a[6]  = trace(scene,x-width/4.0+m[6]*width/8.0,y-height/4.0+m[31]*height/8.0);
			a[7]  = trace(scene,x-width/4.0+m[7]*width/8.0,y+m[32]*height/8.0);
			a[8]  = trace(scene,x-width/4.0+m[8]*width/8.0,y+height/4.0+m[33]*height/8.0);
			a[9]  = trace(scene,x-width/4.0+m[9]*width/8.0,y+height/2.0+m[34]*height/8.0);

			a[10] = trace(scene,x+m[10]*width/8.0,y-height/2.0+m[35]*height/8.0);
			a[11] = trace(scene,x+m[11]*width/8.0,y-height/4.0+m[36]*height/8.0);
			a[12] = trace(scene,x+m[12]*width/8.0,y+m[37]*height/8.0);
			a[13] = trace(scene,x+m[13]*width/8.0,y+height/4.0+m[38]*height/8.0);
			a[14] = trace(scene,x+m[14]*width/8.0,y+height/2.0+m[39]*height/8.0);

			a[15] = trace(scene,x+width/4.0+m[15]*width/8.0,y-height/2.0+m[40]*height/8.0);
			a[16] = trace(scene,x+width/4.0+m[16]*width/8.0,y-height/4.0+m[41]*height/8.0);
			a[17] = trace(scene,x+width/4.0+m[17]*width/8.0,y+m[42]*height/8.0);
			a[18] = trace(scene,x+width/4.0+m[18]*width/8.0,y+height/4.0+m[43]*height/8.0);
			a[19] = trace(scene,x+width/4.0+m[19]*width/8.0,y+height/2.0+m[44]*height/8.0);

			a[20] = trace(scene,x+width/2.0+m[20]*width/8.0,y-height/2.0+m[45]*height/8.0);
			a[21] = trace(scene,x+width/2.0+m[21]*width/8.0,y-height/4.0+m[46]*height/8.0);
			a[22] = trace(scene,x+width/2.0+m[22]*width/8.0,y+m[47]*height/8.0);
			a[23] = trace(scene,x+width/2.0+m[23]*width/8.0,y+height/4.0+m[48]*height/8.0);
			a[24] = trace(scene,x+width/2.0+m[24]*width/8.0,y+height/2.0+m[49]*height/8.0);

			col[0] = col[1] = col[2] = 0;
			for(int i=0;i<25;i++)
				col+=a[i];
			col/=25;
		}
	}
	return col;
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
	this->scene->setConstantAttenuationCoefficient(d);
	this->scene->setCustomDistanceAttenuation(true);
}
void RayTracer::setLinearAttenuationCoefficient(double d)
{
	this->scene->setLinearAttenuationCoefficient(d);
	this->scene->setCustomDistanceAttenuation(true);
}
void RayTracer::setQuadraticAttenuationCoefficient(double d)
{
	this->scene->setQuadraticAttenuationCoefficient(d);
	this->scene->setCustomDistanceAttenuation(true);
}
void RayTracer::setSuperSampling(int i)
{
	m_nSuperSampling = i;
}