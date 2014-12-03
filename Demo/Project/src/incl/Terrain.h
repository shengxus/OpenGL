#ifndef TERRIAN_H
#define TERRIAN_H

#include "Object.h"
#include "Camera.h"

#define VAR_DEPTH	9
#define MAP_SIZE	1024
#define PATCHES_NUM	16
#define POOL_SIZE	25000
#define REND_MAX	10000
#define PATCH_SIZE	(MAP_SIZE/PATCHES_NUM)
#define SCALE		1

struct TriTreeNode {
	TriTreeNode *LeftChild;
	TriTreeNode *RightChild;
	TriTreeNode *BaseNeighbor;
	TriTreeNode *LeftNeighbor;
	TriTreeNode *RightNeighbor;
};

class Patch {
private:
	unsigned char *heightMap;									// Pointer to height map to use
	int offsetX, offsetY;										// World coordinate offset of this patch.
	unsigned char varLeft[1 << (VAR_DEPTH)];				// Left variance tree
	unsigned char varRight[1 << (VAR_DEPTH)];			// Right variance tree
	unsigned char *curVar;							// Which varience we are currently using. [Only valid during the Tessellate and ComputeVariance passes]
	bool varDirty;								// Does the Varience Tree need to be recalculated for this Patch?
	TriTreeNode baseLeft;										// Left base triangle tree node
	TriTreeNode baseRight;									// Right base triangle tree node
public:
	// Some encapsulation functions & extras
	TriTreeNode *GetBaseLeft()  { return &baseLeft; }
	TriTreeNode *GetBaseRight() { return &baseRight; }
	bool isDirty(){ return varDirty; };
	// The static half of the Patch Class
	void init(int heightX, int heightY, unsigned char *hMap);
	void reset();
	void tessellate(const Camera &);
	void render();
	void computeVariance();

	// The recursive half of the Patch Class
	void split(TriTreeNode *tri);
	void recursTessellate(TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY, int node, const Camera &);
	void recursRender(TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY);
	unsigned char recursComputeVariance(int leftX, int leftY, int rightX, int rightY,
		int apexX, int apexY, int node);
};

class Terrain: public Object{
private:
	unsigned char *heightMap;										// HeightMap of the Landscape
	Patch patches[PATCHES_NUM][PATCHES_NUM];	// Array of patches
	static int	nextTriNode;										// Index to next free TriTreeNode
	static TriTreeNode triPool[POOL_SIZE];						// Pool of TriTree nodes for splitting
	static int getNextTriNode() { return nextTriNode; }
	static void setNextTriNode(int nNextNode) { nextTriNode = nNextNode; }
public:
	~Terrain();
	friend class Patch;
	static TriTreeNode* triAllocate();
	void init(char *fileName);
	void reset();
	void tessellate(const Camera &camera);
	virtual void render();
};

#endif