

class Enemy :public Drone
{
private:
	double dist_x;
	double dist_y;
	double radius, radius_limit;
	static int enemy_killcount;

public:
	Enemy() { ; }
	Enemy(double _x, double _y, double _theta) : Drone(_x, _y, _theta) {}
	void inputs(double player_x, double player_y);
	void reset(double _x, double _y, double _theta);
	void stability();
	void calculate();
	void map_rel(Drone name1);
	double get_aim(Drone player);
	double get_radius() { return radius; }
	void check_health(int& kill_counter);
	void reset_killcount();
	int get_killcount();
	~Enemy() { ; }

};
