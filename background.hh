#ifndef BACKGROUND_HH_
#define BACKGROUND_HH_
#include "vfxentity.hh"
#include "shader.hh"
#include "box.hh"
#include <memory>

class Box;
class Background : public VFXEntity {
public:
    Background(Box worldbox, int created_at);
 
    virtual void draw(int frame);

private:
    static std::unique_ptr<Shader> shader_;
    Box worldbox_;
};

#endif
