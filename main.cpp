#include <SFML/Graphics.hpp>
#include "ball.h"

const int WIDTH = 800;
const int HEIGHT = 600;
const int NUM_BALLS = 5;
const int NUM_WALLS = 2;
const int BT = 8;           //border thickness

int main()
{
	sf::RenderWindow theWindow(sf::VideoMode(WIDTH + 2 * BT, HEIGHT + 2 * BT),"Ball Demo");
	sf::RectangleShape border;
	border.setPosition(sf::Vector2f(BT, BT));
	border.setSize(sf::Vector2f(WIDTH, HEIGHT));
	border.setOutlineThickness(BT);
	border.setOutlineColor(sf::Color::Cyan);
	border.setFillColor(sf::Color::Transparent);

	Ball bs[NUM_BALLS];
	Wall ws[NUM_WALLS];

	bs[0] = Ball();
	bs[1] = Ball(sf::Vector2f(700, 300), sf::Vector2f(-120, -200), 20, GOLD);
	bs[2] = Ball(sf::Vector2f(500, 500), sf::Vector2f(250, 100), 25, STONE);
	bs[3] = Ball(sf::Vector2f(300, 450), 200, 91, 30, IRON);
	bs[4] = Ball(sf::Vector2f(400, 100), 130, 280, 35, WOOD);

	bs[0].set_position(40, 280);
	bs[0].set_radius(25);
	bs[0].set_angle(280);
	bs[0].set_speed(-600);

	ws[0] = Wall(sf::Vector2f(180, 260), sf::Vector2f(310, 180));
	ws[1] = Wall(sf::Vector2f(540, 400), ws[0]);

	//ws[0].set_points(sf::Vector2f(300, 200), sf::Vector2f(360, 270));

	sf::Clock theClock;
	sf::Time elps;

	int i, j;
	float r;
	sf::Vector2f pos, vel;

	while (theWindow.isOpen())
	{
		sf::Event event;
		while (theWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				theWindow.close();
		}

		elps = theClock.getElapsedTime();
		theClock.restart();

		for (i = 0; i<NUM_BALLS; i++)
		{
			bs[i].update_position(elps);
			pos = bs[i].get_position();
			vel = bs[i].get_velocity();
			r = bs[i].get_radius();

			if ((pos.x<BT + r && vel.x<0) || (pos.x>WIDTH + BT - r && vel.x>0))
				bs[i].bounce(0);
			if ((pos.y<BT + r && vel.y<0) || (pos.y>HEIGHT + BT - r && vel.y>0))
				bs[i].bounce(90);

			for (j = 0; j<i; j++)
			{
				if (bs[i].is_colliding_with(bs[j]))
					collide(bs[i], bs[j]);
			}
			for (j = 0; j<NUM_WALLS; j++)
			{
				if (bs[i].is_colliding_with(ws[j]))
					collide(bs[i], ws[j]);
			}
		}

		theWindow.clear(sf::Color::Blue);
		theWindow.draw(border);

		for (i = 0; i<NUM_WALLS; i++)
			theWindow.draw(ws[i].get_rectangleShape());

		for (i = 0; i<NUM_BALLS; i++)
			theWindow.draw(bs[i].get_circleShape());

		theWindow.display();
	}

	return 0;
}