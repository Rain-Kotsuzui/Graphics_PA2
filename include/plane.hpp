#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>


class Plane : public Object3D
{
public:
    Plane()
    {
        throw "Plane not defined!";
    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m),norm(normal),d(d)
    {
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        float t1=Vector3f::dot(r.getDirection(), norm);
        if (abs(t1) < 1e-6)
            return false;
        float t = (d-Vector3f::dot(norm,r.getOrigin()))/t1;
        if (t > tmin && t < h.getT())
        {
            h.set(t, material, norm);
            return true;
        }
        else
            return false;
    }

    void drawGL() override
    {
        Object3D::drawGL();
        Vector3f xAxis = Vector3f::RIGHT;
        Vector3f yAxis = Vector3f::cross(norm, xAxis);
        xAxis = Vector3f::cross(yAxis, norm);
        const float planeSize = 10.0;
        glBegin(GL_TRIANGLES);
        glNormal3fv(norm);
        glVertex3fv(d * norm + planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis - planeSize * yAxis);
        glVertex3fv(d * norm + planeSize * xAxis - planeSize * yAxis);
        glNormal3fv(norm);
        glVertex3fv(d * norm + planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis - planeSize * yAxis);
        glEnd();
    }

protected:
    Vector3f norm;
    float d;
};

#endif // PLANE_H
