
class Sound
{
private:
	int n;
	char* p_buffer, * p_buffer_end;
	short int* p_data;

public:
	Sound(char file_name[]);
	void play();
	~Sound() { ; }
};