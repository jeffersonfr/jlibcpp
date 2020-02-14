#include "jgui/japplication.h"
#include "jgui/jwindow.h"

class Menu {

  private:
    std::map<std::string, Menu> _items;
    int _index;
    int _id;
    bool _enabled;

  public:
    Menu()
    {
      _index = 0;
      _id = 0;
      _enabled = true;
    }

    ~Menu()
    {
    }

    bool HasChilds()
    {
      return _items.size() != 0;
    }

    int Count()
    {
      return _items.size();
    }

    int Index()
    {
      return _index;
    }

    void Index(int param)
    {
      if (param > ((int)_items.size() - 1)) {
        param = 0;
      }

      if (param < 0) {
        param = (int)_items.size() - 1;
      }

      _index = param;
    }

    Menu & Current()
    {
      int i = 0;

      for (auto &item : _items) {
        if (i++ == _index) {
          return item.second;
        }
      }

      return *this;
    }

    int Id()
    {
      return _id;
    }

    bool Enabled()
    {
      return _enabled;
    }

    Menu & Id(int param)
    {
      _id = param;

      return *this;
    }

    Menu & Enabled(bool param)
    {
      _enabled = param;

      return *this;
    }

    Menu & operator[](const std::string &key)
    {
      return _items[key];
    }

    void Paint(jgui::Graphics *g)
    {
      jgui::jpoint_t<int> offset {
        .x = 8,
        .y = 8
      };

      g->SetColor(jgui::jcolor_name_t::Orange);
      g->FillRectangle({0, 0, 100, 100});
      g->SetColor(jgui::jcolor_name_t::White);
      g->DrawRectangle({0, 0, 100, 100});

      int index = 0;

      for (auto &item : _items) {
        if (index++ == _index) {
          g->SetColor(jgui::jcolor_name_t::White);
          
          if (item.second.Enabled() == false) {
            g->SetColor(jgui::jcolor_name_t::LightGrey);
          }
        } else {
          g->SetColor(jgui::jcolor_name_t::Blue);
          
          if (item.second.Enabled() == false) {
            g->SetColor(jgui::jcolor_name_t::Grey);
          }
        }


        g->DrawString(item.first, offset);

        if (item.second.HasChilds() == true) {
          jgui::jpoint_t p = offset + jgui::jpoint_t<int>{100 - 16 - 8, 0};

          g->SetColor(jgui::jcolor_name_t::Black);
          g->FillTriangle({p.x, p.y}, {p.x + 8, p.y + 8}, {p.x, p.y + 16});
        }

        offset.y = offset.y + 16;
      }
    }

};

class MenuManager : public jgui::Component {

  private:
    std::vector<Menu> _menus;

  public:

    MenuManager()
    {
    }

    void Push(Menu &menu) 
    {
      _menus.push_back(menu);
    }
    
    void Pop() 
    {
      if (_menus.size() > 0) {
        _menus.erase(_menus.begin() + _menus.size() - 1);
      }
    }

    void Close()
    {
      while (_menus.size() > 0) {
        Pop();
      }
    }

    virtual bool KeyPressed(jevent::KeyEvent *event)
    {
      if (_menus.size() == 0) {
        return false;
      }

      Menu &menu = *_menus.rbegin();

      if (event->GetSymbol() == jevent::jkeyevent_symbol_t::JKS_ENTER or event->GetSymbol() == jevent::jkeyevent_symbol_t::JKS_CURSOR_RIGHT) {
        Menu &submenu = menu.Current();

        if (submenu.Enabled() == true) {
          if (submenu.HasChilds() == true) {
            submenu.Index(0);

            Push(submenu);
          } else {
            // dispatch id

            Close();
          }
        }
      } else if (event->GetSymbol() == jevent::jkeyevent_symbol_t::JKS_ESCAPE or event->GetSymbol() == jevent::jkeyevent_symbol_t::JKS_CURSOR_LEFT) {
        Pop();
      } else if (event->GetSymbol() == jevent::jkeyevent_symbol_t::JKS_m) {
        Close();
      } else if (event->GetSymbol() == jevent::jkeyevent_symbol_t::JKS_CURSOR_UP) {
        menu.Index(menu.Index() - 1);
      } else if (event->GetSymbol() == jevent::jkeyevent_symbol_t::JKS_CURSOR_DOWN) {
        menu.Index(menu.Index() + 1);
      }
      
      Repaint();

      return true;
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      jgui::jpoint_t<int> offset {
        .x = 16,
        .y = 16
      };

      for (auto &menu : _menus) {
        g->Translate(offset);
        menu.Paint(g);
        g->Translate(-offset);

        offset = offset + jgui::jpoint_t<int>{16, 16};
      }
    }

};

class Dummy : public jgui::Window {

  private:
    MenuManager _manager;
    Menu _menu;

  public:
    Dummy():
      jgui::Window(jgui::jsize_t<int>{740, 480})
    {
      _menu["main"]["item1"];
      _menu["main"]["item2"].Id(100);
      _menu["main"]["item3"].Id(101);
      _menu["main"]["item4"].Id(102);

      _menu["main"]["item1"]["item11"].Enabled(false);
      _menu["main"]["item1"]["item12"];
      _menu["main"]["item1"]["item13"];
      _menu["main"]["item1"]["item14"];

      _menu["main"]["item2"]["item21"];
      _menu["main"]["item2"]["item22"];
      _menu["main"]["item2"]["item23"];
      _menu["main"]["item2"]["item24"];

      _menu["main"]["item2"]["item22"]["item221"];
      _menu["main"]["item2"]["item22"]["item222"];
      _menu["main"]["item2"]["item22"]["item223"];

      _manager.Push(_menu["main"]);

      Add(&_manager);

      _manager.RequestFocus();
    }

    virtual ~Dummy()
    {
    }

    virtual bool KeyPressed(jevent::KeyEvent *event)
    {
      if (jgui::Window::KeyPressed(event) == true) {
        return true;
      }

      if (event->GetSymbol() == jevent::jkeyevent_symbol_t::JKS_m) {
        _manager.Push(_menu["main"]);

        Repaint();
      }

      return false;
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      jgui::Window::Paint(g);

      _manager.Paint(g);
    }

};

int main(int argc, char *argv[])
{
  jgui::Application::Init(argc, argv);

  Dummy app;

  jgui::Application::Loop();

  return 0;
}

