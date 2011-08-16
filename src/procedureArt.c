/*
 ============================================================================
 Name        : procedure2.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <SDL.h>
#include <gl.h>
#include <glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>

#include "util.h"
#include "geometry.h"

#define ROTATE_FACTOR 0.01
#define BRANCHES 5
#define DEPTH 5
#define MAX_LINES 5000

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

int numLines = 0;
float rotate1 = 0.0;
float rotate2 = 0.0;

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, WINDOW_WIDTH/WINDOW_HEIGHT, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
}

void procedureArt_ShowLine(struct geometry_TreeNode * node, float adjustedRotate)
{
	int i;
	if (node->children == 0)
	{
		for (i = 0; i < node->numLines; i++)
		{
			struct geometry_LineSegment * line = node->lines[i];
			if (line->id < (SDL_GetTicks() / 5.0))
			{

				glPushMatrix();
				glTranslatef(0.0, 0.0, -7.0);
				glRotatef(adjustedRotate, 5.0, 5.0, 0.0);
				glRotatef(adjustedRotate, 0.0, 5.0, 5.0);
				glRotatef(adjustedRotate, 5.0, 0.0, 5.0);
				glTranslatef(0.0, 0.0, 7.0);

				line->dz = 7.0 - (SDL_GetTicks() / 5.0 - line->id) / 10;
				if (line->dz > 0.0)
				{
					glTranslatef(0.0, 0.0, 7.0 - line->dz);
					glRotatef(line->dz * 10, line->rx, line->ry, line->rz);
					glTranslatef(0.0, 0.0, line->dz - 7.0);
					glTranslatef(0.0, 0.0, line->dz);


					glBegin(GL_LINES);
					glVertex3f(line->x1, line->y1, -7.0);
					glVertex3f(line->x2, line->y2, -7.0);
					glEnd();
				}
				else
				{
					glBegin(GL_LINES);
					glVertex3f(line->x1, line->y1, -7.0);
					glVertex3f(line->x2, line->y2, -7.0);
					glEnd();
				}
				glPopMatrix();
			}

		}
	}
	else
	{
		for (i = 0; i < TREE_ORDER; i++)
		{
			procedureArt_ShowLine(node->children[i], adjustedRotate);
		}
	}

}
void display(Uint32 catchUp)
{
	float adjustedRotate;
	adjustedRotate = catchUp * ROTATE_FACTOR;
	rotate1 += adjustedRotate;
	rotate2 += adjustedRotate;
	glClear(GL_COLOR_BUFFER_BIT);
	procedureArt_ShowLine(geometry_GetRootNode(), rotate1);
}

struct geometry_LineSegment * addLine(float x1, float y1, float x2, float y2)
{
	struct geometry_LineSegment * line = geometry_LineSegment_construct(x1, y1, x2, y2);
	geometry_CheckCollision(line, geometry_GetRootNode(), &x2, &y2);

	struct geometry_LineSegment * disposableLine =  geometry_LineSegment_construct(x1, y1, x2, y2);
	geometry_addLine(disposableLine);
	return line;
}
struct geometry_LineSegment * addLineRandom()
{
	if (rand() % 2)
	{
		float x1 = -5;
		float y1 = randRange(-5.0, 5.0);
		float x2 = 5;
		float y2 = randRange(-5.0, 5.0);
		return addLine(x1, y1, x2, y2);
	}
	else
	{
		float x1 = randRange(-5.0, 5.0);
		float y1 = -5;
		float x2 = randRange(-5.0, 5.0);
		float y2 = 5;
		return addLine(x1, y1, x2, y2);
	}

}
void addLinesPerpendicularRecurse(struct geometry_LineSegment * oldLine, int branches, int maxDepth)
{
	float m, c;
	if (branches > 0 && maxDepth > 0)
	{
		if (numLines < MAX_LINES && fabs(oldLine->x1 - oldLine->x2) > 0.1 && slope(oldLine, &m))
		{
			displacement(oldLine, &c);
			float xRange = fabs(oldLine->y2 - oldLine->y1);

			int i;
			for (i = 0; i < branches; i++)
			{
				float newX1 = randRange(oldLine->x1, oldLine->x2);
				float newY1 = m * newX1 + c;

				float newM = -1.0 / m;
				float newC = newY1 - newX1 * newM;

				float newX2Min = newX1 - xRange * 1.5;
				if (newX2Min < -5.0)
				{
					newX2Min = -5.0;
				}
				float newX2Max = newX1 + xRange * .9;
				if (newX2Max > 5.0)
				{
					newX2Max = 5.0;
				}
				float newX2 = randRange(newX2Min, newX2Max);
				float newY2 = newM * newX2 + newC;

				struct geometry_LineSegment * newLine = addLine(newX1, newY1, newX2, newY2);

				numLines++;
				addLinesPerpendicularRecurse(newLine, branches -1, maxDepth - 1);
				free(newLine);
			}
		}
	}
}
int main(int argc, char *argv[])
{
	int random_seed = time(NULL);
	srand(random_seed);
	printf("RANDOM SEED: %d\n", random_seed);
	geometry_init(5.0, 5.0, -5.0, -5.0);

	int i = 0;

	for(i = 0; i < 10; i++)
	{
		struct geometry_LineSegment * lastLine = addLineRandom();
		addLinesPerpendicularRecurse(lastLine, BRANCHES, DEPTH);
		free(lastLine);
	}
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface* screen;

	screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_OPENGL);
	int running = 1;
	Uint32 start;
	Uint32 remainder;
	init();

	while (running)
	{
		SDL_Event event;

		start = SDL_GetTicks();

		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				running = 0;
				break;
			}
		}
		display(remainder);
		SDL_GL_SwapBuffers();
		remainder = SDL_GetTicks() - start;
		if (1000/5 > remainder)
		{
				SDL_Delay(remainder);

		}
	}
	SDL_Quit();

	//geometry_WalkTree();
	geometry_destroy();
	return 0;
}
