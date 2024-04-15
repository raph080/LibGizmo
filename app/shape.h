#ifndef SHAPE_H
#define SHAPE_H

#include <glm/glm.hpp>
#include <vector>

using namespace std;
using namespace glm;

class Face
{
public:
    Face(vec3 *pt1, vec3 *pt2, vec3 *pt3) { points = {pt1, pt2, pt3}; }
    vector<vec3 *> points;
};

class Shape
{
public:
    Shape() { modelMatrix = mat4(1); }
    vector<vec3> points;
    vector<Face> faces;
    mat4 modelMatrix;
};

class Plane : public Shape
{
public:
    Plane() : Shape()
    {
        points = {vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0),
                  vec3(-1, 1, 0)};

        faces = {Face(&points[0], &points[1], &points[2]),
                 Face(&points[0], &points[2], &points[3])};
    }
};

class Cube : public Shape
{
public:
    Cube() : Shape()
    {
        points = {vec3(1, 1, 1),    vec3(1, -1, 1), vec3(-1, -1, 1),
                  vec3(-1, 1, 1),   vec3(1, 1, -1), vec3(1, -1, -1),
                  vec3(-1, -1, -1), vec3(-1, 1, -1)};

        faces = {
            // top
            Face(&points[0], &points[1], &points[2]),
            Face(&points[0], &points[2], &points[3]),
            // bottom
            Face(&points[4], &points[5], &points[6]),
            Face(&points[4], &points[6], &points[7]),
            // left
            Face(&points[0], &points[1], &points[5]),
            Face(&points[0], &points[5], &points[4]),
            // right
            Face(&points[2], &points[3], &points[7]),
            Face(&points[2], &points[7], &points[6]),
            // front
            Face(&points[1], &points[2], &points[6]),
            Face(&points[1], &points[6], &points[5]),
            // back
            Face(&points[3], &points[0], &points[4]),
            Face(&points[3], &points[4], &points[7]),
        };
    }
};

#endif