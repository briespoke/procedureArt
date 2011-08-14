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
#include "geometry.h"
#include <limits.h>

#define ROTATE_FACTOR 0.01

float rotate1 = 0.0;
float rotate2 = 0.0;

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 640.0/480.0, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
}

void procedureArt_ShowLine(struct geometry_TreeNode * node)
{
	int i;
	if (node->children == 0)
	{
		for (i = 0; i < node->numLines; i++)
		{
			glBegin(GL_LINES);
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(node->lines[i]->x1, node->lines[i]->y1, -5.0);
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(node->lines[i]->x2, node->lines[i]->y2, -5.0);
			glEnd();

		}
	}
	else
	{
		for (i = 0; i < TREE_ORDER; i++)
		{
			procedureArt_ShowLine(node->children[i]);
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

	/*
	glPushMatrix();
	glTranslatef(0.0, 0.0, -5.0);
	glRotatef(rotate1, 5.0, 5.0, 0.0);
	glRotatef(rotate1, 0.0, 5.0, 5.0);
	glRotatef(rotate1, 5.0, 0.0, 5.0);
	glTranslatef(0.0, 0.0, 5.0);

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(0.0, 2.0, -5.0);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-2.0, -2.0, -5.0);
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(2.0, -2.0, -5.0);
	glEnd();
	glPopMatrix();


	//printf("%f\n", adjustedRotate);

	glPushMatrix();
	glTranslatef(0.0, 0.0, -5.0);
	glRotatef(rotate2 * -1.0, 5.0, 5.0, 0.0);
	glRotatef(rotate2 * -1.0, 0.0, 5.0, 5.0);
	glRotatef(rotate2 * -1.0, 5.0, 0.0, 5.0);
	glTranslatef(0.0, 0.0, 5.0);
	glPopMatrix();
*/
	glPushMatrix();
	glTranslatef(0.0, 0.0, -5.0);
	glRotatef(adjustedRotate, 5.0, 5.0, 0.0);
	glRotatef(adjustedRotate, 0.0, 5.0, 5.0);
	glRotatef(adjustedRotate, 5.0, 0.0, 5.0);
	glTranslatef(0.0, 0.0, 5.0);
	procedureArt_ShowLine(geometry_GetRootNode());
}

float randRange(float lower, float upper)
{
	float value = rand() * 1.0;
	float valueFloat = value / RAND_MAX;
	return (upper - lower) * valueFloat + lower;
}
void addLine()
{
	float x1 = randRange(-5.0, 5.0);
	float y1 = randRange(-5.0, 5.0);
	float x2 = randRange(-5.0, 5.0);
	float y2 = randRange(-5.0, 5.0);
	struct geometry_LineSegment * line = geometry_LineSegment_construct(x1, y1, x2, y2);
	geometry_addLine(line);
	//printf("%f, %f, %f, %f\n", x1, y1, x2, y2);
}

int SDL_main(int argc, char *argv[])
{
	geometry_init(5.0, 5.0, -5.0, -5.0);
	int i;
	for (i = 0; i < 200; i++)
	{
		addLine();
	}

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Surface* screen;

	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE | SDL_OPENGL);
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
		//printf("%d\n", remainder);
		display(remainder);
		SDL_GL_SwapBuffers();
		remainder = SDL_GetTicks() - start;
		if (1000/5 > remainder)
		{
				SDL_Delay(remainder);

		}
	}
	SDL_Quit();

	geometry_WalkTree();
	geometry_destroy();
	return 0;
}
