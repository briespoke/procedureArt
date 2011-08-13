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

#define ROTATE_FACTOR 0.1

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

void display(Uint32 catchUp)
{
	float adjustedRotate;
	adjustedRotate = catchUp * ROTATE_FACTOR;
	rotate1 += adjustedRotate;
	rotate2 += adjustedRotate;
	glClear(GL_COLOR_BUFFER_BIT);

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

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0, -2.0, -5.0);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-2.0, 2.0, -5.0);
	glEnd();
	glPopMatrix();
}

int main(int argc, char *argv[])
{
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
	return 0;

}
