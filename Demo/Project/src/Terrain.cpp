#include "incl\Terrain.h"
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <gl\glut.h>
#include "incl\Global.h"

#define LEFT 0
#define RIGHT 1
using namespace std;

static int frameVar = 50;
int	Terrain::nextTriNode;
TriTreeNode Terrain::triPool[POOL_SIZE];
/* patch class */

void Patch::init(int heightX, int heightY, unsigned char *hMap) {
	reset();
	// Store Patch offsets for the world and heightmap.
	offsetX = heightX;
	offsetY = heightY;
	// Store pointer to first byte of the height data for this patch.
	heightMap = &hMap[heightY * MAP_SIZE + heightX];
	varDirty = true;
}

void Patch::reset() {
	// Attach the two m_Base triangles together
	baseLeft.BaseNeighbor = &baseRight;
	baseRight.BaseNeighbor = &baseLeft;
	// clear other relationships
	baseLeft.LeftChild = baseLeft.RightChild = baseRight.LeftChild
		= baseLeft.RightChild = baseLeft.RightNeighbor = baseLeft.LeftNeighbor
		= baseRight.RightNeighbor = baseRight.LeftNeighbor = NULL;
}

void Patch::split(TriTreeNode *tri) {
	// Already splited, no need to do it again.
	if (tri->LeftChild)
		return;
	// If this triangle is not in a proper diamond, force split our base neighbor
	if (tri->BaseNeighbor && (tri->BaseNeighbor->BaseNeighbor != tri))
		split(tri->BaseNeighbor);
	// Create children and link into mesh
	tri->LeftChild = Terrain::triAllocate();
	tri->RightChild = Terrain::triAllocate();
	// If creation failed, just exit.
	if (!tri->LeftChild)
		return;
	// Fill in the information we can get from the parent (neighbor pointers)
	tri->LeftChild->BaseNeighbor = tri->LeftNeighbor;
	tri->LeftChild->LeftNeighbor = tri->RightChild;
	tri->RightChild->BaseNeighbor = tri->RightNeighbor;
	tri->RightChild->RightNeighbor = tri->LeftChild;
	// Link our Left Neighbor to the new children
	if (tri->LeftNeighbor != NULL) {
		if (tri->LeftNeighbor->BaseNeighbor == tri)
			tri->LeftNeighbor->BaseNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->LeftNeighbor == tri)
			tri->LeftNeighbor->LeftNeighbor = tri->LeftChild;
		else if (tri->LeftNeighbor->RightNeighbor == tri)
			tri->LeftNeighbor->RightNeighbor = tri->LeftChild;
	}
	// Link our Right Neighbor to the new children
	if (tri->RightNeighbor != NULL) {
		if (tri->RightNeighbor->BaseNeighbor == tri)
			tri->RightNeighbor->BaseNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->RightNeighbor == tri)
			tri->RightNeighbor->RightNeighbor = tri->RightChild;
		else if (tri->RightNeighbor->LeftNeighbor == tri)
			tri->RightNeighbor->LeftNeighbor = tri->RightChild;
	}
	// Link our Base Neighbor to the new children
	if (tri->BaseNeighbor != NULL) {
		if (tri->BaseNeighbor->LeftChild) {
			tri->BaseNeighbor->LeftChild->RightNeighbor = tri->RightChild;
			tri->BaseNeighbor->RightChild->LeftNeighbor = tri->LeftChild;
			tri->LeftChild->RightNeighbor = tri->BaseNeighbor->RightChild;
			tri->RightChild->LeftNeighbor = tri->BaseNeighbor->LeftChild;
		}
		else
			split(tri->BaseNeighbor);  // Base Neighbor (in a diamond with us) was not split yet, so do that now.
	}
	else {
		// An edge triangle, trivial case.
		tri->LeftChild->RightNeighbor = NULL;
		tri->RightChild->LeftNeighbor = NULL;
	}
}

void Patch::recursTessellate(TriTreeNode *tri, int leftX, int leftY,
	int rightX, int rightY, int apexX, int apexY, int node, const Camera &camera) {
	float TriVariance;
	int centerX = (leftX + rightX) >> 1; // Compute X coordinate of center of Hypotenuse
	int centerY = (leftY + rightY) >> 1; // Compute Y coord...

	if (node < (1 << VAR_DEPTH)) {
		// Extremely slow distance metric (sqrt is used).
		// Replace this with a faster one!
		GzCoord position;
		camera.getPosition(position);
		float distance = 1.0 + sqrt(SQR((float)centerX - position[X]) +
			SQR((float)centerY - position[Z]));

		// Egads!  A division too?  What's this world coming to!
		// This should also be replaced with a faster operation.
		TriVariance = ((float)curVar[node] * MAP_SIZE * 2) / distance;	// Take both distance and variance into consideration
	}

	if ((node >= (1 << VAR_DEPTH)) ||	// IF we do not have variance info for this node, then we must have gotten here by splitting, so continue down to the lowest level.
		(TriVariance > frameVar))	// OR if we are not below the variance tree, test for variance.
	{
		split(tri);														// Split this triangle.

		if (tri->LeftChild &&											// If this triangle was split, try to split it's children as well.
			((abs(leftX - rightX) >= 3) || (abs(leftY - rightY) >= 3)))	// Tessellate all the way down to one vertex per height field entry
		{
			recursTessellate(tri->LeftChild, apexX, apexY, leftX, leftY, centerX, centerY, node << 1, camera);
			recursTessellate(tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY, 1 + (node << 1), camera);
		}
	}
}

void Patch::tessellate(const Camera &camera) {
	// Split each of the base triangles
	curVar = varLeft;
	recursTessellate(&baseLeft, offsetX, offsetY + PATCH_SIZE,
		offsetX + PATCH_SIZE, offsetY, offsetX, offsetY, 1, camera);

	curVar = varRight;
	recursTessellate(&baseRight, offsetX + PATCH_SIZE, offsetY,
		offsetX, offsetY + PATCH_SIZE, offsetX + PATCH_SIZE, offsetY + PATCH_SIZE, 1, camera);
}

unsigned char Patch::recursComputeVariance(int leftX, int leftY, int rightX, int rightY,
	int apexX, int apexY, int node) {
	int centerX = (leftX + rightX) >> 1;		
	int centerY = (leftY + rightY) >> 1;		
	// Get the height value at the middle of the Hypotenuse
	unsigned char centerZ = heightMap[centerY * MAP_SIZE + centerX],
		rightZ = heightMap[rightY * MAP_SIZE + rightX],
		leftZ = heightMap[leftY * MAP_SIZE + leftX],
		apexZ = heightMap[apexY * MAP_SIZE + apexX];
	unsigned char tmpVar = abs((int)centerZ - (((int)leftZ + (int)rightZ) >> 1));
	if ((abs(leftX - rightX) >= 8) || (abs(leftY - rightY) >= 8)) {
		// Final Variance for this node is the max of it's own variance and that of it's children.
		tmpVar = max(tmpVar, recursComputeVariance(apexX, apexY, leftX, leftY, centerX, centerY, node << 1));
		tmpVar = max(tmpVar, recursComputeVariance(rightX, rightY, apexX, apexY, centerX, centerY, 1 + (node << 1)));
	}
	// Store the final variance for this node.  Note Variance is never zero.
	if (node < (1 << VAR_DEPTH))
		curVar[node] = 1 + tmpVar;
	return tmpVar;
}

void Patch::computeVariance() {
	// Compute variance on each of the base triangles...
	curVar = varLeft;
	recursComputeVariance(0, PATCH_SIZE, PATCH_SIZE, 0, 0, 0, 1);
	curVar = varRight;
	recursComputeVariance(PATCH_SIZE, 0, 0, PATCH_SIZE, PATCH_SIZE, 
		PATCH_SIZE, 1);
	// Clear the dirty flag for this patch
	varDirty = false;
}

void Patch::recursRender(TriTreeNode *tri, int leftX, int leftY, int rightX, 
	int rightY, int apexX, int apexY, int type){
	if (tri->LeftChild)	{	// All non-leaf nodes have both children, so just check for one
		int centerX = (leftX + rightX) >> 1;	// Compute X coordinate of center of Hypotenuse
		int centerY = (leftY + rightY) >> 1;	// Compute Y coord...
		recursRender(tri->LeftChild, apexX, apexY, leftX, leftY, centerX, centerY, LEFT);
		recursRender(tri->RightChild, rightX, rightY, apexX, apexY, centerX, centerY, RIGHT);
	}
	else {									// A leaf node!  Output a triangle to be rendered.
		// Actual number of rendered triangles...

		GLfloat leftZ = heightMap[(leftY *MAP_SIZE) + leftX];
		GLfloat rightZ = heightMap[(rightY*MAP_SIZE) + rightX];
		GLfloat apexZ = heightMap[(apexY *MAP_SIZE) + apexX];

		if (type == LEFT)
			glTexCoord2f(0, 1);
		else
			glTexCoord2f(1, 0);
		// Output the LEFT VERTEX for the triangle
		glVertex3f((GLfloat)leftX, (GLfloat)leftZ, (GLfloat)leftY);

		if (type == LEFT)
			glTexCoord2f(1, 0);
		else
			glTexCoord2f(0, 1);
		// Output the RIGHT VERTEX for the triangle
		glVertex3f((GLfloat)rightX, (GLfloat)rightZ, (GLfloat)rightY);

		if (type == LEFT)
			glTexCoord2f(0, 0);
		else
			glTexCoord2f(1, 1);
		// Output the APEX VERTEX for the triangle
		glVertex3f((GLfloat)apexX, (GLfloat)apexZ, (GLfloat)apexY);
	}
}

void Patch::render() {
	// Store old matrix
	glPushMatrix();
	// Translate the patch to the proper world coordinates
	glTranslatef((GLfloat)offsetX, 0, (GLfloat)offsetY);
	glBegin(GL_TRIANGLES);
	recursRender(&baseLeft, 0, PATCH_SIZE, PATCH_SIZE, 0, 0, 0, LEFT);
	recursRender(&baseRight, PATCH_SIZE, 0, 0, PATCH_SIZE, PATCH_SIZE, PATCH_SIZE, RIGHT);
	glEnd();
	// Restore the matrix
	glPopMatrix();
}

/* terrain class */

TriTreeNode* Terrain::triAllocate() {
	TriTreeNode *tmpTri;
	if (nextTriNode >= POOL_SIZE)
		return NULL;
	tmpTri = &(triPool[nextTriNode++]);
	tmpTri->LeftChild = tmpTri->RightChild = NULL;
	return tmpTri;
}

Terrain::~Terrain(){
	if (heightMap){
		unsigned char *gHeightMaster = heightMap - MAP_SIZE;
		free(gHeightMaster);
		heightMap = NULL;
	}
}

inline void loadTerrain(char *fileName, unsigned char **dest) {
	FILE *fp;
	unsigned char *gHeightMaster = (unsigned char *)malloc
		(MAP_SIZE * (MAP_SIZE + 2) * sizeof(unsigned char));
	// Give the rest of the application a pointer to the actual start of the height map.
	*dest = gHeightMaster + MAP_SIZE;
	fp = fopen(fileName, "rb");
	if (fp == NULL) {
		// Clear the board.
		memset(gHeightMaster, 0, MAP_SIZE * (MAP_SIZE + 2) * sizeof(unsigned char));
		return;
	}
	fread(*dest, sizeof(char), MAP_SIZE * MAP_SIZE, fp);
	fclose(fp);
	// Copy the last row of the height map into the extra first row.
	memcpy(gHeightMaster, gHeightMaster + MAP_SIZE * MAP_SIZE, MAP_SIZE);
	// Copy the first row of the height map into the extra last row.
	memcpy(gHeightMaster + MAP_SIZE * MAP_SIZE + MAP_SIZE, gHeightMaster + MAP_SIZE, MAP_SIZE);
}

void Terrain::init(char *fileName) {
	// Store the Height Field array
	if (heightMap == NULL)
		loadTerrain(fileName, &heightMap);
	// Initialize all terrain patches
	for (int i = 0; i < PATCHES_NUM; i++){
		for (int j = 0; j < PATCHES_NUM; j++) {
			patches[i][j].init(j*PATCH_SIZE, i*PATCH_SIZE, heightMap);
			patches[i][j].computeVariance();
		}
	}
}

void Terrain::tessellate(const Camera &camera) {
	// Perform Tessellation
	Patch *patch = patches[0];
	for (int i = 0; i < PATCHES_NUM*PATCHES_NUM; i++, patch++) {
		patch->tessellate(camera);
	}
}

void Terrain::render() {
	Patch *patch = patches[0];
	// Scale the terrain by the terrain scale specified at compile time.
	bindTexture();
	glScalef(1.0f, SCALE, 1.0f);
	for (int nCount = 0; nCount < PATCHES_NUM*PATCHES_NUM; nCount++, patch++){
		patch->render();
	}
	// Check to see if we got close to the desired number of triangles.
	// Adjust the frame variance to a better value.
	if (getNextTriNode() != REND_MAX)
		frameVar += (float)getNextTriNode() / (float)REND_MAX - 1;
	// Bounds checking.
	if (frameVar < 0)
		frameVar = 0;
}

void Terrain::reset() {
	// Perform simple visibility culling on entire patches.
	//   - Define a triangle set back from the camera by one patch size, following
	//     the angle of the frustum.
	//   - A patch is visible if it's center point is included in the angle: Left,Eye,Right
	//   - This visibility test is only accurate if the camera cannot look up or down significantly.
	//
	setNextTriNode(0);
	// Go through the patches performing resets, compute variances, and linking.
	for (int i = 0; i < PATCHES_NUM; i++){
		for (int j = 0; j < PATCHES_NUM; j++){
			// Reset the patch
			patches[i][j].reset();
            // Check to see if this patch has been deformed since last frame.
            // If so, recompute the varience tree for it.
			if (patches[i][j].isDirty())
				patches[i][j].computeVariance();
				// Link all the patches together.
				if (j > 0)
					patches[i][j].GetBaseLeft()->LeftNeighbor = patches[i][j - 1].GetBaseRight();
				else
					patches[i][j].GetBaseLeft()->LeftNeighbor = NULL;		// Link to bordering.
				if (j < (PATCHES_NUM - 1))
					patches[i][j].GetBaseRight()->LeftNeighbor = patches[i][j + 1].GetBaseLeft();
				else
					patches[i][j].GetBaseRight()->LeftNeighbor = NULL;		// Link to bordering.
				if (i > 0)
					patches[i][j].GetBaseLeft()->RightNeighbor = patches[i - 1][j].GetBaseRight();
				else 
					patches[i][j].GetBaseLeft()->RightNeighbor = NULL;		// Link to bordering.
				if (i < (PATCHES_NUM - 1)) 
					patches[i][j].GetBaseRight()->RightNeighbor = patches[i + 1][j].GetBaseLeft();
				else
					patches[i][j].GetBaseRight()->RightNeighbor = NULL;	// Link to bordering.
		}
	}
}