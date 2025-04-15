#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

struct CurvePoint
{
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D
{
protected:
    std::vector<Vector3f> controls;

public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        return false;
    }

    std::vector<Vector3f> &getControls()
    {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint> &data) = 0;

    void drawGL() override
    {
        Object3D::drawGL();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_LIGHTING);
        glColor3f(1, 1, 0);

        glBegin(GL_LINE_STRIP);
        for (auto &control : controls)
        {
            glVertex3fv(control);
        }
        glEnd();
        glPointSize(4);

        glBegin(GL_POINTS);
        for (auto &control : controls)
        {
            glVertex3fv(control);
        }
        glEnd();

        std::vector<CurvePoint> sampledPoints;
        discretize(30, sampledPoints);

        glColor3f(1, 1, 1);
        glBegin(GL_LINE_STRIP);
        for (auto &cp : sampledPoints)
        {
            glVertex3fv(cp.V);
        }
        glEnd();

        glPopAttrib();
    }
};

class BezierCurve : public Curve
{
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points)
    {
        if (points.size() < 4 || points.size() % 3 != 1)
        {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint> &data) override
    {
        data.clear();
        int numSegments = (int)controls.size() / 3;
        for (int i = 0; i < numSegments; i++)
        {
            Vector3f p0 = controls[i * 3];
            Vector3f p1 = controls[i * 3 + 1];
            Vector3f p2 = controls[i * 3 + 2];
            Vector3f p3 = controls[i * 3 + 3];
            for (int j = 0; j <= resolution; j++)
            {
                float t = (float)j / resolution;
                float u = 1 - t;
                CurvePoint cp;
                cp.V = u * u * u * p0 + 3 * u * u * t * p1 + 3 * u * t * t * p2 + t * t * t * p3;
                cp.T = (3 * u * u * (p1 - p0) + 6 * u * t * (p2 - p1) + 3 * t * t * (p3 - p2)).normalized();
                data.push_back(cp);
            }
        }
    }

protected:
};

class BsplineCurve : public Curve
{
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points)
    {
        if (points.size() < 4)
        {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint> &data) override
    {
        data.clear();
        int k = 3;
        float knot[controls.size() + k + 1];
        float B[controls.size() + k];

        // fill in knot vector
        for (int i = 0; i < controls.size() + k + 1; i++)
            knot[i] = (float)(i) / (controls.size() + k);

        for (int tknot = k; tknot < controls.size(); tknot++)
        {

            for (int res = 0; res <= resolution; res++)
            {
                std::fill(B, B + controls.size() + k, 0);
                B[tknot] = 1;
                if(res==resolution)
                B[tknot] = 0,B[tknot+1] = 1;

                float t = knot[tknot] + (knot[tknot + 1] - knot[tknot]) * (res * 1.0 / resolution);
                CurvePoint cp;
                for (int p = 1; p <= k; p++)
                    for (int i = 0; i < controls.size() + k - p; i++)
                    {
                        if (p == k)
                            cp.T += controls[i] * p * (B[i] / (knot[i + p] - knot[i]) - B[i + 1] / (knot[i + p + 1] - knot[i + 1]));
                        B[i] = B[i] * (t - knot[i]) / (knot[i + p] - knot[i]) + B[i + 1] * (knot[i + p + 1] - t) / (knot[i + p + 1] - knot[i + 1]);
                        if (p == k)
                            cp.V += B[i] * controls[i];
                    }
                cp.T = cp.T.normalized();
                data.push_back(cp);
            }
        }
    }

protected:
};

#endif // CURVE_HPP
