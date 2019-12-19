#pragma once

#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp> 

#include "Point.hpp"

using namespace sf;
using namespace std;

namespace game {
	enum GameState {
		MAIN_MENU,
		SETTINGS,
		RUNNING,
		DEAD
	};

	enum Direction {
		STOP = 0,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	struct ControlSet {
		Keyboard::Key up;
		Keyboard::Key down;
		Keyboard::Key right;
		Keyboard::Key left;
	};

	class Snake {
	public:
		Snake(RectangleShape _shape) {
			dir = STOP;
			w_dir = STOP;
			shape = _shape;
		}

		int id;

		ControlSet ctrl;

		RectangleShape shape;

		bool alive = true;

		Point pos;

		Point tail[350];
		int length = 1;
		Point t_last;

		Direction dir;
		Direction w_dir;
	};

	class Game {
	public:
		Game(RenderWindow& w) : window(w) {
		}

		sf::Color m_colors[3] = { Color::Green, Color::Cyan, Color::Magenta };

		ControlSet m_ctrls[3] = {
		{ Keyboard::Key::Up, Keyboard::Key::Down, Keyboard::Key::Right, Keyboard::Key::Left } ,
		{ Keyboard::Key::W, Keyboard::Key::S, Keyboard::Key::D, Keyboard::Key::A },
		{ Keyboard::Key::I, Keyboard::Key::K, Keyboard::Key::L, Keyboard::Key::J } };

		RenderWindow& getWindow();
		Font font;

		GameState state = MAIN_MENU;

		int players = 1;
		vector<Snake> snakes;

		vector<RectangleShape> lines;

		Point fruit[10];

		vector<Point> obstacles;

		// "speed"
		int delay = 75;

		// Defines how many units per column/row
		int grid_size = 20;

		// Menu sizing unit
		int menu_size = 80;

		// Width/height of the playing field in "pixels"
		float unit_size;

		float limit;

		struct {
			float x;
			float y;
		} grid_ref;

		// Menu control, switches based on mode
		int menu_min = 0;
		int menu_item = 0;
		int menu_max = 2;

		// Game loop
		void input();
		void logic();
		bool checkCollision(Snake& s);
		void move(Snake& s);
		void draw();

		void setup();
		void die();

		// Utils
		Point randomCoord();
		void populateFruit();

		// Clear all the arrays
		void clear();

		// Convert a snake by id to an obstacle
		void snakeToObstacle(Snake& s);

		// Take the oposite direction
		Direction dirOposite(Direction dir);

		// Generate new text
		sf::Text newText(String str);
		sf::Text newText(String str, int size);

		// Convert int to sf::String
		sf::String toString(int  n);

		// Attempt to resize the playing field, only in running state
		void resize();
		void redraw();
		void update_view(sf::Event e);
		void map(Transformable& shape, int x, int y);

		void menu_up();
		void menu_down();
	private:
		RenderWindow& window;
	};
}
