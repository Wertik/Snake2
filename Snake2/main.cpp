#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "Game.hpp"
#include "Point.hpp"

using namespace std;
using namespace sf;
using namespace game;

class Hehe {
public:
	Hehe() {
		
	}

	Point p;

	void setP(Point& p1) {
		p = p1;
	}
};

Point getPoint() {
	return Point(4, 7);
}

int main() {
	// sf::ContextSettings settings;
	// settings.antialiasingLevel = 8;

	// RenderWindow w(sf::VideoMode(800, 800), "Snake 2", Style::Close | Style::Titlebar | Style::Resize, settings);

	// Font f;
	// f.loadFromFile("Roboto-Black.ttf");

	Hehe h;
	Point x = Point(4, 7);
	h.setP(x);
	// or
	h.p = x;

	cout << h.p.x << " : " << h.p.y << endl;

	system("pause");

	/*

	Game game(w);

	game.font = f;

	while (w.isOpen()) {
		game.input();
		game.logic();
		game.draw();
	}*/

	return 0;
}