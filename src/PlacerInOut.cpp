#include"PlacerInOut.h"
#include"Plane.h"
using namespace std;

void PlacerInOut::readInsertedTile(vector<Tile*> inserted) { this->inserted = inserted; }

void PlacerInOut::readWire(vector<Wire*> wires) { this->wires = wires; }

void PlacerInOut::readtoInsertTile(vector<Soft_Module*> toInsert) { this->toInsert = toInsert; }

void PlacerInOut::setY(int chipY) { this->chipY = chipY; }

void PlacerInOut::setX(int chipX) { this->chipX = chipX; }

void PlacerInOut::generateData()
{
	minHeight = 9999999;
	ofstream AUX;
	AUX.open("place.aux");
	AUX << "RowBasedPlacement : place.nodes place.nets place.wts place.pl place.scl " << endl;
	nodes();
	nets();
	wts();
	pl();
	scl();
	system("./ntuplace3 -aux place.aux -nodetail -util 0.95");
}

void PlacerInOut::scl()
{
	ofstream SCL;
	SCL.open("place.scl");
	SCL << "UCLA scl 1.0" << endl;
	SCL << "# Created      : Wed Jun 16 10 : 46 : 27 2004" << endl;
	SCL << "# User : sadya@zenit.eecs.umich.edu(Saurabh N Adya, Not Available, NotAvailable / Freysinger; CR0i; 30)" << endl;
	SCL << "# Platform     : Linux 2.4.20 - 31.9bigmem #1 SMP Tue Apr 13 17:11 : 51 EDT 2004 i686 " << endl;
	int repeatTime = chipY / minHeight;
	SCL << "Numrows : " << repeatTime-1 << endl;
	for (int i = 0; i < repeatTime - 2; i++)
	{
		SCL << "CoreRow Horizontal" << endl;
		SCL << "Coordinate : " << i * minHeight << endl;
		SCL << "Height : " << minHeight << endl;
		SCL << "Sitewidth : 1" << endl;
		SCL << "Sitespacing : 1" << endl;
		SCL << "Siteorient : N" << endl;
		SCL << "Sitesymmetry : Y" << endl;
		SCL << "SubrowOrgin : 0 Numsites : "<<chipX << endl; 
		SCL << "End" << endl;
	} 
	SCL << "CoreRow Horizontal" << endl;
	SCL << "Coordinate : " << (repeatTime-1) * minHeight << endl;
	SCL << "Height : " << chipY - (repeatTime-1) * minHeight << endl;
	SCL << "Sitewidth : 1" << endl;
	SCL << "Sitespacing : 1" << endl;
	SCL << "Siteorient : N" << endl;
	SCL << "Sitesymmetry : Y" << endl;
	SCL << "SubrowOrgin : 0 Numsites : " << chipX << endl;
	SCL << "End" << endl;
	SCL.close();
}

void PlacerInOut::pl()
{
	ofstream PL;
	PL.open("place.pl");
	PL << "UCLA pl 1.0" << endl;
	PL << "# Created      : Wed Jun 16 10 : 46 : 27 2004" << endl;
	PL << "# User : sadya@zenit.eecs.umich.edu(Saurabh N Adya, Not Available, NotAvailable / Freysinger; CR0i; 30)" << endl;
	PL << "# Platform     : Linux 2.4.20 - 31.9bigmem #1 SMP Tue Apr 13 17:11 : 51 EDT 2004 i686" << endl;
	for (size_t i = 0; i < toInsert.size(); i++)
	{
		PL << toInsert[i]->getName() << "	1	1 : N" << endl;
	}
	for (size_t i = 0; i < inserted.size(); i++)
	{
		PL << inserted[i]->belong->getName() << "	" << LD(inserted[i]).x << "	" << LD(inserted[i]).y << ": N /FIXED" << endl;
	}
	PL.close();
}

void PlacerInOut::nets()
{
	ofstream NETS;
	NETS.open("place.nets");
	NETS << "UCLA nets 1.0" << endl;
	NETS << "# Created      : Wed Jun 16 10 : 46 : 24 2004" << endl;
	NETS << "# User : sadya@zenit.eecs.umich.edu(Saurabh N Adya, Not Available, NotAvailable / Freysinger; CR0i; 30)" << endl;
	NETS << "# Platform     : Linux 2.4.20 - 31.9bigmem #1 SMP Tue Apr 13 17:11 : 51 EDT 2004 i686" << endl;
	
	NETS << "NumNets : " << wires.size() << endl;
	NETS << "NumPins : " << 2 * wires.size() << endl;
	for (size_t i = 0; i < wires.size(); i++)
	{
		NETS << "NetDegree : 2  net" << i << endl;
		NETS << wires[i]->a->getName() << " I : 0 0" << endl;
		NETS << wires[i]->b->getName() << " O : 0 0" << endl;
	}
	NETS.close();
}

void PlacerInOut::wts()
{
	ofstream WTS;
	WTS.open("place.wts");
	WTS << "UCLA wts 1.0" << endl;
	WTS << "# Created      : Wed Jun 16 10 : 46 : 27 2004" << endl;
	WTS << "# User : sadya@zenit.eecs.umich.edu(Saurabh N Adya, Not Available, NotAvailable / Freysinger; CR0i; 30)" << endl;
	WTS << "# Platform     : Linux 2.4.20 - 31.9bigmem #1 SMP Tue Apr 13 17:11 : 51 EDT 2004 i686" << endl;
	for (size_t i = 0; i < toInsert.size(); i++)
	{
		WTS << toInsert[i]->getName() << "	1" << endl;
	}
	for (size_t i = 0; i < inserted.size(); i++)
	{
		WTS << inserted[i]->belong->getName() << "	20" << endl;
	}
	WTS.close();
}

void PlacerInOut::nodes()
{
	ofstream NODES;
	NODES.open("place.nodes");
	NODES << "UCLA nodes 1.0" << endl;
	NODES << "# Created      : Wed Jun 16 10 : 46 : 23 2004" << endl;
	NODES << "# User : sadya@zenit.eecs.umich.edu(Saurabh N Adya, Not Available, NotAvailable / Freysinger; CR0i; 30)" << endl;
	NODES << "# Platform     : Linux 2.4.20 - 31.9bigmem #1 SMP Tue Apr 13 17:11 : 51 EDT 2004 i686" << endl;
	NODES << "NumNodes : " << toInsert.size() + inserted.size() << endl;
	NODES << "NumTerminals : " << inserted.size() << endl;
	for (size_t i = 0; i < toInsert.size(); i++)
	{
		int h = sqrt(toInsert[i]->getMinArea())/1.2;
		int w = ceil(toInsert[i]->getMinArea() / h)/1.44;
		if(h < minHeight)
			minHeight = h;
		NODES << toInsert[i]->getName() << "	" << w << "	" << h << endl;
	}
	for (size_t i = 0; i < inserted.size(); i++)
	{
		NODES << inserted[i]->belong->getName()<< "	" << width(inserted[i]) << "	" << height(inserted[i])<<"	terminal" << endl;
	}
	NODES.close();
}