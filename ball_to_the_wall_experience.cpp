#include <iostream>
#include <format>

#include "Framework.h"

using std::cout;
using std::endl;
using std::cin;
using std::string;

struct Ball {
	int x;
	int y;
	int radius;
	int speed;
	int direction;
};

struct WindowSize {
	int width = 800;
	int height = 600;
	bool fullscreen = false;
};

WindowSize* userWindowSize = new WindowSize;
extern int __argc;
extern char** __argv;
extern wchar_t** __wargv;

class MyFramework : public Framework {

public:

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		
		int counter;
		string tempArgString;
		cout << ("Program Name Is: %s", argv[0]) << endl;
		if (argc == 1)
			cout << ("\nNo Extra Command Line Argument Passed Other Than Program Name");
		if (argc >= 2)
		{
			cout << ("\nNumber Of Arguments Passed: %d", argc);
			cout << ("\n----Following Are The Command Line Arguments Passed----");
			for (counter = 0; counter < argc; counter++)
			{
				cout << ("\nargv[%d]: %s", counter, argv[counter]);
				if (argv[counter] == "-window")
				{
					tempArgString = argv[counter + 1];
					tempArgString = tempArgString.substr(0, tempArgString.find("x"));
					userWindowSize->width = stoi(tempArgString.substr(0, tempArgString.find("x")));
					userWindowSize->height = stoi(tempArgString.substr(1, tempArgString.find("x")));
					userWindowSize->fullscreen = false;
				};

			};
		};

		width = userWindowSize->width;
		height = userWindowSize->height;
		fullscreen = userWindowSize->fullscreen;
		cout << "This is the output after initialization of MyFramework" << endl;
		cout << "This is my_width" << width << endl;
		cout << "This is my_height" << height << endl;
		cout << "This is my_fullscreen" << fullscreen << endl;
	}

	virtual bool Init() {

		return true;
	}

	virtual void Close() {

	}

	virtual bool Tick() {
		drawTestBackground();
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {

	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {

	}

	virtual void onKeyPressed(FRKey k) {
	}

	virtual void onKeyReleased(FRKey k) {
	}

	virtual const char* GetTitle() override
	{
		return "Arcanoid";
	}
};


int main(int argc, char* argv[])
{

	
	//FirstClass.PreInit(coolInt1, coolInt2 , myBool);
	cout << "Starting game...\n";
	return run(new MyFramework);
}
