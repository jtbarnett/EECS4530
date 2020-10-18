#include <GL/glew.h>
#include <GL/freeglut.h>
#include "PositionInfo.h"

/*
 * Implementation file for PositionInfo
 *
 *
 * Author:  Jerry Heuring
 * Date  :  September 18, 2020
 *
 * Revisions:
 *
 * Bugs:
 *
 */

PositionInfo::PositionInfo() {
    vao = 0;
    bao = 0;
    nbrPoints = 0;
    modelingTransformLoc = 0;
    modelingTransform = vmath::scale(1.0f);
    color = vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f);

}

PositionInfo::PositionInfo(GLuint vao, GLuint bao, int nbrPoints,
    GLuint modelingTransformLoc) {
    this->vao = vao;
    this->bao = bao;
    this->nbrPoints = nbrPoints;
    this->modelingTransformLoc = modelingTransformLoc;
    modelingTransform = vmath::scale(1.0f);
    color = vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

PositionInfo::PositionInfo(GLuint vao, GLuint bao, int nbrPoints,
    GLuint modelingTransformLoc,
    vmath::mat4 modelingTransform, vmath::vec4 color) {
    this->vao = vao;
    this->bao = bao;
    this->nbrPoints = nbrPoints;
    this->modelingTransformLoc = modelingTransformLoc;
    this->modelingTransform = modelingTransform;
    this->color = color;
}

void PositionInfo::draw() {

    /*
     * Build the modeling transforms.
     */
    glUniformMatrix4fv(modelingTransformLoc, 1, GL_FALSE,
        modelingTransform);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, bao);
    glDrawArrays(GL_TRIANGLES, 0, nbrPoints);

}

void PositionInfo::clearTransforms() {
    modelingTransform = vmath::scale(1.0f);
}

void PositionInfo::setModelingTransform(vmath::mat4 modelingTransform) {
    this->modelingTransform = modelingTransform;
}

void PositionInfo::addTranslation(float deltax, float deltay, float deltaz) {
    modelingTransform = vmath::translate(deltax, deltay, deltaz) * modelingTransform;
}

void PositionInfo::addScale(float scalex, float scaley, float scalez) {
    modelingTransform = vmath::scale(scalex, scaley, scalez) * modelingTransform;
}

void PositionInfo::addRotation(float angle, vmath::vec3 axis) {
    modelingTransform = vmath::rotate(angle, axis) * modelingTransform;
}
