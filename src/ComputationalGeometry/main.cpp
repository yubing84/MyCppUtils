#include <iostream>

#include "ComputationalGeometry/SrPrimitive.h"
#include "ComputationalGeometry/SrSegment3D.h"
#include "ComputationalGeometry/SrPlane3D.h"

int main()
{
	SrPoint3D startPoint(1.0, 1.0, -1.0);
	SrPoint3D endPoint(1.0, 1.0, 1.0);

	SrSegment3D segment(startPoint, endPoint);

	SrPlane3D plane(SrVector3D(0.0, 0.0, 1.0), SrPoint3D(0.0, 0.0, 0.0));
	SrPoint3D result;
	plane.intersectSegment(segment, result);

	getchar();
	return 0;
}