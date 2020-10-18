#ifndef __DRAWABLE_H__
#define __DRAWABLE_H__

/*
 * The drawable interface.
 * Currently it contains only one method, the draw method.
 * I'm considering whether to add the BAO and VAO methods
 * in this interface as well but haven't done so as of this
 * time.
 *
 * Author:  Jerry Heuring
 * Date:    September 18, 2020
 *
 * Revisions:
 *
 * Bugs:
 *
 */
class Drawable {
public:
    virtual void draw() = 0;
};

#endif
