
class Bullet
{
private:
	double x;
	double y;
	double theta;
	int id_bullet;
	bool state;			//Shot or not shot yet? Affects trajectory
	double speed;		//Bullet Speed
	int duration;


public:
	Bullet();
	bool& get_state();
	void set_initial(double _x, double _y, double _theta);	//Initialize starting position of bullet
	void trajectory();
	double get_x() { return x; }
	double get_y() { return y; }
	void map_rel(Drone name1);
	~Bullet() { ; }
};