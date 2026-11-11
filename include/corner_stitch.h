#ifndef _CORNER_STITCH_
#define _CORNER_STITCH_

#include<cstddef>
#include<string>
#include<iostream>
#include<vector>
struct Tile;
struct Point;
class Module;

bool in_range(Tile*, Point);
bool y_in_range(Tile*, Point);
bool x_in_range(Tile*, Point);
bool adjcent(Tile*, Tile*);
bool areaCompare(Tile*, Tile*);
bool xCompare(const Point, const Point);
bool yCompare(const Point, const Point);

int height(Tile*);
int width(Tile*);
int area(Tile*);
int height(Module*);
int width(Module*);

std::ostream& operator<<(std::ostream&, Point);
std::ostream& operator<<(std::ostream&, Tile);

template <class T>
void randomize(std::vector<T>& toRand)
{
	for (int i = 0; i < toRand.size()-1; i++)
		std::swap(toRand[i],toRand[i + rand() % (toRand.size()-i)]);
}

struct Point
{
	double y ;
	double x ;
	Point(double y, double x) { this->y = y; this->x = x; }
	Point() { y = -1; x = -1; }
	Point operator+(Point p)const { return Point(y + p.y, x + p.x); }
	Point operator-(Point p)const { return Point(y - p.y, x - p.x); }
	Point operator/(double scalar)const { return Point(y / scalar, x / scalar); }
	Point operator*(double scalar)const { return Point(y * scalar, x * scalar); }
	void toInt() { x = int(x); y = int(y); }
	bool operator==(Point p);
};

struct Tile
{
		Point coord[2];	//left_down right_up
		Tile* stitch[4];
		Module* belong = NULL;
		Tile(Point,int,int);
		Tile(Point, Point);
		Tile(const Tile*);
		Tile(const Tile*, bool);
		Tile(Point, Point, Module*);
		Tile(Point, int, int, Module*);
		Tile(int, int, Point, Module*);
		Tile(int, int, Point);
		Tile();
		~Tile(); 
		bool operator==(Tile);
		bool operator>=(Tile);
		bool is_space() { return belong == NULL; }
		void rotate();
		void clearStitch();

		// Tiles are created/destroyed millions of times during SA. They all
		// come from one free-list pool (see corner_stitch.cpp), so new/delete
		// on a Tile never reaches malloc/free once the pool is warm.
		static void* operator new(std::size_t);
		static void operator delete(void*) noexcept;
};

struct Wire
{
	Module* a;
	Module* b;
	int weight;
	Wire(Module*, Module*, int);
	double HPWL();
};

// Hot accessors: defined here so every translation unit can inline them.
inline Point& LD(Tile* a) { return a->coord[0]; }
inline Point& RU(Tile* a) { return a->coord[1]; }
inline Point LU(Tile* a) { return Point(a->coord[1].y, a->coord[0].x); }
inline Point RD(Tile* a) { return Point(a->coord[0].y, a->coord[1].x); }
inline double& U(Tile* a) { return a->coord[1].y; }
inline double& D(Tile* a) { return a->coord[0].y; }
inline double& L(Tile* a) { return a->coord[0].x; }
inline double& R(Tile* a) { return a->coord[1].x; }
inline Point Center(Tile* a) { return Point(U(a) - D(a), R(a) - L(a)); }

inline int height(Tile* a) { return a->coord[1].y - a->coord[0].y + 1; }
inline int width(Tile* a) { return a->coord[1].x - a->coord[0].x + 1; }
inline int area(Tile* a) { return width(a) * height(a); }

inline bool x_in_range(Tile* tile, Point coord)
{
	if (tile->coord[0].x > coord.x)
		return 0;
	if (tile->coord[1].x < coord.x)
		return 0;
	return 1;
}
inline bool y_in_range(Tile* tile, Point coord)
{
	if (tile->coord[0].y > coord.y)
		return 0;
	if (tile->coord[1].y < coord.y)
		return 0;
	return 1;
}
inline bool in_range(Tile* tile, Point coord)
{
	if (!x_in_range(tile, coord))
		return 0;
	if (!y_in_range(tile, coord))
		return 0;
	return 1;
}
inline bool Point::operator==(Point a)
{
	if (y != a.y)
		return 0;
	if (x != a.x)
		return 0;
	return 1;
}


Tile* getByMinWidth(std::vector<Tile*>);
Tile* getByMaxHeight(std::vector<Tile*>);
Tile* getByMinHeight(std::vector<Tile*>);
Tile* getByMaxWidth(std::vector<Tile*>);

#endif
