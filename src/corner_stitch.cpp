#include"Plane.h"
#include<new>

// ---------------------------------------------------------------------------
// Tile pool.
// A corner-stitched plane holding n solid tiles has at most 3n+1 space tiles,
// so the number of live tiles is small and bounded; only the churn is huge.
// Freed tiles go onto a free list and are handed straight back out, so the
// steady state does no malloc/free at all. The pool grows a whole chunk at a
// time if the bound ever rises (more components per module) and is never
// returned to the OS.
// ---------------------------------------------------------------------------
namespace
{
	union TileSlot
	{
		TileSlot* next;
		alignas(Tile) unsigned char raw[sizeof(Tile)];
	};
	const std::size_t kTileChunk = 4096;
	TileSlot* tileFreeList = NULL;

	void growTilePool()
	{
		TileSlot* chunk = static_cast<TileSlot*>(::operator new(kTileChunk * sizeof(TileSlot)));
		for (std::size_t i = 0; i < kTileChunk; i++)
		{
			chunk[i].next = tileFreeList;
			tileFreeList = &chunk[i];
		}
	}
}

void* Tile::operator new(std::size_t size)
{
	if (size != sizeof(Tile))
		return ::operator new(size);
	if (tileFreeList == NULL)
		growTilePool();
	TileSlot* slot = tileFreeList;
	tileFreeList = slot->next;
	return slot;
}

void Tile::operator delete(void* p) noexcept
{
	if (p == NULL)
		return;
	TileSlot* slot = static_cast<TileSlot*>(p);
	slot->next = tileFreeList;
	tileFreeList = slot;
}

Tile* getByMinWidth(std::vector<Tile*> Tlist)
{
	Tile* cur = Tlist[0];
	for (size_t i = 1; i < Tlist.size(); i++)
	{
		if (width(cur) > width(Tlist[i]))
			cur = Tlist[i];
	}
	return cur;
}

Tile* getByMaxWidth(std::vector<Tile*> Tlist)
{
	Tile* cur = Tlist[0];
	for (size_t i = 1; i < Tlist.size(); i++)
	{
		if (width(cur) < width(Tlist[i]))
			cur = Tlist[i];
	}
	return cur;
}

Tile* getByMaxHeight(std::vector<Tile*> Tlist)
{
	Tile* cur = Tlist[0];
	for (size_t i = 1; i < Tlist.size(); i++)
	{
		if (height(cur) < height(Tlist[i]))
			cur = Tlist[i];
	}
	return cur;
}

Tile* getByMinHeight(std::vector<Tile*> Tlist)
{
	Tile* cur = Tlist[0];
	for (size_t i = 1; i < Tlist.size(); i++)
	{
		if (height(cur) > height(Tlist[i]))
			cur = Tlist[i];
	}
	return cur;
}

std::ostream& operator<<(std::ostream& fout, Point p) 
{
	fout << p.x << " " << p.y; 
	return fout; 
}

std::ostream& operator<<(std::ostream& fout, Tile t)
{
	fout << t.coord[0] << " " << t.coord[1];
	return fout;
}

Tile::Tile(const Tile* cp)
{
	this->coord[0] = cp->coord[0];
	this->coord[1] = cp->coord[1];
	for (size_t i = 0; i < 4; i++)
		this->stitch[i] = cp->stitch[i];
	this->belong = cp->belong;

}

Tile::Tile(const Tile* cp,bool copyStitch)
{
	this->coord[0] = cp->coord[0];
	this->coord[1] = cp->coord[1];
	this->belong = cp->belong;
	if (copyStitch == 1)
	{
		for (size_t i = 0; i < 4; i++)
			this->stitch[i] = cp->stitch[i];
	}
	else
	{
		for (int i = 0; i < 4; i++)
			this->stitch[i] = NULL;
	}
}

Tile::Tile(Point coord, int height, int width)
{
	this->coord[0] = coord;
	this->coord[1] = coord + Point(height, width) - Point(1,1);
	for (int i = 0; i < 4; i++)
		this->stitch[i] = NULL;
}

Tile::Tile(Point coord, int height, int width, Module* belong)
{
	this->coord[0] = coord;
	this->coord[1] = coord + Point(height, width) - Point(1, 1);
	this->belong = belong;
	for (int i = 0; i < 4; i++)
		this->stitch[i] = NULL;
}

Tile::Tile(Point pLeftDown, Point pRightUp, Module* belong)
{
	this->coord[0] = pLeftDown;
	this->coord[1] = pRightUp;
	this->belong = belong;
	for (int i = 0; i < 4; i++)
		this->stitch[i] = NULL;
}

Tile::Tile(Point pLeftDown, Point pRightUp)
{
	this->coord[0] = pLeftDown;
	this->coord[1] = pRightUp;
	for (int i = 0; i < 4; i++)
		this->stitch[i] = NULL;
	this->belong = NULL;
}

Tile::Tile(int h, int w, Point pRU, Module* B)
{
	this->coord[1] = pRU;
	this->coord[0] = pRU - Point(h, w) + Point(1, 1);
	for (int i = 0; i < 4; i++)
		this->stitch[i] = NULL;
	this->belong = B;
}

Tile::Tile()
{
	this->coord[1] = Point(0, 0);
	this->coord[0] = Point(0, 0);
	for (int i = 0; i < 4; i++)
		this->stitch[i] = NULL;
	this->belong = NULL;
}

bool Tile::operator==(Tile b)
{
	if (!(coord[0] == b.coord[0]))
		return 0;
	if (!(coord[1] == b.coord[1]))
		return 0;
	return 1;
}

bool Tile::operator>=(Tile b)
{
	if (!in_range(this, b.coord[0]))
		return 0;
	if (!in_range(this, b.coord[1]))
		return 0;
	return 1;
}

bool areaCompare(Tile* a, Tile* b)
{
	int Area1 = width(a) * height(b);
	int Area2 = width(b) * height(b);
	return Area1 < Area2;
}

bool xCompare(const Point a, const Point b) { return a.x < b.x; }
bool yCompare(const Point a, const Point b) { return a.y < b.y; } 

Tile::~Tile()
{}

void Tile::rotate()
{
	int tmpWidth = height(this);
	int tmpHeight = width(this);
	coord[1].x = coord[0].x + tmpHeight - 1;
	coord[1].y = coord[0].y + tmpWidth - 1;
	return;
}

void Tile::clearStitch()
{
	for (int i = 0; i < 4; i++)
		stitch[i] = NULL;
}

Wire::Wire(Module* a, Module* b, int weight)
{
	this->a = a;
	this->b = b;
	this->weight = weight;
}

double Wire::HPWL()
{
  if(a->getUp()==NULL || b->getUp()==NULL)
    exit(0);

	Point A = a->center();
	Point B = b->center();
	double x1 = std::max({ A.x,B.x });
	double x2 = std::min({ A.x,B.x });
	double y1 = std::max({ A.y,B.y });
	double y2 = std::min({ A.y,B.y }); 
	double lx = x1 - x2;
	double ly = y1 - y2; 
	double wl = lx + ly;
	return double(weight) * wl;
}
