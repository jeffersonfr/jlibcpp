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
/*
	OLC::CAD - A practical example of Polymorphism
	"Damn Gorbette, you made us giggle..." - javidx9

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018-2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  */
#include "jgui/japplication.h"
#include "jgui/jwindow.h"

#include <vector>
#include <list>
#include <string>

struct Shape;

struct sNode {
	Shape *parent;
	jgui::jpoint3d_t pos;
};

float magnitude(jgui::jpoint3d_t t) 
{
  return sqrt(t.x*t.x + t.y*t.y + t.z*t.z);
}

struct Shape {
	std::vector<sNode> vecNodes;
	uint32_t nMaxNodes = 0;

  jgui::Color::jcolor_name_t col = jgui::Color::Green;

	static float fWorldScale;
	static jgui::jpoint3d_t vWorldOffset;

	void WorldToScreen(const jgui::jpoint3d_t &v, int &nScreenX, int &nScreenY)
	{
		nScreenX = (int)((v.x - vWorldOffset.x) * fWorldScale);
		nScreenY = (int)((v.y - vWorldOffset.y) * fWorldScale);
	}

	virtual void DrawYourself(jgui::Graphics *g) = 0;

	sNode* GetNextNode(const jgui::jpoint3d_t &p)
	{
		if (vecNodes.size() == nMaxNodes)
			return nullptr; // Shape is complete so no new nodes to be issued

		// else create new node and add to shapes node vector
		sNode n;
		n.parent = this;
		n.pos = p;
		vecNodes.push_back(n);

		// Beware! - This normally is bad! But see sub classes
		return &vecNodes[vecNodes.size() - 1];
	}

	sNode* HitNode(jgui::jpoint3d_t &p)
	{
		for (auto &n : vecNodes) {
			if (magnitude({p.x - n.pos.x, p.y - n.pos.y, 0.0f}) < 0.01f) {
				return &n;
      }
		}

		return nullptr;
	}

	void DrawNodes(jgui::Graphics *g)
	{
		for (auto &n : vecNodes)
		{
			int sx, sy;
			WorldToScreen(n.pos, sx, sy);
      g->SetColor(jgui::Color::Red);
			g->FillCircle(sx, sy, 2);
		}
	}

  virtual ~Shape()
  {
  }

};

float Shape::fWorldScale = 1.0f;
jgui::jpoint3d_t Shape::vWorldOffset = { 0,0 };

struct Line : public Shape {
	Line()
	{
		nMaxNodes = 2;
		vecNodes.reserve(nMaxNodes);
	}

	void DrawYourself(jgui::Graphics *g) override
	{
		int sx, sy, ex, ey;
		WorldToScreen(vecNodes[0].pos, sx, sy);
		WorldToScreen(vecNodes[1].pos, ex, ey);
    g->SetColor(col);
		g->DrawLine(sx, sy, ex, ey);
	}
};

struct Box : public Shape {
	Box()
	{
		nMaxNodes = 2;
		vecNodes.reserve(nMaxNodes); 
	}

	void DrawYourself(jgui::Graphics *g) override
	{
		int sx, sy, ex, ey;
		WorldToScreen(vecNodes[0].pos, sx, sy);
		WorldToScreen(vecNodes[1].pos, ex, ey);
    g->SetColor(col);
		g->DrawRectangle(sx, sy, ex - sx, ey - sy);
	}
};

struct Circle : public Shape {
	Circle()
	{
		nMaxNodes = 2;
		vecNodes.reserve(nMaxNodes);
	}

	void DrawYourself(jgui::Graphics *g) override
	{
    float fRadius = magnitude({vecNodes[0].pos.x - vecNodes[1].pos.x, vecNodes[0].pos.y - vecNodes[1].pos.y, 0.0f});
		int sx, sy, ex, ey;
		WorldToScreen(vecNodes[0].pos, sx, sy);
		WorldToScreen(vecNodes[1].pos, ex, ey);
    g->SetColor(col);
		g->DrawLine(sx, sy, ex, ey);

    g->SetColor(col);
		g->DrawCircle(sx, sy, (int32_t)(fRadius * fWorldScale));
	}
};

struct Curve : public Shape {
	Curve()
	{
		nMaxNodes = 3;
		vecNodes.reserve(nMaxNodes);
	}

	void DrawYourself(jgui::Graphics *g) override
	{
		int sx, sy, ex, ey;

		if (vecNodes.size() < 3) {
			// Can only draw line from first to second
			WorldToScreen(vecNodes[0].pos, sx, sy);
			WorldToScreen(vecNodes[1].pos, ex, ey);
      g->SetColor(col);
			g->DrawLine(sx, sy, ex, ey);
		}

		if (vecNodes.size() == 3) {
			// Can draw line from first to second
			WorldToScreen(vecNodes[0].pos, sx, sy);
			WorldToScreen(vecNodes[1].pos, ex, ey);
      g->SetColor(col);
			g->DrawLine(sx, sy, ex, ey);

			// Can draw second structural line
			WorldToScreen(vecNodes[1].pos, sx, sy);
			WorldToScreen(vecNodes[2].pos, ex, ey);
      g->SetColor(col);
			g->DrawLine(sx, sy, ex, ey);

			// And bezier curve
			jgui::jpoint3d_t op = vecNodes[0].pos;
			jgui::jpoint3d_t np = op;
			for (float t = 0; t < 1.0f; t += 0.01f)
			{
        jgui::jpoint3d_t
          t1 = {(1 - t)*(1 - t)*vecNodes[0].pos.x, (1 - t)*(1 - t)*vecNodes[0].pos.y, 0.0f},
          t2 = {2*(1 - t)*t*vecNodes[1].pos.x, 2*(1 - t)*t*vecNodes[1].pos.y, 0.0f},
          t3 = {t*t*vecNodes[2].pos.x, t*t*vecNodes[2].pos.y, 0.0f};
				
        np = {t1.x + t2.x + t3.x, t1.y + t2.y + t3.y, 0.0f};
				WorldToScreen(op, sx, sy);
				WorldToScreen(np, ex, ey);
        g->SetColor(col);
				g->DrawLine(sx, sy, ex, ey);
				op = np;
			}
		}
	}
};

class CAD : public jgui::Window {

	private:
    jgui::jpoint3d_t vOffset = {0.0f, 0.0f};
    jgui::jpoint3d_t vStartPan = {0.0f, 0.0f};
    float fScale = 10.0f;
    float fGrid = 1.0f;

    void WorldToScreen(const jgui::jpoint3d_t &v, int &nScreenX, int &nScreenY)
    {
      nScreenX = (int)((v.x - vOffset.x)*fScale);
      nScreenY = (int)((v.y - vOffset.y)*fScale);
    }

    void ScreenToWorld(int nScreenX, int nScreenY, jgui::jpoint3d_t &v)
    {
      v.x = (float)(nScreenX)/fScale + vOffset.x;
      v.y = (float)(nScreenY)/fScale + vOffset.y;
    }

    Shape* tempShape = nullptr;
    std::list<Shape*> listShapes;
    sNode *selectedNode = nullptr;
    jgui::jpoint3d_t vCursor = {0, 0};

  public:
    CAD():
      jgui::Window(0, 0, 720, 480)
    {
      jgui::jsize_t
        size = GetSize();

		  vOffset = {(float)(-size.width/2)/fScale, (float)(-size.height/2)/fScale};
    }

    virtual ~CAD()
		{
      for (std::list<Shape *>::iterator i=listShapes.begin(); i!=listShapes.end(); i++) {
        Shape *shape = (*i);

        delete shape;
      }
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
    {
			if (event->GetSymbol() == jevent::JKS_l) {
        tempShape = new Line();
      } else if (event->GetSymbol() == jevent::JKS_b) {
        tempShape = new Box();
      } else if (event->GetSymbol() == jevent::JKS_c) {
        tempShape = new Circle();
      } else if (event->GetSymbol() == jevent::JKS_s) {
        tempShape = new Curve();
      } else {
        return true;
      }

      selectedNode = tempShape->GetNextNode(vCursor);
      selectedNode = tempShape->GetNextNode(vCursor);

      return true;
    }

		virtual bool MousePressed(jevent::MouseEvent *event)
		{
      jgui::jpoint_t
        elocation = event->GetLocation();

      jgui::jpoint3d_t vMouse = {(float)elocation.x, (float)elocation.y};

      if (event->GetButton() == jevent::JMB_BUTTON2) {
        vStartPan = vMouse;
      }

      jgui::jpoint3d_t vMouseBeforeZoom;
      ScreenToWorld((int)vMouse.x, (int)vMouse.y, vMouseBeforeZoom);

      jgui::jpoint3d_t vMouseAfterZoom;
      ScreenToWorld((int)vMouse.x, (int)vMouse.y, vMouseAfterZoom);
      vOffset = {vOffset.x + (vMouseBeforeZoom.x - vMouseAfterZoom.x), vOffset.y + (vMouseBeforeZoom.y - vMouseAfterZoom.y), 0.0f};

      vCursor.x = floorf((vMouseAfterZoom.x + 0.5f) * fGrid);
      vCursor.y = floorf((vMouseAfterZoom.y + 0.5f) * fGrid);

      for (auto &shape : listShapes) {
        selectedNode = shape->HitNode(vCursor);

        if (selectedNode != nullptr) {
          selectedNode->pos = vCursor;

          break;
        }
      }

			return true;
		}

		virtual bool MouseReleased(jevent::MouseEvent *event)
		{
      if (tempShape != nullptr) {
        selectedNode = tempShape->GetNextNode(vCursor);

        if (selectedNode == nullptr) {
          tempShape->col = jgui::Color::White;
          listShapes.push_back(tempShape);
        }
      }

			return true;
		}

		virtual bool MouseMoved(jevent::MouseEvent *event)
		{
      jgui::jpoint_t
        elocation = event->GetLocation();

      jgui::jpoint3d_t vMouse = {(float)elocation.x, (float)elocation.y};

      if (event->GetButtons() & jevent::JMB_BUTTON2) {
        vOffset = {vOffset.x - (vMouse.x - vStartPan.x)/fScale, vOffset.y - (vMouse.y - vStartPan.y)/fScale, 0.0f};
        vStartPan = vMouse;
      }

      jgui::jpoint3d_t vMouseBeforeZoom;
      ScreenToWorld((int)vMouse.x, (int)vMouse.y, vMouseBeforeZoom);

      jgui::jpoint3d_t vMouseAfterZoom;
      ScreenToWorld((int)vMouse.x, (int)vMouse.y, vMouseAfterZoom);
      vOffset = {vOffset.x + (vMouseBeforeZoom.x - vMouseAfterZoom.x), vOffset.y + (vMouseBeforeZoom.y - vMouseAfterZoom.y), 0.0f};

      vCursor.x = floorf((vMouseAfterZoom.x + 0.5f) * fGrid);
      vCursor.y = floorf((vMouseAfterZoom.y + 0.5f) * fGrid);

      if (selectedNode != nullptr) {
        selectedNode->pos = vCursor;
      }

			return true;
		}

		virtual bool MouseWheel(jevent::MouseEvent *event)
		{
      if (event->GetClickCount() > 0) {
        fScale *= 1.1f;
      } else if (event->GetClickCount() < 0) {
        fScale *= 0.9f;
      }

      if (fScale < 1.0f) {
        fScale = 1.0f;
      }

      if (fScale > 100.0f) {
        fScale = 100.0f;
      }

			return true;
		}

		virtual void ShowApp() 
		{
			do {
				Repaint();

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
			} while (IsHidden() == false);
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::jsize_t
				size = GetSize();

			g->Clear();
			g->SetColor(0x20, 0x20, 0x80, 0xff);
		
      int sx, sy;
      int ex, ey;

      jgui::jpoint3d_t vWorldTopLeft, vWorldBottomRight;
      ScreenToWorld(0, 0, vWorldTopLeft);
      ScreenToWorld(size.width, size.height, vWorldBottomRight);

      vWorldTopLeft.x = floor(vWorldTopLeft.x);
      vWorldTopLeft.y = floor(vWorldTopLeft.y);
      vWorldBottomRight.x = ceil(vWorldBottomRight.x);
      vWorldBottomRight.y = ceil(vWorldBottomRight.y);

      for (float x = vWorldTopLeft.x; x < vWorldBottomRight.x; x += fGrid) {
        for (float y = vWorldTopLeft.y; y < vWorldBottomRight.y; y += fGrid) {
          WorldToScreen({ x, y }, sx, sy);
          g->SetRGB(0xff00f000, sx, sy);
        }
      }

      g->SetColor(jgui::Color::Gray);

      WorldToScreen({ 0,vWorldTopLeft.y }, sx, sy);
      WorldToScreen({ 0,vWorldBottomRight.y }, ex, ey);
      g->DrawLine(sx, sy, ex, ey);
      WorldToScreen({ vWorldTopLeft.x,0 }, sx, sy);
      WorldToScreen({ vWorldBottomRight.x,0 }, ex, ey);
      g->DrawLine(sx, sy, ex, ey);

      Shape::fWorldScale = fScale;
      Shape::vWorldOffset = vOffset;

      for (auto &shape : listShapes) {
        shape->DrawYourself(g);
        shape->DrawNodes(g);
      }

      if (tempShape != nullptr) {
        tempShape->DrawYourself(g);
        tempShape->DrawNodes(g);
      }

      WorldToScreen(vCursor, sx, sy);
      g->SetColor(jgui::Color::White);
      g->DrawCircle(sx, sy, 3);

      g->SetFont(GetTheme()->GetFont("window.font"));
      g->DrawString("X:[" + std::to_string(vCursor.x) + "], Y:[" + std::to_string(vCursor.y) + "]", 10, 10);
    }

};

int main(int argc, char **argv)
{
  jgui::Application::Init(argc, argv);

  CAD app;

  app.SetTitle("CAD");
  app.Exec();

  jgui::Application::Loop();

  return 0;
}

