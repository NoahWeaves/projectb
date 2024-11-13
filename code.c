/*

Written by Montek Singh
Copyright and all rights reserved by Montek Singh
Last Updated:  Nov 8, 2024

Permission granted to use this only for students, teaching/learning assistants
and instructors of the COMP 541 course at UNC Chapel Hill.
For any other use, contact Montek Singh first.

*/


/*

This is a C template for initial development
of your demo app for COMP541 find projects!

You must compile and run this code in an ANSI
compatible terminal.  You can use the terminal app
in the course VM.  For macOS and Linux users, the
standard terminal/shell on your laptop is also ANSI
compatible.

Open a terminal and compile and run the code as follows:

	gcc code.c
	./a.out

*/



/* Specify the keys here that get_key() will look for,
returning 1 if the first key was found, 2, for the second key, etc.,
and returning 0 if none of these keys was found.
In the actual board-level implementation, you will define
scancodes instead of characters, and you can use specify 
key releases as well.
*/

int key_array[] = {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', ' ', '1', '2', '3'};


int key_array2[] = {};

/* Specify the keys here that get_key2() will look for. */




/* Let us define our sprites.  These will be text approximations
of the actual sprites used in the board implementation.
Here, each sprite is specified by:
	{ text character, foreground color, background color }

For example, specifying a sprite as
	{'.', white, red},
means it is drawn as a white dot over a red background.

Specify the number of sprites first (Nchars), and then the
attributes of each sprite.
*/

// type definition for emulating sprites (see below)
typedef struct {
	char char_to_display;
	int fg_color;
	int bg_color;
} sprite_attr;


#define Nchars 4

enum colors {black, red, green, yellow, blue, magenta, cyan, white};

sprite_attr sprite_attributes[Nchars] = {
	{' ', white, white},
	{'V', green, green},
	{'O', black, black},
	{'\\', blue, white}
};


//===============================================================
// Here are the functions available for I/O.  These correspond
// one-to-one to functions available in MIPS assembly in the
// helper files provided.
//
// NOTE:  There is one function specific to the C implementation
// that is not needed in the assembly implementation:
//     void initialize_IO(char* smem_initfile);
//===============================================================

void my_pause(int N);  	// N is hundredths of a second

void putChar_atXY(int charcode, int col, int row);
	// puts a character at screen location (X, Y)

int getChar_atXY(int col, int row);
	// gets the character from screen location (X, Y)

int get_key();
	// if a key has been pressed and it matches one of the
	// characters specified in key_array[], return the
	// index of the key in that array (starting with 1),
	// else return 0 if no valid key was pressed.

int get_key2();
	// similar to get_key(), but looks for key in
	// key_array2[].

int pause_and_getkey(int N);
	// RECOMMENDED!
	// combines pause(N) with get_key() to produce a 
	// *responsive* version of reading keyboard input

void pause_and_getkey_2player(int N, int* key1, int* key2);
	// 2-player version of pause_and_getkey().

int get_accel();
	// returns the accelerometer value:  accelX in bits [31:16], accelY in bits [15:0]
	// to emulate accelerometer, use the four arrow keys

int get_accelX();
	// returns X tilt value (increases back-to-front)

int get_accelY();
	// returns Y tilt value (increases right-to-left)

void put_sound(int period);
	// visually shows approximate sound tone generated
	// you will not hear a sound, but see the tone highlighted on a sound bar

void sound_off();
	// turns sound off

void put_leds(int pattern);
	// put_leds: set the LED lights to a specified pattern
	//   displays on row #31 (below the screen display)

void initialize_IO(char* smem_initfile);


//===============================================================
// This is the code for your demo app!
//===============================================================

#define GRID_WIDTH 16
#define GRID_HEIGHT 8
#define GRID_START_X 10    // Starting X coordinate on screen
#define GRID_START_Y 10    // Starting Y coordinate on screen

int note_periods[] = {
    910567,   // C3  
    858785,   // D3
    803571,   // E3
    757576,   // F3
    714286,   // G3
    674157,   // A3
    636943,   // B3
    601685    // C4
};

void update_leds(int col) {
    put_leds(1 << (GRID_WIDTH - 1 - col));  // Shift 1 to (15-col) position
}

int current_column = 0;
int grid_notes[GRID_WIDTH][GRID_HEIGHT] = {0};  // Add this with other globals
int is_playing = 0;

void play_column_notes(int col) {
    static int note_index = 0;  // Keep track of which note we're playing
    
    // Either play the note at current position or silence
    if(grid_notes[col][note_index] != -1) {
        put_sound(note_periods[grid_notes[col][note_index]]);
    } else {
        sound_off();
    }
    
    // Always move to next position
    note_index = (note_index + 1) & 7; // wont compile easily. can use bitwise and with 7 or something.
}

void draw_grid() {
    // Draw the marker row
    for(int x = 0; x < GRID_WIDTH; x++) {
        if(x == current_column) {
            putChar_atXY(1, GRID_START_X + x, GRID_START_Y - 1);  // Marker sprite for current column
        } else {
            putChar_atXY(0, GRID_START_X + x, GRID_START_Y - 1);  // Empty sprite for other columns
        }
    }
    
    // Draw the empty grid cells
    for(int y = 0; y < GRID_HEIGHT; y++) {
        for(int x = 0; x < GRID_WIDTH; x++) {
            if(grid_notes[x][y] == -1) {
                putChar_atXY(0, GRID_START_X + x, GRID_START_Y + y);  // Empty cell
            } else {
                putChar_atXY(2, GRID_START_X + x, GRID_START_Y + y);  // Note cell
            }
        }
    }
}


int main() {
	initialize_IO("");

	for(int x = 0; x < GRID_WIDTH; x++) {
    for(int y = 0; y < GRID_HEIGHT; y++) {
        grid_notes[x][y] = -1;  // -1 means empty cell
    }
	}

	draw_grid();

	while(1) {
    int key = pause_and_getkey(1);
    
    // Handle playback controls
    if(key == 10) {        // Play (1)
        is_playing = 1;
    }
    else if(key == 11) {   // Pause (2)
        is_playing = 0;
    }
    else if(key == 12) {   // Reset (3)
        is_playing = 0;
        current_column = 0;
        draw_grid();
    }
    
    // Handle note input and space only when not playing
    else if(!is_playing) {
        if(key == 9) {    // Space
            current_column = (current_column + 1) % GRID_WIDTH;
            draw_grid();
			update_leds(current_column);
        }
        else if(key >= 1 && key <= 8) {  // a through k
            if(grid_notes[current_column][key-1] == -1) {
                grid_notes[current_column][key-1] = key-1;
            } else {
                grid_notes[current_column][key-1] = -1;
            }
            draw_grid();
        }
    }
    
    // Move playback cursor if playing
    if(is_playing) {
        static int counter = 0;
        counter++;
        if(counter >= 100) {  // Move to next column every 10 cycles
            counter = 0;
            current_column = (current_column + 1) % GRID_WIDTH;
            draw_grid();
			update_leds(current_column);
        }
        play_column_notes(current_column);  // Play notes every cycle
    } else {
        sound_off();  // Make sure sound is off when not playing
    }
}

	return 0;

}


// The file below has the implementation of all of the helper functions
#include "procs.c"
