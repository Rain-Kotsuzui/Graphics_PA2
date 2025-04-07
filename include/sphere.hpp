#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <glut.h>


class Sphere : public Object3D
{
public:
    Sphere()
    {
        // unit ball at the center
        center = Vector3f::ZERO;
        radius = 1;
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material), radius(radius), center(center)
    {
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        auto delta = [](float a, float b, float c)
        { return b * b - 4 * a * c; };
        float del = delta(1, 2 * Vector3f::dot(r.getDirection(), r.getOrigin() - center), (r.getOrigin() - center).squaredLength() - radius * radius);
        if (del < 0)
            return false;
        float t = -Vector3f::dot(r.getDirection(), r.getOrigin() - center) - sqrt(del) / 2;

        if (t > tmin && t < h.getT())
        {
            Vector3f norm = (r.pointAtParameter(t) - center).normalized();
            h.set(t, material, norm);
            return true;
        }
        else
            return false;
    }

    void drawGL() override
    {
        Object3D::drawGL();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(center.x(), center.y(), center.z());
        glutSolidSphere(radius, 80, 80);
        glPopMatrix();
    }

protected:
    Vector3f center;
    float radius;
};

#endif
