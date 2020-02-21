#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jbufferedimage.h"

const jgui::jsize_t<int> SCREEN_SIZE = {
  .width = 640,
  .height = 480
};

const jgui::jsize_t<int> BLOCK_SIZE = {
  .width = 32,
  .height = 32
};

jgui::jpoint_t<int> BLOCK_DIVISOR {
  .x = BLOCK_SIZE.width, 
  .y = BLOCK_SIZE.height
};

const int PLAYER_STEP = 8;

const float GRAVITY = 1.0;

const float INITIAL_VELOCITY = 32;

const std::vector<std::string> scene = {
  "................................................",
  "................................................",
  "................................................",
  "................................................",
  "................................................",
  "................................................",
  "................................................",
  "................................................",
  "........#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.........",
  ".......#...............................#........",
  "......#.................................#.......",
  ".....#...................................#......",
  "....#.....................................#.....",
  "...#.......................................#....",
  "..#.........................................#...",
  "________________________________________________",
};

class Dummy : public jgui::Window {

  private:
    std::map<jevent::jkeyevent_symbol_t, bool> _keys;
    jgui::jpoint_t<int> _player_position {
      .x = 64,
      .y = 64
    };
    float _v = 0.0f;

  public:
    Dummy():
      jgui::Window(SCREEN_SIZE)
    {
      SetFramesPerSecond(30);
    }

    virtual ~Dummy()
    {
    }

    virtual bool KeyPressed(jevent::KeyEvent *event)
    {
      auto s = event->GetSymbol();
  
      _keys[s] = true;

      return true;
    }

    virtual bool KeyReleased(jevent::KeyEvent *event)
    {
      auto s = event->GetSymbol();
  
      _keys[s] = false;

      return true;
    }

    bool Collide(jgui::jpoint_t<int> p)
    {
      p = p/BLOCK_DIVISOR;

      jgui::jpoint_t<int>
        p0 = {p + jgui::jpoint_t<int>{0, 0}},
        p1 = {p + jgui::jpoint_t<int>{0, 1}},
        p2 = {p + jgui::jpoint_t<int>{1, 0}},
        p3 = {p + jgui::jpoint_t<int>{1, 1}};

      if ((scene[p0.y][p0.x] == '.') and
          (scene[p1.y][p1.x] == '.') and
          (scene[p2.y][p2.x] == '.') and
          (scene[p3.y][p3.x] == '.')) {
        return false;
      }

      return true;
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      static jgui::Image *tiles = new jgui::BufferedImage("images/mario.png");

      jgui::Window::Paint(g);

      jgui::jpoint_t
        old_player_position = _player_position;

      if (_keys[jevent::JKS_SPACE]) {
        if (_v == 0.0) {
          _v = INITIAL_VELOCITY;
        }
      }

      if (_keys[jevent::JKS_CURSOR_LEFT]) {
        _player_position.x -= PLAYER_STEP;

        if (_player_position.x < 0) {
          _player_position.x = 0;
        }
 
        // collision
        jgui::jpoint_t<int>
          index = _player_position/BLOCK_DIVISOR;

        if (scene[index.y][index.x - 1] != '.') {
          _player_position = index*BLOCK_DIVISOR;
        }
      }

      if (_keys[jevent::JKS_CURSOR_RIGHT]) {
        _player_position.x += PLAYER_STEP;

        if (_player_position.x > (int)(scene[0].size()*BLOCK_SIZE.width - BLOCK_SIZE.width)) {
          _player_position.x = (int)(scene[0].size()*BLOCK_SIZE.width - BLOCK_SIZE.width);
        }

        jgui::jpoint_t<int>
          index = _player_position/BLOCK_DIVISOR;

        if (scene[index.y][index.x + 1] != '.') {
          _player_position = index*BLOCK_DIVISOR;
        }
      }

      _player_position.y = _player_position.y - _v;
      _v = _v - GRAVITY;

      if (_v < -BLOCK_SIZE.height/2) {
        _v = -BLOCK_SIZE.height/2;
      }

      if (_v > BLOCK_SIZE.height/2) {
        _v = BLOCK_SIZE.height/2;
      }

      if (_player_position.y > (int)(scene.size()*BLOCK_SIZE.height - BLOCK_SIZE.height)) {
        _v = 0.0;
        _player_position.y = (int)(scene.size()*BLOCK_SIZE.height - BLOCK_SIZE.height);
      }

      jgui::jpoint_t<int> offset = {
        .x = _player_position.x - SCREEN_SIZE.width/2,
        .y = _player_position.y - SCREEN_SIZE.height/2
      };

      if (offset.x < 0) {
        offset.x = 0;
      }

      if (offset.x > (int)(scene[0].size()*BLOCK_SIZE.width - SCREEN_SIZE.width)) {
        offset.x = (int)scene[0].size()*BLOCK_SIZE.width - SCREEN_SIZE.width;
      }

      if (offset.y < 0) {
        offset.y = 0;
      }

      if (offset.y > (int)(scene.size()*BLOCK_SIZE.height - SCREEN_SIZE.height)) {
        offset.y = (int)(scene.size()*BLOCK_SIZE.height - SCREEN_SIZE.height);
      }

      // draw scene
      g->SetCompositeFlags(jgui::JCF_SRC);
      g->SetBlittingFlags(jgui::JBF_NEAREST);

      for (int j=0; j<(int)scene.size(); j++) {
        for (int i=0; i<(int)scene[j].size(); i++) {
          jgui::jrect_t<int>
            src {
              .point = {0, 0},
              .size = {0, 0}
            },
            dst {
              .point = {i*BLOCK_SIZE.width - offset.x, j*BLOCK_SIZE.height - offset.y}, 
              .size = BLOCK_SIZE
            };

          char block = scene[j][i];

          if (block == '.') {
            src = {81, 423, 16, 16};
          } else if (block == '#') {
            src = {123, 516, 16, 16};
          } else if (block == '_') {
            src = {444, 202, 16, 16};
          }
            
          g->DrawImage(tiles, src, dst);
        }
      }

			g->SetColor(jgui::jcolor_name_t::Green);
			g->FillRectangle(jgui::jrect_t<int>{_player_position - offset, BLOCK_SIZE});

      Repaint();
    }

};

int main(int argc, char *argv[])
{
  jgui::Application::Init(argc, argv);

  Dummy app;

  jgui::Application::Loop();

  return 0;
}

