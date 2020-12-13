
class Drone
{
protected:
	double mass;
	double hp;							//Drone health (100 pixels) - used as reference between green bar and damage received
	double aim_angle;
	double x, x_dot, x_dotdot;
	double y, y_dot, y_dotdot;
	double theta;
	double in_thrust, out_thrust;
	double acceleration;
	double in_roll;
	double delta_time;
	int power_count, power_time;
	int index, GS[N_GS];			//Game Pad Controller Variables
	int id_drone;

public:
	Drone() { ; }
	Drone(double _x, double _y, double _theta);
	void calculate();
	void reset(double _x, double _y, double _theta);
	void reset2(double _x, double _y, double _theta);
	double get_x();
	double get_x_dot() { return x_dot; }
	double& get_y();
	double get_y_dot() { return y_dot; }
	double get_theta();
	double& get_hp();					//Used as an access function for hp variable, return reference to get and set variable
	void set_delta_time();
	void inputs();
	void stability();
	void power();
	void controller();
	void bounce();
	double get_aim();
	bool get_gamepad_shoot();
	void draw();
	void get_damage();
	~Drone() { ; }
};
