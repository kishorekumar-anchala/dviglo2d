#include "app.hpp"

#include <dviglo/fs/fs_base.hpp>
#include <dviglo/gl_utils/texture_cache.hpp>
#include <dviglo/main/engine_params.hpp>
#include <dviglo/main/timer.hpp>

using namespace glm;

App::App(const vector<StrUtf8>& args)
    : Application(args)
{
}

App::~App()
{
}

void App::setup()
{
    engine_params::log_path = get_pref_path("dviglo2d", "minimal_app") + "log.log";
    engine_params::window_size = {900, 700};
    engine_params::msaa_samples = 8; // При включении крэшится на сервере ГитХаба в Линуксе ....
    engine_params::window_mode = WindowMode::windowed;
}

void App::start()
{
    StrUtf8 base_path = get_base_path();

    // Attempt to load texture
    texture_ = DV_TEXTURE_CACHE->get(base_path + "engine_test_data/textures/tile128.png");
    if (!texture_) {
        std::cerr << "Error: Failed to load texture 'tile128.png'." << std::endl;
        // Handle error (maybe load a fallback texture or exit)
    }

    sprite_batch_ = make_unique<SpriteBatch>();
    if (!sprite_batch_) {
        std::cerr << "Error: Failed to create SpriteBatch." << std::endl;
        // Handle error (exit or fallback)
    }

    // Attempt to load fonts
    r_20_font_ = make_unique<SpriteFont>(SFSettingsSimple(base_path + "engine_test_data/fonts/ubuntu/Ubuntu-R.ttf", 20));
    my_font_ = make_unique<SpriteFont>(SFSettingsSimple(base_path + "engine_test_data/fonts/ubuntu/Ubuntu-R.ttf", 60, true, 0, 0x9000CAFF));

    if (!r_20_font_ || !my_font_) {
        std::cerr << "Error: Failed to load fonts." << std::endl;
        // Handle error (maybe use fallback font or exit)
    }
}

void App::handle_sdl_event(const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        on_key(event.key);
        return;

    default:
        // React to application closing and window resizing
        Application::handle_sdl_event(event);
        return;
    }
}

void App::on_key(const SDL_KeyboardEvent& event_data)
{
    if (event_data.type == SDL_EVENT_KEY_DOWN && event_data.repeat == false
        && event_data.scancode == SDL_SCANCODE_ESCAPE)
    {
        should_exit_ = true;
    }
}

static f32 rotation = 0.f;
static StrUtf8 fps_text = "FPS: ?";

void App::update(i64 ns)
{
    static i64 frame_counter = 0;
    static i64 time_counter = 0;

    ++frame_counter;
    time_counter += ns;

    // Update fps_text every half second
    if (time_counter >= ns_per_s / 2)
    {
        i64 fps = frame_counter * ns_per_s / time_counter;
        fps_text = format("FPS: {}", fps);
        frame_counter = 0;
        time_counter = 0;
    }

    rotation += ns * 0.000'000'000'1f;
    while (rotation >= 360.f)
        rotation -= 360.f;
}

void App::draw()
{
    // Clear the screen with a color
    glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Ensure sprite_batch_ is valid before proceeding with drawing
    if (sprite_batch_) {
        sprite_batch_->prepare_ogl(true);

        sprite_batch_->triangle_.v0 = {{800.f, 0.f}, 0xFF00FF00};
        sprite_batch_->triangle_.v1 = {{800.f, 300.f}, 0xFF0000FF};
        sprite_batch_->triangle_.v2 = {{0.f, 300.f}, 0xFFFFFFFF};
        sprite_batch_->add_triangle();

        sprite_batch_->set_shape_color(0xFFFF0000);
        sprite_batch_->draw_triangle({400.f, 0.f}, {400.f, 600.f}, {0.f, 600.f});

        sprite_batch_->set_shape_color(0x90FFFF00);
        sprite_batch_->draw_rect({300.f, 300.f, 300.f, 100.f});

        // Check if texture is valid before drawing sprites
        if (texture_) {
            sprite_batch_->draw_sprite(texture_.get(), {100.f, 100.f});
            sprite_batch_->draw_sprite(texture_.get(), {500.f, 100.f}, nullptr, 0xFFFFFFFF, rotation);
        }

        sprite_batch_->draw_string(fps_text, r_20_font_.get(), {4.f, 1.f}, 0xFF000000);
        sprite_batch_->draw_string(fps_text, r_20_font_.get(), {3.f, 0.f}, 0xFFFFFFFF);

        // Get mouse position and display it
        f32 mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        sprite_batch_->draw_string("Привет!", my_font_.get(), {mouse_x, mouse_y});
        
        sprite_batch_->flush();
    } else {
        std::cerr << "Error: SpriteBatch is not initialized." << std::endl;
    }
}
