#ifndef MENU_H_
#define MENU_H_

#include "map.h"

class Menu {
private:
	Map map;
public:
	void InitialMenu();
	void LoadMap();
	void ReformatingMap();
	void ShowSolution();
};

#endif
