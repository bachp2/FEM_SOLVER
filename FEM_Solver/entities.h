#pragma once
#include <Eigen/Dense>
#include <SDL.h>
#include <vector>
#include <memory>
#include <cmath>
using namespace Eigen;

//base struct to construct every other entities
struct Entity
{
	typedef float real;
	Vector2f position;
	SDL_Color color;
	bool isSelected;
	int id;
	Entity(Vector2f pos, SDL_Color c): position(pos), color(c), isSelected(false){}
	void move(int x, int y) {
		position(0) = x;
		position(1) = y;
	}
};

//create Entity with immutability
struct ConstraintEntity
{
	typedef float real;
	Vector2f position;
	SDL_Color color;
	int id;
	ConstraintEntity(SDL_Color c): color(c) {};
	void move(int x, int y) {
		position(0) = x;
		position(1) = y;
	}
};

struct World : Entity
{
	float width, height;
	World(Vector2f pos, SDL_Color color, real w, real h) : Entity(pos, color), 
														width(w), height(h) {}
};

struct Node : Entity
{
	real size;
	SDL_Rect rect;
	Node(Vector2f pos, SDL_Color color, int s) : Entity(pos, color), size(s) {
		rect.x = pos(0)-size/2.0f;
		rect.y = pos(1)-size/2.0f;
		rect.w = size;
		rect.h = size;
	}
	void updatePosition(int x, int y) {
		position(0) = x;
		position(1) = y;
		rect.x = position(0) - size / 2.0f;
		rect.y = position(1) - size / 2.0f;
		rect.w = size;
		rect.h = size;
	}
};

struct Element : ConstraintEntity
{
	typedef std::shared_ptr<Node> nPtr;
	nPtr node1;
	nPtr node2;
	float thickness;
	float L;
	Element(nPtr& n1, nPtr& n2, SDL_Color c, float t) : node1(n1), node2(n2), thickness(t), ConstraintEntity(c) {}
};

struct Truss
{
	typedef std::shared_ptr<Node> nPtr;
	typedef std::shared_ptr<Element> ePtr;
	std::vector<nPtr> nodes;		  //array of pointers
	std::vector<ePtr> elements;       //array of pointers
	int originx;
	int originy;

	void appendNode(nPtr& n) {
		//weird I got a problem when use its raw pointer
		nodes.push_back(n);
		n->id = nodes.size();
	}

	void appendElement(ePtr& e) {
		elements.push_back(e);
		e->id = elements.size();
	}

	void connectNodes(nPtr& n) {
		if (!nodes.empty()) {
			ePtr eptr(new Element(nodes.back(), n, { 250,255,0,255 }, 3));
			appendElement(eptr);
		}
	}

	bool checkNodeOverlapped(nPtr& n, int mouseX, int mouseY) {
		if (n == nullptr) return false;
		auto nx = n->position(0);
		auto ny = n->position(1);
		auto ns = n->size;
		auto inXRange = (mouseX <= nx+ns && mouseX >= nx-ns);
		auto inYRange = (mouseY <= ny+ns && mouseY >= ny-ns);
		return inXRange && inYRange;
	}

	bool isMouseOnNode(nPtr& n, int mouseX, int mouseY) {
		if (n == nullptr) return false;
		auto nx = n->position(0);
		auto ny = n->position(1);
		auto ns2 = n->size/2;
		auto inXRange = (mouseX <= nx + ns2 && mouseX >= nx - ns2);
		auto inYRange = (mouseY <= ny + ns2 && mouseY >= ny - ns2);
		return inXRange && inYRange;
	}

	Truss(int ox, int oy) : originx(ox), originy(oy) {}
};