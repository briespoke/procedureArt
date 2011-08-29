/*
 ============================================================================
 Name        : procedureArt.c
 Author      : Brian Murphy
 Version     : 0.1
 Copyright   : Brian Murphy, 2011
 Description : Procedural art generation test program
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h>

#include "header.h"
#include "util.h"
#include "geometry.h"
#include "soil/SOIL.h"

#define ROTATE_FACTOR 0.01
#define BRANCHES 5
#define DEPTH 5
#define MAX_LINES 5000

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

int numLines = 0;
float rotate1 = 0.0;
float rotate2 = 0.0;
GLuint texture;

#define GROUND -7.0

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, WINDOW_WIDTH/WINDOW_HEIGHT, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);


	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glClearDepth(1.0);
	glEnable( GL_TEXTURE_2D );

	//GLuint texture;
	texture = SOIL_load_OGL_texture
	(
//		"/Users/wavenger/Documents/workspace/procedureArt/Debug/building.png",
		"C:\\Development\\procedureArt\\Debug\\building.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);

	/* check for an error during the load process */
	if( 0 == texture )
	{
		printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
	}
	/*
	// allocate a texture name
	glGenTextures( 1, &texture );

	// select our current texture
	glBindTexture( GL_TEXTURE_2D, texture );

	// select modulate to mix texture with color for shading
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	// when texture area is small, bilinear filter the closest mipmap
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	                 GL_LINEAR_MIPMAP_NEAREST );
	// when texture area is large, bilinear filter the original
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// the texture wraps over at the edges (repeat)
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GLU_RGB, GL_UNSIGNED_BYTE, texture);
	*/

}

void procedureArt_DrawLines(float adjustedRotate)
{
	struct geometry_LineSegment *line;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, texture);

	geometry_ResetLineIterator();
	while ((line = geometry_GetNextLine()) != 0)
	{
		if (line->id < (SDL_GetTicks() / 5.0))
		{

			glPushMatrix();
			glTranslatef(0.0, 0.0, GROUND);
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
			}

			float myLength;

			length(line, & myLength);
			//printf("%f\n", myLength);

			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0, 0.0);         glVertex3f(line->x1, line->y1, GROUND);
			glTexCoord2f(1.0, 0.0);         glVertex3f(line->x2, line->y2, GROUND);
			glTexCoord2f(1.0, 1.0); 		glVertex3f(line->x1, line->y1, GROUND + line->height);
			glTexCoord2f(0.0, 1.0);         glVertex3f(line->x2, line->y2, GROUND + line->height);
			glEnd();

			GLfloat light_ambient[] =
			{0.2, 0.2, 0.2, 1.0};
			GLfloat light_diffuse[] =
			{0.5, 0.5, 0.5, 0.5};
			GLfloat light_specular[] =
			{0.5, 0.5, 0.5, 1.0};
			GLfloat light_position[] =
			{0.0, 0.0, 0.0, -5.0};

			glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
			glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
			glLightfv(GL_LIGHT0, GL_POSITION, light_position);

			glPopMatrix();
		}
	}
}
void display(Uint32 catchUp)
{
	float adjustedRotate;
	adjustedRotate = catchUp * ROTATE_FACTOR;
	rotate1 += adjustedRotate;
	rotate2 += adjustedRotate;
	procedureArt_DrawLines(rotate1);
}

struct geometry_LineSegment * addLine(float x1, float y1, float x2, float y2, float height)
{
	struct geometry_LineSegment * line = geometry_LineSegment_construct(x1, y1, x2, y2, height);
	geometry_CheckCollision(line, &x2, &y2);

	struct geometry_LineSegment * disposableLine =  geometry_LineSegment_construct(x1, y1, x2, y2, height);
	geometry_addLine(disposableLine);
	return line;
}
struct geometry_LineSegment * addLineRandom()
{
	if (rand() % 2)
	{
		float height = randRange(0.0, 0.1);
		float x1 = -5;
		float y1 = randRange(-5.0, 5.0);
		float x2 = 5;
		float y2 = randRange(-5.0, 5.0);
		return addLine(x1, y1, x2, y2, height);
	}
	else
	{
		float height = randRange(0.0, 0.1);
		float x1 = randRange(-5.0, 5.0);
		float y1 = -5;
		float x2 = randRange(-5.0, 5.0);
		float y2 = 5;
		return addLine(x1, y1, x2, y2, height);
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

				float height = randRange(0.0, 2.5);
				struct geometry_LineSegment * newLine = addLine(newX1, newY1, newX2, newY2, height);

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

	screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_HWSURFACE | SDL_OPENGL);
	int running = 1;
	Uint32 start;
	Uint32 remainder = 0;
	Uint32 catchup = 0;
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
		display(1000/60 + catchup);
		SDL_GL_SwapBuffers();
		remainder = SDL_GetTicks() - start;
		if (remainder < 1000 / 60)
		{
			catchup = 0;
			SDL_Delay(1000 / 60 - remainder);
		}
		else
		{
			catchup = remainder - 1000 / 60;
		}
	}
	SDL_Quit();

	//geometry_WalkTree();
	geometry_destroy();
	return 0;
}
