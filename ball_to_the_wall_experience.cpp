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
	virtual void PreInit(int &width, int &height, bool &fullscreen)
	{
		width = userWindowSize->width;
		height = userWindowSize->height;
		fullscreen = userWindowSize->fullscreen;
	}

	virtual bool Init()
	{
		getScreenSize(resolution_h, resolution_w);
		resolution_param_h = resolution_h / static_cast<float>(800);
		resolution_param_w = resolution_w / static_cast<float>(600);
		cout << "Current resolution is: " << resolution_h << "x" << resolution_w << endl;
		cout << "Resolution scale is: " << resolution_param_h << "x" << resolution_param_w << endl;
		cout << "Ticks from library initialization" << getTickCount() << endl;

		return true;
	}

	virtual void Close()
	{
		cout << "Close function called" << endl;
	}

	//Tick dzia³a w ka¿dej chwili, wiêc screen jest renderowany w ka¿dej chwili
	virtual bool Tick()
	{	
		//Adding scale for different resolutions
		static bool _what_time = false;
		static int scaled_size_h = 384 * resolution_param_h * 0.25;
		static int scaled_size_w = 128 * resolution_param_w * 0.25;
		static Sprite* test_block = createSprite(".\\data\\01-Breakout-Tiles.png");
		if (_what_time == false)
		{
			cout << "Scaled the size to " << scaled_size_h << " and " << scaled_size_w << endl;
			_what_time = true;
		};
		setSpriteSize(test_block, scaled_size_h, scaled_size_w);
		for (	int y = 0 ; y <= scaled_size_h; y += scaled_size_h / 4)
		{
			for (int x = 0; x <= scaled_size_w*10; x += scaled_size_w *4)
			{
				drawSprite(test_block, x , y);
			}
		}
		
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)
	{
		//cout << "Mouse moved" << endl;
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
