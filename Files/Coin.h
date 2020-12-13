
/*
Declare array of coin objects. Map each object location x and y position from a map_locations function, updates with moving map. Draw coins (if state = 1)
the to the screen above the layers but below the drone After: they can be grabbed with grab_coin function and state = 0 to disappear... so only draw if state = 1
reset_coin function to return coin object to state 1.
*/

class Coin {
	double coin_x;
	double coin_y;
	bool coin_state;					//when state == 0, coin disappears, coin returns to state == 1 after time passes
	static int collected_coins;			//static variable shared among all objects, they will all increment this value when they are grabbed! (grab_coin function being called)
	double respawn_timer;

	/*  No longer apart of the coin class:
	int id_coin1;
	int id_coin2;
	int id_coin3;
	int id_coin4;
	double coin_rotating;
	*/

public:
	Coin();										//constructor for each coin object, all should spawn ie: state == 1
	double& get_x() { return coin_x; };			//access function necessary when mapping coins from a text file
	double& get_y() { return coin_y; };			//access function necessary when mapping coins from a text file
	bool& get_state() { return coin_state; };
	int& get_collected_coins() { return collected_coins; };
	double& get_respawn_timer() { return respawn_timer; };
	void Coin::draw_coin(Drone& name1, map& layer, Animation& coin_animations);	 //this is called to draw each coin object, using layer 4 relative displacement and coin animation
	Box* hitbox;								//Each coin will have an area hitbox
	~Coin();
};