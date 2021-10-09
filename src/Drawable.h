#ifndef SRC_DRAWABLE_H
#define SRC_DRAWABLE_H

class Renderer;

class Drawable
{
public:
    virtual void draw(const Renderer &window) const = 0;
};

#endif /* SRC_DRAWABLE_H */
