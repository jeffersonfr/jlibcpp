#include "jevent/jkeyevent.h"

#include <string>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <linux/input-event-codes.h>
#include <xkbcommon/xkbcommon.h>

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <wayland-util.h>

#define WL_VERSION 1

#define MOD_SHIFT_MASK          0x01
#define MOD_ALT_MASK            0x02
#define MOD_CONTROL_MASK        0x04

// Define minimal interval to issue a display (ms)
#define DISPLAY_UPDATE_INTERVAL 25
#define EVENT_WAIT_TIMEOUT  25
#define  MOUSE_CLICK_INTERVAL  200

struct display {
  struct wl_display*          display;
  struct wl_registry*         registry;
  struct wl_compositor*       compositor;
  struct wl_subcompositor*    subcompositor;
  struct wl_shell*            shell;
  struct wl_shm*              shm;
  struct wl_output*           output;

  int32_t                     width;
  int32_t                     height;
};

struct shm_surface;

struct input {
  struct wl_display*          display;
  struct wl_seat*             seat;
  struct wl_pointer*          pointer;
  struct wl_keyboard*         keyboard;
  struct wl_touch*            touch;

  // Active Surface
  struct shm_surface*         activeSurface;

  uint32_t                    modifiers;

  // The most recent pointer location
  int                         x;
  int                         y;
  uint32_t                    button;
  uint32_t                    click_count;
  uint32_t                    last_click_time;

  // Keyboard
  struct xkb_context*         xkb_context;

  struct {
    struct xkb_keymap *keymap;
    struct xkb_state *state;
    xkb_mod_mask_t control_mask;
    xkb_mod_mask_t alt_mask;
    xkb_mod_mask_t shift_mask;
  } xkb;

  // key repeat
  int32_t                     repeat_rate_sec;
  int32_t                     repeat_rate_nsec;
  int32_t                     repeat_delay_sec;
  int32_t                     repeat_delay_nsec;

  int                         repeat_timer_fd;
  char                        repeat_char;
  uint32_t                    repeat_keycode;
  uint32_t                    repeat_key;

  int                         epoll_fd;

  // Mutex to protect event list
  pthread_mutex_t             lock;

  // Pending events
  struct wl_list              event_list;
};

/**
 * Define wayland events
 */
enum EventKind {
  MOUSE_EVENT,
  KEY_EVENT,
  DISPLAY_EVENT,
  SURFACE_EVENT    // event to manipulate surface
};

enum MouseAction {
  MOUSE_ENTER,
  MOUSE_MOVE,
  MOUSE_LEAVE,
  MOUSE_BUTTON_PRESS,
  MOUSE_BUTTON_RELEASE,
  MOUSE_CLICK,
  MOUSE_DRAG,
  MOUSE_WHEEL
};

enum KeyAction {
  KEY_PRESS,
  KEY_TYPE,
  KEY_RELEASE
};

enum SurfaceAction {
  SURFACE_MAP,
  SURFACE_UNMAP,
  SURFACE_DISPOSE
};

struct MouseEvent {
  enum MouseAction action;
  int32_t          x;
  int32_t          y;
  uint32_t         button;
  uint32_t         click_count;
};

struct KeyEvent {
  enum KeyAction action;
  uint32_t       keyCode;
  char           keyChar;
};

struct shm_surface {
  struct wl_surface*          surface;
  struct wl_shell_surface*    shell_surface;
  struct wl_buffer*           buffer;
  struct wl_shm_pool*         pool;

  void*                       content;

  int64_t                     id;
  int32_t                     x;
  int32_t                     y;
  int32_t                     width;
  int32_t                     height;
  int32_t                     pixel_depth;
  uint32_t                    format;

  // Time to track last update
  struct timespec             last_update;
};

typedef struct shm_surface ShmSurface;

struct SurfaceEvent {
  enum SurfaceAction action;
  ShmSurface*        surface;
  int32_t            width;
  int32_t            height;
};

struct DisplayEvent {
  struct shm_surface* surface;
  struct timespec     timestamp;
};

struct event {
  struct wl_list  link;
  enum EventKind  kind;
  long            id;

  uint32_t        modifiers;

  union {
    struct MouseEvent   m;
    struct KeyEvent     k;
    struct DisplayEvent d;
    struct SurfaceEvent s;
  } e;
};

typedef struct event Event;

struct name_keyCode {
  const char* name;
  uint32_t code;
};

typedef void (*EventHandler)();

