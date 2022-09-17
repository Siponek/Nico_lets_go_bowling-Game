/// <summary>
/// Prototype of game to be run with custom SDL2 library. Code from custom library is hidden / cannot see what SDL2 components are being used.\
/// Bug could possibly be that the spirtes are drown from weird coordinates. 
/// </summary>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <vector>

#include "Framework.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::for_each;
using namespace std::filesystem;

struct Ball
{
	int x = 1;
	int y = 1;
	int radius;
	int speed;
	int x_direction, y_direction;
	bool isActive = false;
};

//? Temporarty struct for managing window of the game
struct WindowSize
{
	mutable int width = 800;
	mutable int height = 600;
	mutable bool fullscreen = false;
};

struct colored_block
{
	Sprite* block_sprite;
	int x, y;

};

class State
{
public:
	int x_player;
	int y_player;
	Ball ball_player;
	vector<colored_block> list_of_blocks;
	
	State()
	{
		x_player = 0;
		y_player = 0;
		ball_player.x = 0;
		ball_player.y = 0;
		ball_player.radius = 0;
		ball_player.speed = 0;
		ball_player.x_direction = 0;
		ball_player.y_direction = 0;
	}
};

int __argc;
char** __argv;
wchar_t** __wargv;
static float resolution_param_h, resolution_param_w;
static int resolution_h, resolution_w;
extern WindowSize* userWindowSize = new WindowSize();

void millisecond_wait(unsigned ms);
// Function for managing CLI intput for window resolution
void manageCLIwindow();
void drawSpriteStruct(colored_block tmp_struct);
/// Funtion to manage ball collision with screen borders
void manageBallCollision(State *_state_of_game, int *_scaled_size_ball_w, int *_scaled_size_ball_h);

class MyFramework : public Framework
{

public:
	FRKey key_api;
	FRMouseButton mouse_api;
	State state_of_game;
	
	Sprite* enemy_block_1;
	Sprite* player_sprite;
	Sprite* ball_sprite;
	bool _init_arena = false;
	int size_for_blocks_h, size_for_blocks_w;
	int scaled_size_for_blocks_h, scaled_size_for_blocks_w;
	int size_player_h, size_player_w;
	int scaled_size_player_h, scaled_size_player_w;
	int size_ball_h, size_ball_w;
	int scaled_size_ball_h, scaled_size_ball_w;
	bool _init_time_mouse = false;
	bool mouse_locked = false;
	
	virtual void PreInit(int &width, int &height, bool &fullscreen)
	{
		width = userWindowSize->width;
		height = userWindowSize->height;
		fullscreen = userWindowSize->fullscreen;
	}

	virtual bool Init()
	{
		enemy_block_1 = createSprite(".\\data\\01-Breakout-Tiles.png");
		player_sprite = createSprite(".\\data\\49-Breakout-Tiles.png");
		ball_sprite = createSprite(".\\data\\62-Breakout-Tiles.png");
		
		getSpriteSize(enemy_block_1, size_for_blocks_w, size_for_blocks_h);
		getSpriteSize(player_sprite, size_player_w, size_player_h);
		getSpriteSize(ball_sprite, size_ball_w, size_ball_h);
		
		getScreenSize(resolution_w, resolution_h);

		resolution_param_w = resolution_w / static_cast<float>(800);
		resolution_param_h = resolution_h / static_cast<float>(600);
		scaled_size_for_blocks_h = size_for_blocks_h / static_cast <float>(4) * resolution_param_h;
		scaled_size_for_blocks_w = size_for_blocks_w / static_cast <float>(4) * resolution_param_w;
		scaled_size_player_h = size_for_blocks_h / static_cast <float>(4) * resolution_param_h;
		scaled_size_player_w = size_for_blocks_w / static_cast <float>(4) * resolution_param_w;
		scaled_size_ball_h = size_ball_h / static_cast <float>(4) * resolution_param_h;
		scaled_size_ball_w = size_ball_w / static_cast <float>(4) * resolution_param_w;


		setSpriteSize(enemy_block_1, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
		setSpriteSize(player_sprite, scaled_size_player_w, scaled_size_player_h);
		setSpriteSize(ball_sprite, scaled_size_ball_w, scaled_size_ball_h);

		//state_of_game.y_player = resolution_w / 2 - (size_player_w / 2);
		state_of_game.y_player = resolution_h - 50;
		state_of_game.ball_player.speed = 1;
		cout << "Current resolution is: " << resolution_h << "x" << resolution_w << endl;
		cout << "Resolution scale is: " << resolution_param_h << "x" << resolution_param_w << endl;
		cout << "Ticks from library initialization" << getTickCount() << endl;
		cout << "Scaled the size blocks to " << scaled_size_for_blocks_h << " and " << scaled_size_for_blocks_w << endl;
		cout << "Scaled the size mouse to " << scaled_size_player_h << " and " << scaled_size_player_w << endl;
		
		//Manage of the state of the blocks
		for (int x = 0; x <= scaled_size_for_blocks_w * 8; x += scaled_size_for_blocks_w)
		{
			for (int y = 0; y <= scaled_size_for_blocks_h * 5; y += scaled_size_for_blocks_h)
			{
				//drawSprite(enemy_block_1, x , y);
				colored_block temp_struct{};
				temp_struct.block_sprite = createSprite(".\\data\\01-Breakout-Tiles.png");
				temp_struct.x = x;
				temp_struct.y = y;
				getSpriteSize(temp_struct.block_sprite, size_for_blocks_w, size_for_blocks_h);
				setSpriteSize(temp_struct.block_sprite, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
				state_of_game.list_of_blocks.push_back(temp_struct);


			}
		}
		

		return true;
	}

	virtual void Close()
	{
		cout << "Close function called" << endl;
	}

	//Tick each moment - need to re-draw everything per frame
	virtual bool Tick()
	{
		//for_each(state_of_game.list_of_blocks.begin(), state_of_game.list_of_blocks.end(), drawSpriteStruct(state_of_game.list_of_blocks));
		for (auto temp_struct_from_vector : state_of_game.list_of_blocks)
			drawSpriteStruct(temp_struct_from_vector);
		//Drawing player sprite
		manageBallCollision(&state_of_game, &scaled_size_ball_w, &scaled_size_ball_h);
		drawSprite(player_sprite, state_of_game.x_player - (scaled_size_player_w /2), state_of_game.y_player);
		//Drawing ball sprite
		//Problem with ball sprite - speed is connected to ticks == FPS


		drawSprite(ball_sprite, state_of_game.ball_player.x , state_of_game.ball_player.y);

		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)
	{
		//cout << "Mouse moved at x,y : " << x << "," << y << endl;
		cout << "Relative position : " << xrelative << "," << yrelative << endl;
		if(_init_time_mouse == true)
		{
			state_of_game.ball_player.x = x - (scaled_size_ball_w / 2);
			state_of_game.ball_player.y = y - (scaled_size_ball_h / 2);
			state_of_game.ball_player.x_direction = xrelative;
			state_of_game.ball_player.y_direction = -abs(yrelative);

			
			_init_time_mouse = false;
		}
		
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased)
	{
		cout << "Mouse button clicked" << endl;
		if (int(button) == 0 and isReleased == false)
		{
			_init_time_mouse = true;
		}
	}

	virtual void onKeyPressed(FRKey k)
	{	
		cout << "Key pressed: "<< int(k) << endl;
		if (int(k) == 0)
		{
			state_of_game.x_player += 10;
		}
		else if (int(k) == 1)
		{
			state_of_game.x_player -= 10;
		}
		
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

void drawSpriteStruct(colored_block tmp_struct)
{
	drawSprite(tmp_struct.block_sprite, tmp_struct.x, tmp_struct.y);
}


/// Funtion to manage ball collision with screen borders
void manageBallCollision(State* _state_of_game, int* _scaled_size_ball_w, int* _scaled_size_ball_h)
{
	//int temp = *_scaled_size_ball_w;
	if ((_state_of_game->ball_player.x >= resolution_w - *_scaled_size_ball_w) or (_state_of_game->ball_player.x <= 0))
	{
		//cout << "_state_of_game.ball_player.x " << _state_of_game->ball_player.x << (_state_of_game->ball_player.x >= resolution_w - *_scaled_size_ball_w) << endl;
		_state_of_game->ball_player.x_direction *= -1;
	}
	if ((_state_of_game->ball_player.y >= resolution_h - *_scaled_size_ball_h) or (_state_of_game->ball_player.y <= 0))
	{
		_state_of_game->ball_player.y_direction *= -1;
	}
	_state_of_game->ball_player.x += (_state_of_game->ball_player.x_direction * _state_of_game->ball_player.speed);
	_state_of_game->ball_player.y += (_state_of_game->ball_player.y_direction * _state_of_game->ball_player.speed);
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
