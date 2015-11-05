#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.

	vec3f p = r.getPosition();
	vec3f d = r.getDirection();
	vec3f pos;
	double t[6];	//up,down,left,right,front,back

	// initialization
	for (int i = 0; i < 6; i++){
		t[i] = -1;
	}

	int isInside;

	if (p[0] >= -0.5&&p[0] <= 0.5&&p[1] >= -0.5&&p[1] <= 0.5&&p[2] >= -0.5&&p[2] <= 0.5)
	{
		isInside = -1; //inside
	}
	else
	{
		isInside = 1; //outside
	}

	// up
	if (abs(d[1]) > RAY_EPSILON)
	{
		t[0] = (0.5 - p[1]) / d[1];
		pos = r.at(t[0]);
		if (pos[0]>0.5 || pos[0]<-0.5 || pos[2]>0.5 || pos[2]<-0.5)
		{
			t[0] = -1;
		}
	}
	// down
	if (abs(d[1]) > RAY_EPSILON)
	{
		t[1] = (-0.5 - p[1]) / d[1];
		pos = r.at(t[1]);
		if (pos[0]>0.5 || pos[0]<-0.5 || pos[2]>0.5 || pos[2]<-0.5)
		{
			t[1] = -1;
		}
	}
	// left
	if (abs(d[0]) > RAY_EPSILON)
	{
		t[2] = (-0.5 - p[0]) / d[0];
		pos = r.at(t[2]);
		if (pos[1]>0.5 || pos[1]<-0.5 || pos[2]>0.5 || pos[2]<-0.5)
		{
			t[2] = -1;
		}
	}
	// right
	if (abs(d[0]) > RAY_EPSILON)
	{
		t[3] = (0.5 - p[0]) / d[0];
		pos = r.at(t[3]);
		if (pos[1]>0.5 || pos[1]<-0.5 || pos[2]>0.5 || pos[2]<-0.5)
		{
			t[3] = -1;
		}
	}
	//front
	if (abs(d[2]) > RAY_EPSILON)
	{
		t[4] = (0.5 - p[2]) / d[2];
		pos = r.at(t[4]);
		if (pos[1]>0.5 || pos[1]<-0.5 || pos[0]>0.5 || pos[0]<-0.5)
		{
			t[4] = -1;
		}
	}
	//back
	if (abs(d[2]) > RAY_EPSILON)
	{
		t[5] = (-0.5 - p[2]) / d[2];
		pos = r.at(t[5]);
		if (pos[1]>0.5 || pos[1]<-0.5 || pos[0]>0.5 || pos[0]<-0.5)
		{
			t[5] = -1;
		}
	}

	int closest = -1;

	for (int i = 0; i<6; i++)
	{
		if (t[i] > RAY_EPSILON)
		{
			if (closest == -1)
			{
				closest = i;
			}
			else if (t[closest] > t[i])
			{
				closest = i;
			}
		}
	}

	if (closest == -1)
		return false;

	i.obj = this;
	i.t = t[closest];
	switch (closest){
	case 0:
		i.N = vec3f(0.0, 1.0 * isInside, 0.0);
		break;
	case 1:
		i.N = vec3f(0.0, -1.0 * isInside, 0.0);
		break;
	case 2:
		i.N = vec3f(-1.0 * isInside, 0.0, 0.0);
		break;
	case 3:
		i.N = vec3f(1.0 * isInside, 0.0, 0.0);
		break;
	case 4:
		i.N = vec3f(0.0, 0.0, 1.0 * isInside);
		break;
	case 5:
		i.N = vec3f(0.0, 0.0, -1.0 * isInside);
		break;
	}

	return true;
	//return false;
}
