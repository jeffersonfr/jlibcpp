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
   Ported to DirectFB by Denis Oliver Kropp <dok@convergence.de>

   Written by Mark Vojkovich <markv@valinux.com>

   Modified by Pierluigi Passaro <info@phoenixsoftware.it>
   Based on originale df_knuckles demo: modified to open generic Wavefront .obj
*/
#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jbufferedimage.h"
#include "jgui/jraster.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#undef  CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

enum RenderType {
  FLAT_SHADED,
  WIRE_FRAME,
  AS_POINTS
};

struct Vertex {
  float x, y, z;
};

struct Triangle {
  int a, b, c;
};

struct Tri3D {
  Vertex *a, *b, *c;
  float depth;
  Vertex normal;
  struct Tri3D *next;
};

static float IdentityMatrix[9] = { 
  1.0, 0.0, 0.0,
  0.0, 1.0, 0.0,
  0.0, 0.0, 1.0
};

class Render3D : public jgui::Window {

	private:
    float CTM[9];
    float Scratch[9];
    float Cosine[3600];
    float Sine[3600];

    int Obj3dVerticiesCount = 0;
    int Obj3dTrianglesCount = 0;
    Vertex* Obj3dVerticies = 0;
    Triangle* Obj3dTriangles = 0;
    float Obj3dMaxCoordinate = 0;

    bool BackfaceCull = false;
    bool DoubleBuffer = true;
    bool Lighting = true;
    RenderType PrimitiveType = FLAT_SHADED;
    float ScaleFactor = 0.0012;

    Vertex Light1 = {0.0,  0.0, 1.0};
    Vertex Light2 = {0.2, -0.2, 0.4};

    Tri3D* Triangles = NULL;
    Vertex* TransformedVerticies = NULL;

		int dxL, dyL;

	private:
    void SetMaxCoordinate(void)
    {
      int vertex;
      float coordinate;

      for (vertex = 0; vertex < Obj3dVerticiesCount; vertex ++) {
        coordinate = fabsf(Obj3dVerticies[vertex].x);

        if (Obj3dMaxCoordinate < coordinate) {
          Obj3dMaxCoordinate = coordinate;
        }

        coordinate = fabsf(Obj3dVerticies[vertex].y);

        if (Obj3dMaxCoordinate < coordinate) {
          Obj3dMaxCoordinate = coordinate;
        }

        coordinate = fabsf(Obj3dVerticies[vertex].z);

        if (Obj3dMaxCoordinate < coordinate) {
          Obj3dMaxCoordinate = coordinate;
        }
      }
    }

    void ParseObj3d(FILE *fp, std::string path)
    {
#define OBJ3D_TOKEN " "
#define OBJ3D_LINE_LEN_MAX 80

      char line[OBJ3D_LINE_LEN_MAX];
      char* pch = NULL;
      int vertex = 0;
      int triangle = 0;

      // round 1: scan the file to count vertex and triangle
      while (fgets(line, OBJ3D_LINE_LEN_MAX, fp) != NULL) {
        pch = strtok(line, OBJ3D_TOKEN);
        if (pch == NULL) {
          continue;
        } else if (strcmp(pch, "v") == 0) {
          Obj3dVerticiesCount++;
        } else if (strcmp(pch, "f") == 0) {
          Obj3dTrianglesCount++;
        }
      }

      Obj3dVerticies = (Vertex*)calloc(Obj3dVerticiesCount, sizeof(Vertex));
      Obj3dTriangles = (Triangle*)calloc(Obj3dTrianglesCount, sizeof(Triangle));

      printf("Found %dv, %dt\n", Obj3dVerticiesCount, Obj3dTrianglesCount);

      // round 2: rescan the file to populate vertex and triangle
      if (fseek(fp, 0, SEEK_SET) != 0) {
        printf("%s %d: fseek error %d, trying reopening\n", __func__, __LINE__, errno);
        fclose(fp);
        fp = fopen(path.c_str(), "r");

        if (fp == NULL) {
          printf("%s %d: error %d reopening %s\n", __func__, __LINE__, errno, path.c_str());
          exit(0);
        }
      }

      while (fgets(line, OBJ3D_LINE_LEN_MAX, fp) != NULL) {
        pch = strtok(line, OBJ3D_TOKEN);

        if (pch == NULL) {
          continue;
        } else if (strcmp(pch, "v") == 0) {
          if (vertex == Obj3dVerticiesCount) {
            printf("Found %s %d: ignoring unexpected line %s\n", __func__, __LINE__, line);
            continue;
          }
          Obj3dVerticies[vertex].x = atof(strtok(NULL, OBJ3D_TOKEN));
          Obj3dVerticies[vertex].y = atof(strtok(NULL, OBJ3D_TOKEN));
          Obj3dVerticies[vertex].z = atof(strtok(NULL, OBJ3D_TOKEN));
          vertex++;
        } else if (strcmp(pch, "f") == 0) {
          if (triangle == Obj3dTrianglesCount) {
            printf("%s %d: ignoring unexpected line %s\n", __func__, __LINE__, line);
            continue;
          }

          Obj3dTriangles[triangle].a = atoi(strtok(NULL, OBJ3D_TOKEN)) - 1;
          Obj3dTriangles[triangle].b = atoi(strtok(NULL, OBJ3D_TOKEN)) - 1;
          Obj3dTriangles[triangle].c = atoi(strtok(NULL, OBJ3D_TOKEN)) - 1;
          triangle++;
        }
      }

      printf("Loaded %dv, %dt\n", vertex, triangle);
    }

    int LoadObj3d(std::string path)
    {
      FILE* fp = NULL;

      if (path.empty() == true) {
        printf("%s %d: path NULL or empty\n", __func__, __LINE__);
        return -1;
      }

      if ((fp = fopen(path.c_str(), "r")) == NULL) {
        printf("%s %d: Unable to open file %s\n", __func__, __LINE__, path.c_str());
        return -1;
      }

      ParseObj3d(fp, path);
      fclose(fp);
      SetMaxCoordinate();

      return 0;
    }

    void MultiplyMatrix(float *A, float *B)
    {
      float tmp[3];
      int row, column;

      // make sure to compile with loop unrolling
      for(row = 0; row < 3; row++) {
        memcpy(tmp, A + (row * 3), sizeof(float) * 3);

        for(column = 0; column < 3; column++) {
          A[(row * 3) + column] = 
            (tmp[0] * B[column + 0]) +
            (tmp[1] * B[column + 3]) +
            (tmp[2] * B[column + 6]);
        }
      }
    }

    void RotateLight(Vertex *light, int dx, int dy)
    {
      float matrix[9];
      float X, Y, Z;

      memcpy(matrix, IdentityMatrix, sizeof(matrix));

      if (dx) {
        while (dx >= 3600) {
          dx -= 3600;
        }

        while (dx < 0) {
          dx += 3600;
        }

        bzero(Scratch, sizeof(Scratch));

        Scratch[4] = Cosine[dx];
        Scratch[8] = Cosine[dx];
        Scratch[5] = -Sine[dx];
        Scratch[7] = Sine[dx];
        Scratch[0] = 1.0;

        MultiplyMatrix(matrix, Scratch);
      }

      if (dy) {
        while (dy >= 3600) {
          dy -= 3600;
        }

        while (dy < 0) {
          dy += 3600;
        }

        bzero(Scratch, sizeof(Scratch));

        Scratch[0] = Cosine[dy];
        Scratch[2] = Sine[dy];
        Scratch[6] = -Sine[dy];
        Scratch[8] = Cosine[dy];
        Scratch[4] = 1.0;

        MultiplyMatrix(matrix, Scratch);
      }

      X = light->x;
      Y = light->y;
      Z = light->z;

      light->x =  (X * matrix[0]) + (Y * matrix[1]) + (Z * matrix[2]); 
      light->y =  (X * matrix[3]) + (Y * matrix[4]) + (Z * matrix[5]);
      light->z =  (X * matrix[6]) + (Y * matrix[7]) + (Z * matrix[8]);
    }

    void MultiplyVector(Vertex *V, Vertex *R) 
    {
      float divisor;

      R->x =  (V->x * CTM[0]) + (V->y * CTM[1]) + (V->z * CTM[2]); 
      R->y =  (V->x * CTM[3]) + (V->y * CTM[4]) + (V->z * CTM[5]);
      R->z =  (V->x * CTM[6]) + (V->y * CTM[7]) + (V->z * CTM[8]); 

      divisor = (R->z + 350.0) / 250.0;

      if (divisor < 0) {
        divisor = -divisor;
      }

      R->x *= divisor;
      R->y *= divisor;
    }

    void InitMatrix(void)
    {
      int i;

      for (i = 0; i < 3600; i++) {
        Cosine[i] = (float)cos(2.0 * M_PI * (float)i / 3600.0);
        Sine[i]   = (float)sin(2.0 * M_PI * (float)i / 3600.0);
      }
    }

    void Scale(float x, float y, float z)
    {
      bzero(Scratch, sizeof(Scratch));

      Scratch[0] = x;
      Scratch[4] = y;
      Scratch[8] = z;

      MultiplyMatrix(CTM, Scratch); 
    }

    void Rotate(int degree_tenths, char axis)
    {
      while (degree_tenths >= 3600) {
        degree_tenths -= 3600;
      }

      while (degree_tenths < 0) {
        degree_tenths += 3600;
      }

      bzero(Scratch, sizeof(Scratch));

      switch(axis){
        case 'x':
          Scratch[4] = Cosine[degree_tenths];
          Scratch[8] = Cosine[degree_tenths];
          Scratch[5] = -Sine[degree_tenths];
          Scratch[7] = Sine[degree_tenths];
          Scratch[0] = 1.0;
          break;
        case 'y':
          Scratch[0] = Cosine[degree_tenths];
          Scratch[2] = Sine[degree_tenths];
          Scratch[6] = -Sine[degree_tenths];
          Scratch[8] = Cosine[degree_tenths];
          Scratch[4] = 1.0;
          break;
        case 'z':
          Scratch[0] = Cosine[degree_tenths];
          Scratch[1] = -Sine[degree_tenths];
          Scratch[3] = Sine[degree_tenths];
          Scratch[4] = Cosine[degree_tenths];
          Scratch[8] = 1.0;
          break;
      }

      MultiplyMatrix(CTM, Scratch); 
    }

    void SetupMatrix(float scale)
    {
      memcpy(CTM, IdentityMatrix, sizeof(CTM));

      Scale(scale, -scale, scale);
      Rotate(1800, 'y');
    }

    void LoadIt (std::string path)
    {
      if (LoadObj3d(path) == 0) {
        ScaleFactor = 0.25 / Obj3dMaxCoordinate;
        Triangles = (Tri3D*)calloc(Obj3dTrianglesCount, sizeof(Tri3D));
        TransformedVerticies = (Vertex*)calloc(Obj3dVerticiesCount, sizeof(Vertex));
      }
    }

    void DrawTriangle(jgui::Raster &raster, float light1, float light2, Tri3D* tri)
    {
      uint8_t r, g, b;
      int X, Y;

      X = SCREEN_WIDTH >> 1;
      Y = SCREEN_HEIGHT >> 1;

      r = light1 * 255.0;
      g = (light1 * light1) * 255.0;
      b = light1 * 64.0 + light2 * 64.0;

      raster.SetColor(0xff000000 | r << 0x10 | g << 0x08 | b);

      switch (PrimitiveType) {
        case FLAT_SHADED:
          raster.FillTriangle(
						{(int)(tri->a->x + X), (int)(tri->a->y + Y)}, 
						{(int)(tri->b->x + X), (int)(tri->b->y + Y)}, 
						{(int)(tri->c->x + X), (int)(tri->c->y + Y)});
          break;
        case WIRE_FRAME:
          raster.DrawTriangle(
						{(int)(tri->a->x + X), (int)(tri->a->y + Y)}, 
						{(int)(tri->b->x + X), (int)(tri->b->y + Y)}, 
						{(int)(tri->c->x + X), (int)(tri->c->y + Y)});
          break;
        default:
          break;
      }
    }

	public:
		Render3D(std::string path):
			jgui::Window({SCREEN_WIDTH, SCREEN_HEIGHT})
		{
      LoadIt(path);

      ScaleFactor *= SCREEN_HEIGHT;

      InitMatrix();
      SetupMatrix(ScaleFactor);

      dxL = 11;
      dyL = 7;
		}

		virtual ~Render3D()
		{
		}

    virtual bool KeyPressed(jevent::KeyEvent *event)
    {
			if (jgui::Window::KeyPressed(event) == true) {
				return true;
			}

			int step = 20;

			if (event->GetSymbol() == jevent::JKS_CURSOR_UP) {
				Rotate (step * 2, 'x');
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_DOWN) {
				Rotate (-step * 2, 'x');
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_LEFT) {
				Rotate (step * 2, 'y');
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_RIGHT) {
				Rotate (-step * 2, 'y');
      } else if (event->GetSymbol() == jevent::JKS_SPACE) {
        if (PrimitiveType == FLAT_SHADED) {
          PrimitiveType = WIRE_FRAME;
        } else {
          PrimitiveType = FLAT_SHADED;
        }
      } else if (event->GetSymbol() == jevent::JKS_TAB) {
        BackfaceCull = !BackfaceCull;
      } else if (event->GetSymbol() == jevent::JKS_l) {
        if (rand()%50 == 0) {
          dxL += rand()%5 - 2;
        }
          
        if (rand()%50 == 0) {
          dyL += rand()%5 - 2;
        }

        if (dxL | dyL) {
          RotateLight(&Light1, dxL, dyL);
        }
      }

      Repaint();

      return true;
    }

    virtual bool MouseMove(jevent::MouseEvent *event)
    {
      jgui::jpoint_t
        elocation = event->GetLocation();

      Rotate (-elocation.x * 2, 'x');
      Rotate (elocation.y * 2, 'y');

      Repaint();

      return true;
    }

		virtual void Paint(jgui::Graphics *g) 
		{
      jgui::Window::Paint(g);

      // jgui::jsize_t
      //   size = GetSize();

      int count, NumUsed = 0;
      Tri3D *current = Triangles;
      Tri3D *first = Triangles;
      Tri3D *pntr, *prev;
      Triangle *points = Obj3dTriangles;
      Vertex *transPoints = TransformedVerticies;
      Vertex *untransPoints = Obj3dVerticies;
      Vertex A, B;
      float length;
      float light1, light2;

      count = Obj3dVerticiesCount;

      while (count--) {
        MultiplyVector(untransPoints++, transPoints++);
      }

      first->next = NULL;

      count = Obj3dTrianglesCount;

      while (count--) {
        current->a = TransformedVerticies + points->a;
        current->b = TransformedVerticies + points->b;
        current->c = TransformedVerticies + points->c;

        A.x = current->b->x - current->a->x;
        A.y = current->b->y - current->a->y;
        A.z = current->b->z - current->a->z;

        B.x = current->c->x - current->b->x;
        B.y = current->c->y - current->b->y;
        B.z = current->c->z - current->b->z;

        current->normal.z = (A.x * B.y) - (A.y * B.x);

        if (BackfaceCull && (current->normal.z >= 0.0)) {
          points++;
          continue;
        }

        current->normal.y = (A.z * B.x) - (A.x * B.z);
        current->normal.x = (A.y * B.z) - (A.z * B.y);

        current->depth = current->a->z + current->b->z + current->c->z;

        // Not the smartest sorting algorithm
        if (NumUsed) {
          prev = NULL;
          pntr = first;

          while (pntr) {
            if (current->depth > pntr->depth) {
              if (pntr->next) {
                prev = pntr;
                pntr = pntr->next;
              } else {
                pntr->next = current;
                current->next = NULL;
                break;
              }
            } else {
              if (prev) {
                prev->next = current;
                current->next = pntr;
              } else {
                current->next = pntr;
                first = current;
              }
              break;
            }
          }
        }

        NumUsed++;
        current++;
        points++;
      }

      jgui::Raster 
				raster(g->GetCairoSurface());

      while (first) {
        if (Lighting) {
          length = ((first->normal.x * first->normal.x) + (first->normal.y * first->normal.y) + (first->normal.z * first->normal.z));
          length = (float)sqrtf((float)length);
          light1 = -((first->normal.x * Light1.x) + (first->normal.y * Light1.y) + (first->normal.z * Light1.z)) / length;
          light1 = CLAMP (light1, 0.0, 1.0);
          light2 = abs((first->normal.x * Light2.x) + (first->normal.y * Light2.y) + (first->normal.z * Light2.z)) / length;
          light2 = CLAMP (light2, 0.0, 1.0);
        } else {
          light1 = 1.0;
          light2 = 0.0;
        }

        DrawTriangle(raster, light1, light2, first);

        first = first->next;
      }
    }

};

int main(int argc, char **argv)
{
  if (argc != 2) {
    printf("usage: %s <file.obj>\n", argv[0]);

    return -1;
  }

	jgui::Application::Init(argc, argv);

	Render3D app(argv[1]);

	app.SetTitle("Render3D");
  app.Exec();

	jgui::Application::Loop();

	return 0;
}
