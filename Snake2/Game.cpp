#include "Game.hpp"
#include "Point.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;
using namespace game;
using namespace chrono;

RenderWindow& Game::getWindow() {
	return window;
}

// Called before actual game start
void Game::setup() {
	resize();
	redraw();

	// Populate snakes
	for (int i = 0; i < players; i++) {

		RectangleShape shape = RectangleShape(Vector2f(unit_size + 1, unit_size + 1));
		Snake s = Snake(shape);

		s.id = i;

		s.shape.setFillColor(m_colors[i]);

		s.pos = randomCoord();

		cout << "RND: " << s.pos.x << " : " << s.pos.y << endl;

		s.ctrl = m_ctrls[i];

		snakes.push_back(s);
	}

	clear();

	// Fruit
	populateFruit();

	state = RUNNING;
	std::cout << "Game setup.." << std::endl;
}

void Game::die() {
	menu_item = 0;
	menu_max = 1;

	state = DEAD;

	std::cout << "Everyone is dead.." << std::endl;
}

void Game::input() {
	auto view = window.getView();

	Event event;

	while (window.pollEvent(event)) {
		if (event.type == Event::Closed)
			window.close();

		if (event.type == Event::KeyPressed)
			if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
				window.close();

		if (state == MAIN_MENU) {
			if (event.type == Event::Resized) {
				update_view(event);
				resize();
			}
			else if (event.type == Event::KeyPressed) {
				if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
					if (menu_item > menu_min)
						menu_item -= 1;
					else if (menu_item == menu_min)
						menu_item = menu_max;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
					if (menu_item < menu_max)
						menu_item += 1;
					else if (menu_item == menu_max)
						menu_item = menu_min;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
					switch (menu_item) {
					case 0:
						// Start playing
						setup();
						break;
					case 1:
						state = SETTINGS;
						menu_min = 0;
						menu_item = 0;
						menu_max = 3;
						break;
					case 2:
						window.close();
						break;
					}
				}
			}
		}
		else if (state == RUNNING) {

			if (event.type == Event::Resized) {
				update_view(event);
				resize();
				redraw();
			}
			else if (event.type == Event::KeyPressed) {
				for (Snake& s : snakes) {
					if (Keyboard::isKeyPressed(s.ctrl.up) && s.dir != DOWN)
						s.w_dir = UP;
					else if (Keyboard::isKeyPressed(s.ctrl.down) && s.dir != UP)
						s.w_dir = DOWN;
					else if (Keyboard::isKeyPressed(s.ctrl.left) && s.dir != RIGHT)
						s.w_dir = LEFT;
					else if (Keyboard::isKeyPressed(s.ctrl.right) && s.dir != LEFT)
						s.w_dir = RIGHT;
					else if (Keyboard::isKeyPressed(Keyboard::Key::Add)) {
						s.length++;
						s.tail[s.length - 1].x = -2;
						s.tail[s.length - 1].y = -2;
					}
				}
			}
		}
		else if (state == DEAD) {
			if (event.type == Event::Resized) {
				view.setSize(event.size.width, event.size.height);
			}
			else if (event.type == Event::KeyPressed) {
				if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
					if (menu_item > menu_min)
						menu_item -= 1;
					else if (menu_item == menu_min)
						menu_item = menu_max;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
					if (menu_item < menu_max)
						menu_item += 1;
					else if (menu_item == menu_max)
						menu_item = menu_min;

				}
				else if (Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
					switch (menu_item) {
					case 0:
						// Start playing
						setup();
						break;
					case 1:
						state = MAIN_MENU;
						menu_min = 0;
						menu_item = 0;
						menu_max = 2;
						break;
					}
				}
			}
		}
		else if (state == SETTINGS) {
			if (event.type == Event::Resized) {
				view.setSize(event.size.width, event.size.height);
			}
			else if (event.type == Event::KeyPressed) {
				if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
					if (menu_item > menu_min)
						menu_item -= 1;
					else if (menu_item == menu_min)
						menu_item = menu_max;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
					if (menu_item < menu_max)
						menu_item += 1;
					else if (menu_item == menu_max)
						menu_item = menu_min;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
					switch (menu_item) {
					case 0:
						state = MAIN_MENU;
						menu_min = 0;
						menu_item = 0;
						menu_max = 2;
						break;
					}
				}
				if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
					if (players > 1 && menu_item == 1)
						players--;
					else if (grid_size > 5 && menu_item == 2)
						grid_size--;
					else if (delay > 50 && menu_item == 3)
						delay--;
				}
				else if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
					if (players < 3 && menu_item == 1)
						players++;
					else if (grid_size < 80 && menu_item == 2)
						grid_size++;
					else if (delay < 500 && menu_item == 3)
						delay++;
				}
			}
		}
	}
}

typedef time_point<system_clock> TimePoint;
typedef duration<float> float_seconds;
TimePoint clock_end;

bool Game::checkCollision(Snake& s) {
	// Obstacle collision
	for (Point& o : obstacles) {
		if (o == s.pos) {
			s.alive = false;
			snakeToObstacle(s);
			return false;
		}
	}

	// Own tail collision
	for (int i = 1; i < s.length; i++) {
		Point& t = s.tail[i];

		if (t == s.pos) {
			s.alive = false;
			snakeToObstacle(s);
			return false;
		}
	}

	// Check collision with other snakes
	for (Snake& s1 : snakes) {
		if (!s1.alive || s.id == s1.id)
			continue;

		// Head collision
		// Decide based on directions and length
		if (s1.pos == s.pos) {

			// Head to head
			if (s1.dir == dirOposite(s.dir)) {
				if (s.length > s1.length) {
					s1.alive = false;
					snakeToObstacle(s1);
				}
				else if (s1.length <= s.length) {
					s.alive = false;
					snakeToObstacle(s);
					return false;
				}
			}
			else if (s1.dir == STOP) {
				s.length++;

				s1.alive = false;

				s.tail[s.length - 1].x = -2;
				s.tail[s.length - 1].y = -2;
			}
			else {
				snakeToObstacle(s1);
				s1.alive = false;
			}

			break;
		}

		// Tail collision
		if (s1.alive)
			for (int i = 1; i < s1.length; i++) {
				Point& t = s1.tail[i];

				if (t == s.pos) {
					s.alive = false;
					snakeToObstacle(s);
					return false;
				}
			}
	}

	// Fruit!
	for (Point& f : fruit) {
		if (f.x == s.pos.x && f.y == s.pos.y) {
			s.length++;

			s.tail[s.length - 1].x = -2;
			s.tail[s.length - 1].y = -2;

			f.x = -1;
			f.y = -1;

			populateFruit();
		}
	}

	return true;
}

void Game::move(Snake& s) {

	Point m;

	s.dir = s.w_dir;

	switch (s.dir) {
	case UP:
		m.y -= 1;
		break;
	case DOWN:
		m.y += 1;
		break;
	case LEFT:
		m.x -= 1;
		break;
	case RIGHT:
		m.x += 1;
		break;
	}

	if (m.x >= grid_size)
		m.x = 0;
	else if (m.x < 0)
		m.x = (grid_size - 1);

	if (m.y >= grid_size)
		m.y = 0;
	else if (m.y < 0)
		m.y = (grid_size - 1);

	// Update snake
	s.pos += m;

	// Tail movement
	int prevX = 0, prevY = 0, prev2X = 0, prev2Y = 0;

	// Save last, previous tail in case we need to create an obstacle
	s.t_last = s.tail[s.length - 1];

	s.tail[0].x = prevX = s.pos.x;
	s.tail[0].y = prevY = s.pos.y;

	for (int i = 1; i < s.length; i++) {
		prev2X = s.tail[i].x;
		prev2Y = s.tail[i].y;

		s.tail[i].x = prevX;
		s.tail[i].y = prevY;

		prevX = prev2X;
		prevY = prev2Y;
	}
}

void Game::logic() {
	if (state == RUNNING) {
		auto diff = clock_end - system_clock::now();
		auto secs = duration_cast<float_seconds>(diff);

		if (secs.count() < 0) {

			clock_end = system_clock::now();
			clock_end += chrono::milliseconds(delay);

			bool dead = false;

			for (Snake& s : snakes) {

				move(s);

				checkCollision(s);

				dead = dead && !s.alive;
			}

			if (dead)
				die();
		}
	}
}

void Game::draw() {
	window.clear();

	if (state == MAIN_MENU) {
		Text txt = Text("PLAY", font, menu_size * 0.6);
		txt.setOrigin(Vector2f(txt.getLocalBounds().width / 2u, txt.getLocalBounds().height / 2u));

		Text txt1 = Text(txt);
		txt1.setString("SETTINGS");

		Text txt2 = Text(txt);
		txt2.setString("QUIT");

		map(txt, window.getView().getSize().x / 2u, menu_size * 2);
		map(txt1, window.getView().getSize().x / 2u, menu_size * 3);
		map(txt2, window.getView().getSize().x / 2u, menu_size * 4);

		RectangleShape crc = RectangleShape(Vector2f(menu_size * 0.6, menu_size * 0.6));
		crc.setFillColor(Color::Red);

		crc.setOrigin(Vector2f(crc.getLocalBounds().width / 2u, crc.getLocalBounds().height / 2u));

		map(crc, (window.getView().getSize().x / 2u) + txt.getOrigin().x - (menu_size * 2),
			txt.getPosition().y + txt.getOrigin().y + menu_size * menu_item);

		window.draw(txt);
		window.draw(txt1);
		window.draw(txt2);
		window.draw(crc);
	}
	else if (state == RUNNING) {

		for (auto o : lines) {
			window.draw(o);
		}

		// Draw snakes
		for (Snake& s : snakes) {
			if (!s.alive)
				continue;

			int x = s.pos.x * unit_size;
			int y = s.pos.y * unit_size;

			map(s.shape, x, y);

			window.draw(s.shape);

			// Tail
			for (int i = 1; i < s.length; i++) {
				Point& t = s.tail[i];

				RectangleShape sh = RectangleShape(s.shape);

				x = t.x * unit_size;
				y = t.y * unit_size;

				map(sh, x, y);

				window.draw(sh);
			}
		}

		RectangleShape o_sh = RectangleShape(Vector2f(unit_size + 1, unit_size + 1));
		o_sh.setFillColor(Color::White);

		// Obstacles
		for (Point& o : obstacles) {
			int x = o.x * unit_size;
			int y = o.y * unit_size;

			RectangleShape o_sh1 = RectangleShape(o_sh);

			map(o_sh1, x, y);

			window.draw(o_sh1);
		}

		// Fruit
		struct {
			float x;
			float y;
		} size = { (unit_size * 0.5), (unit_size * 0.5) };

		RectangleShape f_obj = RectangleShape(Vector2f(size.x, size.y));
		f_obj.setFillColor(Color::Red);

		for (Point& f : fruit) {
			if (f.x != -1) {
				int x = f.x * unit_size + unit_size / 2u - size.x / 2.f;
				int y = f.y * unit_size + unit_size / 2u - size.y / 2.f;

				RectangleShape f_shape = RectangleShape(f_obj);
				map(f_shape, x, y);

				window.draw(f_shape);
			}
		}
	}
	else if (state == DEAD) {
		Text txt = Text("PLAY AGAIN", font, menu_size * 0.6);
		txt.setOrigin(Vector2f(txt.getLocalBounds().width / 2u, txt.getLocalBounds().height / 2u));

		Text txt2 = Text(txt);
		txt2.setString("BACK");

		txt.setPosition(window.getView().getSize().x / 2u, window.getView().getSize().y / 16 * 7);
		txt2.setPosition(window.getView().getSize().x / 2u, window.getView().getSize().y / 16 * 8);

		RectangleShape crc = RectangleShape(Vector2f(menu_size * 0.5f, menu_size * 0.5f));
		crc.setOrigin(Vector2f(10, crc.getLocalBounds().height / 2u));
		crc.setFillColor(Color::Red);
		crc.setPosition((window.getView().getSize().x / 2u) - ((txt.getLocalBounds().width / 2u) +
			(0.5 * menu_size)), window.getView().getSize().y / 16 * 7 +
			txt.getLocalBounds().height / 2u - 2 + menu_item * 50);

		window.draw(txt);
		window.draw(txt2);
		window.draw(crc);
	}
	else if (state == SETTINGS) {
		Text back = newText("BACK", menu_size * 0.35);
		back.setPosition(menu_size, menu_size * 0.4);

		if (menu_item == 0)
			back.setFillColor(Color::Red);

		// PLAYERS

		Text player = newText("PLAYERS", menu_size * 0.35);
		player.setPosition(window.getView().getSize().x / 2, menu_size * 2);

		if (menu_item == 1)
			player.setFillColor(Color::Red);

		Text num = newText(toString(players), menu_size * 0.35);
		num.setPosition(window.getView().getSize().x / 2, menu_size * 2.4);

		CircleShape left = CircleShape(5, 3);
		left.setOrigin(left.getRadius(), left.getRadius());
		left.rotate(-90);
		left.setFillColor(Color::White);
		left.setPosition(window.getView().getSize().x / 2 - 50, menu_size * 2.50);

		CircleShape right = CircleShape(left);
		right.rotate(180);
		right.setPosition(window.getView().getSize().x / 2 + 50, menu_size * 2.50);

		// GRID_SIZE

		Text grid = newText("GRID SIZE", menu_size * 0.35);
		grid.setPosition(window.getView().getSize().x / 2, menu_size * 2.8);

		if (menu_item == 2)
			grid.setFillColor(Color::Red);

		Text g_num = newText(toString(grid_size), menu_size * 0.35);
		g_num.setPosition(window.getView().getSize().x / 2, menu_size * 3.2);

		CircleShape l1 = CircleShape(left);
		l1.setPosition(window.getView().getSize().x / 2 - 50, menu_size * 3.3);

		CircleShape r1 = CircleShape(l1);
		r1.rotate(180);
		r1.setPosition(window.getView().getSize().x / 2 + 50, menu_size * 3.3);

		// SPEED

		Text speed = newText("SPEED", menu_size * 0.35);
		speed.setPosition(window.getView().getSize().x / 2, menu_size * 3.6);

		if (menu_item == 3)
			speed.setFillColor(Color::Red);

		Text s_num = newText(toString(delay), menu_size * 0.35);
		s_num.setPosition(window.getView().getSize().x / 2, menu_size * 4.0);

		CircleShape l2 = CircleShape(left);
		l2.setPosition(window.getView().getSize().x / 2 - 50, menu_size * 4.1);

		CircleShape r2 = CircleShape(l2);
		r2.rotate(180);
		r2.setPosition(window.getView().getSize().x / 2 + 50, menu_size * 4.1);

		window.draw(back);
		window.draw(player);
		window.draw(num);
		window.draw(left);
		window.draw(right);
		window.draw(grid);
		window.draw(g_num);
		window.draw(l1);
		window.draw(r1);
		window.draw(l2);
		window.draw(r2);
		window.draw(speed);
		window.draw(s_num);
	}

	window.display();
}

Point Game::randomCoord() {

	// Is point already taken?
	bool ok = false;

	Point rnd;

	int rnd_attempts = 0;

	// Is this better than recursion?!
	while (!ok) {
		if (rnd_attempts > grid_size * 2) {
			Point p = Point(-3, -3);
			cout << p.x << " : " << p.y << endl;
			return p;
		}

		rnd_attempts++;

		rnd = { rand() % grid_size, rand() % grid_size };

		bool taken = false;

		// Check if any fruit occupies the position
		for (Point& f : fruit) {
			if (f.x == rnd.x && f.y == rnd.y)
			{
				taken = true;
				break;
			}
		}

		// Check for snail head
		if (!taken)
			for (Snake& s : snakes) {
				if (s.pos.x == rnd.x && s.pos.y == rnd.y) {
					taken = true;
					break;
				}

				// Tail
				for (auto t : s.tail) {
					if (t.x == rnd.x && t.y == rnd.y) {
						taken = true;
						break;
					}
				}
			}

		ok = !taken;
	}

	cout << "RND RET: " << rnd.x << " : " << rnd.y << endl;

	return rnd;
}

void Game::clear() {
	for (Point& f : fruit) {
		f.x = -1;
		f.y = -1;
	}

	for (Snake& s : snakes) {
		for (Point& t : s.tail) {
			t.x = -4;
			t.y = -4;
		}
	}

	for (Point& o : obstacles) {
		o.x = -3;
		o.y = -3;
	}
}

void Game::populateFruit() {
	for (Point& f : fruit) {
		if (f.x == -1) {
			Point rnd = randomCoord();

			if (rnd.x == -3 && rnd.y == -3)
				break;

			f.x = rnd.x;
			f.y = rnd.y;
		}
	}
}

Direction Game::dirOposite(Direction dir1) {
	switch (dir1) {
	case UP:
		return DOWN;
	case LEFT:
		return RIGHT;
	case RIGHT:
		return LEFT;
	case DOWN:
		return UP;
	}
}

void Game::snakeToObstacle(Snake& s) {
	for (Point& t : s.tail) {
		if (t.x != -4 && t.y != -4) {
			obstacles.push_back({ t.x, t.y });
			cout << "Added obstacle at " << t.x << " : " << t.y << endl;
		}
	}
}

sf::Text Game::newText(String str) {
	return newText(str, 32);
}

sf::Text Game::newText(String str, int size) {
	Text text = Text(str, font, size);
	text.setOrigin(text.getLocalBounds().width / 2u, text.getLocalBounds().height / 2u);
	return text;
}

sf::String Game::toString(int n) {
	ostringstream os;
	os << n;
	return os.str();
}

void Game::redraw() {
	lines.clear();

	RectangleShape ver = RectangleShape(Vector2f(0.1f, limit));
	ver.setFillColor(Color(160, 160, 160));

	RectangleShape hor = RectangleShape(Vector2f(limit, 0.1f));
	hor.setFillColor(Color(160, 160, 160));

	for (int i = 1; i < grid_size; i++) {
		int x = i * unit_size;

		RectangleShape s = RectangleShape(ver);
		map(s, x, 0);

		RectangleShape s1 = RectangleShape(hor);
		map(s1, 0, x);

		lines.push_back(s);
		lines.push_back(s1);
	}

	for (Snake& s : snakes) {
		s.shape.setSize(Vector2f(unit_size + 1, unit_size + 1));
	}
}

void Game::update_view(sf::Event e) {
	sf::View view = window.getView();

	view.setSize(e.size.width, e.size.height);

	window.setView(view);
}

void Game::resize() {
	// Get resized dimensions
	float w = window.getView().getSize().x;
	float h = window.getView().getSize().y;

	// Take the limit.. the smaller dimension
	limit = h >= w ? w : h;

	cout << "Size limit defined to " << limit << endl;

	grid_ref = { (w - limit) / 2, (h - limit) / 2 };

	unit_size = limit / grid_size;
	menu_size = limit / 16;

	cout << "Unit dimension defined to " << unit_size << endl;
}

void Game::map(Transformable& shape, int x, int y) {
	shape.setPosition(window.mapPixelToCoords(Vector2i(grid_ref.x + x, grid_ref.y + y)));
}

void Game::menu_up() {
	if (menu_item > menu_min)
		menu_item -= 1;
	else if (menu_item == menu_min)
		menu_item = menu_max;
}

void Game::menu_down() {
	if (menu_item < menu_max)
		menu_item += 1;
	else if (menu_item == menu_max)
		menu_item = menu_min;
}