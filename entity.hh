#ifndef ENTITY_HH
#define ENTITY_HH
#include <functional>
#include <vector>

class Entity {
public:
    typedef std::function<void(Entity &)> Callback;

    Entity(int frame)
    : created_at_(frame) {}

    bool collectable() {
        return collect_;
    }

    void add_collect_callback(Callback cb) {
        on_collect_cbs_.push_back(cb);
    }

    virtual void draw(int frame) = 0;
    virtual void update(int frame, int fps) {}
    virtual void collect();
    virtual ~Entity() {};

protected:
    const int created_at_;

private:
    bool collect_ = false;

    std::vector<Callback> on_collect_cbs_;
};

#endif
