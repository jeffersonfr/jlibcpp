#include "jgui/japplication.h"
#include "jgui/jwindow.h"

const jgui::jsize_t<int> SCREEN_SIZE = {
  .width = 640,
  .height = 480
};

const jgui::jsize_t<int> BLOCK_SIZE = {
  .width = 32,
  .height = 32
};

const int PLAYER_STEP = 5;

const float GRAVITY = 1.6;

const float INITIAL_VELOCITY = 32;

const std::vector<std::string> scene = {
  "################################################",
  "#..............................................#",
  "#..............................................#",
  "#..............................................#",
  "#..............................................#",
  "#..............................................#",
  "#..............................................#",
  "#..............................................#",
  "#.......#.#.#.#.#.#.#.#.#.#.#.#.#.#.#.#........#",
  "#......#...............................#.......#",
  "#.....#.................................#......#",
  "#....#...................................#.....#",
  "#...#.....................................#....#",
  "#..#.......................................#...#",
  "#.#.........................................#..#",
  "################################################",
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

    virtual void Paint(jgui::Graphics *g) 
    {
      jgui::Window::Paint(g);

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
      }

      if (_keys[jevent::JKS_CURSOR_RIGHT]) {
        _player_position.x += PLAYER_STEP;

        if (_player_position.x > (int)(scene[0].size()*BLOCK_SIZE.width - BLOCK_SIZE.width)) {
          _player_position.x = (int)(scene[0].size()*BLOCK_SIZE.width - BLOCK_SIZE.width);
        }
      }

      _player_position.y = _player_position.y - _v;
      _v = _v - GRAVITY;

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
      for (int j=0; j<(int)scene.size(); j++) {
        for (int i=0; i<(int)scene[j].size(); i++) {
          char block = scene[j][i];

          if (block == '.') {
            g->SetColor(jgui::jcolor_name_t::Red);
            g->FillRectangle({i*BLOCK_SIZE.width - offset.x, j*BLOCK_SIZE.height - offset.y, BLOCK_SIZE});
          } else if (block == '#') {
            g->SetColor(jgui::jcolor_name_t::Blue);
            g->FillRectangle({i*BLOCK_SIZE.width - offset.x, j*BLOCK_SIZE.height - offset.y, BLOCK_SIZE});
          }
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

