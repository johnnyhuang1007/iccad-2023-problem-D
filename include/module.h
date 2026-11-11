#ifndef _MODULE_H_
#define _MODULE_H_
#include"corner_stitch.h"
#include<iostream>
#include<list>
#include<queue>
#include<vector>
#include<cmath>

class Module;

Point LD(Module*);
Point RU(Module*);
Point RD(Module*);
Point LU(Module*);

class Module
{
protected:
	int min_area;
	std::string name;
	Tile* bound_Tile[4];
	std::list<Wire*> wireSet;
public:
	Module(std::string, int);
	Point center()
	{
		return Point((RU(bound_Tile[1]).y + LD(bound_Tile[3]).y) / 2 + 0.5, (RU(bound_Tile[2]).x + LD(bound_Tile[0]).x) / 2 + 0.5);
	}
	int getMinArea() { return min_area; }
	std::string getName() { return name; }
	void getWire(Wire* newWire) { wireSet.push_back(newWire); }
	Point optimPos();
	double PosPerformCheck(Point);
	void callWire();
	virtual Tile* get_root() { return bound_Tile[0]; }
	virtual bool is_fix() { return 1; }
	virtual void ripOff(Tile* a) {};
	virtual void update(Tile* a) {};
	virtual void update() {};
	Tile* getLeft() { return bound_Tile[0]; }
	Tile* getRight() { return bound_Tile[2]; }
	Tile* getUp() { return bound_Tile[1]; }
	Tile* getDown() { return bound_Tile[3]; }
	virtual ~Module() = default;
	friend int width(Module*);
	friend int height(Module*);

};

inline int width(Module* a) { return RU(a->bound_Tile[2]).x - LD(a->bound_Tile[0]).x + 1; }
inline int height(Module* a) { return RU(a->bound_Tile[1]).y - LD(a->bound_Tile[3]).y + 1; }
inline Point LD(Module* a) { return Point(LD(a->getDown()).y, LD(a->getLeft()).x); }
inline Point RU(Module* a) { return Point(RU(a->getUp()).y, RU(a->getRight()).x); }
inline Point RD(Module* a) { return Point(LD(a->getDown()).y, RU(a->getRight()).x); }
inline Point LU(Module* a) { return Point(RU(a->getUp()).y, LD(a->getLeft()).x); }

class Soft_Module :public Module
{
private:
	int cur_area = 0;

	void updateState();
public:
	std::vector<Tile*> component;
	Soft_Module(std::string, int);
	bool checkLegal();
	bool LegalifAdd(Tile*);
	bool is_fix() { return 0; }
	void obtainNewTile(Tile*);

	Point pseudoCenter(Tile*);
	Point HPWLdir();
	Point estHPWL(Point);

	void update();
	void update(Tile*);
	void updateRoot(Tile*);
	void ripTillRoot();
	void ripOff(Tile*);
	void ripOff();

	void get_Tile(Tile*);
	void print();
	Tile* get_root() { return *(component.begin()); }


	int getCurArea() { return cur_area; }
	int leftArea() { return min_area - cur_area; }
	void ripoffALL();
	std::vector<Tile*> getComp() { return component; }

	std::vector<Point> listPoint(Tile* (*point_finding)(Point, Tile*));
	~Soft_Module();
};

class Fixed_Module :public Module
{
private:
	Tile* root; 
public:
	Fixed_Module(std::string, Point, int, int);
	Tile* get_root() { return root; }
	std::string get_name() { return name; }
	bool is_fix() { return 1; }
	~Fixed_Module();
};

// inline: called once per unit step of the shrink loops in minHPWLStretch
inline bool Soft_Module::LegalifAdd(Tile* toInst)
{
	if (component.size() == 0)
		return 0;
	Point newLD = LD(this);
	Point newRU = RU(this);
	if (newLD.y > LD(toInst).y)
		newLD.y = LD(toInst).y;
	if (newLD.x > LD(toInst).x)
		newLD.x = LD(toInst).x;
	if (newRU.y < RU(toInst).y)
		newRU.y = RU(toInst).y;
	if (newRU.x < RU(toInst).x)
		newRU.x = RU(toInst).x;


	double cover = (double)(newRU.x - newLD.x + 1) * (newRU.y - newLD.y + 1);
	if (cur_area + area(toInst) < min_area)
		return 0;
	if (ceil(cover) > ((cur_area + area(toInst)) * 5) / 4)
		return 0;
	
	if ((newRU.x - newLD.x + 1) * 2 < (newRU.y - newLD.y + 1))
		return 0;
	if ((newRU.y - newLD.y + 1) * 2 < (newRU.x - newLD.x + 1))
		return 0;
		
	return 1;
}

#endif
