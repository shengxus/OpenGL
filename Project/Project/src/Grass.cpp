#include "incl/Grass.h"
#include "incl\Billboards.h"


void Grass::render(){

	float spacing[3] = { -0.05, 0, 0.05};
	int space = 0;
	float offset[3] = { -0.05, 0, 0.05};

	for( int i = -20; i < 20; i++)
		for(int j = -20; j < 20; j++) {
			glPushMatrix();

			if(space != 2){ space++; }
			else{ space = 0;}

			glTranslatef(offset[space], 0, offset[space]);
			glTranslatef(i * 10.0*spacing[space], 0, j * 10.0*spacing[space]);
			
			billboardBegin();
				
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);glVertex3f(-0.8f, 0.0f, 0.0f);
				glTexCoord2f(1,0);glVertex3f(0.8f, 0.0f, 0.0f);
				glTexCoord2f(1,1);glVertex3f(0.8f, 1.0f,  0.0f);
				glTexCoord2f(0,1);glVertex3f(-0.8f, 1.0f,  0.0f);
			glEnd();
				
			billboardEnd();
			glPopMatrix();

		}
	glBindTexture(GL_TEXTURE_2D,0);

}