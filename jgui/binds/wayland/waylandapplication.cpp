/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "binds/include/nativewindow.h"
#include "binds/wayland/waylandapplication.h"

#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>
#include <mutex>
#include <atomic>

namespace jgui {

/** \brief */
static struct display _display;
/** \brief */
static ShmSurface *sg_surface = nullptr;
/** \brief */
static struct input _input;
/** \brief */
static bool _input_inited;
/** \brief */
static jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static jgui::jrect_t<int> sg_previous_bounds;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;

static struct name_keyCode name_keyCode_map[] = {
  {"Return",      jevent::JKS_ENTER},
  {"KP_Enter",    jevent::JKS_ENTER},
  {"BackSpace",   jevent::JKS_BACKSPACE},
  {"Tab",         jevent::JKS_TAB},
  {"Cancel",      jevent::JKS_CANCEL},
  // {"Clear",       jevent::JKS_CLEAR},
  {"Shift_L",     jevent::JKS_SHIFT},
  {"Shift_R",     jevent::JKS_SHIFT},
  {"Control_L",   jevent::JKS_CONTROL},
  {"Control_R",   jevent::JKS_CONTROL},
  {"Alt_L",       jevent::JKS_ALT},
  {"Alt_R",       jevent::JKS_ALT},
  {"Pause",       jevent::JKS_PAUSE},
  // {"Caps_Lock",   jevent::JKS_CAPS_LOCK},
  {"Escape",      jevent::JKS_ESCAPE},
  {"Prior",       jevent::JKS_PAGE_UP},
  {"KP_Prior",    jevent::JKS_PAGE_UP},
  {"Next",        jevent::JKS_PAGE_DOWN},
  {"KP_Next",     jevent::JKS_PAGE_DOWN},
  {"End",         jevent::JKS_END},
  {"KP_End",      jevent::JKS_END},
  {"Home",        jevent::JKS_HOME},
  {"KP_Home",     jevent::JKS_HOME},
  {"Left",        jevent::JKS_CURSOR_LEFT},
  {"KP_Left",     jevent::JKS_CURSOR_LEFT},
  {"Up",          jevent::JKS_CURSOR_UP},
  {"KP_Up",       jevent::JKS_CURSOR_UP},
  {"Right",       jevent::JKS_CURSOR_RIGHT},
  {"KP_Right",    jevent::JKS_CURSOR_RIGHT},
  {"Down",        jevent::JKS_CURSOR_DOWN},
  {"KP_Down",     jevent::JKS_CURSOR_DOWN},
  {"comma",       jevent::JKS_COMMA},
  {"minus",       jevent::JKS_MINUS_SIGN},
  {"KP_Subtract", jevent::JKS_MINUS_SIGN},
  {"period",      jevent::JKS_PERIOD},
  {"KP_Decimal",  jevent::JKS_PERIOD},
  {"slash",       jevent::JKS_SLASH},
  {"space",       jevent::JKS_SPACE},
  {"0",           jevent::JKS_0},
  {"KP_0",        jevent::JKS_0},
  {"1",           jevent::JKS_1},
  {"KP_1",        jevent::JKS_1},
  {"2",           jevent::JKS_2},
  {"KP_2",        jevent::JKS_2},
  {"3",           jevent::JKS_3},
  {"KP_3",        jevent::JKS_3},
  {"4",           jevent::JKS_4},
  {"KP_4",        jevent::JKS_4},
  {"5",           jevent::JKS_5},
  {"KP_5",        jevent::JKS_5},
  {"6",           jevent::JKS_6},
  {"KP_6",        jevent::JKS_6},
  {"7",           jevent::JKS_7},
  {"KP_7",        jevent::JKS_7},
  {"8",           jevent::JKS_8},
  {"KP_8",        jevent::JKS_8},
  {"9",           jevent::JKS_9},
  {"KP_9",        jevent::JKS_9},
  {"semicolon",   jevent::JKS_SEMICOLON},
  {"equal",       jevent::JKS_EQUALS_SIGN},
  {"a",           jevent::JKS_a},
  {"A",           jevent::JKS_a},
  {"b",           jevent::JKS_b},
  {"B",           jevent::JKS_b},
  {"c",           jevent::JKS_c},
  {"C",           jevent::JKS_c},
  {"d",           jevent::JKS_d},
  {"D",           jevent::JKS_d},
  {"e",           jevent::JKS_e},
  {"E",           jevent::JKS_e},
  {"f",           jevent::JKS_f},
  {"F",           jevent::JKS_f},
  {"g",           jevent::JKS_g},
  {"G",           jevent::JKS_g},
  {"h",           jevent::JKS_h},
  {"H",           jevent::JKS_h},
  {"i",           jevent::JKS_i},
  {"I",           jevent::JKS_i},
  {"j",           jevent::JKS_j},
  {"J",           jevent::JKS_j},
  {"k",           jevent::JKS_k},
  {"K",           jevent::JKS_k},
  {"l",           jevent::JKS_l},
  {"L",           jevent::JKS_l},
  {"m",           jevent::JKS_m},
  {"M",           jevent::JKS_m},
  {"n",           jevent::JKS_n},
  {"N",           jevent::JKS_n},
  {"o",           jevent::JKS_o},
  {"O",           jevent::JKS_o},
  {"p",           jevent::JKS_p},
  {"P",           jevent::JKS_p},
  {"q",           jevent::JKS_q},
  {"Q",           jevent::JKS_q},
  {"r",           jevent::JKS_r},
  {"R",           jevent::JKS_r},
  {"s",           jevent::JKS_s},
  {"S",           jevent::JKS_s},
  {"t",           jevent::JKS_t},
  {"T",           jevent::JKS_t},
  {"u",           jevent::JKS_u},
  {"U",           jevent::JKS_u},
  {"v",           jevent::JKS_v},
  {"V",           jevent::JKS_v},
  {"w",           jevent::JKS_w},
  {"W",           jevent::JKS_w},
  {"x",           jevent::JKS_x},
  {"X",           jevent::JKS_x},
  {"y",           jevent::JKS_y},
  {"Y",           jevent::JKS_y},
  {"z",           jevent::JKS_z},
  {"Z",           jevent::JKS_z},
  {"bracketleft", jevent::JKS_SQUARE_BRACKET_LEFT},
  {"backslash",   jevent::JKS_BACKSLASH},
  {"bracketright",jevent::JKS_SQUARE_BRACKET_RIGHT},
  {"asterisk",    jevent::JKS_STAR},
  {"KP_Multiply", jevent::JKS_STAR},
  {"plus",        jevent::JKS_PLUS_SIGN},
  {"KP_Add",      jevent::JKS_PLUS_SIGN},
  {"KP_Divide",   jevent::JKS_SLASH},
  {"Delete",      jevent::JKS_DELETE},
  {"KP_Delete",   jevent::JKS_DELETE},
  // {"Num_Lock",    jevent::JKS_NUM_LOCK},
  // {"Scroll_Lock", jevent::JKS_SCROLL_LOCK},
  {"F1",          jevent::JKS_F1},
  {"F2",          jevent::JKS_F2},
  {"F3",          jevent::JKS_F3},
  {"F4",          jevent::JKS_F4},
  {"F5",          jevent::JKS_F5},
  {"F6",          jevent::JKS_F6},
  {"F7",          jevent::JKS_F7},
  {"F8",          jevent::JKS_F8},
  {"F9",          jevent::JKS_F9},
  {"F10",         jevent::JKS_F10},
  {"F11",         jevent::JKS_F11},
  {"F12",         jevent::JKS_F12},
  // {"F13",         jevent::JKS_F13},
  // {"F14",         jevent::JKS_F14},
  // {"F15",         jevent::JKS_F15},
  // {"F16",         jevent::JKS_F16},
  // {"F17",         jevent::JKS_F17},
  // {"F18",         jevent::JKS_F18},
  // {"F19",         jevent::JKS_F19},
  // {"F20",         jevent::JKS_F20},
  // {"F21",         jevent::JKS_F21},
  // {"F22",         jevent::JKS_F22},
  // {"F23",         jevent::JKS_F23},
  // {"F24",         jevent::JKS_F24},
  {"Insert",      jevent::JKS_INSERT},
  // {"Help",        jevent::JKS_HELP},
  {"grave",       jevent::JKS_GRAVE_ACCENT},
  {"apostrophe",  jevent::JKS_APOSTROPHE},
  {"bar",         jevent::JKS_VERTICAL_BAR},
  {"ampersand",   jevent::JKS_AMPERSAND},
  {"quotedbl",    jevent::JKS_QUOTATION},
  {"less",        jevent::JKS_LESS_THAN_SIGN},
  {"greater",     jevent::JKS_GREATER_THAN_SIGN},
  // {"braceleft",   jevent::JKS_BRACELEFT},
  // {"braceright",  jevent::JKS_BRACERIGHT},
  {"at",          jevent::JKS_AT},
  {"colon",       jevent::JKS_COLON},
  {"asciicircum", jevent::JKS_CIRCUMFLEX_ACCENT},
  {"dollar",      jevent::JKS_DOLLAR_SIGN},
  {"exclam",      jevent::JKS_EXCLAMATION_MARK},
  {"parenleft",   jevent::JKS_PARENTHESIS_LEFT},
  {"numbersign",  jevent::JKS_NUMBER_SIGN},
  {"parenright",  jevent::JKS_PARENTHESIS_RIGHT},
  {"underscore",  jevent::JKS_UNDERSCORE},
  {"percent",     jevent::JKS_5},
  {"asciitilde",  jevent::JKS_APOSTROPHE},
  {"question",    jevent::JKS_SLASH},
  {nullptr, jevent::JKS_UNKNOWN}
};

static void display_handle_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y, int32_t w, int32_t h, int32_t subpixel, const char *make, const char *model, int32_t transform) 
{
  printf("display_handle_geometry:: %dx%d+%d+%d, subpixel: %d, model: %s, transform: %s\n", x, y, w, h, subpixel, make, model);
}

static void display_handle_mode(void *data, struct wl_output *wl_output, uint32_t flags, int32_t w, int32_t h, int32_t refresh) 
{
  _display.width = w;
  _display.height = h;
}

static const struct wl_output_listener output_listener = {
  display_handle_geometry,
  display_handle_mode
};

bool init_input(struct input* input);

static void registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version) 
{
  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    _display.compositor = (wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, WL_VERSION);
  } else if (strcmp(interface, wl_shell_interface.name) == 0) {
    _display.shell = (wl_shell*)wl_registry_bind(registry, id, &wl_shell_interface, WL_VERSION);
  } else if (strcmp(interface, wl_output_interface.name) == 0) {
    _display.output = (wl_output*)wl_registry_bind(registry, id, &wl_output_interface, WL_VERSION);
    wl_output_add_listener(_display.output, &output_listener, NULL);
  } else if (strcmp(interface, wl_shm_interface.name) == 0) {
    _display.shm = (wl_shm*)wl_registry_bind(registry, id, &wl_shm_interface, WL_VERSION);
  } else if (strcmp(interface, wl_subcompositor_interface.name) == 0) {
    _display.subcompositor = (wl_subcompositor*)wl_registry_bind(registry, id, &wl_subcompositor_interface, WL_VERSION);
  } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    _input.seat = (wl_seat*)wl_registry_bind(registry, id, &wl_seat_interface, 2);
    _input.display = _display.display;

    if (!init_input(&_input)) {
      fprintf(stderr, "Failed to initialize input\n");
      abort();
    } else {
      _input_inited = true;
    }
  }
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t id) 
{
}

static const struct wl_registry_listener registry_listener = {
  registry_handler,
  registry_remover
};

bool wayland_init() 
{
  _display.display = wl_display_connect(NULL);

  if (_display.display == NULL) {
    return false;
  }

  _display.registry = wl_display_get_registry(_display.display);

  if (_display.registry == NULL) {
    return false;
  }

  wl_registry_add_listener(_display.registry, &registry_listener, NULL);

  wl_display_dispatch(_display.display);
  wl_display_roundtrip(_display.display);

  return true;
}

bool is_input_inited() 
{
  return _input_inited;
}

static void unwatch_fd(struct input* input, int fd) {
  epoll_ctl(input->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void cleanup_event(struct input* input) {
  unwatch_fd(input, input->repeat_timer_fd);
  unwatch_fd(input, wl_display_get_fd(input->display));

  close(input->epoll_fd);
}

struct input * get_input()
{
  return &_input;
}

Event* next_event() 
{
  struct input* input = get_input();
  Event* event = NULL;
  
  pthread_mutex_lock(&input->lock);
  
  if (!wl_list_empty(&input->event_list)) {
    wl_list_for_each_reverse(event, &input->event_list, link) {
      wl_list_remove(&event->link);
  
      break;
    }
  }

  pthread_mutex_unlock(&input->lock);

  return event;
}

void drain_events()
{
  Event* event;

  while((event = next_event()) != NULL) {
    free(event);
  }
}

void destroy_input(struct input* input)
{
  cleanup_event(input);
  close(input->repeat_timer_fd);

  drain_events();

  if (input->touch != NULL) {
    wl_touch_release(input->touch);
  }

  if (input->pointer != NULL) {
    wl_pointer_release(input->pointer);
  }

  if (input->keyboard != NULL) {
    wl_keyboard_release(input->keyboard);
  }
}

void wayland_cleanup() 
{
  if (_display.compositor != NULL) {
    wl_compositor_destroy(_display.compositor);
  }

  if (_display.subcompositor != NULL) {
    wl_subcompositor_destroy(_display.subcompositor);
  }

  if (_display.shell != NULL) {
    wl_shell_destroy(_display.shell);
  }

  if (_display.shm != NULL) {
    wl_shm_destroy(_display.shm);
  }

  if (_display.output != NULL) {
    wl_output_destroy(_display.output);
  }

  destroy_input(&_input);

  if (_display.display != NULL) {
    wl_display_disconnect(_display.display);
  }
}

int32_t get_display_height() 
{
  return _display.height;
}

int32_t get_display_width() 
{
  return _display.width;
}

void display_flush()
{
  wl_display_dispatch_pending(_display.display);
  wl_display_flush(_display.display);
}

struct display * get_display() 
{ 
  return &_display; 
}

static void shell_surface_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial) 
{
  wl_shell_surface_pong(shell_surface, serial);
}

static void shell_surface_configure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height) 
{
}

static struct wl_shell_surface_listener shell_surface_listener = {
  .ping = shell_surface_ping,
  .configure = shell_surface_configure
};

static struct wl_surface* make_surface() {
  struct display* display = get_display();

  return wl_compositor_create_surface(display->compositor);
}

static struct wl_shell_surface* make_shell_surface(struct wl_surface* surface)
{
  struct display* display = get_display();
  struct wl_shell_surface* shell_surface = wl_shell_get_shell_surface(display->shell, surface);

  if (shell_surface == NULL) {
    return NULL;
  }

  wl_shell_surface_add_listener(shell_surface, &shell_surface_listener, NULL);
  wl_shell_surface_set_toplevel(shell_surface);

  return shell_surface;
}

static struct wl_shm_pool* make_shm_pool(int32_t width, int32_t height, int32_t pixel_depth, void** addr) 
{
  struct wl_shm_pool  *pool;
  char tmp[] = "/tmp/wayland_mmap_XXXXXX";
  int fd;

  int32_t stride = width * pixel_depth;
  int32_t size = stride * height;

  fd = mkostemp(tmp, O_CLOEXEC); //, O_RDWR | O_CREAT | O_TRUNC);
  
  if (fd < 0) {
    return NULL;
  }

  if (ftruncate(fd, (size_t)size) < 0) {
    return NULL;
  }

  void* map = mmap(NULL, (size_t)size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
  if (map == MAP_FAILED) {
    close(fd);
    return NULL;
  }

  *addr = map;

  pool = wl_shm_create_pool(get_display()->shm, fd, size);

  return pool;
}

static struct wl_buffer* make_buffer(struct wl_shm_pool *pool, int32_t width, int32_t height, uint32_t format, int32_t pixel_depth) 
{
  return wl_shm_pool_create_buffer(pool, 0, width, height, width * pixel_depth, format);
}

ShmSurface* CreateShmScreenSurface(int64_t id, int32_t x, int32_t y, int32_t width, int32_t height, int32_t pixel_depth) 
{
  struct wl_surface*          surface;
  struct wl_shell_surface*    shell_surface;

  struct wl_shm_pool*         pool;
  struct wl_buffer*           buffer;
  void*                       content;

  surface = make_surface();
  
  if (surface == NULL) {
    return NULL;
  }

  shell_surface = make_shell_surface(surface);
  
  if (shell_surface == NULL) {
    wl_surface_destroy(surface);
  
    return NULL;
  }

  pool = make_shm_pool(get_display_width(), get_display_height(), pixel_depth, &content);
  
  if (pool == NULL) {
    wl_surface_destroy(surface);
    wl_shell_surface_destroy(shell_surface);
  
    return NULL;
  }

  buffer = make_buffer(pool, width, height, WL_SHM_FORMAT_XRGB8888, pixel_depth);
 
  if (buffer == NULL) {
    wl_shm_pool_destroy(pool);
    wl_surface_destroy(surface);
    wl_shell_surface_destroy(shell_surface);
  
    return NULL;
  }

  struct shm_surface* ssf = (struct shm_surface*)malloc(sizeof(struct shm_surface));
  
  if (ssf == NULL) {
    wl_buffer_destroy(buffer);
    wl_shm_pool_destroy(pool);
    wl_surface_destroy(surface);
    wl_shell_surface_destroy(shell_surface);
  
    return NULL;
  }

  memset((void*)ssf, 0, sizeof(struct shm_surface));

  ssf->surface = surface;
  ssf->shell_surface = shell_surface;
  ssf->buffer = buffer;
  ssf->pool = pool;

  ssf->content = content;
  ssf->id = id;
  ssf->x = x;
  ssf->y = y;
  ssf->width = width;
  ssf->height = height;
  ssf->pixel_depth = pixel_depth;
  ssf->format = WL_SHM_FORMAT_XRGB8888;

  wl_surface_set_user_data(surface, ssf);

  wl_surface_attach(surface, buffer, 0, 0);
  wl_surface_commit(surface);

  return ssf;
}

void DestroyShmScreenSurface(ShmSurface* surf) 
{
  struct input* input = get_input();
  
  input->activeSurface = NULL;

  wl_buffer_destroy(surf->buffer);
  wl_shm_pool_destroy(surf->pool);
  wl_surface_destroy(surf->surface);
  wl_shell_surface_destroy(surf->shell_surface);

  free(surf);
}

void UnmapShmScreenSurface(ShmSurface* surf) 
{
  wl_surface_attach(surf->surface, NULL, 0, 0);
  wl_surface_commit(surf->surface);
  wl_buffer_destroy(surf->buffer);
  surf->buffer = NULL;
  surf->width = 0;
  surf->height = 0;
}

bool RemapShmScreenSurface(ShmSurface* surface, int32_t width, int32_t height) 
{
  struct wl_buffer* buffer = make_buffer(surface->pool, width, height, WL_SHM_FORMAT_XRGB8888, surface->pixel_depth);
  
  if (buffer == NULL) {
    return false;
  }
 
  surface->width = width;
  surface->height = height;

  wl_surface_attach(surface->surface, buffer, 0, 0);
  wl_surface_damage(surface->surface, 0, 0, width, height);
  wl_surface_commit(surface->surface);
  surface->buffer = buffer;
  
  return true;
}

void ResizeShmScreenSurface(ShmSurface* surf, int32_t width, int32_t height) 
{
  UnmapShmScreenSurface(surf);
  RemapShmScreenSurface(surf, width, height);
}

uint32_t find_keycode_by_name(const char* name)
{
  struct name_keyCode* map = name_keyCode_map;
  
  for (; map->name != NULL; map ++) {
    if (strcmp(map->name, name) == 0) {
      return (uint32_t)map->code;
    }
  }
  
  return 0;
}

static void append_event(Event* event)
{
  struct input* input = get_input();
  
  event->modifiers = input->modifiers;
  
  pthread_mutex_lock(&input->lock);
  
  wl_list_insert(&input->event_list, &event->link);
  
  pthread_mutex_unlock(&input->lock);
}

void new_mouse_event(enum MouseAction action, long id, int32_t x, int32_t y, uint32_t button, uint32_t click_count)
{
  Event* event = (Event*)malloc(sizeof(Event));

  if (event != NULL) {
    event->kind = MOUSE_EVENT;
    event->id = id;
    event->e.m.action = action;
    event->e.m.button = button;
    event->e.m.x = x;
    event->e.m.y = y;
    event->e.m.click_count = click_count;
  
    append_event(event);
  }
}

void new_key_event(enum KeyAction action, long id, uint32_t keyCode, char keyChar)
{
  Event* event = (Event*)malloc(sizeof(Event));

  if (event != NULL) {
    event->kind = KEY_EVENT;
    event->id = id;
    event->e.k.action = action;
    event->e.k.keyCode = keyCode;
    event->e.k.keyChar = keyChar;
    append_event(event);
  }
}

void new_display_flush_event(ShmSurface* surface)
{
  Event* event = (Event*)malloc(sizeof(Event));

  if (event != NULL) {
    event->kind = DISPLAY_EVENT;
    event->e.d.surface = surface;
    clock_gettime(CLOCK_REALTIME, &event->e.d.timestamp);
  
    append_event(event);
  }
}

void new_surface_event(enum SurfaceAction action, ShmSurface* surface, int32_t width, int32_t height) {
  Event* event = (Event*)malloc(sizeof(Event));
  
  if (event != NULL) {
    event->kind = SURFACE_EVENT;
    event->e.s.action = action;
    event->e.s.surface = surface;
    event->e.s.width = width;
    event->e.s.height = height;
  
    append_event(event);
  }
}

long timestamp_diff(struct timespec* ts1, struct timespec* ts2)
{
  return (ts1->tv_sec * 1000 + ts1->tv_nsec / 1000000) - (ts2->tv_sec * 1000 + ts2->tv_nsec / 1000000);
}

static void wayland_event_handler()
{
  struct input* input = get_input();

  wl_display_dispatch(input->display);
}

static void repeat_timer_handler()
{
  struct input* input = get_input();
  uint64_t exp;

  if (read(input->repeat_timer_fd, &exp, sizeof(exp)) == 0LL) {
    ;
  }

  uint32_t keycode = input->repeat_keycode;
  char ch = input->repeat_char;

  // Nothing to repeat
  if (keycode == 0) {
    return;
  }

  ShmSurface* activeSurface = input->activeSurface;
  
  if (activeSurface == NULL) {
    return;
  }

  // Generate key repeat
  new_key_event(KEY_PRESS, activeSurface->id, keycode, ch);
  
  if (ch != '\0') {
    new_key_event(KEY_TYPE, activeSurface->id, keycode, ch);
  }
}

static void watch_fd(struct input* input, int fd, uint32_t events, EventHandler fn)
{
  struct epoll_event ep;

  ep.events = events;
  ep.data.ptr = (void *)fn;

  epoll_ctl(input->epoll_fd, EPOLL_CTL_ADD, fd, &ep);
}

bool init_event(struct input* input)
{
  input->epoll_fd = epoll_create(1);

  if (input->epoll_fd == -1) {
    return false;
  }

  // Register Wayland event handler
  watch_fd(input, wl_display_get_fd(input->display), EPOLLIN, wayland_event_handler);
  // Register keyboard repeat timer
  watch_fd(input, input->repeat_timer_fd, EPOLLIN, repeat_timer_handler);

  return true;
}

static void handle_display_event(Event* evt)
{
  ShmSurface* surface= evt->e.d.surface;
  
  // A pending flush event, that occurs earlier than most recent flush, should be executed.
  if (timestamp_diff(&evt->e.d.timestamp, &surface->last_update) >= 0) {
    clock_gettime(CLOCK_REALTIME, &surface->last_update);
    wl_surface_attach(surface->surface, surface->buffer, 0, 0);
    wl_surface_commit(surface->surface);
  
    display_flush();
  }
}

static void handle_surface_event(Event* evt)
{
  ShmSurface* surface= evt->e.s.surface;

  switch(evt->e.s.action) {
    case SURFACE_DISPOSE:
      printf("DISPOSE\n");
      DestroyShmScreenSurface(surface);
  
      break;
    case SURFACE_UNMAP:
      printf("UN-MAP\n");
      UnmapShmScreenSurface(surface);
      
      break;
    case SURFACE_MAP:
      printf("MAP\n");
      RemapShmScreenSurface(surface, surface->width, surface->height);
      
      break;
  }
}

static void keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard, uint32_t format, int fd, uint32_t size) 
{
  struct input* input = (struct input*)data;
  struct xkb_keymap *keymap;
  struct xkb_state *state;
  char *map_str;

  if (data == NULL) {
    close(fd);
 
    return;
  }

  if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
    close(fd);
    
    return;
  }

  map_str = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
  
  if (map_str == MAP_FAILED) {
    close(fd);
    
    return;
  }

  keymap = xkb_keymap_new_from_string(input->xkb_context, map_str, XKB_KEYMAP_FORMAT_TEXT_V1, (xkb_keymap_compile_flags)0);
  
  munmap(map_str, size);
  close(fd);

  if (!keymap) {
    return;
  }

  state = xkb_state_new(keymap);
  
  if (!state) {
    xkb_keymap_unref(keymap);
  
    return;
  }

  xkb_keymap_unref(input->xkb.keymap);
  xkb_state_unref(input->xkb.state);
  
  input->xkb.keymap = keymap;
  input->xkb.state = state;

  input->xkb.control_mask = (xkb_mod_mask_t)(1 << xkb_keymap_mod_get_index(input->xkb.keymap, "Control"));
  input->xkb.alt_mask = (xkb_mod_mask_t)(1 << xkb_keymap_mod_get_index(input->xkb.keymap, "Mod1"));
  input->xkb.shift_mask = (xkb_mod_mask_t)(1 << xkb_keymap_mod_get_index(input->xkb.keymap, "Shift"));
}

static void keyboard_handle_enter(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys)
{
  if (surface != NULL) {
    ShmSurface* activeSurface = (ShmSurface*)wl_surface_get_user_data(surface);
    struct input* input = get_input();
    input->activeSurface = activeSurface;
  }
}

static void stop_keyboard_repeat(struct input* input)
{
  struct itimerspec   its;

  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = 0;
  
  timerfd_settime(input->repeat_timer_fd, 0, &its, NULL);
  
  input->repeat_char = '\0';
  input->repeat_keycode = 0;
}

static void start_keyboard_repeat(struct input* input, char ch, uint32_t keycode)
{
  struct itimerspec its;

  input->repeat_char = ch;
  input->repeat_keycode = keycode;

  its.it_interval.tv_sec = input->repeat_rate_sec;
  its.it_interval.tv_nsec = input->repeat_rate_nsec;
  its.it_value.tv_sec = input->repeat_delay_sec;
  its.it_value.tv_nsec = input->repeat_delay_nsec;
  
  timerfd_settime(input->repeat_timer_fd, 0, &its, NULL);
}

static void keyboard_handle_leave(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface)
{
  struct input* input = (struct input*)data;

  stop_keyboard_repeat(input);
  
  input->activeSurface = NULL;
}

static void keyboard_handle_key(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state_w)
{
  struct input*              input = (struct input*)data;
  enum wl_keyboard_key_state state = (wl_keyboard_key_state)state_w;
  uint32_t                   code;
  int                        num_syms;
  const xkb_keysym_t         *syms;
  xkb_keysym_t               sym;

  code = key + 8;
 
  if (!input->xkb.state) {
    return;
  }

  ShmSurface* activeSurface = input->activeSurface;
  
  // No active surface
  if (activeSurface == NULL) {
    return;
  }

  num_syms = xkb_state_key_get_syms(input->xkb.state, code, &syms);
  
  if (num_syms == 1) {
    sym = syms[0];
  } else {
    return;
  }

  char buffer[128];
  char ch = '\0';

  // The key has valid utf8 character
  if (xkb_keysym_to_utf8(sym, buffer, sizeof(buffer)) > 0) {
    ch = buffer[0];

    if (ch == '\r') {
      ch = '\n';
    }
  }

  uint32_t mapped_keycode = code;
  
  if (xkb_keysym_get_name(sym, buffer, sizeof(buffer)) > 0) {
    mapped_keycode = find_keycode_by_name(buffer);
  
    if (mapped_keycode == 0) {
      return;
    }
  } else {
    return;
  }

  if (state == WL_KEYBOARD_KEY_STATE_RELEASED) { // && key == input->repeat_key) {
    stop_keyboard_repeat(input);
    new_key_event(KEY_RELEASE, activeSurface->id, mapped_keycode, ch);
  } else if (state == WL_KEYBOARD_KEY_STATE_PRESSED && xkb_keymap_key_repeats(input->xkb.keymap, code)) {
    input->repeat_key = key;

    new_key_event(KEY_PRESS, activeSurface->id, mapped_keycode, ch);
 
    if (ch != '\0') {
      new_key_event(KEY_TYPE, activeSurface->id, mapped_keycode, ch);
    }

    start_keyboard_repeat(input, ch, mapped_keycode);
  }
}

static void keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
  struct input* input = (struct input*)data;
  xkb_mod_mask_t mask;

  // If we're not using a keymap, then we don't handle PC-style modifiers
  if (!input->xkb.keymap) {
    return;
  }

  xkb_state_update_mask(input->xkb.state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
  
  mask = xkb_state_serialize_mods(input->xkb.state, (xkb_state_component)(XKB_STATE_MODS_DEPRESSED | XKB_STATE_MODS_LATCHED));
  input->modifiers = 0;
  
  if (mask & input->xkb.control_mask) {
    input->modifiers |= MOD_CONTROL_MASK;
  }

  if (mask & input->xkb.alt_mask) {
    input->modifiers |= MOD_ALT_MASK;
  }

  if (mask & input->xkb.shift_mask) {
    input->modifiers |= MOD_SHIFT_MASK;
  }
}

static void keyboard_handle_repeat_info(void *data, struct wl_keyboard *keyboard, int32_t rate, int32_t delay) {

  struct input* input = (struct input*)data;

  if (rate == 0) return;

  if (rate == 1) {
    input->repeat_rate_sec = 1;
    input->repeat_rate_nsec = 0;
  } else {
    input->repeat_rate_sec = 0;
    input->repeat_rate_nsec = 1000000000 / rate;
  }

  input->repeat_delay_sec = delay / 1000;
  input->repeat_delay_nsec = (delay % 1000) * 1000 * 1000;
}

static const struct wl_keyboard_listener keyboard_listener = {
  keyboard_handle_keymap,
  keyboard_handle_enter,
  keyboard_handle_leave,
  keyboard_handle_key,
  keyboard_handle_modifiers,
  keyboard_handle_repeat_info
};

static void pointer_handle_enter(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t sx_w, wl_fixed_t sy_w)
{
  if (surface != NULL) {
    int x = wl_fixed_to_int(sx_w);
    int y = wl_fixed_to_int(sy_w);
    ShmSurface* activeSurface = (ShmSurface*)wl_surface_get_user_data(surface);
    struct input* input = get_input();
    
    input->activeSurface = activeSurface;
    input->x = x;
    input->y = y;
    
    if (activeSurface != NULL) {
      new_mouse_event(MOUSE_ENTER, activeSurface->id, x, y, 0, 0);
    }
  }
}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface)
{
  struct input* input = get_input();
  ShmSurface* activeSurface = input->activeSurface;

  input->activeSurface = NULL;
  input->x = -1;
  input->y = -1;
  
  if (activeSurface != NULL) {
    new_mouse_event(MOUSE_LEAVE, activeSurface->id, 0, 0, 0, 0);
  }
}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t sx_w, wl_fixed_t sy_w)
{
  int x = wl_fixed_to_int(sx_w);
  int y = wl_fixed_to_int(sy_w);
  struct input* input = get_input();
  ShmSurface* activeSurface = input->activeSurface;
  
  // No active surface
  if (activeSurface == NULL) {
    return;
  }

  input->x = x;
  input->y = y;

  if (input->button == 0) {
    new_mouse_event(MOUSE_MOVE, activeSurface->id, x, y, 0, 0);
  } else {
    new_mouse_event(MOUSE_DRAG, activeSurface->id, x, y, input->button, 0);
  }
}

static void pointer_handle_button(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state_w) 
{
  enum MouseAction action = (state_w == WL_POINTER_BUTTON_STATE_PRESSED) ? MOUSE_BUTTON_PRESS : MOUSE_BUTTON_RELEASE;

  struct input* input = get_input();
  ShmSurface* activeSurface = input->activeSurface;
  
  // No active surface
  if (activeSurface == NULL) {
    return;
  }

  new_mouse_event(action, activeSurface->id, input->x, input->y, button, 0);
  
  if (action == MOUSE_BUTTON_RELEASE) {
    uint32_t click_interval = time - input->last_click_time;

    input->button = 0;
    
    if (click_interval <= MOUSE_CLICK_INTERVAL) {
      input->click_count ++;
    } else {
      input->click_count = 1;
    }
    
    input->last_click_time = time;
    
    new_mouse_event(MOUSE_CLICK, activeSurface->id, input->x, input->y, button, input->click_count);
  } else {
    input->button = button;
  }
}

static void pointer_handle_axis(void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
  struct input* input = get_input();
  ShmSurface* activeSurface = input->activeSurface;
  
  // No active surface
  if (activeSurface == NULL) {
    return;
  }

  int delta = wl_fixed_to_int(value);
  
  new_mouse_event(MOUSE_WHEEL, activeSurface->id, input->x, input->y, (delta < 0 ) ? 4 : 0, 0);
}

static const struct wl_pointer_listener pointer_listener = {
  .enter  = pointer_handle_enter,
  .leave  = pointer_handle_leave,
  .motion = pointer_handle_motion,
  .button = pointer_handle_button,
  .axis   = pointer_handle_axis
};

static void seat_handle_capabilities(void *data, struct wl_seat *seat, uint32_t caps)
{
  struct input *input1 = (struct input *)data;

  if ((caps & WL_SEAT_CAPABILITY_POINTER) && !input1->pointer) {
    input1->pointer = wl_seat_get_pointer(seat);
    wl_pointer_set_user_data(input1->pointer, input1);
    wl_pointer_add_listener(input1->pointer, &pointer_listener, input1);
  } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && input1->pointer) {
    wl_pointer_release(input1->pointer);
    input1->pointer = NULL;
  }

  if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !input1->keyboard) {
    input1->keyboard = wl_seat_get_keyboard(seat);
    wl_keyboard_set_user_data(input1->keyboard, input1);
    wl_keyboard_add_listener(input1->keyboard, &keyboard_listener, input1);
  } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && input1->keyboard) {
    wl_keyboard_release(input1->keyboard);
    input1->keyboard = NULL;
  }
}

static void seat_handle_name(void *data, struct wl_seat *seat, const char *name)
{
}

static const struct wl_seat_listener seat_listener = {
  seat_handle_capabilities,
  seat_handle_name
};

bool init_input(struct input* input1)
{
  if (input1->seat == NULL) {
    return false;
  }

  input1->xkb_context = xkb_context_new((xkb_context_flags)0);
  
  if (input1->xkb_context == NULL) {
    return false;
  }

  // Default key repeat settings
  // Initial delay 250 milliseconds
  input1->repeat_delay_sec = 0;
  input1->repeat_delay_nsec = 1000 * 1000 * 250;
  
  // Repeat every 100 milliseconds
  input1->repeat_rate_sec = 0;
  input1->repeat_rate_nsec = 1000 * 1000 * 100;

  // Initial repeat timer
  input1->repeat_timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
  
  if (input1->repeat_timer_fd == -1) {
    return false;
  }

  // Initial event pump
  if (!init_event(input1)) {
    return false;
  }

  // Initial pending event list and lock
  pthread_mutex_init(&input1->lock, NULL);
  wl_list_init(&input1->event_list);

  wl_seat_add_listener(input1->seat, &seat_listener, input1);
  wl_seat_set_user_data(input1->seat, input1);

  return true;
}

int main()
{

  return 0;
}

void Application::Init(int argc, char **argv)
{
  if (wayland_init() == false) {
		throw jexception::RuntimeException("Unable to init wayland");
  }

  struct display *wdisplay = get_display();

  if (wdisplay == NULL) {
		throw jexception::RuntimeException("Unable to get display for wayland");
  }

  int 
    w = get_display_width(),
    h = get_display_height();

	sg_screen.width = w;
	sg_screen.height = h;
  
  sg_quitting = false;
}

static void InternalPaint()
{
	if (sg_jgui_window == nullptr || sg_jgui_window->IsVisible() == false) {
		return;
	}

  jrect_t<int> 
    bounds = sg_jgui_window->GetBounds();

  if (sg_back_buffer != nullptr) {
    jgui::jsize_t<int>
      size = sg_back_buffer->GetSize();

    if (size.width != bounds.size.width or size.height != bounds.size.height) {
      delete sg_back_buffer;
      sg_back_buffer = nullptr;
    }
  }

  if (sg_back_buffer == nullptr) {
    sg_back_buffer = new jgui::BufferedImage(jgui::JPF_RGB32, bounds.size);
  }

  jgui::Graphics 
    *g = sg_back_buffer->GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jgui::JCF_SRC);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

  g->Flush();
  
  if (Application::FrameRate(sg_jgui_window->GetFramesPerSecond()) == true) {
    return;
  }

  uint32_t *src = (uint32_t *)sg_back_buffer->LockData();
  uint32_t *dst = (uint32_t *)sg_surface->content;

  wl_surface_damage(sg_surface->surface, 0, 0, sg_surface->width, sg_surface->height);

  for (int i=0; i<bounds.size.width*bounds.size.height; i++) {
    dst[i] = src[i];
  }

  wl_surface_attach(sg_surface->surface, sg_surface->buffer, 0, 0);
  wl_surface_commit(sg_surface->surface);

  display_flush();

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  Event *event;

  if (!is_input_inited()) {
    return;
  }

  std::thread
    redraw([]() {
        while (sg_quitting == false) {
          if (sg_repaint.exchange(false) == true) {
            InternalPaint();
          }
        }
    });

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  while (sg_quitting == false) {
    event = next_event();
    
    if (event != NULL) {
      if (event->kind == MOUSE_EVENT)  {
        static jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;

        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        sg_mouse_x = event->e.m.x;
        sg_mouse_y = event->e.m.y;

        sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.width - 1);
        sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.height - 1);

        if (event->e.m.action == MOUSE_ENTER) {
          sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_ENTERED));

          continue;
        } else if (event->e.m.action == MOUSE_LEAVE) {
          sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_LEAVED));

          continue;
        } else if (event->e.m.action == MOUSE_MOVE or event->e.m.action == MOUSE_DRAG) {
          type = jevent::JMT_MOVED;
        } else if (event->e.m.action == MOUSE_BUTTON_PRESS or event->e.m.action == MOUSE_BUTTON_RELEASE) {
          if (event->e.m.button == BTN_LEFT) {
            button = jevent::JMB_BUTTON1;
          } else if (event->e.m.button == BTN_MIDDLE) {
            button = jevent::JMB_BUTTON2;
          } else if (event->e.m.button == BTN_RIGHT) {
            button = jevent::JMB_BUTTON3;
          }

          if (event->e.m.action == MOUSE_BUTTON_PRESS) {
            type = jevent::JMT_PRESSED;
            buttons = (jevent::jmouseevent_button_t)(buttons | button);
          } else if (event->e.m.action == MOUSE_BUTTON_RELEASE) {
            type = jevent::JMT_RELEASED;
            buttons = (jevent::jmouseevent_button_t)(buttons & ~button);
          }
        } else if (event->e.m.action == MOUSE_CLICK) {
          // action = "click";
        } else if (event->e.m.action == MOUSE_WHEEL) {
          type = jevent::JMT_ROTATED;
          mouse_z = (int)event->e.m.click_count;
        }

        if (sg_jgui_window->GetEventManager()->IsAutoGrab() == true && buttons != jevent::JMB_NONE) {
          // grab input
        } else {
          // release input
        }

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if (event->kind == KEY_EVENT)  {
        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

        if ((event->modifiers & MOD_SHIFT_MASK) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        } else if ((event->modifiers & MOD_CONTROL_MASK) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        } else if ((event->modifiers & MOD_ALT_MASK) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        // } else if ((event.key.keysym.mod & KMOD_LGUI) != 0) {
        //  mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SUPER);
        // } else if ((event.key.keysym.mod & KMOD_RGUI) != 0) {
        //   mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SUPER);
        // } else if ((event.key.keysym.mod & KMOD_MODE) != 0) {
        // 	mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALTGR);
        // } else if ((event.key.keysym.mod & KMOD_CAPS) != 0) {
        // 	mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CAPS_LOCK);
        // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
        // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
        // } else if ((event.key.keysym.mod & ) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_SUPER);
        // } else if ((event.key.keysym.mod & ) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_HYPER);
        }

        type = jevent::JKT_UNKNOWN;

        if (event->e.k.action == KEY_PRESS) {
          type = jevent::JKT_PRESSED;
        } else if (event->e.k.action == KEY_RELEASE) {
          type = jevent::JKT_RELEASED;
        // } else if (event->e.k.action == KEY_TYPE) {
        }

        jevent::jkeyevent_symbol_t symbol = (jevent::jkeyevent_symbol_t)event->e.k.keyCode;

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event->kind == DISPLAY_EVENT)  {
        handle_display_event(event);
      } else if (event->kind == SURFACE_EVENT)  {
        handle_surface_event(event);
      }

      free(event);
    } else {
      struct epoll_event ep[16];
      int count;
      EventHandler fn;
      struct input* input = get_input();

      count = epoll_wait(input->epoll_fd, ep, sizeof(ep)/sizeof(struct epoll_event), EVENT_WAIT_TIMEOUT);

      for (int index = 0; index < count; index ++) {
        fn = (EventHandler)ep[index].data.ptr;

        fn();
      }
    }
  }

  redraw.join();

  sg_jgui_window->SetVisible(false);
}

jsize_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;
  
  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

void Application::SetVerticalSyncEnabled(bool enabled)
{
}

bool Application::IsVerticalSyncEnabled()
{
  return true;
}

NativeWindow::NativeWindow(jgui::Window *parent, jgui::jrect_t<int> bounds):
	jgui::WindowAdapter()
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (sg_surface != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jgui_window = parent;

  sg_previous_bounds = bounds;

  sg_surface = CreateShmScreenSurface(0, 0, 0, bounds.size.width, bounds.size.height, 4); 

  RemapShmScreenSurface(sg_surface, sg_surface->width, sg_surface->height);
}

NativeWindow::~NativeWindow()
{
  if (sg_surface != nullptr) {
    new_surface_event(SURFACE_UNMAP, (ShmSurface*)sg_surface, 0, 0);
    new_surface_event(SURFACE_DISPOSE, (ShmSurface*)sg_surface, 0, 0);
  }

  wayland_cleanup();

  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
}

void NativeWindow::SetTitle(std::string title)
{
}

std::string NativeWindow::GetTitle()
{
	return std::string();
}

void NativeWindow::SetOpacity(float opacity)
{
}

float NativeWindow::GetOpacity()
{
	return 1.0;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
}

bool NativeWindow::IsUndecorated()
{
  return true;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
  ResizeShmScreenSurface(sg_surface, width, height);
}

jgui::jrect_t<int> NativeWindow::GetBounds()
{
	jgui::jrect_t<int> t {
    .point = {
      sg_surface->x, sg_surface->y
    },
    .size = {
      sg_surface->width, sg_surface->height
    }
  };

	return t;
}
		
void NativeWindow::SetResizable(bool resizable)
{
}

bool NativeWindow::IsResizable()
{
  return false;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	return p;
}

void NativeWindow::SetVisible(bool visible)
{
}

bool NativeWindow::IsVisible()
{
  return true;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jgui_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
}

bool NativeWindow::IsCursorEnabled()
{
	return true; 
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  /*
  SDL_SystemCursor type = SDL_SYSTEM_CURSOR_ARROW;

  if (style == JCS_DEFAULT) {
    type = SDL_SYSTEM_CURSOR_ARROW;
  } else if (style == JCS_CROSSHAIR) {
    type = SDL_SYSTEM_CURSOR_CROSSHAIR;
  } else if (style == JCS_EAST) {
  } else if (style == JCS_WEST) {
  } else if (style == JCS_NORTH) {
  } else if (style == JCS_SOUTH) {
  } else if (style == JCS_HAND) {
    type = SDL_SYSTEM_CURSOR_HAND;
  } else if (style == JCS_MOVE) {
    type = SDL_SYSTEM_CURSOR_SIZEALL;
  } else if (style == JCS_NS) {
    type = SDL_SYSTEM_CURSOR_SIZENS;
  } else if (style == JCS_WE) {
    type = SDL_SYSTEM_CURSOR_SIZEWE;
  } else if (style == JCS_NW_CORNER) {
  } else if (style == JCS_NE_CORNER) {
  } else if (style == JCS_SW_CORNER) {
  } else if (style == JCS_SE_CORNER) {
  } else if (style == JCS_TEXT) {
    type = SDL_SYSTEM_CURSOR_IBEAM;
  } else if (style == JCS_WAIT) {
    type = SDL_SYSTEM_CURSOR_WAIT;
  }

  SDL_Cursor
    *cursor = SDL_CreateSystemCursor(type);

  SDL_SetCursor(cursor);
  // TODO:: SDL_FreeCursor(cursor);
  */
  
  sg_jgui_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
  /*
	if ((void *)shape == nullptr) {
		return;
	}

	jsize_t<int> 
    t = shape->GetSize();
	uint32_t 
    data[t.width*t.height];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.width, t.height});

	SDL_Surface 
    *surface = SDL_CreateRGBSurfaceFrom(data, t.width, t.height, 32, t.width*4, 0, 0, 0, 0);

	if (surface == nullptr) {
		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != nullptr) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);
  */
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
	// TODO::
}

jwindow_rotation_t NativeWindow::GetRotation()
{
	return jgui::JWR_NONE;
}

void NativeWindow::SetIcon(jgui::Image *image)
{
  /*
  if (image == nullptr) {
    return;
  }

  jgui::jsize_t<int> 
    size = image->GetSize();
  uint32_t 
    *data = (uint32_t *)image->LockData();

  sg_jgui_icon = image;

  // todo anything ...

  image->UnlockData();
  */
}

jgui::Image * NativeWindow::GetIcon()
{
  return sg_jgui_icon;
}

}
