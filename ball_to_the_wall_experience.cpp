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
//using namespace std::filesystem;
struct Player
{
	int x_player, x_max_player;
	int y_player, y_max_player;
};

struct Ball
{
	Sprite* ball_sprite;
	int x = 1;
	int y = 1;
	int radius = 0;
	int speed = 0;
	int x_direction = 0, y_direction = 0;
	bool isActive = false;
	
	int left() { return x ; }
	int right() { return x + radius * 2; }
	int top() { return y ; }
	int bottom() { return y + radius * 2; }
	Ball()
	{
		ball_sprite = createSprite(".\\data\\62-Breakout-Tiles.png");
		x = 0;
		y = 0;
		radius = 0;
		speed = 0;
		x_direction = 0;
		y_direction = 0;
		isActive = false;
	};
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
	int x, y, x_max,y_max, width, height;
	int hit_points;
	bool is_alive;
	int left() { return x ; }
	int right() { return x + width; }
	int top() { return y ; }
	int bottom() { return y + height; }

};

class State
{
public:
	int x_player, x_max_player;
	int y_player, y_max_player;
	Ball ball_object;
	vector<colored_block> list_of_blocks;
	State()
	{
		x_player = 0;
		y_player = 0;
		x_max_player = 0;
		y_max_player = 0;
		ball_object.x = 400;
		ball_object.y = 700;
		ball_object.radius = 0;
		ball_object.speed = 1;
		ball_object.x_direction = 0;
		ball_object.y_direction = 0;
	}
	
};

int __argc;
char** __argv;
wchar_t** __wargv;
static float resolution_param_h, resolution_param_w;
static int resolution_h, resolution_w;
extern WindowSize* userWindowSize = new WindowSize();

int vector_size = 0;

//void millisecondWait(unsigned ms);
// Function for managing CLI intput for window resolution
void manageCLIwindow();
void drawSpriteStruct(colored_block tmp_struct);
/// Funtion to manage ball collision with screen borders
void manageBallCollision(State *_state_of_game, int *_scaled_size_ball_w, int *_scaled_size_ball_h);
void manageblockCollision(State* _state_of_game, colored_block* temp_struct_var);
void removeBlock(std::vector<colored_block>& blocks, int amount_of_hp);
void manageBallSpeed(State* _state_of_game);

//void manageBallCollisionWithBlocks(State* _state_of_game, int* _scaled_size_ball_w, int* _scaled_size_ball_h);


class MyFramework : public Framework
{
public:
	FRKey key_api;
	FRMouseButton mouse_api;
	State state_of_game;
	Sprite* player_sprite;
	Sprite* ball_sprite;
	Sprite* backgroud_sprite;
	bool _init_arena = false;
	int size_for_blocks_h, size_for_blocks_w;
	int scaled_size_for_blocks_h, scaled_size_for_blocks_w;
	int size_player_h, size_player_w;
	int scaled_size_player_h, scaled_size_player_w;
	int size_ball_h, size_ball_w;
	int scaled_size_ball_h, scaled_size_ball_w;
	bool _init_time_mouse = false;
	bool _init_time_key_left = false;
	bool _init_time_key_right = false;
	
	/// <summary>
	/// Nothing can be called here. All initialization should be done in Init() method.
	/// </summary>
	/// <returns> nothing </returns>
	virtual void PreInit(int &width, int &height, bool &fullscreen)
	{
		cout << "Pre-init called" << endl;
		width = userWindowSize->width;
		height = userWindowSize->height;
		fullscreen = userWindowSize->fullscreen;
		///By default I recognise the resolution as 800x600, so I scale the sprites form that value
	}

	virtual bool Init()
	{
		cout << "Init called" << endl;
		player_sprite = createSprite(".\\data\\49-Breakout-Tiles.png");
		ball_sprite = createSprite(".\\data\\62-Breakout-Tiles.png");
		backgroud_sprite = createSprite(".\\data\\02-BACKGROUND.jpg");
		getSpriteSize(player_sprite, size_player_w, size_player_h);
		getSpriteSize(ball_sprite, size_ball_w, size_ball_h);
		getScreenSize(resolution_w, resolution_h);

		//This doesn't have to be recalculated each time - const?
		resolution_param_w = resolution_w / static_cast<float>(800);
		resolution_param_h = resolution_h / static_cast<float>(600);

		setSpriteSize(backgroud_sprite, resolution_w, resolution_h);
		
		/// <summary>
		/// Blocks
		/// </summary>
		/// <returns></returns>
		for (int x = 0; x <= 7; x ++)
		//for (int x = 0; x <= scaled_size_for_blocks_w * 8; x += scaled_size_for_blocks_w)
		{
			for (int y = 0; y <= 5; y ++)
			{
				//drawSprite(enemy_block_1, x , y);
				colored_block temp_struct{};
				temp_struct.block_sprite = createSprite(".\\data\\01-Breakout-Tiles.png");
				getSpriteSize(temp_struct.block_sprite, size_for_blocks_w, size_for_blocks_h);
				/// <summary>
				/// This does not need to be recomputed every loop.
				/// </summary>
				scaled_size_for_blocks_h = size_for_blocks_h / static_cast <float>(4) * resolution_param_h;
				scaled_size_for_blocks_w = size_for_blocks_w / static_cast <float>(4) * resolution_param_w;
				setSpriteSize(temp_struct.block_sprite, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
				temp_struct.x = x * scaled_size_for_blocks_w;
				temp_struct.y = y * scaled_size_for_blocks_h;
				temp_struct.width = x + scaled_size_for_blocks_w;
				temp_struct.height = y + scaled_size_for_blocks_h;
				temp_struct.x_max = temp_struct.x + temp_struct.width;
				temp_struct.y_max = temp_struct.y + temp_struct.height;
				temp_struct.hit_points = 1;
				state_of_game.list_of_blocks.push_back(temp_struct);
			}
		}
		vector_size = state_of_game.list_of_blocks.size();
		/// <summary>
		/// Ball
		/// </summary>
		scaled_size_player_h = size_for_blocks_h / static_cast <float>(4) * resolution_param_h;
		scaled_size_player_w = size_for_blocks_w / static_cast <float>(4) * resolution_param_w;
		scaled_size_ball_h = size_ball_h / static_cast <float>(4) * resolution_param_h;
		scaled_size_ball_w = size_ball_w / static_cast <float>(4) * resolution_param_w;
		//Does not have to be round but w/e
		state_of_game.ball_object.radius = scaled_size_ball_w / 2;
		setSpriteSize(ball_sprite, scaled_size_ball_w, scaled_size_ball_h);

		/// <summary>
		/// Player
		/// </summary>
		state_of_game.x_player = resolution_w / 2 - (size_player_w / 4);
		state_of_game.y_player = resolution_h - 50;
		state_of_game.x_max_player = state_of_game.x_player + scaled_size_player_w;
		state_of_game.y_max_player = state_of_game.y_player + scaled_size_player_h;
		state_of_game.ball_object.speed = 1;
		setSpriteSize(player_sprite, scaled_size_player_w, scaled_size_player_h);
		
		cout << "Current resolution is: " << resolution_h << "x" << resolution_w << endl;
		cout << "Resolution scale is: " << resolution_param_h << "x" << resolution_param_w << endl;
		cout << "Ticks from library initialization" << getTickCount() << endl;
		//cout << "Scaled the size blocks to " << scaled_size_for_blocks_h << " and " << scaled_size_for_blocks_w << endl;
		//cout << "Scaled the size mouse to " << scaled_size_player_h << " and " << scaled_size_player_w << endl;
		
		//Manage of the state of the blocks
		

		return true;
	}

	virtual void Close()
	{
		cout << "Close function called, bye bye" << endl;
	}

	//Tick each moment - need to re-draw everything per frame
	virtual bool Tick()
	{
		drawSprite(backgroud_sprite, 0, 0);
		for (auto& temp_struct_var : state_of_game.list_of_blocks)
		{
			drawSpriteStruct(temp_struct_var);
			manageblockCollision(&state_of_game, &temp_struct_var);
		}
		
		manageBallCollision(&state_of_game, &scaled_size_ball_w, &scaled_size_ball_h);
		//Problem with ball sprite - speed is connected to ticks == FPS
		manageBallSpeed(&state_of_game);
		drawSprite(ball_sprite, state_of_game.ball_object.x , state_of_game.ball_object.y);

		///Manage keys pressed-released
		drawSprite(player_sprite, state_of_game.x_player, state_of_game.y_player);
		
		if (_init_time_key_left)
		{
			state_of_game.x_player -= 1;
			state_of_game.x_max_player -= 1;
		}
		if (_init_time_key_right)
		{
			state_of_game.x_player += 1;
			state_of_game.x_max_player += 1;
		}
		removeBlock(state_of_game.list_of_blocks, 0);

		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)
	{
		//Blokcing the user input either to XY 1,-1 or -1,-1 since you cannot control the ball movement
		if(_init_time_mouse == true)
		{
			state_of_game.ball_object.x = x - (scaled_size_ball_w / 2);
			state_of_game.ball_object.y = y - (scaled_size_ball_h / 2);
			state_of_game.ball_object.x_direction = xrelative;
			//Y direction is inverted in window
			state_of_game.ball_object.y_direction = -abs(yrelative);
			//checks for the first time mouse is moved to give smallest speed
			if (state_of_game.ball_object.x_direction == 0)
				state_of_game.ball_object.x_direction = 1;
			//else if (state_of_game.ball_object.x_direction < -1)
			//	state_of_game.ball_object.x_direction = -1;
			//This doesn't need to be checked on positive, because of {-abs()}
			if (state_of_game.ball_object.y_direction >= 0)
				state_of_game.ball_object.y_direction = -1;
			
		cout << "Direction relative XY : " << xrelative << "," << yrelative << endl;
		cout << "Direction XY : " << state_of_game.ball_object.x_direction << "," << state_of_game.ball_object.y_direction << endl;
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
			_init_time_key_right = true;
		}
		else if (int(k) == 1)
		{
			_init_time_key_left = true;
		}
	}

	virtual void onKeyReleased(FRKey k)
	{
		cout << "Key released: " << int(k) << endl;
		//if (int(k) == 0)
		//{
		//	{
		//		state_of_game.x_player += 10;
		//	}
		//}
		//else if (int(k) == 1)
		//{
		//	{
		//		state_of_game.x_player -= 10;
		//	}
		//}
		
		if (int(k) == 0)
		{
			_init_time_key_right = false;
		}
		else if (int(k) == 1)
		{
			_init_time_key_left = false;
		}
	}

	virtual const char *GetTitle() override
	{
		cout << "GetTitle function called" << endl;
		return "Arcanoid fucking sucks";
	}
};
//TODO implement waiting
 //void millisecondWait(unsigned ms)
 //{
 //	std::chrono::milliseconds dura(ms);
 //	std::this_thread::sleep_for(dura);
 //}

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

/// <summary>
/// Funtion to take a struct with sprite and draw it. Used for auto "for c: vector" loop 
/// </summary>
/// <param name="_state_of_game"></param>
/// <param name="_scaled_size_ball_w"></param>
/// <param name="_scaled_size_ball_h"></param>
void drawSpriteStruct(colored_block tmp_struct)
{
	drawSprite(tmp_struct.block_sprite, tmp_struct.x, tmp_struct.y);
}

void manageBallSpeed(State* _state_of_game)
{
	/// Ball vector of velocity manegment
	_state_of_game->ball_object.x += (_state_of_game->ball_object.x_direction * _state_of_game->ball_object.speed);
	_state_of_game->ball_object.y += (_state_of_game->ball_object.y_direction * _state_of_game->ball_object.speed);
}

/// Funtion to manage ball collision with screen borders //TODO To many things in one function
void manageBallCollision(State* _state_of_game, int* _scaled_size_ball_w, int* _scaled_size_ball_h)
{
	/// Ball collision with screen borders
	if ((_state_of_game->ball_object.x >= resolution_w - *_scaled_size_ball_w) or (_state_of_game->ball_object.x <= 0))
	{
		//cout << "_state_of_game.ball_object.x " << _state_of_game->ball_object.x << (_state_of_game->ball_object.x >= resolution_w - *_scaled_size_ball_w) << endl;
		_state_of_game->ball_object.x_direction *= -1;
	}
	if ((_state_of_game->ball_object.y >= resolution_h - *_scaled_size_ball_h) or (_state_of_game->ball_object.y <= 0))
	{
		_state_of_game->ball_object.y_direction *= -1;
	}
	/// Ball colision with player
	if (_state_of_game->ball_object.x >=_state_of_game->x_player and _state_of_game->ball_object.x <= _state_of_game->x_max_player)
	{
		if (_state_of_game->ball_object.y >= _state_of_game->y_player and _state_of_game->ball_object.y <= _state_of_game->y_max_player)
		{
			//GAME OVER
			_state_of_game->ball_object.y_direction *= -1;
		}
	}
}
bool isIntersecting(colored_block* block , Ball ball)
{
	return block->right() >= ball.left() && block->left() <= ball.right() && block->bottom() >= ball.top() && block->top() <= ball.bottom();
}

void manageblockCollision(State* _state_of_game, colored_block* temp_struct_var)
{
	/// Ball collision with blocks
//for (auto temp_struct_var : _state_of_game->list_of_blocks) // bad because this auto is a copy, with auto& it is a reference
		if (!isIntersecting(temp_struct_var, _state_of_game->ball_object)) return;
		/// Calculating what is the position of the ball depending on the distance from left-right top-bot
		int overlapLeft{ _state_of_game->ball_object.right() - temp_struct_var->left()};
		int overlapRight{ temp_struct_var->right() - _state_of_game->ball_object.left() };
		int overlapTop{ _state_of_game->ball_object.bottom() - temp_struct_var->top() };
		int overlapBottom{ temp_struct_var->bottom() - _state_of_game->ball_object.top() };
		bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
		bool ballFromTop(abs(overlapTop) < abs(overlapBottom));
		int minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
		int minOverlapY{ ballFromTop ? overlapTop : overlapBottom };
		if (abs(minOverlapX) < abs(minOverlapY))
			_state_of_game->ball_object.x_direction = ballFromLeft ? -_state_of_game->ball_object.speed : _state_of_game->ball_object.speed;
		else
			_state_of_game->ball_object.y_direction = ballFromTop ? -_state_of_game->ball_object.speed : _state_of_game->ball_object.speed;
		temp_struct_var->hit_points -= 1;
		cout << "HIT!"<< temp_struct_var->hit_points << "HP left!" << endl;
}

void removeBlock(vector<colored_block>& blocks, int amount_of_hp) {

	
	//std::remove_if(blocks.begin(), blocks.end(), [&](colored_block const& blocks) {
	//	return blocks.hit_points <= amount_of_hp;
	//	});

 
	blocks.erase(remove_if(begin(blocks), end(blocks),
		[&](const colored_block& blocks )
		{
			return blocks.hit_points <= amount_of_hp;
		}),
		end(blocks));


}

int main(int argc, char *argv[])
{
	cout << "MAIN: Starting game..." << endl;
	manageCLIwindow();
	return run(new MyFramework);
}
