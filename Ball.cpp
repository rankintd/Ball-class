#pragma once
#include "ball.h"

//returns the magnitude (length) of the argument vector
float magnitude(const sf::Vector2f &v)
{
	return sqrt(v.x*v.x + v.y*v.y);
}

//returns the distance between the argument points/vectors
float distance(const sf::Vector2f &v, const sf::Vector2f &w)
{
	return magnitude(v - w);
}

//returns a number in range [0, 360) representing the angle the vector points.
//0 represents due east.
//angles from 0 to 90 represent positive x- and y-components.
//in the current form, returns 90 if the argument is the zero vector
float angle(const sf::Vector2f &v)
{
	if (v.x == 0)
	{
		if (v.y < 0) return 270;
		return 90;
	}

	float angle = atan(v.y / v.x) * 180 / PI;       //now -90 < angle < 90
	if (v.x < 0) angle += 180;
	else if (v.y < 0) angle += 360;              //Q4

	return angle;
}

//returns the dot product of the argument vectors
float dot(const sf::Vector2f &v, const sf::Vector2f &w)
{
	return v.x*w.x + v.y*w.y;
}

//sets the magnitude of the argument vector to len while preserving the vector's direction.
//if len is negative, the resulting vector will point in the opposite direction.
//if the argument vector is zero, the resulting vector will point in the y-direction
void set_magnitude(sf::Vector2f &v, float len)
{
	float curr = magnitude(v);
	if (curr == 0) v = sf::Vector2f(0, len);
	else v *= len / curr;
}

//changes the argument vector's angle to the specified value while preserving its length
void set_vector_angle(sf::Vector2f &v, float ang)
{
	float curr = magnitude(v);
	v.x = curr * cos(ang*PI / 180);
	v.y = curr * sin(ang*PI / 180);
}

//returns the vector projection of v onto w, or proj_w (v)
sf::Vector2f proj(const sf::Vector2f &v, const sf::Vector2f &w)
{
	if (dot(w, w) == 0)
	{
		handle_error(0);
		return v;
	}
	return dot(w, v) / dot(w, w) * w;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

//Default Wall constructor
Wall::Wall()
{
	*this = Wall(sf::Vector2f(20, 20), sf::Vector2f(30, 30), thDefault, wcDefault);
}

//Constructor
Wall::Wall(sf::Vector2f p1, sf::Vector2f p2)
{
	*this = Wall(p1, p2, thDefault, wcDefault);
}

//Constructor
Wall::Wall(sf::Vector2f p1, sf::Vector2f p2, float th)
{
	*this = Wall(p1, p2, th, wcDefault);
}

//Primary constructor
//allows only positive values for thickness
Wall::Wall(sf::Vector2f p1, sf::Vector2f p2, float th, sf::Color c)
{
	pt1 = p1;
	pt2 = p2;

	if (th<0) thickness = -th;
	else if (th == 0) thickness = thDefault;
	else thickness = th;

	fill_color = c;

	update_vectors();
}

//Constructor - creates a copy of wall w which looks identical but starts at the specified point
Wall::Wall(sf::Vector2f p1, const Wall& w)
{
	*this = Wall(p1, p1 + w.v_l, w.thickness, w.fill_color);
}

//Constructor - creates a copy of wall w whose endpoints are the specified points
Wall::Wall(sf::Vector2f p1, sf::Vector2f p2, const Wall& w)
{
	*this = Wall(p1, p2, w.thickness, w.fill_color);
}

//keeps the length and thickness vectors of wall updated.
//must be called any time the endpoints of the wall are altered.
//v_l points from pt1 to pt2 (along the length of the wall), and
//v_th points from pt1 to a corner (along the thickness/half the width of the wall)
void Wall::update_vectors()
{
	v_l = pt2 - pt1;

	v_th = sf::Vector2f(0, thickness);
	if (magnitude(v_l)>0) set_vector_angle(v_th, angle(v_l) + 90);
}

//returns a RectangleShape object which can be drawn
sf::RectangleShape Wall::get_rectangleShape() const
{
	sf::RectangleShape rect(sf::Vector2f(get_length(), 2 * thickness));
	rect.setOrigin(0, thickness);
	rect.setPosition(pt1);
	rect.setRotation(angle(v_l));
	rect.setFillColor(fill_color);

	return rect;
}

//sets the endpoints of this wall to the specified points and updates v_l and v_th accordingly
void Wall::set_points(sf::Vector2f p1, sf::Vector2f p2)
{
	pt1 = p1;
	pt2 = p2;

	update_vectors();
}

//sets the thickness of this wall to the specified value and updates v_th accordingly.
//allows only positive values for thickness
void Wall::set_thickness(float th)
{
	if (th<0) thickness = -th;
	else if (th == 0) thickness = thDefault;
	else thickness = th;

	set_magnitude(v_th, thickness);
}

//returns the coordinates of the specified point if the wall were re-oriented so pt1 sits
//at the origin and pt2 lies on the positive x-axis.
//under this system, the wall spans x-coordinates 0 to get_length() and y-coordinates -thickness to thickness
sf::Vector2f Wall::relative_coordinates(const sf::Vector2f &pt) const
{
	sf::Vector2f rel = pt - pt1;
	return sf::Vector2f(dot(rel, v_l) / magnitude(v_l), dot(rel, v_th) / thickness);
}

//returns true if the specified point lies in the interior of this wall; false otherwise
bool Wall::contains(const sf::Vector2f &pt) const
{
	sf::Vector2f coords = relative_coordinates(pt);

	return (coords.x >= 0 && coords.x <= get_length() && coords.y >= -thickness && coords.y <= thickness);
}

//returns the ACTUAL COORDINATES of the corner of this wall nearest to
//the point with the specified RELATIVE COORDINATES.
sf::Vector2f Wall::nearest_corner(const sf::Vector2f &rel_pt) const
{
	if (rel_pt.x <= get_length() / 2)
	{
		if (rel_pt.y >= 0) return pt1 + v_th;
		else return pt1 - v_th;
	}
	else
	{
		if (rel_pt.y >= 0) return pt2 + v_th;
		else return pt2 - v_th;
	}
	//to find the corner nearest a point, pt, where pt is in actual/global coordinates instead,
	//call nearest_corner(relative_coordinates(pt));
}

///////////////////////////////////////////////////////////////////////////////////////////////////

//Default Ball constructor
Ball::Ball()
{
	*this = Ball(sf::Vector2f(50, 50), sf::Vector2f(50, 50));
}

//Constructor
Ball::Ball(sf::Vector2f pos, sf::Vector2f vel)
{
	*this = Ball(pos, vel, rDefault, cDefault, dDefault);
}

//Constructor
Ball::Ball(sf::Vector2f pos, sf::Vector2f vel, float r)
{
	*this = Ball(pos, vel, r, cDefault, dDefault);
}

//Constructor
Ball::Ball(sf::Vector2f pos, sf::Vector2f vel, float r, sf::Color c)
{
	*this = Ball(pos, vel, r, c, dDefault);
}

//Primary Constructor
//radius and density will be set to nonnegative numbers; speed will not exceed MAX_SPEED
Ball::Ball(sf::Vector2f pos, sf::Vector2f vel, float r, sf::Color c, float dens)
{
	if (r<0) r = -r;               //ensures radius and density are nonnegative and in range
	if (r>MAX_RADIUS) r = MAX_RADIUS;
	if (dens<0) dens = -dens;

	position = pos;
	velocity = vel;
	radius = r;

	if (get_speed()>MAX_SPEED) set_speed(MAX_SPEED);

	fill_color = c;
	density = dens;

	mass = dens * 4.0 / 3.0 * PI * r*r*r;
}

//Constructor
Ball::Ball(sf::Vector2f pos, sf::Vector2f vel, float r, Material mat)
{
	//densities are in g/cm^3, from Wikipedia

	switch (mat)
	{
	case WOOD:
		*this = Ball(pos, vel, r, TAN, 0.70);
		break;

	case STONE:
		*this = Ball(pos, vel, r, GRAY, 2.00);
		break;

	case IRON:
		*this = Ball(pos, vel, r, sf::Color::Black, 7.87);
		break;

	case GOLD:
		*this = Ball(pos, vel, r, GLD, 19.32);
		break;

	case DEF:
	default:
		*this = Ball(pos, vel, r, cDefault, dDefault);
		break;
	}
}

//Constructor
Ball::Ball(sf::Vector2f pos, float spd, float ang)
{
	*this = Ball(pos, spd, ang, rDefault, cDefault, dDefault);
}

//Constructor
Ball::Ball(sf::Vector2f pos, float spd, float ang, float r)
{
	*this = Ball(pos, spd, ang, r, cDefault, dDefault);
}

//Constructor
Ball::Ball(sf::Vector2f pos, float spd, float ang, float r, sf::Color c)
{
	*this = Ball(pos, spd, ang, r, c, dDefault);
}

//Constructor
Ball::Ball(sf::Vector2f pos, float spd, float ang, float r, sf::Color c, float dens)
{
	float velx = spd * cos(ang*PI / 180);
	float vely = spd * sin(ang*PI / 180);

	*this = Ball(pos, sf::Vector2f(velx, vely), r, c, dens);
}

//Constructor
Ball::Ball(sf::Vector2f pos, float spd, float ang, float r, Material mat)
{
	float velx = spd * cos(ang*PI / 180);
	float vely = spd * sin(ang*PI / 180);

	*this = Ball(pos, sf::Vector2f(velx, vely), r, mat);
}

//Constructor - creates a copy of ball b (with the same velocity) at the specified position.
//Higher speeds will be reduced to MAX_SPEED
Ball::Ball(sf::Vector2f pos, const Ball &b)
{
	*this = Ball(pos, b.velocity, b.radius, b.fill_color, b.density);
}

//Constructor - creates a copy of ball b with the specified position and velocity
//Higher speeds will be reduced to MAX_SPEED
Ball::Ball(sf::Vector2f pos, sf::Vector2f vel, const Ball &b)
{
	*this = Ball(pos, vel, b.radius, b.fill_color, b.density);
}

//Constructor - creates a copy of ball b with the specified position, speed, and direction angle
//Higher speeds will be reduced to MAX_SPEED
Ball::Ball(sf::Vector2f pos, float spd, float ang, const Ball &b)
{
	*this = Ball(pos, spd, ang, b.radius, b.fill_color, b.density);
}

//returns the ball's speed (i.e. the length of its velocity vector)
float Ball::get_speed() const
{
	return magnitude(velocity);
}

//returns a number in range [0, 360) representing the degree angle of the ball's current motion.
//0 represents due east.
//angles from 0 to 90 represent positive x- and y-components of velocity.
//in the current form, returns 90 if the ball is not moving
float Ball::get_angle() const
{
	return angle(velocity);
}

//returns a CircleShape object which can be drawn
sf::CircleShape Ball::get_circleShape() const
{
	sf::CircleShape circ(radius, 24);
	circ.setPosition(position.x - radius, position.y - radius);     //CircleShape considers position to be the
	//top left corner of the ball, not the center
	circ.setFillColor(fill_color);
	return circ;
}

//Setters
//for now, no functions for directly setting color, density, or mass
void Ball::set_position(sf::Vector2f pos)
{
	position = pos;
}

void Ball::set_position(float a, float b)
{
	position = sf::Vector2f(a, b);
}

//does not let speed exceed MAX_SPEED
void Ball::set_velocity(sf::Vector2f vel)
{
	velocity = vel;
	float spd = get_speed();
	if (spd>MAX_SPEED) set_speed(MAX_SPEED);
}

//a and b are vector components, not speed and angle
void Ball::set_velocity(float a, float b)
{
	set_velocity(sf::Vector2f(a, b));
}

//only allows nonnegative r up to MAX_RADIUS.
//updates mass after updating radius
void Ball::set_radius(float r)
{
	if (r<0) r = -r;
	if (r>MAX_RADIUS) r = MAX_RADIUS;
	radius = r;
	mass = density * 4.0 / 3.0 * PI * r*r*r;
}

//changes the ball's speed to the argument value while preserving its direction of motion.
//for a negative argument, the ball will go the opposite direction.
//if the ball is currently not moving, its direction of motion is considered to be 90
void Ball::set_speed(float sp)
{
	if (sp>MAX_SPEED) sp = MAX_SPEED;
	else if (sp<-MAX_SPEED) sp = -MAX_SPEED;

	set_magnitude(velocity, sp);
}

//changes the ball's direction of motion to the provided value while preserving its speed
void Ball::set_angle(float ang)
{
	set_vector_angle(velocity, ang);
}

//returns true if this ball is colliding with the argument ball, false otherwise.
//if the balls touch at exactly one point, it is not considered a collision
bool Ball::is_colliding_with(const Ball &b) const
{
	if (distance(position, b.position) - (radius + b.radius) < 0) return true;
	return false;
}

//returns true if the argument point lies in the interior of this ball; false otherwise.
//identical to testing for collision with a "ball" with radius 0
bool Ball::is_colliding_with(const sf::Vector2f &point) const
{
	if (distance(position, point) < radius) return true;
	return false;
}

//returns true if this ball is colliding with the argument wall; false otherwise.
//if the ball and wall touch at exactly one point, it is not considered a collision
bool Ball::is_colliding_with(const Wall &w) const
{
	sf::Vector2f coords = w.relative_coordinates(position);

	//eliminate easy cases where there is obviously no collision
	if (coords.y <= -w.get_thickness() - radius || coords.y >= w.get_thickness() + radius) return false;
	if (coords.x <= -radius || coords.x >= w.get_length() + radius) return false;

	if (coords.x >= 0 && coords.x <= w.get_length()) return true;
	else if (coords.y >= -w.get_thickness() && coords.y <= w.get_thickness()) return true;
	else
	{
		sf::Vector2f corner = w.nearest_corner(coords);
		if (distance(position, corner)<radius) return true;
	}
	return false;
}

//bounces against a wall *perpendicular* to the argument angle
//a vertical wall (horizontal bounce) is represented by angle 0 or 180
void Ball::bounce(float ang)
{
	float alpha = get_angle();
	float beta = ang + 90;            //parallel to wall

	set_angle(2 * beta - alpha);
}

//alters this ball's velocity to account for bouncing off the argument wall.
//does not change the ball's velocity if it does not collide with the wall
//(so ball/wall collision does not need to be checked first).
//uses stutter protection if STUTTER_PROTECTION is set to true
void Ball::bounce_off_wall(const Wall &w)
{
	sf::Vector2f coords = w.relative_coordinates(position);

	//eliminate easy cases where there is obviously no collision
	if (coords.y <= -w.get_thickness() - radius || coords.y >= w.get_thickness() + radius) return;
	if (coords.x <= -radius || coords.x >= w.get_length() + radius) return;

	if (coords.x >= 0 && coords.x <= w.get_length())                     //bounces off main length of wall
	{
		if (!STUTTER_PROTECTION || dot(velocity, w.get_thick_vector()) * coords.y <= 0)
			bounce(angle(w.get_thick_vector()));
	}
	else
	{
		if (coords.y >= -w.get_thickness() && coords.y <= w.get_thickness())     //bounces off edge of wall
		{
			if (!STUTTER_PROTECTION || dot(velocity, w.get_length_vector()) * coords.x <= 0)
				bounce(angle(w.get_length_vector()));
		}
		else
		{
			sf::Vector2f corner = w.nearest_corner(coords);

			if (distance(position, corner)<radius)                      //bounces off corner of wall
			{
				if (!STUTTER_PROTECTION || dot(velocity, position - corner) <= 0)
					bounce(angle(position - corner));
			}
		}
	}
}

//updates the ball's position to account for the specified time passing.
//does not check for collisions
void Ball::update_position(sf::Time dT)
{
	float dt = dT.asSeconds();

	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
}

//alters the velocities of ball1 and ball2 to account for a collision.
//does not take rotational energy/torque into account.
//uses stutter protection if STUTTER_PROTECTION is set to true.
//***Does not check whether ball1 and ball2 are actually colliding***
void collide(Ball &ball1, Ball &ball2)
{
	float m = ball1.mass;
	float n = ball2.mass;

	sf::Vector2f v1 = ball1.velocity;
	sf::Vector2f v2 = ball2.velocity;

	sf::Vector2f x1 = ball1.position;
	sf::Vector2f x2 = ball2.position;

	if (x1 != x2)
	{
		if (!STUTTER_PROTECTION || dot(v2 - v1, x2 - x1)<0)
		{
			sf::Vector2f pr = proj(v2 - v1, x2 - x1);
			ball1.velocity = v1 + 2 * n / (m + n)*pr;
			ball2.velocity = v2 - 2 * m / (m + n)*pr;
			//formulas from Wikipedia - Elastic Collision
		}
	}
}

//calls bounce_off_wall
void collide(Ball &b, Wall &w)
{
	b.bounce_off_wall(w);
}

//can be edited and used if needed
int handle_error(int code)
{
	//error...
	return code;
}