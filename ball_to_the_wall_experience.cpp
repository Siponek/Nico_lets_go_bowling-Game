/// <summary>
/// Prototype of game to be run with custom SDL2 library. Code from custom library is hidden / cannot see what SDL2 components are being used.\
/// Bug could possibly be that the spirtes are drown from weird coordinates.
/// </summary>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <vector>
#include <random>

#include "Framework.h"

#define player_collision_multiplier 1.15
#define block_collision_multiplier 0.9
#define ghost_mode_duration 3000
#define ghost_mode_interval 10000


using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::for_each;

void manageCLIwindow();
void millisecondWait(unsigned ms);
template <typename Prob = double> //This should go into .h
bool generateRandomBoolean(const Prob p = 0.5)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(0, 1);
	return distrib(gen) < p;
}


typedef struct Player
{
	Sprite* player_sprite;
	int x_player, x_max_player;
	int y_player, y_max_player;
	int width, height;
	int buffed;
	int left() { return x_player; }
	int right() { return x_player + width; }
	int top() { return y_player; }
	int bottom() { return y_player + height; }
	Player()
	{
		player_sprite = nullptr;
		x_player = 0;
		x_max_player = 0;
		y_player = 0;
		y_max_player = 0;
		width = 0;
		height = 0;
		buffed = 0;
	}
	
} Player ;

typedef struct Ball
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
		ball_sprite = nullptr;
		x = 0;
		y = 0;
		radius = 0;
		speed = 0;
		x_direction = 0;
		y_direction = 0;
		isActive = false;
	};
}Ball;

//? Temporarty struct for managing window of the game
struct WindowSize
{
	mutable int width = 800;
	mutable int height = 600;
	mutable bool fullscreen = false;
};

typedef struct ColoredBlock
{
	Sprite* full_hp_sprite;
	Sprite* special_sprite_1;
	Sprite* special_sprite_2;
	Sprite* hp_1_sprite;
	Sprite* hp_2_sprite;
	Sprite* hp_3_sprite;
	
	int x, y, x_max,y_max, width, height;
	int hit_points;
	int id_block;
	bool is_ghost;
	bool has_buff;
	int left() { return x ; }
	int right() { return x + width; }
	int top() { return y ; }
	int bottom() { return y + height; }
	ColoredBlock()
	{
		full_hp_sprite = nullptr;
		special_sprite_1 = nullptr;
		special_sprite_2 = nullptr;
		hp_1_sprite = nullptr;
		hp_2_sprite = nullptr;
		hp_3_sprite = nullptr;
		x = 0;
		y = 0;
		x_max = 0;
		y_max = 0;
		width = 0;
		height = 0;
		hit_points = 2;
		id_block = 0;
		is_ghost = false;
		has_buff = generateRandomBoolean();
	}
} ColoredBlock;

typedef struct BuffBlock
{
	Sprite* buff_sprite;
	int x, y, x_max, y_max, width, height, id_buff, y_direction, speed;
	bool is_drawn;
	int left() { return x; }
	int right() { return x + width; }
	int top() { return y; }
	int bottom() { return y + height; }
	BuffBlock()
	{
		buff_sprite = nullptr;
		id_buff = 0;
		x = 0;
		y = 0;
		x_max = 0;
		y_max = 0;
		width = 0;
		height = 0;
		y_direction = 1;
		speed = 0;
		is_drawn = false;
	}
}BuffBlock;

int __argc;
char** __argv;
wchar_t** __wargv;
static float resolution_param_h, resolution_param_w;
static int resolution_h, resolution_w;
extern WindowSize* userWindowSize = new WindowSize();
// Function for managing CLI intput for window resolution
class State
{
public:
	Player player;
	Ball ball_object;
	vector<ColoredBlock> list_of_blocks;
	vector<BuffBlock> list_of_buff_blocks;
	Sprite* backgroud_sprite;
	unsigned int game_timer;
	unsigned int ghost_mode_timer;
	bool game_over;
	bool ghost_mode;
	
	State()
	{
		player = Player();
		ball_object = Ball();
		backgroud_sprite = nullptr;
		ball_object.x = 400;
		ball_object.y = 700;
		ball_object.radius = 0;
		ball_object.speed = 1;
		ball_object.x_direction = 0;
		ball_object.y_direction = 0;
		game_timer = 0;
		ghost_mode_timer = 0;
		game_over = false;
		ghost_mode = false;
	}
	
	/// Funtion to take a struct with sprite and draw it. Used for auto "for c: vector" loop 
	void drawSpriteStructBlocks()
	{
		for (auto& temp_struct_var : this->list_of_blocks)
		{
			if (!(temp_struct_var.is_ghost and ghost_mode))
			{
				manageblockCollision(&temp_struct_var);
				if (temp_struct_var.hit_points == 2)
				{
					drawSprite(temp_struct_var.full_hp_sprite, temp_struct_var.x, temp_struct_var.y);
				}
				else
				{
					drawSprite(temp_struct_var.hp_1_sprite, temp_struct_var.x, temp_struct_var.y);
				}
			}
			else
				/// For ghost mode
			{
				drawSprite(temp_struct_var.special_sprite_1, temp_struct_var.x, temp_struct_var.y);
			}
		}
	}
	void drawSpriteStructBuff()
	{
		for (auto& temp_buff : this->list_of_buff_blocks)
		{
			if (temp_buff.is_drawn)
			{
					drawSprite(temp_buff.buff_sprite, temp_buff.x, temp_buff.y);
			}
		}
	}
	void manageblockCollision(ColoredBlock* temp_struct_var)
	{
		/// Ball collision with blocks
		if (!isIntersectingPointer(temp_struct_var, &this->ball_object)) return;
		/// Calculating what is the position of the ball depending on the distance from left-right top-bot
		int overlapLeft{ this->ball_object.right() - temp_struct_var->left() };
		int overlapRight{ temp_struct_var->right() - this->ball_object.left() };
		int overlapTop{ this->ball_object.bottom() - temp_struct_var->top() };
		int overlapBottom{ temp_struct_var->bottom() - this->ball_object.top() };
		bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
		bool ballFromTop(abs(overlapTop) < abs(overlapBottom));
		int minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
		int minOverlapY{ ballFromTop ? overlapTop : overlapBottom };
		///	slowing down on hit
		if (abs(minOverlapX) < abs(minOverlapY))
			this->ball_object.x_direction = ballFromLeft ? -this->ball_object.speed : this->ball_object.speed;
		else
			this->ball_object.y_direction = ballFromTop ? -this->ball_object.speed : this->ball_object.speed;
		temp_struct_var->hit_points -= 1;
		double tmp = (sqrt(pow(this->ball_object.x_direction, 2) + pow(this->ball_object.y_direction, 2)) * block_collision_multiplier);
		this->ball_object.speed = int(sqrt(pow(this->ball_object.x_direction, 2) + pow(this->ball_object.y_direction, 2)) * block_collision_multiplier);
		//cout << "HIT!" << temp_struct_var->hit_points << "HP left!" << endl;
		//cout << "That is float tmp speed " << tmp << endl;
	}
	void manageBallSpeed()
	{
		/// Ball vector of velocity manegment
		this->ball_object.x += (this->ball_object.x_direction * this->ball_object.speed);
		this->ball_object.y += (this->ball_object.y_direction * this->ball_object.speed);
	}
	void manageBuffSpeed()
	{
		for (auto& temp_buff : this->list_of_buff_blocks)
		{
			if (temp_buff.is_drawn)
			{
				temp_buff.y += temp_buff.speed;
			}
		}
	}
	
	/// Funtion to manage ball collision with screen borders //TODO To many things in one function
	void manageBallCollision(int* _scaled_size_ball_w, int* _scaled_size_ball_h, int* resolution_width, int* resolution_height)
	{
		/// Ball collision with screen borders
		if ((this->ball_object.x >= *resolution_width - *_scaled_size_ball_w) or (this->ball_object.x <= 0))
		{
			//cout << "this.ball_object.x " << this->ball_object.x << (this->ball_object.x >= resolution_w - *_scaled_size_ball_w) << endl;
			this->ball_object.x_direction *= -1;
		}
		if ((this->ball_object.y <= 0) or (this->ball_object.y >= *resolution_height - *_scaled_size_ball_h))
		{
			this->ball_object.y_direction *= -1;
		}
		if (this->ball_object.y >= *resolution_height - *_scaled_size_ball_h)
		{
					//GAME OVER
			cout << "/////__GAME OVER__/////" << endl;
			game_over = true;
		}
		/// Ball colision with player
		if (this->ball_object.x >= this->player.x_player and this->ball_object.x <= this->player.x_max_player)
		{
			if (this->ball_object.y >= this->player.y_player and this->ball_object.y <= this->player.y_max_player)
			{
				this->ball_object.y_direction *= -1;
				this->ball_object.speed = int(sqrt(pow(this->ball_object.x_direction, 2) + pow(this->ball_object.y_direction, 2)) * player_collision_multiplier);
				cout << "This is the current speed " << this->ball_object.speed << endl;
			}
		}
	}
	/// Player collision with BuffBlock
	void manageBuffCollision()
	{
		for (auto& temp_buff : this->list_of_buff_blocks)
		{
			if (isIntersectingNormal(temp_buff, this->player))
			{
				temp_buff.is_drawn = false;
			}
		}
	}
	void removeBlock(int amount_of_hp)
	{
		_spawnBuffs(amount_of_hp);
		/// In case of adding new level then this is for the cleanup
		for (auto& block : this->list_of_blocks)
		{
			if (block.hit_points <= amount_of_hp)
			{
				destroySprite(block.full_hp_sprite);
				destroySprite(block.hp_1_sprite);
				if (block.is_ghost)
				{
					destroySprite(block.special_sprite_1);
				}
			}
		}
		this->list_of_blocks.erase(remove_if(this->list_of_blocks.begin(), this->list_of_blocks.end(),
			[&](const ColoredBlock& blocks)
			{
				return blocks.hit_points <= amount_of_hp;
			}),
			end(this->list_of_blocks));
		//Spawn buff
	}
	
	void removeBuffBlocks(int edge_of_the_screen)
	{
		for (auto& temp_buff : this->list_of_buff_blocks)
		{
			if (temp_buff.y >= edge_of_the_screen)
			{
				destroySprite(temp_buff.buff_sprite);
			}
		}
		this->list_of_buff_blocks.erase(remove_if(this->list_of_buff_blocks.begin(), this->list_of_buff_blocks.end(),
			[&](const BuffBlock& temp_buff)
			{
				return temp_buff.y >= edge_of_the_screen;
			}),
			end(this->list_of_buff_blocks));
	}
	
	void victory()
	{
		if (!(this->list_of_blocks.size() <= 0))
			return;
		cout << "/////__VICTORY__/////" << endl;
		cout << "Reseting stage in..." << endl << endl;
		for (int i=3; i!=0; i--)
		{
			cout <<i <<"..." << endl;
			millisecondWait(1000);
		}
		game_over = true;
	}
	

/// <summary>
/// PRIVATE SECTION
/// </summary>
	private:
	void _spawnBuffs(int amount_of_hp)
	{
		for (auto& temp_struct_var : this->list_of_blocks)
		{
			if (temp_struct_var.hit_points <= amount_of_hp and temp_struct_var.has_buff)
			{
				for (auto& temp_buff : this->list_of_buff_blocks)
				{
					if (temp_buff.id_buff == temp_struct_var.id_block)
					{
						cout << "ID: "<< temp_buff.id_buff<< "Block ID: " << temp_struct_var.id_block << " Buffs are drawn!" << endl;
						
						temp_buff.is_drawn = true;
						temp_buff.speed = 1;
					}
				}
			}
		}
	};
	
	template <typename  T1, typename  T2>
	bool isIntersectingNormal(T1 objA, T2 objB) noexcept
	{
		return objA.right() >= objB.left() && objA.left() <= objB.right() &&
			objA.bottom() >= objB.top() && objA.top() <= objB.bottom();
	};
	template <typename  T1, typename  T2>
	bool isIntersectingPointer(T1* objA, T2* objB) noexcept
	{
		return objA->right() >= objB->left() && objA->left() <= objB->right() &&
			objA->bottom() >= objB->top() && objA->top() <= objB->bottom();
	};
	
};

/// Funtion to manage ball collision with screen borders
class MyFramework : public Framework
{
public:
	State state_of_game;
	int size_for_blocks_h = 0, size_for_blocks_w = 0;
	int size_for_buffs_h = 0, size_for_buffs_w = 0;
	int scaled_size_for_blocks_h = 0, scaled_size_for_blocks_w = 0;
	int scaled_size_for_buffs_h = 0, scaled_size_for_buffs_w = 0;
	int size_player_h = 0, size_player_w = 0;
	int scaled_size_player_h = 0, scaled_size_player_w = 0;
	int size_ball_h = 0, size_ball_w = 0;
	int scaled_size_ball_h = 0, scaled_size_ball_w = 0;
	int current_id_index = 0;
	bool _init_arena = false;
	bool _init_time_mouse = false;
	bool _init_time_key_left = false;
	bool _init_time_key_right = false;
	const bool draw_blocks_buffs = true;
	
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
		this->state_of_game.player.player_sprite = createSprite(".\\data\\49-Breakout-Tiles.png");
		this->state_of_game.ball_object.ball_sprite = createSprite(".\\data\\62-Breakout-Tiles.png");
		this->state_of_game.backgroud_sprite = createSprite(".\\data\\02-BACKGROUND.jpg");
		getSpriteSize(this->state_of_game.player.player_sprite, size_player_w, size_player_h);
		getSpriteSize(this->state_of_game.ball_object.ball_sprite, size_ball_w, size_ball_h);
		getScreenSize(resolution_w, resolution_h);
		//This doesn't have to be recalculated each time - const?
		resolution_param_w = resolution_w / static_cast<float>(800);
		resolution_param_h = resolution_h / static_cast<float>(600);
		setSpriteSize(this->state_of_game.backgroud_sprite, resolution_w, resolution_h);
		current_id_index = 0;
		/// Filling vector with blocks
		for (int x = 0; x <= 7; x ++)
		{
			for (int y = 0; y <= 5; y ++)
			{
				/// Ghost block
				if ( x == y or x - 2 == y or x - 4 == y or x - 6 == y or y-2 == x or y-4 == x)
				{
					ColoredBlock temp_struct{};
					if (temp_struct.has_buff and draw_blocks_buffs)
					{
						/// BLOCK PART
						temp_struct.full_hp_sprite = createSprite(".\\data\\15-Breakout-Tiles.png");
						temp_struct.hp_1_sprite = createSprite(".\\data\\06-Breakout-Tiles.png");
						temp_struct.special_sprite_1 = createSprite(".\\data\\11-Breakout-Tiles.png");
						getSpriteSize(temp_struct.full_hp_sprite, size_for_blocks_w, size_for_blocks_h);
						scaled_size_for_blocks_h = size_for_blocks_h / static_cast <float>(4) * resolution_param_h;
						scaled_size_for_blocks_w = size_for_blocks_w / static_cast <float>(4) * resolution_param_w;
						setSpriteSize(temp_struct.full_hp_sprite, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
						setSpriteSize(temp_struct.special_sprite_1, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
						setSpriteSize(temp_struct.hp_1_sprite, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
						temp_struct.x = x * scaled_size_for_blocks_w;
						temp_struct.y = y * scaled_size_for_blocks_h;
						temp_struct.width = x + scaled_size_for_blocks_w;
						temp_struct.height = y + scaled_size_for_blocks_h;
						temp_struct.x_max = temp_struct.x + temp_struct.width;
						temp_struct.y_max = temp_struct.y + temp_struct.height;
						temp_struct.hit_points = 2;
						temp_struct.is_ghost = true;
						temp_struct.id_block = current_id_index;
						/// BUFF PART
						BuffBlock temp_buff{};
						temp_buff.buff_sprite = createSprite(".\\data\\59-Breakout-Tiles.png");
						getSpriteSize(temp_buff.buff_sprite, size_for_buffs_w, size_for_buffs_h);
						scaled_size_for_buffs_w = size_for_buffs_w / static_cast <float>(4) * resolution_param_w;
						scaled_size_for_buffs_h = size_for_buffs_h / static_cast <float>(4) * resolution_param_h;
						setSpriteSize(temp_buff.buff_sprite, scaled_size_for_buffs_w, scaled_size_for_buffs_h);
						temp_buff.x = x * scaled_size_for_blocks_w + (scaled_size_for_blocks_w / 4 );
						temp_buff.y = y * scaled_size_for_blocks_h + (scaled_size_for_blocks_h / 4 );
						temp_buff.width = x + scaled_size_for_buffs_w;
						temp_buff.height = y + scaled_size_for_buffs_h;
						temp_buff.x_max = temp_buff.x + temp_buff.width;
						temp_buff.y_max = temp_buff.y + temp_buff.height;
						temp_buff.id_buff = temp_struct.id_block;
						
						this->state_of_game.list_of_buff_blocks.push_back(temp_buff);
						this->state_of_game.list_of_blocks.push_back(temp_struct);
						current_id_index += 2;					}
					else
					{
					temp_struct.full_hp_sprite = createSprite(".\\data\\05-Breakout-Tiles.png");
					temp_struct.hp_1_sprite = createSprite(".\\data\\06-Breakout-Tiles.png");
					temp_struct.special_sprite_1 = createSprite(".\\data\\11-Breakout-Tiles.png");
					getSpriteSize(temp_struct.full_hp_sprite, size_for_blocks_w, size_for_blocks_h);
					scaled_size_for_blocks_h = size_for_blocks_h / static_cast <float>(4) * resolution_param_h;
					scaled_size_for_blocks_w = size_for_blocks_w / static_cast <float>(4) * resolution_param_w;
					setSpriteSize(temp_struct.full_hp_sprite, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
					setSpriteSize(temp_struct.special_sprite_1, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
					setSpriteSize(temp_struct.hp_1_sprite, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
					temp_struct.x = x * scaled_size_for_blocks_w;
					temp_struct.y = y * scaled_size_for_blocks_h;
					temp_struct.width = x + scaled_size_for_blocks_w;
					temp_struct.height = y + scaled_size_for_blocks_h;
					temp_struct.x_max = temp_struct.x + temp_struct.width;
					temp_struct.y_max = temp_struct.y + temp_struct.height;
					temp_struct.hit_points = 2;
					temp_struct.is_ghost = true;
					this->state_of_game.list_of_blocks.push_back(temp_struct); 
					}
				}
				/// Normal block
				else
				{
					/// BLOCK PART
					ColoredBlock temp_struct{};
					if (temp_struct.has_buff and draw_blocks_buffs)
					{
						temp_struct.full_hp_sprite = createSprite(".\\data\\15-Breakout-Tiles.png");
					}
					else
					{
						temp_struct.full_hp_sprite = createSprite(".\\data\\01-Breakout-Tiles.png");
					}
					temp_struct.hp_1_sprite = createSprite(".\\data\\02-Breakout-Tiles.png");
					getSpriteSize(temp_struct.full_hp_sprite, size_for_blocks_w, size_for_blocks_h);
					scaled_size_for_blocks_h = size_for_blocks_h / static_cast <float>(4) * resolution_param_h;
					scaled_size_for_blocks_w = size_for_blocks_w / static_cast <float>(4) * resolution_param_w;
					setSpriteSize(temp_struct.full_hp_sprite, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
					setSpriteSize(temp_struct.hp_1_sprite, scaled_size_for_blocks_w, scaled_size_for_blocks_h);
					temp_struct.x = x * scaled_size_for_blocks_w;
					temp_struct.y = y * scaled_size_for_blocks_h;
					temp_struct.width = x + scaled_size_for_blocks_w;
					temp_struct.height = y + scaled_size_for_blocks_h;
					temp_struct.x_max = temp_struct.x + temp_struct.width;
					temp_struct.y_max = temp_struct.y + temp_struct.height;
					temp_struct.hit_points = 2;
					temp_struct.is_ghost = false;
					temp_struct.id_block = current_id_index;
					/// BUFF PART
					BuffBlock temp_buff{};
					temp_buff.buff_sprite = createSprite(".\\data\\59-Breakout-Tiles.png");
					getSpriteSize(temp_buff.buff_sprite, size_for_buffs_w, size_for_buffs_h);
					scaled_size_for_buffs_w = size_for_buffs_w / static_cast <float>(4) * resolution_param_w;
					scaled_size_for_buffs_h = size_for_buffs_h / static_cast <float>(4) * resolution_param_h;
					setSpriteSize(temp_buff.buff_sprite, scaled_size_for_buffs_w, scaled_size_for_buffs_h);
					temp_buff.x = x * scaled_size_for_blocks_w + (scaled_size_for_blocks_w / 4);
					temp_buff.y = y * scaled_size_for_blocks_h + (scaled_size_for_blocks_h / 4);
					temp_buff.width = x + scaled_size_for_buffs_w;
					temp_buff.height = y + scaled_size_for_buffs_h;
					temp_buff.x_max = temp_buff.x + temp_buff.width;
					temp_buff.y_max = temp_buff.y + temp_buff.height;
					temp_buff.id_buff = temp_struct.id_block;

					this->state_of_game.list_of_buff_blocks.push_back(temp_buff);
					this->state_of_game.list_of_blocks.push_back(temp_struct);
				}
				current_id_index += 2;
			}
		}
		/// <summary>
		/// Ball
		/// </summary>
		scaled_size_player_h = size_for_blocks_h / static_cast <float>(4) * resolution_param_h;
		scaled_size_player_w = size_for_blocks_w / static_cast <float>(4) * resolution_param_w;
		scaled_size_ball_h = size_ball_h / static_cast <float>(4) * resolution_param_h;
		scaled_size_ball_w = size_ball_w / static_cast <float>(4) * resolution_param_w;
		//Does not have to be round, but w/e
		this->state_of_game.ball_object.radius = scaled_size_ball_w / 2;
		this->state_of_game.ball_object.x = resolution_w / 2 - (size_player_w / 4) ;
		this->state_of_game.ball_object.y = resolution_h - scaled_size_player_h - 20;
		this->state_of_game.ball_object.x_direction = 0;
		this->state_of_game.ball_object.y_direction = 0;
		setSpriteSize(this->state_of_game.ball_object.ball_sprite, scaled_size_ball_w, scaled_size_ball_h);

		/// <summary>
		/// Player
		/// </summary>
		this->state_of_game.player.x_player = resolution_w / 2 - (size_player_w / 4);
		this->state_of_game.player.y_player = resolution_h - scaled_size_player_h;
		this->state_of_game.player.x_max_player = this->state_of_game.player.x_player + scaled_size_player_w;
		this->state_of_game.player.y_max_player = this->state_of_game.player.y_player + scaled_size_player_h;
		setSpriteSize(this->state_of_game.player.player_sprite, scaled_size_player_w, scaled_size_player_h);
		cout << "Current resolution is: " << resolution_h << "x" << resolution_w << endl;
		cout << "Resolution scale is: " << resolution_param_h << "x" << resolution_param_w << endl;
		cout << "Ticks from library initialization" << getTickCount() << endl;
		this->state_of_game.game_over = false;
		return true;
	}

	virtual void Close()
	{
		cout << "Close function called, bye bye" << endl;
	}

	//Tick each moment - need to re-draw everything per frame
	virtual bool Tick()
	{
		///?	Problem with ball sprite - speed is connected to ticks == FPS
		///? Also draw sprite takes int instead of float so the speed cannot be '15%' or whatever since you cannot draw 0.15 pixels and speed 100 is impossible to follow.
		///? only solution would be to assaign speed to ticks and then draw sprite based on that, draw on irregular intervals.
		drawSprite(this->state_of_game.backgroud_sprite, 0, 0);
		drawSprite(this->state_of_game.ball_object.ball_sprite, this->state_of_game.ball_object.x , this->state_of_game.ball_object.y);
		drawSprite(this->state_of_game.player.player_sprite, this->state_of_game.player.x_player, this->state_of_game.player.y_player);
		this->state_of_game.drawSpriteStructBlocks();
		this->state_of_game.drawSpriteStructBuff();
		this->state_of_game.manageBallCollision(&scaled_size_ball_w, &scaled_size_ball_h, &resolution_w, &resolution_h);
		this->state_of_game.manageBallSpeed();
		this->state_of_game.manageBuffSpeed();
		///Manage keys pressed-released
		if (_init_time_key_left)
		{
			this->state_of_game.player.x_player -= 1;
			this->state_of_game.player.x_max_player -= 1;
		}
		if (_init_time_key_right)
		{
			this->state_of_game.player.x_player += 1;
			this->state_of_game.player.x_max_player += 1;
		}
		/// Cleanup of "dead" blocks and spawn buffs in their place
		this->state_of_game.removeBlock(0);
		this->state_of_game.removeBuffBlocks(resolution_w);
		this->state_of_game.victory();
		if (this->state_of_game.game_over)
		{
			/// Memory CLEANUP
			/// Potential memory leak depending if destroySrpite() is needed to free the memory.
			cout << "RESETING" << endl;
			for (auto& block : this->state_of_game.list_of_blocks)
			{
				destroySprite(block.full_hp_sprite);
				destroySprite(block.hp_1_sprite);
				if (block.is_ghost)
				{
					destroySprite(block.special_sprite_1);
				}
			}
			for (auto& buff : this->state_of_game.list_of_buff_blocks)
			{
				destroySprite(buff.buff_sprite);
			}
			destroySprite(this->state_of_game.ball_object.ball_sprite);
			destroySprite(this->state_of_game.player.player_sprite);
			destroySprite(this->state_of_game.backgroud_sprite);
			this->state_of_game.list_of_blocks.clear();
			this->state_of_game.list_of_buff_blocks.clear();
			//this->this->state_of_game.list_of_blocks.clear();
			//this->this->state_of_game.list_of_buff_blocks.clear();
				
			this->Init();
		}
		if (this->state_of_game.game_timer >= ghost_mode_interval)
		{
			this->state_of_game.ghost_mode = true;
			this->state_of_game.game_timer = 0;
			cout << "Ghost mode activated" << endl;
		}
		if (this->state_of_game.ghost_mode)
		{
			this->state_of_game.ghost_mode_timer++;
			if (this->state_of_game.ghost_mode_timer >= ghost_mode_duration)
			{
				this->state_of_game.ghost_mode = false;
				this->state_of_game.ghost_mode_timer = 0;
				cout << "Ghost mode deactivated" << endl;
			}
		}
		this->state_of_game.game_timer++;
		
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative)
	{
		//Blokcing the user input either to XY 1,-1 or -1,-1 since you cannot control the ball movement
		if(_init_time_mouse == true)
		{
			this->state_of_game.ball_object.x = x - (scaled_size_ball_w / 2);
			this->state_of_game.ball_object.y = y - (scaled_size_ball_h / 2);
			this->state_of_game.ball_object.x_direction = xrelative;
			//Y direction is inverted in window
			this->state_of_game.ball_object.y_direction = -abs(yrelative);
			//checks for the first time mouse is moved to give smallest speed
			if (this->state_of_game.ball_object.x_direction == 0)
				this->state_of_game.ball_object.x_direction = 1;
			if (this->state_of_game.ball_object.y_direction >= 0)
				this->state_of_game.ball_object.y_direction = -1;
			this->state_of_game.ball_object.speed = int(sqrt(pow(this->state_of_game.ball_object.x_direction, 2) + pow(this->state_of_game.ball_object.y_direction, 2)));
		//cout << "Launch speed is: " << state_of_game.ball_object.speed<< endl;
		//cout << "Direction XY : " << state_of_game.ball_object.x_direction << "," << state_of_game.ball_object.y_direction << endl;
		_init_time_mouse = false;
		}
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased)
	{
		//cout << "Mouse button clicked" << endl;
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
		//cout << "GetTitle function called" << endl;
		return "Arcanoid fucking sucks";
	}
};
//TODO implement waiting
 void millisecondWait(unsigned ms)
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
	cout << "MAIN: Starting game..." << endl;
	manageCLIwindow();
	return run(new MyFramework);
}
