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
#include "binds/include/nativeapplication.h"
#include "binds/include/nativewindow.h"

#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>
#include <mutex>
#include <atomic>

#include <linux/input.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#define DRM_DEVICE "/dev/dri/card0"

namespace jgui {

struct modeset_buf {
  uint32_t width;
  uint32_t height;
  uint32_t stride;
  uint32_t size;
  uint32_t handle;
  uint8_t *map;
  uint32_t fb;
};

struct modeset_dev {
  struct modeset_dev *next;
  unsigned int front_buf;
  struct modeset_buf bufs[2];
  drmModeModeInfo mode;
  uint32_t conn;
  int crtc;
  drmModeCrtc *saved_crtc;
  bool cleanup;
  uint8_t r, g, b;
  bool r_up, g_up, b_up;
};

struct cursor_params_t {
  jgui::Image *cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
struct modeset_buf;
/** \brief */
struct modeset_dev;

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static int sg_handler = -1;
/** \brief */
static struct modeset_dev *sg_modeset_list = NULL;
/** \brief */
static bool sg_pending = false;
/** \brief */
static std::map<jcursor_style_t, struct cursor_params_t> sg_jgui_cursors;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t sg_screen = {0, 0};
/** \brief */
static struct cursor_params_t sg_cursor_params;
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;

static int modeset_create_fb(struct modeset_buf *buf)
{
  struct drm_mode_create_dumb creq;
  struct drm_mode_destroy_dumb dreq;
  struct drm_mode_map_dumb mreq;
  int ret;

  memset(&creq, 0, sizeof(creq));
  creq.width = buf->width;
  creq.height = buf->height;
  creq.bpp = 32;
  ret = drmIoctl(sg_handler, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
  if (ret < 0) {
    fprintf(stderr, "cannot create dumb buffer (%d): %m\n", errno);
    return -errno;
  }
  buf->stride = creq.pitch;
  buf->size = creq.size;
  buf->handle = creq.handle;

  ret = drmModeAddFB(sg_handler, buf->width, buf->height, 24, 32, buf->stride, buf->handle, &buf->fb);
  if (ret) {
    fprintf(stderr, "cannot create framebuffer (%d): %m\n", errno);
    ret = -errno;
    goto err_destroy;
  }

  memset(&mreq, 0, sizeof(mreq));
  mreq.handle = buf->handle;
  ret = drmIoctl(sg_handler, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
  if (ret) {
    fprintf(stderr, "cannot map dumb buffer (%d): %m\n", errno);
    ret = -errno;
    goto err_fb;
  }

  buf->map = (uint8_t*)mmap(0, buf->size, PROT_READ | PROT_WRITE, MAP_SHARED, sg_handler, mreq.offset);
  if (buf->map == MAP_FAILED) {
    fprintf(stderr, "cannot mmap dumb buffer (%d): %m\n", errno);
    ret = -errno;
    goto err_fb;
  }

  /* clear the framebuffer to 0 */
  memset(buf->map, 0, buf->size);

  return 0;

err_fb:
  drmModeRmFB(sg_handler, buf->fb);

err_destroy:
  memset(&dreq, 0, sizeof(dreq));
  dreq.handle = buf->handle;
  drmIoctl(sg_handler, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

  return ret;
}

static void modeset_destroy_fb(struct modeset_buf *buf)
{
  struct drm_mode_destroy_dumb dreq;

  munmap(buf->map, buf->size);

  drmModeRmFB(sg_handler, buf->fb);

  memset(&dreq, 0, sizeof(dreq));
  dreq.handle = buf->handle;
  drmIoctl(sg_handler, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
}

static void modeset_cleanup()
{
  struct modeset_dev *iter;
  drmEventContext ev;

  memset(&ev, 0, sizeof(ev));
  ev.version = DRM_EVENT_CONTEXT_VERSION;
  ev.page_flip_handler = 0;

  while (sg_modeset_list) {
    iter = sg_modeset_list;
    sg_modeset_list = iter->next;

    drmModeSetCrtc(sg_handler, iter->saved_crtc->crtc_id, iter->saved_crtc->buffer_id, iter->saved_crtc->x, iter->saved_crtc->y, &iter->conn, 1, &iter->saved_crtc->mode);
    drmModeFreeCrtc(iter->saved_crtc);

    modeset_destroy_fb(&iter->bufs[1]);
    modeset_destroy_fb(&iter->bufs[0]);

    free(iter);
  }
}

static int modeset_find_crtc(drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev)
{
  drmModeEncoder *enc;
  int i, j;
  int32_t crtc;
  struct modeset_dev *iter;

  if (conn->encoder_id)
    enc = drmModeGetEncoder(sg_handler, conn->encoder_id);
  else
    enc = NULL;

  if (enc) {
    if (enc->crtc_id) {
      crtc = enc->crtc_id;
      for (iter = sg_modeset_list; iter; iter = iter->next) {
        if (iter->crtc == crtc) {
          crtc = -1;
          break;
        }
      }

      if (crtc >= 0) {
        drmModeFreeEncoder(enc);
        dev->crtc = crtc;
        return 0;
      }
    }

    drmModeFreeEncoder(enc);
  }

  for (i = 0; i < conn->count_encoders; ++i) {
    enc = drmModeGetEncoder(sg_handler, conn->encoders[i]);
    if (!enc) {
      fprintf(stderr, "cannot retrieve encoder %u:%u (%d): %m\n", i, conn->encoders[i], errno);
      continue;
    }

    for (j = 0; j < res->count_crtcs; ++j) {
      if (!(enc->possible_crtcs & (1 << j)))
        continue;

      crtc = res->crtcs[j];
      for (iter = sg_modeset_list; iter; iter = iter->next) {
        if (iter->crtc == crtc) {
          crtc = -1;
          break;
        }
      }

      if (crtc >= 0) {
        drmModeFreeEncoder(enc);
        dev->crtc = crtc;
        return 0;
      }
    }

    drmModeFreeEncoder(enc);
  }

  fprintf(stderr, "cannot find suitable CRTC for connector %u\n", conn->connector_id);

  return -ENOENT;
}

static int modeset_setup_dev(drmModeRes *res, drmModeConnector *conn, struct modeset_dev *dev)
{
  int ret;

  if (conn->connection != DRM_MODE_CONNECTED) {
    fprintf(stderr, "ignoring unused connector %u\n",
      conn->connector_id);
    return -ENOENT;
  }

  if (conn->count_modes == 0) {
    fprintf(stderr, "no valid mode for connector %u\n", conn->connector_id);
    return -EFAULT;
  }

  memcpy(&dev->mode, &conn->modes[0], sizeof(dev->mode));
  dev->bufs[0].width = conn->modes[0].hdisplay;
  dev->bufs[0].height = conn->modes[0].vdisplay;
  dev->bufs[1].width = conn->modes[0].hdisplay;
  dev->bufs[1].height = conn->modes[0].vdisplay;
  fprintf(stderr, "mode for connector %u is %ux%u\n", conn->connector_id, dev->bufs[0].width, dev->bufs[0].height);

  ret = modeset_find_crtc(res, conn, dev);

  if (ret) {
    fprintf(stderr, "no valid crtc for connector %u\n", conn->connector_id);
    return ret;
  }

  ret = modeset_create_fb(&dev->bufs[0]);
  if (ret) {
    fprintf(stderr, "cannot create framebuffer for connector %u\n", conn->connector_id);
    return ret;
  }

  ret = modeset_create_fb(&dev->bufs[1]);
  if (ret) {
    fprintf(stderr, "cannot create framebuffer for connector %u\n", conn->connector_id);
    modeset_destroy_fb(&dev->bufs[0]);
    return ret;
  }

  return 0;
}

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol, bool shift)
{
	switch (symbol) {
		case 0x1c:
			return jevent::JKS_ENTER; // jevent::JKS_RETURN;
		case 0x0e:
			return jevent::JKS_BACKSPACE;
		case 0x0f:
			return jevent::JKS_TAB;
		// case SDLK_CANCEL:
		//	return jevent::JKS_CANCEL;
		case 0x01:
			return jevent::JKS_ESCAPE;
		case 0x39:
			return jevent::JKS_SPACE;
		case 0x29:
			return (shift == false)?jevent::JKS_APOSTROPHE:jevent::JKS_QUOTATION;
		case 0x33:
			return (shift == false)?jevent::JKS_COMMA:jevent::JKS_LESS_THAN_SIGN;
		case 0x0c:
			return (shift == false)?jevent::JKS_MINUS_SIGN:jevent::JKS_UNDERSCORE;
		case 0x34:  
			return (shift == false)?jevent::JKS_PERIOD:jevent::JKS_GREATER_THAN_SIGN;
		case 0x59:
			return (shift == false)?jevent::JKS_SLASH:jevent::JKS_QUESTION_MARK;
		case 0x0b:     
			return (shift == false)?jevent::JKS_0:jevent::JKS_PARENTHESIS_RIGHT;
		case 0x02:
			return (shift == false)?jevent::JKS_1:jevent::JKS_EXCLAMATION_MARK;
		case 0x03:
			return (shift == false)?jevent::JKS_2:jevent::JKS_AT;
		case 0x04:
			return (shift == false)?jevent::JKS_3:jevent::JKS_NUMBER_SIGN;
		case 0x05:
			return (shift == false)?jevent::JKS_4:jevent::JKS_DOLLAR_SIGN;
		case 0x06:
			return (shift == false)?jevent::JKS_5:jevent::JKS_PERCENT_SIGN;
		case 0x07:
			return (shift == false)?jevent::JKS_6:jevent::JKS_CIRCUMFLEX_ACCENT;
		case 0x08:
			return (shift == false)?jevent::JKS_7:jevent::JKS_AMPERSAND;
		case 0x09:
			return (shift == false)?jevent::JKS_8:jevent::JKS_STAR;
		case 0x0a:
			return (shift == false)?jevent::JKS_9:jevent::JKS_PARENTHESIS_LEFT;
		case 0x35:
			return (shift == false)?jevent::JKS_SEMICOLON:jevent::JKS_COLON;
		case 0x0d: 
			return (shift == false)?jevent::JKS_EQUALS_SIGN:jevent::JKS_PLUS_SIGN;
		case 0x1b:
			return (shift == false)?jevent::JKS_SQUARE_BRACKET_LEFT:jevent::JKS_CURLY_BRACKET_LEFT;
		case 0x56:   
			return (shift == false)?jevent::JKS_BACKSLASH:jevent::JKS_VERTICAL_BAR;
		case 0x2b:
			return (shift == false)?jevent::JKS_SQUARE_BRACKET_RIGHT:jevent::JKS_CURLY_BRACKET_RIGHT;
		case 0x1e:       
			return (shift == false)?jevent::JKS_a:jevent::JKS_A;
		case 0x30:
			return (shift == false)?jevent::JKS_b:jevent::JKS_B;
		case 0x2e:
			return (shift == false)?jevent::JKS_c:jevent::JKS_C;
		case 0x20:
			return (shift == false)?jevent::JKS_d:jevent::JKS_D;
		case 0x12:
			return (shift == false)?jevent::JKS_e:jevent::JKS_E;
		case 0x21:
			return (shift == false)?jevent::JKS_f:jevent::JKS_F;
		case 0x22:
			return (shift == false)?jevent::JKS_g:jevent::JKS_G;
		case 0x23:
			return (shift == false)?jevent::JKS_h:jevent::JKS_H;
		case 0x17:
			return (shift == false)?jevent::JKS_i:jevent::JKS_I;
		case 0x24:
			return (shift == false)?jevent::JKS_j:jevent::JKS_J;
		case 0x25:
			return (shift == false)?jevent::JKS_k:jevent::JKS_K;
		case 0x26:
			return (shift == false)?jevent::JKS_l:jevent::JKS_L;
		case 0x32:
			return (shift == false)?jevent::JKS_m:jevent::JKS_M;
		case 0x31:
			return (shift == false)?jevent::JKS_n:jevent::JKS_N;
		case 0x18:
			return (shift == false)?jevent::JKS_o:jevent::JKS_O;
		case 0x19:
			return (shift == false)?jevent::JKS_p:jevent::JKS_P;
		case 0x10:
			return (shift == false)?jevent::JKS_q:jevent::JKS_Q;
		case 0x13:
			return (shift == false)?jevent::JKS_r:jevent::JKS_R;
		case 0x1f:
			return (shift == false)?jevent::JKS_s:jevent::JKS_S;
		case 0x14:
			return (shift == false)?jevent::JKS_t:jevent::JKS_T;
		case 0x16:
			return (shift == false)?jevent::JKS_u:jevent::JKS_U;
		case 0x2f:
			return (shift == false)?jevent::JKS_v:jevent::JKS_V;
		case 0x11:
			return (shift == false)?jevent::JKS_w:jevent::JKS_W;
		case 0x2d:
			return (shift == false)?jevent::JKS_x:jevent::JKS_X;
		case 0x15:
			return (shift == false)?jevent::JKS_y:jevent::JKS_Y;
		case 0x2c:
			return (shift == false)?jevent::JKS_z:jevent::JKS_Z;
		// case SDLK_BACKQUOTE:
		//	return jevent::JKS_GRAVE_ACCENT;
		case 0x28:  
			return (shift == false)?jevent::JKS_TILDE:jevent::JKS_CIRCUMFLEX_ACCENT;
		case 0x6f:
			return jevent::JKS_DELETE;
		case 0x69:
			return jevent::JKS_CURSOR_LEFT;
		case 0x6a:
			return jevent::JKS_CURSOR_RIGHT;
		case 0x67:  
			return jevent::JKS_CURSOR_UP;
		case 0x6c:
			return jevent::JKS_CURSOR_DOWN;
		case 0x6e:  
			return jevent::JKS_INSERT;
		case 0x66:     
			return jevent::JKS_HOME;
		case 0x6b:
			return jevent::JKS_END;
		case 0x68:
			return jevent::JKS_PAGE_UP;
		case 0x6d:
			return jevent::JKS_PAGE_DOWN;
		case 0x63:   
			return jevent::JKS_PRINT;
		case 0x77:
			return jevent::JKS_PAUSE;
		// case SDLK_RED:
		//	return jevent::JKS_RED;
		// case SDLK_GREEN:
		//	return jevent::JKS_GREEN;
		// case SDLK_YELLOW:
		//	return jevent::JKS_YELLOW;
		// case SDLK_BLUE:
		//	return jevent::JKS_BLUE;
		case 0x3b:
			return jevent::JKS_F1;
		case 0x3c:
			return jevent::JKS_F2;
		case 0x3d:
			return jevent::JKS_F3;
		case 0x3e:
			return jevent::JKS_F4;
		case 0x3f:
			return jevent::JKS_F5;
		case 0x40:
			return jevent::JKS_F6;
		case 0x41:    
			return jevent::JKS_F7;
		case 0x42:
			return jevent::JKS_F8;
		case 0x43:  
			return jevent::JKS_F9;
		case 0x44: 
			return jevent::JKS_F10;
		case 0x57:
			return jevent::JKS_F11;
		case 0x58:
			return jevent::JKS_F12;
		case 0x2a: // left
		case 0x36: // right
			return jevent::JKS_SHIFT;
		case 0x1d: // left
		case 0x61: // right
			return jevent::JKS_CONTROL;
		case 0x38: // left
			return jevent::JKS_ALT;
		case 0x64: 
		  return jevent::JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jevent::JKS_META;
		case 0x7d:
			return jevent::JKS_SUPER;
		// case SDLK_HYPER:
		//	return jevent::JKS_HYPER;
		default: 
			break;
	}

	return jevent::JKS_UNKNOWN;
}

NativeApplication::NativeApplication():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::NativeApplication");
}

NativeApplication::~NativeApplication()
{
}

void NativeApplication::InternalInit(int argc, char **argv)
{
  struct modeset_dev *iter;
  struct modeset_buf *buf;
  uint64_t has_dumb;
  
  int fd = open(DRM_DEVICE, O_RDWR | O_CLOEXEC);

  if (fd < 0) {
    throw jexception::RuntimeException("Unable to open drm device");
  }

  if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
    close(fd);

    throw jexception::RuntimeException("Unable to get capabilities from device");
  }

  drmModeConnector *conn;
  struct modeset_dev *dev;
  drmModeRes *res = drmModeGetResources(fd);

  if (!res) {
    close(fd);

    throw jexception::RuntimeException("Unable to retrieve DRM resources");
  }

  sg_handler = fd;

  for (int i = 0; i < res->count_connectors; ++i) {
    conn = drmModeGetConnector(fd, res->connectors[i]);

    if (!conn) {
      printf("cannot retrieve DRM connector %u:%u (%d): %m\n", i, res->connectors[i], errno);

      continue;
    }

    dev = (modeset_dev*)malloc(sizeof(*dev));
    memset(dev, 0, sizeof(*dev));
    dev->conn = conn->connector_id;

    int ret = modeset_setup_dev(res, conn, dev);

    if (ret) {
      if (ret != -ENOENT) {
        errno = -ret;

        printf("cannot setup device for connector %u:%u (%d): %m\n", i, res->connectors[i], errno);
      }

      drmModeFreeConnector(conn);
      free(dev);

      continue;
    }

    drmModeFreeConnector(conn);
    dev->next = sg_modeset_list;
    sg_modeset_list = dev;
  }

  drmModeFreeResources(res);

  for (iter = sg_modeset_list; iter; iter = iter->next) {
      iter->saved_crtc = drmModeGetCrtc(fd, iter->crtc);
      buf = &iter->bufs[iter->front_buf];

      if (drmModeSetCrtc(fd, iter->crtc, buf->fb, 0, 0, &iter->conn, 1, &iter->mode)) {
          fprintf(stderr, "cannot set CRTC for connector %u (%d): %m\n", iter->conn, errno);
      }

      if (iter == sg_modeset_list) { // first element ...
        sg_screen.width = buf->width;
        sg_screen.height = buf->height;
  
        printf("screen size:: %ux%u\n", sg_screen.width, sg_screen.height);
      }
  }

  sg_quitting = false;

#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = new jgui::BufferedImage(JPF_ARGB, w, h);												\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, ix*w, iy*h, w, h, 0, 0);	\
																																				\
	sg_jgui_cursors[type] = t;																										\

	struct cursor_params_t t;
	int w = 30,
			h = 30;

	Image *cursors = new jgui::BufferedImage(_DATA_PREFIX"/images/cursors.png");

	CURSOR_INIT(JCS_DEFAULT, 0, 0, 8, 8);
	CURSOR_INIT(JCS_CROSSHAIR, 4, 3, 15, 15);
	CURSOR_INIT(JCS_EAST, 4, 4, 22, 15);
	CURSOR_INIT(JCS_WEST, 5, 4, 9, 15);
	CURSOR_INIT(JCS_NORTH, 6, 4, 15, 8);
	CURSOR_INIT(JCS_SOUTH, 7, 4, 15, 22);
	CURSOR_INIT(JCS_HAND, 1, 0, 15, 15);
	CURSOR_INIT(JCS_MOVE, 8, 4, 15, 15);
	CURSOR_INIT(JCS_NS, 2, 4, 15, 15);
	CURSOR_INIT(JCS_WE, 3, 4, 15, 15);
	CURSOR_INIT(JCS_NW_CORNER, 8, 1, 10, 10);
	CURSOR_INIT(JCS_NE_CORNER, 9, 1, 20, 10);
	CURSOR_INIT(JCS_SW_CORNER, 6, 1, 10, 20);
	CURSOR_INIT(JCS_SE_CORNER, 7, 1, 20, 20);
	CURSOR_INIT(JCS_TEXT, 7, 0, 15, 15);
	CURSOR_INIT(JCS_WAIT, 8, 0, 15, 15);
	
	delete cursors;
}

void NativeApplication::InternalPaint()
{
  if (sg_jgui_window == nullptr or sg_jgui_window->IsVisible() == false) {
    return;
  }

  jregion_t 
    bounds = sg_jgui_window->GetBounds();

  if (sg_back_buffer != nullptr) {
    jgui::jsize_t
      size = sg_back_buffer->GetSize();

    if (size.width != bounds.width or size.height != bounds.height) {
      delete sg_back_buffer;
      sg_back_buffer = nullptr;
    }
  }

  if (sg_back_buffer == nullptr) {
    sg_back_buffer = new jgui::BufferedImage(jgui::JPF_RGB32, bounds.width, bounds.height);
  }

  jgui::Graphics 
    *g = sg_back_buffer->GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jgui::JCF_SRC_OVER);

  sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

  if (sg_cursor_enabled == true) {
    g->DrawImage(sg_cursor_params.cursor, sg_mouse_x, sg_mouse_y);
  }
  
  g->Flush();
  
  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

  struct modeset_dev *dev = sg_modeset_list; // INFO:: first monitor
  struct modeset_buf *buf = &dev->bufs[dev->front_buf ^ 1];
  uint32_t *src = (uint32_t *)data;
  uint32_t *dst = (uint32_t *)buf->map;

  for (int j = 0; j < bounds.height; j++) {
    for (int i = 0; i < bounds.width; i++) {
      *dst++ = *src++;
    }
  }

  dev->front_buf ^= 1;

  sg_back_buffer->UnlockData();

  sg_pending = true;

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  struct input_event ev;
  bool shift = false;
  int mouse_x = 0, mouse_y = 0;
  uint32_t last_mouse_state = 0x00;
  
  int 
    fdk = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY);

  if (fdk == -1) {
    printf("Cannot open the key device\n");
  }

  fcntl(fdk, F_SETFL, O_NONBLOCK);

  int 
    fdm = open("/dev/input/mice", O_RDONLY);

  if(fdm == -1) {   
    printf("Cannot open the mouse device\n");
  }   

  fcntl(fdm, F_SETFL, O_NONBLOCK);

  while (sg_quitting == false) {
    if (mouse_x != sg_mouse_x or mouse_y != sg_mouse_y) {
      mouse_x = sg_mouse_x;
      mouse_y = sg_mouse_y;

      if (sg_cursor_enabled == true) {
        sg_repaint.store(true);
      }
    }
    
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }
    
    if (read(fdk, &ev, sizeof ev) == sizeof(ev)) {
      if (ev.type == EV_KEY) {
        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

        /*
        if ((event.key.keysym.mod & KMOD_LSHIFT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        } else if ((event.key.keysym.mod & KMOD_RSHIFT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        } else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        } else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        // } else if ((event.key.keysym.mod & ) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_ALTGR);
        // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
        // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
        // } else if ((event.key.keysym.mod & ) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_SUPER);
        // } else if ((event.key.keysym.mod & ) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_HYPER);
        }
        */

        type = jevent::JKT_UNKNOWN;

        if (ev.value == 1 or ev.value == 2) {
          type = jevent::JKT_PRESSED;

          // TODO:: grab pointer events
        } else if (ev.value == 0) {
          type = jevent::JKT_RELEASED;

          // TODO:: ungrab pointer events
        }

        if (ev.code == 0x2a or ev.code == 0x36) { // LSHIFT, RSHIFT
          shift = (bool)ev.value;
        }

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev.code, shift);

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));

        // continue;
      }
    }

    signed char data[3];

    if (read(fdm, data, sizeof(data)) == sizeof(data)) {
      int 
        buttonMask = data[0];
      int 
        x = sg_mouse_x + data[1],
        y = sg_mouse_y - data[2];
      
      x = (x < 0)?0:(x > sg_screen.width)?sg_screen.width:x;
      y = (y < 0)?0:(y > sg_screen.height)?sg_screen.height:y;

      jevent::jmouseevent_button_t button = jevent::JMB_NONE;
      jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
      int mouse_z = 0;

      type = jevent::JMT_PRESSED;

      if (sg_mouse_x != x || sg_mouse_y != y) {
        type = jevent::JMT_MOVED;
      }

      sg_mouse_x = x;
      sg_mouse_y = y;

      if ((buttonMask & 0x01) == 0 && (last_mouse_state & 0x01)) {
        type = jevent::JMT_RELEASED;
      } else if ((buttonMask & 0x02) == 0 && (last_mouse_state & 0x02)) {
        type = jevent::JMT_RELEASED;
      } else if ((buttonMask & 0x04) == 0 && (last_mouse_state & 0x04)) {
        type = jevent::JMT_RELEASED;
      } 

      if ((buttonMask & 0x01) != (last_mouse_state & 0x01)) {
        button = jevent::JMB_BUTTON1;
      } else if ((buttonMask & 0x02) != (last_mouse_state & 0x02)) {
        button = jevent::JMB_BUTTON3;
      } else if ((buttonMask & 0x04) != (last_mouse_state & 0x04)) {
        button = jevent::JMB_BUTTON2;
      }

      last_mouse_state = buttonMask;

      sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x + sg_cursor_params.hot_x, sg_mouse_y + sg_cursor_params.hot_y}, mouse_z));

      continue;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));

  sg_quitting = true;
 
  close(fdk);
  close(fdm);

  sg_jgui_window->SetVisible(false);
}

void NativeApplication::InternalQuit()
{
  sg_quitting = true;

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

  // if (sg_surface != nullptr) {
  // 	throw jexception::RuntimeException("Cannot create more than one window");
  // }

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

  sg_mouse_x = 0;
  sg_mouse_y = 0;

  SetCursor(sg_jgui_cursors[JCS_DEFAULT].cursor, sg_jgui_cursors[JCS_DEFAULT].hot_x, sg_jgui_cursors[JCS_DEFAULT].hot_y);
}

NativeWindow::~NativeWindow()
{
  if (sg_cursor_params.cursor != nullptr) {
    delete sg_cursor_params.cursor;
    sg_cursor_params.cursor = nullptr;
  }

  modeset_cleanup();
  close(sg_handler);
  
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

void NativeWindow::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == nullptr) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  // TODO:: sg_jgui_window precisa ser a window que contem ela
  // TODO:: pegar os windows por evento ou algo assim
  sg_jgui_window = parent;

  sg_jgui_window->SetParent(nullptr);
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
}

jgui::jregion_t NativeWindow::GetBounds()
{
	jgui::jregion_t t = {
    .x = 0,
    .y = 0,
    .width = sg_screen.width,
    .height = sg_screen.height
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
  sg_mouse_x = (x < 0)?0:(x > sg_screen.width)?sg_screen.width:x;
  sg_mouse_y = (y < 0)?0:(y > sg_screen.height)?sg_screen.height:y;
}

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t p;

	p.x = sg_mouse_x;
	p.y = sg_mouse_y;

	return p;
}

void NativeWindow::SetVisible(bool visible)
{
}

bool NativeWindow::IsVisible()
{
  return sg_quitting == false;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jgui_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = (enabled == false)?false:true;
}

bool NativeWindow::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
	sg_jgui_cursor = style;

	SetCursor(sg_jgui_cursors[style].cursor, sg_jgui_cursors[style].hot_x, sg_jgui_cursors[style].hot_y);
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

  if (sg_cursor_params.cursor != nullptr) {
    delete sg_cursor_params.cursor;
    sg_cursor_params.cursor = nullptr;
  }

  sg_cursor_params.cursor = dynamic_cast<jgui::Image *>(shape->Clone());

  sg_cursor_params.hot_x = hotx;
  sg_cursor_params.hot_y = hoty;
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t NativeWindow::GetRotation()
{
	return jgui::JWR_NONE;
}

void NativeWindow::SetIcon(jgui::Image *image)
{
  sg_jgui_icon = image;
}

jgui::Image * NativeWindow::GetIcon()
{
  return sg_jgui_icon;
}

}
