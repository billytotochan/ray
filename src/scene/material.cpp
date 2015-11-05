#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	vec3f color = vec3f( 0.0, 0.0, 0.0);
	color += ke;
	
	vec3f ambientColor = scene->getAmbientLight();
	color[0] += ka[0] * ambientColor[0];
	color[1] += ka[1] * ambientColor[1];
	color[2] += ka[2] * ambientColor[2];

	vec3f point = r.at(i.t);
	vec3f zeroVector = vec3f(0.0,0.0,0.0);

	list<Light*>::const_iterator it;
	for (it = scene->beginLights(); it != scene->endLights(); it++){

		vec3f incidentLight = -((*it)->getDirection( point)).normalize();
		vec3f reflectLight = (-incidentLight + 2 * incidentLight.dot(i.N) / i.N.length_squared() * i.N).normalize();

		vec3f lightColor = (*it)->getColor(point);

		vec3f diffuseIndex = kd * max( i.N.normalize().dot( incidentLight), 0.0);
		vec3f specularIndex = ks * pow( max( -r.getDirection().dot(reflectLight),0.0) , shininess);

		//printf("%f", diffuseIndex[0]);

		for (int i = 0; i < 3; i++){
			color[i] += lightColor[i] * ( diffuseIndex[i] + specularIndex[i]);
			if (color[i] > 1) color[i] = 1;
		}
	}

	//printf("%f ", color[0]);

	return color;
}
