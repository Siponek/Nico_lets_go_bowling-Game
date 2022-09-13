/// <summary>
/// Prototype of game to be run with custom SDL2 library. Code from custom library is hidden / cannot see what SDL2 components are being used.\
/// Bug could possibly be that the spirtes are drown from weird coordinates. 
/// </summary>
#include <iostream>
#include <filesystem>

#include "Framework.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using namespace std::filesystem;

struct Ball
{
	int x = 1;
	int y = 1;
	int radius;
	int speed;
	int direction;
};

//? Temporarty struct for managing window of the game
struct WindowSize
{
	mutable int width = 800;
	mutable int height = 600;
	mutable bool fullscreen = false;
};

// These are going to be shared with dll or something?
extern WindowSize *userWindowSize = new WindowSize();
extern int __argc;
extern char **__argv;
extern wchar_t **__wargv;
static float resolution_param_h, resolution_param_w;
static int resolution_h, resolution_w;
void millisecond_wait(unsigned ms);

class MyFramework : public Framework
{

public:
	FRKey key_api;
	FRMouseButton mouse_api;
	virtual void PreInit(int &width, int &height, bool &fullscreen)
	{
		width = userWindowSize->width;
		height = userWindowSize->height;
		fullscreen = userWindowSize->fullscreen;
	}

	virtual bool Init()
	{
		getScreenSize(resolution_w, resolution_h);
		resolution_param_w = resolution_w / static_cast<float>(800);
		resolution_param_h = resolution_h / static_cast<float>(600);
		cout << "Current resolution is: " << resolution_h << "x" << resolution_w << endl;
		cout << "Resolution scale is: " << resolution_param_h << "x" << resolution_param_w << endl;
		cout << "Ticks from library initialization" << getTickCount() << endl;

		return true;
	}

	virtual void Close()
	{
		cout << "Close function called" << endl;
	}

	//Tick each moment - need to re-draw everything per frame
	virtual bool Tick()
	{	
		//Adding scale for different resolutions
		static bool _init_arena = false;
		static int size_h, size_w;
		static Sprite* test_block = createSprite(".\\data\\01-Breakout-Tiles.png");
		getSpriteSize(test_block, size_w, size_h);
		static int scaled_size_h = size_h / static_cast <float>(4) * resolution_param_h;
		static int scaled_size_w = size_w / static_cast <float>(4) * resolution_param_w;
		if (_init_arena == false)
		{
			cout << "Scaled the size to " << scaled_size_h << " and " << scaled_size_w << endl;
			_init_arena = true;
		};
		setSpriteSize(test_block, scaled_size_w, scaled_size_h);
		for (int x = 0; x <= scaled_size_w*8; x += scaled_size_w)
		{
			for (int y = 0 ; y <= scaled_size_h*5; y += scaled_size_h )
			{
				drawSprite(test_block, x , y);
				 
			}

		}
		
		static Sprite* test_mouse_block = createSprite(".\\data\\49-Breakout-Tiles.png");
		static int size_mouse_h, size_mouse_w;
		static bool _init_time_mouse = false;
		getSpriteSize(test_mouse_block, size_mouse_w, size_mouse_h);
		static int scaled_size_mouse_h = size_mouse_h / static_cast <float>(4) * resolution_param_h;
		static int scaled_size_mosue_w = size_mouse_w / static_cast <float>(4) * resolution_param_w;
		if (_init_time_mouse == false)
		{
			_init_time_mouse = true;
			setSpriteSize(test_mouse_block, scaled_size_w, scaled_size_h);
			cout << "Initalized mouse at "<< resolution_w / 2 << " and " << resolution_h / 2 << endl;
		};
		
		drawSprite(test_mouse_block, resolution_w /2 - (size_mouse_w/2), resolution_h - 50);
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)
	{
		//static Sprite* test_mouse_block = createSprite(".\\data\\49-Breakout-Tiles.png");
		//static int size_mouse_h, size_mouse_w;
		//static bool _what_time = false;
		//getSpriteSize(test_mouse_block, size_mouse_w, size_mouse_h);
		//static int scaled_size_h = size_mouse_h / static_cast <float>(4) * resolution_param_h;
		//static int scaled_size_w = size_mouse_w / static_cast <float>(4) * resolution_param_w;
		//if (_what_time == false)
		//{
		//	cout << "Scaled the size to " << scaled_size_h << " and " << scaled_size_w << endl;
		//	_what_time = true;
		//};
		//setSpriteSize(test_mouse_block, scaled_size_w, scaled_size_h);
		//drawSprite(test_mouse_block, 50, 5);
		cout << "Mouse moved at x,y : " << x << "," << y << endl;
		cout << "Relative position : " << xrelative << "," << yrelative << endl;

		showCursor(true);
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased)
	{
		cout << "Mouse button clicked" << endl;
	}

	virtual void onKeyPressed(FRKey k)
	{
		cout << "Key pressed" << endl;
	}

	virtual void onKeyReleased(FRKey k)
	{
		cout << "Key released" << endl;
	}

	virtual const char *GetTitle() override
	{
		cout << "GetTitle function called" << endl;
		return "Arcanoid fucking sucks";
	}
};

 void millisecond_wait(unsigned ms)
 {
 	std::chrono::milliseconds dura(ms);
 	std::this_thread::sleep_for(dura);
 }
 // Function for managing CLI intput for window resolution
void manageCLIwindow()
{
	int counter;
	string tempArgStringFirst = "", tempArgStringSecond = "";
	cout << "Program Name Is: " << __argv[0] << endl;
	if (__argc == 1)
	{
		cout << ("\nNo Extra Command Line Argument Passed Other Than Program Name") << endl;
	}
	else if (__argc >= 2)
	{
		cout << "Number Of Arguments Passed: " << __argc << endl;
		cout << ("----Following Are The Command Line Arguments Passed----") << endl;
		for (counter = 0; counter < __argc; counter++)
		{
			cout << "__argv[" << counter << "]:" << __argv[counter] << endl;
			tempArgStringFirst = __argv[counter];
			if (tempArgStringFirst == "-w")
			{
				tempArgStringSecond = __argv[counter + 1];
				userWindowSize->width = std::stoi(tempArgStringSecond);
			}
			else if (tempArgStringFirst == "-h")
			{
				tempArgStringSecond = __argv[counter + 1];
				userWindowSize->height = std::stoi(tempArgStringSecond);
			}
			else if (tempArgStringFirst == "-f")
			{
				userWindowSize->fullscreen = true;
			}
			else if (tempArgStringFirst == "-window")
			{
				tempArgStringFirst = __argv[counter + 1];
				tempArgStringSecond = __argv[counter + 1];
				tempArgStringFirst = tempArgStringFirst.substr(0, tempArgStringFirst.find("x"));
				tempArgStringSecond = tempArgStringSecond.substr(tempArgStringFirst.length() + 1, tempArgStringSecond.length());
			};
		}

		try
		{
			userWindowSize->width = std::stoi(tempArgStringFirst);
			userWindowSize->height = std::stoi(tempArgStringSecond);
		}
		catch (const std::exception &)
		{
			cout << "Error: Invalid arguments" << endl;
			exit(11);
		}

		cout << "Assigning values to window : " << std::stoi(tempArgStringFirst) << " and " << stoi(tempArgStringSecond) << endl;
		userWindowSize->width = std::stoi(tempArgStringFirst);
		userWindowSize->height = stoi(tempArgStringSecond);
		userWindowSize->fullscreen = false;
	};
}



int main(int argc, char *argv[])
{
	//	Co tu sie odpierdala
	std::filesystem::path path_to_directory = std::filesystem::current_path();
	const char *path_to_directory_char = "C:\\Users\\szink\\OneDrive\\Pulpit\\C C++ Projects\\unrealntership\\vs_solutions_SZ\\ball_to_the_wall_experience\\data\\01-Breakout-Tiles.png";
	// path_to_directory_char = "C:\Users\szink\OneDrive\Pulpit\C C++ Projects\unrealntership\vs_solutions_SZ\ball_to_the_wall_experience\data\01-Breakout-Tiles.png";
	path_to_directory += "\\data\\01-Breakout-Tiles.png";
	cout << "MAIN: Starting game..." << endl;
	cout << "MAIN: Current path for image is: " << path_to_directory << endl;
	cout << "MAIN: Current path for image is Cstring: " << path_to_directory_char << endl;

	manageCLIwindow();
	return run(new MyFramework);
}
