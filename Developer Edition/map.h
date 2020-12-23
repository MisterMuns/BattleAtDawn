
//map class using Parallax technique.
class map { //This class creates the layer objects that the environment is composed of

	int id_layer;		//id of the layer object that is set when constructor is called
	double layer_x;	//layer object's x position
	double layer_y;	//layer object's y position
	double last_x; //start position of layer, used for draw_coin
	double last_y; //start position of layer, used for draw_coin
	double layer_scale;
	double df;			//depth factor, the depth of the layer will influence scroll rate
	double max_pos_x;		//furthest layer will displace right as drone moves LEFT
	double max_neg_x;		//furthest layer will displace left as drone moves RIGHT
	double max_pos_y;		//furthest layer will displace up as drone moves DOWN
	double max_neg_y;		//furthest layer will displace down as drone moves UP

public:
	map(char layer_file_name[], double depth_factor, double layerX, double layerY, double scale, double _max_pos_x, double _max_neg_x, double _max_pos_y, double _max_neg_y);		//constructor to initialize the layer object, called outside of the infinite draw loop
	void reset(double layerX, double layerY);
	void draw_layer(Drone& name1);			//this will contain "draw_sprite()" and manipulate x and y of layer to track drone object properly.Called inside infinite draw loop
	double get_layerX();
	double get_layerY();
	double get_lastX();
	double get_lastY();
	double get_max_pos_x();
	double get_max_pos_y();
	double get_max_neg_x();
	double get_max_neg_y();
	~map() { ; }
};