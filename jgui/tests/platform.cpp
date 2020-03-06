#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jbufferedimage.h"
#include "jmath/jvector.h"

#include <typeinfo>
#include <cctype>

#include <cxxabi.h>

// s<cammelCase>: static variable
// g<cammelCase>: global variable
// sg<cammelCase>: static global variable
// _<cammelCase>: internal class members
// <cammelCase>: local variables and functions

#define Log() std::cout << "[" << __FILE__ << ":" << __LINE__ << "] " << __PRETTY_FUNCTION__ << std::endl;

#define TypeName(object) std::string(abi::__cxa_demangle(typeid(object).name(), 0, 0, 0))

const jgui::jsize_t<int> sgScreenSize = {
  .width = 640,
  .height = 480
};

std::mutex sgMutex;

class AssetsManager {

  private:
    std::map<std::string, jgui::Image *> _images;

  public:
    static AssetsManager & Instance()
    {
      static AssetsManager instance;

      return instance;
    }

    jgui::Image * Load(std::string path)
    {
      auto pair = _images.find(path);
      
      if (pair  != _images.end()) {
        return pair->second;
      }

      jgui::Image *image = new jgui::BufferedImage(path);

      _images[path] = image;

      return image;
    }

    void Release()
    {
      for (auto &pair : _images) {
        delete pair.second;
      }

      _images.clear();
    }

};

class Entity;

class Component {

  private:
    Entity *_owner;

  public:
    Component()
    {
    }

    virtual ~Component()
    {
    }

    void Owner(Entity *owner)
    {
      _owner = owner;
    }

    Entity * Owner()
    {
      return _owner;
    }

    virtual void Initialize()
    {
    }

    virtual void Update(float tick) = 0;
    
    virtual void Render(jgui::Graphics *g) = 0;

    friend std::ostream & operator<<(std::ostream& out, const Component &component)
    {
      out << "\t\tComponent<" << TypeName(component) << ">";

      return out;
    }

};

class Entity : public Component {

  private:
    std::vector<Component *> 
      _components;
    std::string 
      _id;

  public:
    Entity(std::string id):
      Component(),
      _id(id)
    {
    }

    virtual ~Entity()
    {
      for (int i=0; i<(int)_components.size(); i++) {
        delete _components[i];
      }

      _components.clear();
    }

    virtual void Update(float tick) override
    {
      for (auto &component : _components) {
        component->Update(tick);
      }
    }
    
    virtual void Render(jgui::Graphics *g) override
    {
      for (auto &component : _components) {
        component->Render(g);
      }
    }

    std::string Id()
    {
      return _id;
    }

    template<typename T, typename ...Args> T * Create(Args &&...args)
    {
      T *t = new T(std::forward<Args>(args)...);

      sgMutex.lock();

      _components.push_back(t);

      sgMutex.unlock();

      t->Owner(this);
      t->Initialize();

      return t;
    }

    template<typename T> T * GetComponent()
    {
      std::string id = TypeName(T);

      for (auto component : _components) {
        std::string cid = TypeName(*component);

        if (id == cid) { // INFO:: verify if the component T exists
          return (T *)component;
        }

        T *dyn = dynamic_cast<T *>(component);

        if (dyn != nullptr) { // INFO:: verify if some of the components inherit the component T
          std::string cid = TypeName(dyn);

          cid.pop_back(); // INFO:: remove the '*' at the last position of string

          if (id == cid) {
            return (T *)component;
          }
        }
      }

      throw std::runtime_error(std::string("There isn't a(n) ") + TypeName(T) + " registered in entity");
    }

    template<typename T> void RemoveComponent()
    {
      std::string id = TypeName(T);
      
      sgMutex.lock();

      _components.erase(std::remove_if(_components.begin(), _components.end(), [id=id] (const Component *param) { 
            std::string cid = TypeName(*param);

            if (id == cid) { // INFO:: verify if the component T exists
              return true;
            }

            return false;
      }), _components.end());

      sgMutex.unlock();
    }

    template<typename T> bool Exists()
    {
      try {
        GetComponent<T>();

        return true;
      } catch (std::runtime_error &e) {
        return false;
      }
    }

    std::vector<Component *> & ListComponents()
    {
      return _components;
    }

    friend std::ostream & operator<<(std::ostream& out, Entity &param)
    {
      out << "\tEntity<" << TypeName(param) << "> "<< param.Id() << "\n";

      for (auto &component : param.ListComponents()) {
        out << *component << "\n";
      }

      return out;
    }

};

class StaticComponent : public Component {

  public:
    jgui::jpoint_t<float>
      pos;
    float
      scale,
      radians;

  public:
    StaticComponent(jgui::jpoint_t<float> pos, float scale = 1.0f, float radians = 0.0f):
      Component(),
      pos(pos),
      scale(scale),
      radians(radians)
    {
    }

    ~StaticComponent()
    {
    }

    virtual void Update(float tick) override
    {
    }
    
    virtual void Render(jgui::Graphics *g) override
    {
    }

};

class TransformComponent : public StaticComponent {

  public:
    jgui::jpoint_t<float>
      vel;

  public:
    TransformComponent(jgui::jpoint_t<float> pos, jgui::jpoint_t<float> vel, float scale = 1.0f, float radians = 0.0f):
      StaticComponent(pos, scale, radians),
      vel(vel)
    {
    }

    ~TransformComponent()
    {
    }

    virtual void Update(float tick) override
    {
      pos = pos + vel*tick;
    }
    
    virtual void Render(jgui::Graphics *g) override
    {
    }

};

class SpriteComponent : public Component {

  protected:
    jgui::Image 
      *_image;
    jgui::jrect_t<int>
      _dst;

  public:
    jgui::jrect_t<int>
      src;

  public:
    SpriteComponent(jgui::Image *image):
      Component(),
      _image(image)
    {
      src = jgui::jrect_t<int>{{0, 0}, _image->GetSize()};
    }

    ~SpriteComponent()
    {
    }

    virtual void Update(float tick) override
    {
      StaticComponent *transform = Owner()->GetComponent<StaticComponent>();

      _dst = jgui::jrect_t<int>{transform->pos, src.size*transform->scale};
    }
    
    virtual void Render(jgui::Graphics *g) override
    {
      StaticComponent *transform = Owner()->GetComponent<StaticComponent>();

      if (transform->radians != 0.0f) {
        g->DrawImage(_image, src, _dst);
      } else {
        jgui::Image *rotate = _image->Rotate(transform->radians);

        g->DrawImage(rotate, src, _dst);

        delete rotate;
      }
    }

};

class AnimatedSpriteComponent : public SpriteComponent {

  private:
    int
      _rows,
      _cols,
      _refRow;
    int
      _index;

  public:
    AnimatedSpriteComponent(jgui::Image *image, int rows, int cols, int refRow):
      SpriteComponent(image),
      _rows(rows),
      _cols(cols),
      _refRow(refRow)
    {
      src = jgui::jrect_t<int>{{0, 0}, _image->GetSize()/jgui::jsize_t<int>{cols, rows}};
    }

    ~AnimatedSpriteComponent()
    {
    }

    virtual void Update(float tick) override
    {
      static float count = 0.0f;

      SpriteComponent::Update(tick);

      _index = int(count*16)%_cols;
      count = count + tick;
    }
    
    virtual void Render(jgui::Graphics *g) override
    {
      StaticComponent *transform = Owner()->GetComponent<StaticComponent>();

      if (transform->radians != 0.0f) {
        g->DrawImage(_image, src + jgui::jpoint_t<int>{_index*src.size.width, _refRow*src.size.height}, _dst);
      } else {
        jgui::Image *rotate = _image->Rotate(transform->radians);

        g->DrawImage(rotate, src + jgui::jpoint_t<int>{_index*src.size.width, _refRow*src.size.height}, _dst);

        delete rotate;
      }
    }

};

class TrembleComponent : public Component {

  private:
    float
      _oldScale;

  public:
    TrembleComponent():
      Component()
    {
    }

    ~TrembleComponent()
    {
      StaticComponent *transform = Owner()->GetComponent<StaticComponent>();

      transform->scale = _oldScale;
    }

    virtual void Initialize() override
    {
      StaticComponent *transform = Owner()->GetComponent<StaticComponent>();

      _oldScale = transform->scale;
    }

    virtual void Update(float tick) override
    {
      static float count = 0.0f;

      StaticComponent *transform = Owner()->GetComponent<StaticComponent>();

      if (int(count*16)%2) {
        transform->scale = 1.0f + float(random()%16)/100.0f;
      }

      count = count + tick;
    }
    
    virtual void Render(jgui::Graphics *g) override
    {
    }

};

class EntityManager : public Component {

  private:
    std::vector<Entity *> _entities;

  public:
    EntityManager():
      Component()
    {
    }

    ~EntityManager()
    {
    }

    virtual void Initialize() override
    {
    }

    virtual void Update(float tick) override
    {
      for (auto &entity : _entities) {
        entity->Update(tick);
      }
    }
    
    virtual void Render(jgui::Graphics *g) override
    {
      for (auto &entity : _entities) {
        entity->Render(g);
      }
    }

    Entity & Create(std::string id)
    {
      std::transform(id.begin(), id.end(), id.begin(),
        [] (uint8_t c) {
          return std::tolower(c); 
        }
      );

      Entity *entity = new Entity(id);

      sgMutex.lock();
      
      _entities.push_back(entity);
      
      sgMutex.unlock();

      entity->Initialize();

      return *entity;
    }

    void DestroyEntities()
    {
      for (int i=0; i<(int)_entities.size(); i++) {
        delete _entities[i];
      }

      _entities.clear();
    }

    Entity * GetEntity(std::string id)
    {
      for (auto entity : _entities) {
        if (id == entity->Id()) {
          return entity;
        }
      }

      return nullptr;
    }

    std::vector<Entity *> & ListEntities()
    {
      return _entities;
    }

    friend std::ostream & operator<<(std::ostream& out, EntityManager &param)
    {
      out << "EntityManager<" << TypeName(param) << ">\n";

      for (auto &entity : param.ListEntities()) {
        out << *entity << "";
      }

      return out;
    }
};

class Game : public jgui::Window {

  private:
    EntityManager
      _entityManager;
    std::map<jevent::jkeyevent_symbol_t, bool> 
      _keys;
    std::chrono::steady_clock::time_point
      _ticks;

  private:
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
      static auto old = std::chrono::steady_clock::now();
      
      auto 
        now = std::chrono::steady_clock::now();

      jgui::Window::Paint(g);

      g->SetBlittingFlags(jgui::JBF_NEAREST);

      sgMutex.lock();

      // ProcessInput();
      _entityManager.Update(std::chrono::duration<float>(now - old).count());
      _entityManager.Render(g);

      sgMutex.unlock();

      Repaint();
      
      old = now;
    }

  public:
    Game(jgui::jsize_t<int> size):
      jgui::Window(size)
    {
      srand(time(nullptr));

      _ticks = std::chrono::steady_clock::now();

      _entityManager.Initialize();
      
      SetFramesPerSecond(60);
    }

    virtual ~Game()
    {
    }

    void LoadLevel(int level)
    {
      _entityManager.DestroyEntities();

      if (level == 0) { // first level .. only a test
        // INFO:: enemies
        for (int i=0; i<12; i++) {
          Entity 
            &entity = _entityManager.Create("enemy");

          if (random()%2) {
            entity.Create<TransformComponent>(
                jgui::jpoint_t<long>{random()%sgScreenSize.width, random()%sgScreenSize.height}, 
                jgui::jpoint_t<long>{random()%64 - 32, random()%64 - 32}, 1.0f, M_PI/3.0f);
          } else {
            entity.Create<StaticComponent>(
                jgui::jpoint_t<long>{random()%sgScreenSize.width, random()%sgScreenSize.height});
          }

          entity.Create<SpriteComponent>(
              AssetsManager::Instance().Load("assets/images/tank-big-left.png"));
        }

        // INFO:: chopper
        Entity 
          &entity = _entityManager.Create("player");

        entity.Create<StaticComponent>(
            jgui::jpoint_t<long>{random()%sgScreenSize.width, random()%sgScreenSize.height}, 
            1.0f, M_PI/3.0f);

        entity.Create<AnimatedSpriteComponent>(
            AssetsManager::Instance().Load("assets/images/chopper-spritesheet.png"), 4, 2, 1);

        entity.RemoveComponent<StaticComponent>();

        entity.Create<TransformComponent>(
            jgui::jpoint_t<long>{random()%sgScreenSize.width, random()%sgScreenSize.height}, 
            jgui::jpoint_t<long>{random()%64 - 32, random()%64 - 32}, 2.0f);

        entity.Create<TrembleComponent>();

        std::cout << _entityManager << std::endl;
      }
    }

    bool IsPressed(jevent::jkeyevent_symbol_t key)
    {
      return _keys[key];
    }

};

int main(int argc, char *argv[])
{
  jgui::Application::Init(argc, argv);

  Game app(sgScreenSize);

  app.LoadLevel(0);

  jgui::Application::Loop();

  return 0;
}

