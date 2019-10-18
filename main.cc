#include <assert.h>
#include <stdlib.h>
#include <deque>
#include <vector>
#include <memory>
#include <atomic>
#include <utility>
#include <cmath>
#include <iostream>
#include <functional>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_opengl.h>

#define FPS 60

ALLEGRO_DISPLAY *display = NULL;

struct Box {
    float x;
    float y;
    float w;
    float h;

    bool collides(const Box &b) const;
    bool collides(int x, int y) const;
    std::pair<int, int> local_coords(int x, int y) const;
    int area() const;
    Box scaled(float coeff) const;
};

bool Box::collides(const Box &b) const {
    return x < b.x + b.w &&
           x + w > b.x &&
           y < b.y + b.h &&
           y + h > b.y;
}

bool Box::collides(int px, int py) const {
    return px > x && px < x + w &&
           py > y && py < y + h;
}

std::pair<int, int> Box::local_coords(int px, int py) const {
    return std::make_pair(
        px - x,
        py - y
    );
}

int Box::area() const {
    return w * h;
}

Box Box::scaled(float c) const {
    return Box{x * c, y * c, w * c, h * c};
}

struct Collision {
    Collision()
    : collided(false), x(0), y(0) {}

    Collision(int x, int y)
    : collided(true), x(x), y(y) {}

    bool collided;
    int x;
    int y;

    operator bool() const {
        return collided;
    }
};

class Sprite {
public:
    explicit Sprite(Box b)
    : box_(b) {
        bitmap_ = al_create_bitmap(b.w, b.h);
        assert(bitmap_ != NULL);
    }

    ~Sprite() {
        if (bitmap_ != NULL)
            al_destroy_bitmap(bitmap_);
    }

    ALLEGRO_BITMAP *bitmap() {
        return bitmap_;
    }

    void draw();

    Collision collides(const Sprite &s) const;

    float &x() {
        return box_.x;
    }

    float &y() {
        return box_.y;
    }

    float w() const {
        return box_.w;
    }

    float h() const {
        return box_.h;
    }

    Box box() const {
        return box_;
    }

private:
    Box box_;
    ALLEGRO_BITMAP *bitmap_;
};

void Sprite::draw() {
    al_draw_bitmap(bitmap_, (int)(x()), (int)(y()), 0);
}

Collision Sprite::collides(const Sprite &s) const {
    Collision retv;

    if (!box_.collides(s.box_))
        return retv;

    // pixel sweep optimization
    if (s.box_.area() < box_.area())
        return s.collides(*this);

    ALLEGRO_LOCKED_REGION *sa_lk = al_lock_bitmap(bitmap_,
                           ALLEGRO_PIXEL_FORMAT_ABGR_8888,
                                   ALLEGRO_LOCK_READWRITE);
    ALLEGRO_LOCKED_REGION *sb_lk = al_lock_bitmap(s.bitmap_,
                             ALLEGRO_PIXEL_FORMAT_ABGR_8888,
                                     ALLEGRO_LOCK_READWRITE);
    unsigned char *sa_data = (unsigned char *)(sa_lk->data);
    unsigned char *sb_data = (unsigned char *)(sb_lk->data);
    int sap = sa_lk->pitch;
    int sbp = sb_lk->pitch;

    // for every point in the sprite
    for (int px = box_.x; px < box_.x + box_.w; px++) {
        for (int py = box_.y; py < box_.y + box_.h; py++) {
            if (s.box_.collides(px, py)) {
                auto la = box_.local_coords(px, py);
                int lax = la.first;
                int lay = la.second;

                auto lb = s.box_.local_coords(px, py);
                int lbx = lb.first;
                int lby = lb.second; 

                int alpha_a = sa_data[lay * sap + lax * 4];
                int alpha_b = sb_data[lby * sbp + lbx * 4];

                if (alpha_a && alpha_b) {
                    retv.collided = true;
                    retv.x = px;
                    retv.y = py; 
                    goto cleanup;
                }
            }
        }
    }

    // C-style allegro interface
cleanup:
    al_unlock_bitmap(bitmap_);
    al_unlock_bitmap(s.bitmap_);
    return retv;
}

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
    virtual void update(int frame) {}
    virtual void collect();
    virtual ~Entity() {};

protected:
    const int created_at_;

private:
    bool collect_ = false;

    std::vector<Callback> on_collect_cbs_;
};

void Entity::collect() {
    for (auto &cb : on_collect_cbs_)
        cb(*this);

    collect_ = true;
}

class Asset {};

class Shader : public Asset {
public:
    Shader(const char *vs, const char *fs);

    void use();
    static void reset();

private:
    ALLEGRO_SHADER *shader_;
};

Shader::Shader(const char *vs, const char *fs) {
    shader_ = al_create_shader(ALLEGRO_SHADER_AUTO);
    if (!al_attach_shader_source_file(shader_, ALLEGRO_VERTEX_SHADER, vs)) {
        std::cerr << al_get_shader_log(shader_) << std::endl;
        exit(1);
    }

    if (!al_attach_shader_source_file(shader_, ALLEGRO_PIXEL_SHADER, fs)) {
        std::cerr << al_get_shader_log(shader_) << std::endl;
        exit(1);
    }

    if (!al_build_shader(shader_)) {
        std::cerr << al_get_shader_log(shader_) << std::endl;
        exit(1);
    }
}

void Shader::use() {
    assert(shader_ != NULL);
    al_use_shader(shader_);
}

void Shader::reset() {
    al_use_shader(NULL);
}

class EntityTracker;
class GameEntity : public Entity {
public:
    Collision collides(const GameEntity &e) {
        if (collision_group_ == e.collision_group_)
            return Collision();

        return sprite_.collides(e.sprite_);
    }

    void add_collision_callback(Callback cb) {
        on_collision_cbs_.push_back(cb);
    }

    virtual void on_keypress(int keycode) {};
    virtual void on_collision(int frame, Collision c) {};
    virtual void collect();
    virtual ~GameEntity() {}

protected:
    GameEntity(Box entbox, Box worldbox, EntityTracker &entities,
                             int collision_group, int created_at)
    : Entity(created_at), sprite_(entbox), worldbox_(worldbox),
      entities_(entities), collision_group_(collision_group) {}

    Sprite sprite_;
    Box worldbox_;
    EntityTracker &entities_;
    int collision_group_;

    std::vector<Callback> on_collision_cbs_;
};

void GameEntity::collect() {
    for (auto &cb : on_collision_cbs_)
        cb(*this);

    Entity::collect();
}

class Asteroid : public GameEntity {
public:
    Asteroid(Box entbox, Box worldbox, EntityTracker &entities,
         int created_at, float rotation, float downward_speed);
    virtual ~Asteroid() {};

    virtual void update(int frame);
    virtual void draw(int frame);
    virtual void on_collision(int frame, Collision c);

private:
    static std::unique_ptr<Shader> shader_;

    void gen_sprite();

    Sprite pre_rotation_;
    float rotation_; // rad/s
    float downward_speed_;
    float health_;
};

std::unique_ptr<Shader> Asteroid::shader_;

Asteroid::Asteroid(Box entbox, Box worldbox, EntityTracker &entities,
                int created_at, float rotation, float downward_speed)
: GameEntity(entbox, worldbox, entities, 1, created_at), pre_rotation_(entbox),
                          rotation_(rotation), downward_speed_(downward_speed),
                                                     health_(rand() % 12 + 3) {
    if (!shader_) {
        shader_ = std::make_unique<Shader>(
            "texture_vs.glsl",
            "asteroid_fs.glsl"
        );
    }

    gen_sprite();
}

void Asteroid::gen_sprite() {
    std::vector<float> vertices;
    int vertex_count_ = rand() % 6 + 5; 
    const float step = 2 * M_PI / (float)(vertex_count_);
    for (int i = 0; i < vertex_count_; i++) {
        float len = .2f + (rand() % 3) / 10.f;
        float angle = i * step;
        float x = (.5f + sin(angle) * len) * sprite_.w();
        float y = (.5f + cos(angle) * len) * sprite_.h();

        vertices.push_back(x);
        vertices.push_back(y);
    }

    int g = 25 + rand() % 125;
    ALLEGRO_COLOR color = al_map_rgb(g, g, g);

    al_set_target_bitmap(pre_rotation_.bitmap());
    al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));
    al_draw_filled_polygon(vertices.data(), vertex_count_, color);
}

void Asteroid::draw(int frame) {
    shader_->use();

    al_set_shader_float("frame", (float)(frame));
    al_set_shader_float("created_at", (float)(created_at_));
    al_set_shader_float("speed", (float)(downward_speed_));
    sprite_.draw();

    Shader::reset();
}

class Ship : public GameEntity {
public:
    Ship(Box shipbox, Box worldbox, EntityTracker &entities, int created_at,
                                                               float accel);

    virtual void update(int frame);
    virtual void draw(int frame);
    virtual void on_keypress(int keycode);
    virtual void on_collision(int frame, Collision c);

private:
    static std::unique_ptr<Shader> shader_;

    float accel_;
    int fire_cooldown_ = 10;

    float horz_speed_ = 0;
    float next_frame_dv_ = 0;
    bool fire_projectile_ = 0;
    int last_projectile_frame_ = 0;
};

std::unique_ptr<Shader> Ship::shader_;

Ship::Ship(Box entbox, Box worldbox, EntityTracker &entities, int created_at,
                                                                 float accel)
: GameEntity(entbox, worldbox, entities, 2, created_at), accel_(accel) {
    if (!shader_) {
        shader_ = std::make_unique<Shader>(
            "texture_vs.glsl",
            "ship_fs.glsl"
        );
    }

    ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);
    al_set_target_bitmap(sprite_.bitmap());
    al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));

    Box ship = entbox.scaled(.5f);
    float x1 = 0 + .25f * entbox.w;
    float y1 = ship.h + .25f * entbox.h;
    float x2 = ship.w / 2 + .25f * entbox.w;
    float y2 = 0 + .25f * entbox.h;
    float x3 = ship.w + .25f * entbox.w;
    float y3 = ship.h + .25f * entbox.h;
    al_draw_filled_triangle(x1, y1, x2, y2, x3, y3, white);
}

void Ship::draw(int frame) {
    shader_->use();
    al_set_shader_float("frame", (float)(frame));
    al_set_shader_float("created_at", (float)(created_at_));
    al_set_shader_float("projectile_cooldown", (float)(frame
                - last_projectile_frame_) / fire_cooldown_);
    sprite_.draw();
    Shader::reset();
}

void Ship::on_keypress(int keycode) {
    switch (keycode) {
    case ALLEGRO_KEY_LEFT:
        next_frame_dv_ = -accel_;
        break;
    case ALLEGRO_KEY_RIGHT:
        next_frame_dv_ = accel_;
        break;
    case ALLEGRO_KEY_UP:
        fire_projectile_ = true;
        break;
    }
}

class Projectile : public GameEntity {
public:
    Projectile(Box entbox, Box worldbox, EntityTracker &entities,
                                    int created_at, float accel);

    virtual void update(int frame);
    virtual void draw(int frame);
    virtual void on_collision(int frame, Collision c);

private:
    static std::unique_ptr<Shader> shader_;

    float vert_speed_;
    float health_;
};

std::unique_ptr<Shader> Projectile::shader_;

Projectile::Projectile(Box entbox, Box worldbox, EntityTracker &entities,
                                             int created_at, float speed)
: GameEntity(entbox, worldbox, entities, 3, created_at), vert_speed_(speed),
                                                  health_(rand() % 16 + 1) {
    if (!shader_) {
        shader_ = std::make_unique<Shader>(
            "texture_vs.glsl",
            "projectile_fs.glsl"
        );
    }

    ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);
    al_set_target_bitmap(sprite_.bitmap());
    al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));
    al_draw_filled_rectangle(0, 0, entbox.w, entbox.h, white);
}

void Projectile::update(int frame) {
    sprite_.y() -= vert_speed_;

    if (!sprite_.box().collides(worldbox_)) {
        collect();
        return;
    }
}

void Projectile::draw(int frame) {
    shader_->use();
    al_set_shader_float("frame", (float)(frame));
    al_set_shader_float("created_at", (float)(created_at_));
    sprite_.draw();
    Shader::reset();
}

class VFXEntity : public Entity {
public:
    using Entity::Entity;
};

class EntityTracker {
public:
    typedef std::unique_ptr<GameEntity> GameEntityPtr;
    typedef std::unique_ptr<VFXEntity> VFXEntityPtr;
    typedef std::unique_ptr<Entity> EntityPtr;

    GameEntity &add(GameEntityPtr p) {
        game_entities_.push_back(std::move(p));
        return *game_entities_.back();
    }

    VFXEntity &add(VFXEntityPtr p) {
        vfx_entities_.push_back(std::move(p));
        return *vfx_entities_.back();
    }

    void draw(int frame);
    void update(int frame);
    void clear_game_entities();

    void on_keypress(int keycode);

    int count() const;

private:
    template <typename Container>
    static void collect_entities(Container &entities);

    void check_collisions(int frame);

    std::deque<GameEntityPtr> game_entities_;
    std::deque<VFXEntityPtr> vfx_entities_;
};

void EntityTracker::clear_game_entities() {
    game_entities_.clear();
}

int EntityTracker::count() const {
    return game_entities_.size() + vfx_entities_.size();
}

void EntityTracker::draw(int frame) {
    for (auto &e : game_entities_) {
        al_set_target_backbuffer(display);
        e->draw(frame);
    }
    for (auto &e : vfx_entities_) {
        al_set_target_backbuffer(display);
        e->draw(frame);
    }
}

void EntityTracker::update(int frame) {
    for (auto &e : game_entities_)
        e->update(frame);
    for (auto &e : vfx_entities_)
        e->update(frame);

    check_collisions(frame);
    collect_entities(game_entities_);
    collect_entities(vfx_entities_);
}

void EntityTracker::on_keypress(int keycode) {
    for (auto &e : game_entities_)
        e->on_keypress(keycode);
}

class Game {
public:
    enum State {
        SETUP,
        GAME,
        OVER        
    };

    Game(Box worldbox);

    void draw();
    void update();
    void on_keypress(int keycode);

    const EntityTracker &entities() const {
        return entities_;
    }

    int frame() const {
        return frame_;
    }

    State state() const {
        return state_;
    }

private:
    void spawn_asteroids();
    void init_scene();
    void spawn_ship();
    void update_game_state();

    void change_game_state(enum State new_state);
    int frames_since_prev_state() const {
        return frame_ - state_changed_at_;
    }

    Box worldbox_;
    EntityTracker entities_;

    enum State state_;
    int state_changed_at_;
    int frame_;
};

void Game::change_game_state(enum State new_state) {
    switch (new_state) {
    case SETUP:
        entities_.clear_game_entities();
        break;
    case GAME:
        spawn_ship();
        break;
    case OVER:
        break;
    default:
        break;
    }

    state_ = new_state;
    state_changed_at_ = frame_;
}

void Game::spawn_ship() {
    const float shipw = 100;
    const float shiph = 100;
    auto ship = std::make_unique<Ship>(
        Box{
            worldbox_.w / 2 - shipw / 2,
            worldbox_.h - shiph,
            shipw,
            shiph},
        worldbox_, entities_, 0, 2.4f
    );
    ship->add_collision_callback(
        [this](Entity &) -> void {
            change_game_state(OVER);
        }
    );

    entities_.add(std::move(ship));
}

void Game::draw() {
    entities_.draw(frame_);
}

void Game::update_game_state() {
    switch (state_) {
    case SETUP:
        if (frames_since_prev_state() > 30)
            change_game_state(GAME);
        break;
    case OVER:
        if (frames_since_prev_state() > 300)
            change_game_state(SETUP);
        break;
    case GAME:
    default:
        break;
    }
}

void Game::update() {
    if (state_ == GAME)
        spawn_asteroids();

    entities_.update(frame_);
    update_game_state();

    frame_++;
}

void Game::spawn_asteroids() {
    if (!(frame_ % 50)) {
        const float rotation = M_PI / (rand() % 30 + 1);
        const float diameter = 100;
        const float speed = .1f + (rand()%40) / 10.f;
        entities_.add(
            std::make_unique<Asteroid>(
                Box{
                    rand() % (int)(worldbox_.w) - diameter,
                    -diameter,
                    diameter,
                    diameter 
                }, worldbox_, entities_, frame_, rotation, speed
            )
        );
    }
}

void Game::on_keypress(int keycode) {
    entities_.on_keypress(keycode);
}

void EntityTracker::check_collisions(int frame) {
    for (auto ea = game_entities_.begin(); ea != game_entities_.end();
                                                               ea++) {
        for (auto eb = game_entities_.begin(); eb != game_entities_.end();
                                                                   eb++) {
            if (ea == eb)
                continue;

            Collision c = (*ea)->collides(**eb);
            if (c) {
                (*ea)->on_collision(frame, c);
                (*eb)->on_collision(frame, c);
            }
        }
    }
}

template <typename Container>
void EntityTracker::collect_entities(Container &entities) {
    bool retry;
    do {
        retry = false;
        for (auto it = entities.begin(); it != entities.end(); it++) {
            if ((*it)->collectable()) {
                entities.erase(it);
                retry = true;
                break;
            }
        }
    } while(retry);
}

void Ship::update(int frame) {
    if (sprite_.x() < 0 || sprite_.x() > worldbox_.w - sprite_.w()) {
        horz_speed_ = -horz_speed_ / 2.f;
        next_frame_dv_ = .0f;

        if (sprite_.x() < 0)
            sprite_.x() = 0;
        else if (sprite_.x() > worldbox_.w - sprite_.w())
            sprite_.x() = worldbox_.w - sprite_.w();
    }

    if (next_frame_dv_ != .0f) {
        horz_speed_ += next_frame_dv_;
        next_frame_dv_ = .0f;
    } else {
        if (horz_speed_ >= accel_)
            horz_speed_ -= accel_;
        else if (horz_speed_ <= accel_)
            horz_speed_ += accel_;

        if (fabs(horz_speed_) <= accel_)
            horz_speed_ = .0f;
    }

    if (fire_projectile_) {
        fire_projectile_ = false;
        if (fabs(last_projectile_frame_ - frame) > fire_cooldown_) {
            last_projectile_frame_ = frame;

            Box prbox{
                sprite_.x() + sprite_.w() / 2,
                sprite_.y() - 7,
                3,
                5
            };

            entities_.add(
                std::make_unique<Projectile>(
                    prbox, worldbox_, entities_, frame, 12.f
                )
            );
        }
    }

    sprite_.x() += horz_speed_;
}

class Explosion : public VFXEntity {
public:
    Explosion(int created_at, int lifetime, float cx, float cy,
                                              float maxradius);

    virtual void draw(int frame);
    virtual void update(int frame);

private:
    static std::unique_ptr<Shader> shader_;

    float cx_;
    float cy_;
    float maxradius_;
    int lifetime_;
};

std::unique_ptr<Shader> Explosion::shader_;

class Background : public VFXEntity {
public:
    Background(Box worldbox, int created_at);
 
    virtual void draw(int frame);

private:
    static std::unique_ptr<Shader> shader_;
    Box worldbox_;
};

std::unique_ptr<Shader> Background::shader_;

Background::Background(Box worldbox, int created_at)
: VFXEntity(created_at), worldbox_(worldbox) {
    if (!shader_) {
        shader_ = std::make_unique<Shader>(
            "generic_vs.glsl",
            "background_fs.glsl"
        );
    }
}

void Background::draw(int frame) {
    shader_->use();

    float dimensions[2] = { worldbox_.w, worldbox_.h };
    al_set_shader_float_vector("dimensions", 2, &dimensions[0], 1);

    ALLEGRO_COLOR color;
    al_draw_filled_rectangle(0, 0, worldbox_.w, worldbox_.h, color);

    Shader::reset();
}

Explosion::Explosion(int created_at, int lifetime, float cx, float cy,
                                                      float maxradius)
: VFXEntity(created_at), cx_(cx), cy_(cy), maxradius_(maxradius),
                                            lifetime_(lifetime) {
    if (!shader_) {
        shader_ = std::make_unique<Shader>(
            "generic_vs.glsl",
            "explosion_fs.glsl"
        );
    }
}

void Explosion::draw(int frame) {
    shader_->use();

    float explosion_center[2] = { cx_, cy_ };
    al_set_shader_float_vector("explosion_center", 2, &explosion_center[0], 1);
    al_set_shader_float("progress", (float)(frame - created_at_) / lifetime_);
    al_set_shader_float("maxradius", maxradius_);

    ALLEGRO_COLOR color;
    al_draw_filled_rectangle(cx_ - maxradius_, cy_ - maxradius_,
                     cx_ + maxradius_, cy_ + maxradius_, color);

    Shader::reset();
}

void Explosion::update(int frame) {
    if (frame > created_at_ + lifetime_)
        collect();
}

void Projectile::on_collision(int frame, Collision c) {
    entities_.add(
        std::make_unique<Explosion>(
            frame,
            300,
            c.x,
            c.y,
            200.f
        )
    );

    if (health_-- <= 0)
        collect();
}

void Asteroid::update(int frame) {
    sprite_.y() += downward_speed_;

    if (sprite_.y() > 0 && !sprite_.box().collides(worldbox_)) {
        entities_.add(
            std::make_unique<Explosion>(
                frame,
                800,
                sprite_.x(),
                sprite_.y() + 100.f,
                1200.f
            )
        );

        collect();
        return;
    }

    float angle = rotation_ * frame / FPS;
    int cx = sprite_.w() / 2;
    int cy = sprite_.h() / 2;

    al_set_target_bitmap(sprite_.bitmap());
    al_clear_to_color(al_map_rgba_f(0, 0, 0, 0));
    al_draw_rotated_bitmap(pre_rotation_.bitmap(), cx, cy, cx, cy, angle, 0);
}

Game::Game(Box worldbox)
: worldbox_(worldbox), frame_(0) {
    init_scene();
    change_game_state(SETUP);
}

void Asteroid::on_collision(int frame, Collision c) {
    if (--health_ <= 0) {
        entities_.add(
            std::make_unique<Explosion>(
                frame,
                400,
                sprite_.x() + sprite_.w() / 2,
                sprite_.y() + sprite_.h() / 2,
                400.f
            )
        );
        entities_.add(
            std::make_unique<Explosion>(
                frame,
                50,
                sprite_.x() + sprite_.w() / 2,
                sprite_.y() + sprite_.h() / 2,
                400.f + (float)(rand() % 200)
            )
        );

        collect();
    }
}

void Ship::on_collision(int frame, Collision c) {
        entities_.add(
            std::make_unique<Explosion>(
                frame,
                50,
                sprite_.x() + sprite_.w() / 2,
                sprite_.y() + sprite_.h() / 2,
                4800.f
            )
        );
        entities_.add(
            std::make_unique<Explosion>(
                frame,
                1000,
                sprite_.x() + sprite_.w() / 2,
                sprite_.y() + sprite_.h() / 2,
                3600.f
            )
        );

        collect();
}

void Game::init_scene() {
    entities_.add(
        std::make_unique<Background>(
            worldbox_, 0
        )
    );
}

main() {
    const int dw = 800;
    const int dh = 600;

    al_init();
    al_set_config_value(al_get_system_config(), "trace", "level", "debug");
    al_install_keyboard();
    al_init_font_addon();

    al_set_new_display_flags(ALLEGRO_PROGRAMMABLE_PIPELINE | ALLEGRO_OPENGL);
    display = al_create_display(dw, dh);

    ALLEGRO_KEYBOARD_STATE key_state;
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_COLOR white = al_map_rgb_f(1, 1, 1);

    al_start_timer(timer);
    Game game(Box{0, 0, dw, dh});

    bool done = false;
    bool need_redraw = true;
    bool background = false;
    while (!done) {
        const int w = al_get_display_width(display);
        const int h = al_get_display_height(display);

        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        switch (event.type) {
        case ALLEGRO_EVENT_KEY_DOWN:
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
           done = true;
           break;

        case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
           background = true;
           al_acknowledge_drawing_halt(event.display.source);
           break;
        case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
           background = false;
           al_acknowledge_drawing_resume(event.display.source);
           break;
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
           al_acknowledge_resize(event.display.source);
           break;
        case ALLEGRO_EVENT_TIMER:
           game.update();
           need_redraw = true;
           break;
        }

        al_get_keyboard_state(&key_state);
        if (al_key_down(&key_state, ALLEGRO_KEY_RIGHT))
            game.on_keypress(ALLEGRO_KEY_RIGHT);
        if (al_key_down(&key_state, ALLEGRO_KEY_LEFT))
            game.on_keypress(ALLEGRO_KEY_LEFT);
        if (al_key_down(&key_state, ALLEGRO_KEY_UP))
            game.on_keypress(ALLEGRO_KEY_UP);

        if (need_redraw) {
            al_set_target_backbuffer(display);
            al_clear_to_color(al_map_rgb_f(0, 0, 0));

            game.draw();

            al_set_target_backbuffer(display);
            int entities = game.entities().count();
            al_draw_textf(font, white, 0, 0, 0, "frame: %d, entities: %d", game.frame(), entities);
            al_flip_display();
            need_redraw = false;
        }
    }; 

    return 0;
}
