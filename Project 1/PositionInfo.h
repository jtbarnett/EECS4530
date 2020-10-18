#ifndef __POSITIONINFO_H__
#define __POSITIONINFO_H__
#include <GL/glew.h>
#include <GL/glut.h>
#include "vmath.h"
#include <GL/gl.h>
#include "Drawable.h"
using namespace std;
/*
 * This class is designed to hold position an transform information for an
 * object that can be displayed.  It extends a class, drawable, that
 * is an interface requiring the presence of a draw method.  I'm looking
 * for a general way of handling drawables and the interface may expand
 * in the future.
 *
 * Author:  Jerry Heuring
 * Date:  September 18, 2020
 *
 * Revisions:
 *
 * Bugs:
 *   Should include a location for the objectColor Uniform.
 *
 *   Don't like the idea of changing the number of points for a draw
 *   using the set method -- seems dangerous but it is in here right
 *   now.
 *
 *   Color information may be ignored if colors are supplied with the
 *   data in the Buffer array object and enabled as an attribute.
 *
 */
class PositionInfo : public Drawable {
protected:
    vmath::mat4  modelingTransform;   // The modeling transform for this object
    GLuint vao, bao;                  // vertex and buffer array objects
    int  nbrPoints;                   // number of points for a DrawArray call
    vmath::vec4 color;                // color for this object -- passed as a Uniform
    GLuint modelingTransformLoc;      // location in the program for the modelingTransform
public:
    PositionInfo();
    PositionInfo(GLuint vao, GLuint bao, int nbrPoints,
        GLuint modelingTransformLoc = 0);
    PositionInfo(GLuint vao, GLuint bao, int nbrPoints,
        GLuint modelingTransformLoc,
        vmath::mat4 modelingTransform,
        vmath::vec4 color);

    void draw();
    void clearTransforms();
    void setModelingTransform(vmath::mat4 modelingTransform);
    vmath::mat4 getModelingTransform() { return modelingTransform; }
    void addTranslation(float deltax, float deltay, float deltaz);
    void addScale(float scalex, float scaley, float scalez);
    void addRotation(float angle, vmath::vec3 axis);
    GLuint getVAO() { return vao; }
    GLuint getBAO() { return bao; }
    vmath::vec4 getColor() { return color; }
    int getNbrPoints() { return nbrPoints; }
    void setNbrPoints(int nbrPoints) { this->nbrPoints = nbrPoints; }

};
#endif
