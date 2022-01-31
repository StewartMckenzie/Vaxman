#include <array>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <chrono>
#include "Headers/Global.hpp"
#include "Headers/Pacman.hpp"
#include "Headers/Ghost.hpp"
#include "Headers/GhostManager.hpp"



GhostManager::GhostManager() :
	current_wave(0),
	wave_timer(LONG_SCATTER_DURATION),
	GhostList({ Ghost(GhostType::red), Ghost(GhostType::pink), Ghost(GhostType::blue), Ghost(GhostType::orange) }),
	leadGhost(&GhostList[0]),
	home(Position(160, 144)),
	homeExit(Position(160, 112)),
	GhostCount(GhostList.size())
{

}

void GhostManager::multiplyGhosts()
{
	std::cout <<"multiply!"<<std::endl;
	bool leadingGhost = false;
	int currentSize = GhostList.size();

	for (int i=0;i< currentSize;i++ )
	{

		if (!leadingGhost && GhostList[i].type ==0) {
			leadGhost = &GhostList[i];
	
			leadingGhost = true;
		}
		if (GhostList[i].alive && GhostList[i].frightened_mode<1) {
			GhostList.push_back(Ghost(GhostList[i].type));
			GhostList.back().set_position(GhostList[i].get_position().x, GhostList[i].get_position().y);
			GhostList.back().reset(home, homeExit);
			std::cout << i<<std::endl;
		}

	
		
	
	}
	spawnTimer.reset();
}

void GhostManager::draw(bool i_flash, sf::RenderWindow& i_window)
{
	if (multiply) {

	
		multiplyGhosts();
		multiply = false;
}
	for (Ghost& ghost : GhostList)
	{
		ghost.draw(i_flash, i_window);
	}
}

void GhostManager::reset(unsigned char i_level, const std::vector<Position>& i_ghost_positions)
{
	current_wave = 0;

	//This is how we're increasing the difficulty.
	wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));

	for (unsigned char a = 0; a < 4; a++)
	{


		GhostList[a].set_position(i_ghost_positions[a].x, i_ghost_positions[a].y);
	}

	for (Ghost& ghost : GhostList)
	{
		//We use the blue ghost to get the location of the house and the red ghost to get the location of the exit.

		ghost.reset(GhostList[2].get_position(), GhostList[0].get_position());
	}

}

void GhostManager::update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, Pacman& i_pacman)
{
	if(GhostList.size() >= MAX_GHOSTS) { i_pacman.set_dead(1); }
	if (0 == i_pacman.get_energizer_timer()) //We won't update the wave timer when Pacman is energized.
	{
		if (0 == wave_timer)
		{
			if (7 > current_wave)
			{
				current_wave++;

				for (Ghost& ghost : GhostList)
				{
					ghost.switch_mode();
				
				}
			}

			//I took the rules from the website.
			if (1 == current_wave % 2)
			{
				wave_timer = CHASE_DURATION;
			}
			else if (2 == current_wave)
			{
				wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));
			}
			else
			{
				wave_timer = static_cast<unsigned short>(SHORT_SCATTER_DURATION / pow(2, i_level));
			}
		}
		else
		{
			wave_timer--;
		}
	}


	for (Ghost& ghost : GhostList)
	{
	
		ghost.update(i_level, i_map, *leadGhost, i_pacman);
	}
}