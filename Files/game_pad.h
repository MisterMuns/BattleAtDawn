
// get an array GS that holds the gamepad state.
// index is the number of the gamepad (index = 1 is gamepad #1, 
// index = 2 is gamepad #2, etc.).
// the function returns 0 if success and 1 if there's an error
int gamepad_state(int GS[], int index);

// size of array GS
#define N_GS 16

// elements of GS array
#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_X 2
#define BUTTON_Y 3
#define BUTTON_UP 4
#define BUTTON_DOWN 5
#define BUTTON_LEFT 6
#define BUTTON_RIGHT 7
#define TRIG_LEFT 8
#define TRIG_RIGHT 9
#define LEFT_STICK_X 10
#define LEFT_STICK_Y 11
#define RIGHT_STICK_X 12
#define RIGHT_STICK_Y 13
#define LEFT_STICK_BUTTON 14
#define RIGHT_STICK_BUTTON 15