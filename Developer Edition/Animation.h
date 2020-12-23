
class Animation
{
private:
	int sequence[20];
	int size;
	int index;
	double duration;
	int delay;
	int x;
	int y;
	double scale;
	bool trigger_state;
public:
	Animation(char file_seq[], double _duration, double _scale);
	void trigger(double _x, double _y);
	void animate();
	bool& return_trig() { return trigger_state; }
	~Animation() { ; }
};