#include "Field.h"
#include <assert.h>
#include <list>





void Field::Tile::drawTile(Graphics& gfx, const Vei2 ScreenPos)
{
	pos = ScreenPos;
	Color c;
	switch (state)
	{
	case State::Path:
		c = Colors::Blue;
		break;
	case State::Wall:
		c = Colors::White;
		break;
	case State::Start:
		c = Colors::Green;
		break;
	case State::End:
		c = Colors::Red;
		break;
	case State::Normal:
		c = Colors::Gray;
		break;
	}

	for (int j = 1; j < 19; j++)
	{
		for (int i = 1; i < 19; i++)
		{
			gfx.PutPixel(pos.x + i, pos.y + j, c);
		}
	}
}

void Field::Tile::setAsStart()
{
	state = State::Start;
}

void Field::Tile::setAsEnd()
{
	state = State::End;
}

bool Field::Tile::checkVisited()
{
	return visited;
}

bool Field::Tile::checkWall()
{
	return state == State::Wall;
}

void Field::Tile::setAsVisited()
{
	visited = true;
}

void Field::Tile::setAsUnVisited()
{
	visited = false;
}

Vei2 Field::Tile::getSrcPos()
{
	return pos;
}

void Field::Tile::setAsWall()
{
	state = State::Wall;
}

void Field::Tile::setAsPath()
{
	state = State::Path;
}

void Field::Tile::setAsEmpty()
{
	state = State::Normal;
}

void Field::drawScene(Graphics& gfx)
{
	
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			field[y * width + x].drawTile(gfx, { xOffset + x * tileSize, yOffset + y * tileSize });
		}
	}
}

Field::Tile& Field::atTile(const Vei2 & gridPos)
{
	return field[gridPos.y * width + gridPos.x];
}

const Field::Tile& Field::atTile(const Vei2 & gridPos) const
{
	return field[gridPos.y * width + gridPos.x];
}

Vei2 Field::ScreenToGrid(const Vei2 & srcPos)
{
	Vei2 offset = { xOffset,yOffset };
	return (srcPos - offset) / tileSize ;
}

void Field::onLeftClick(const Vei2 & scrPos)
{
	Vei2 gridPos = ScreenToGrid(scrPos);
	assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
	Tile& newTile = atTile(gridPos);
	if (startPos.x == -1 && startPos.y == -1)
	{
		newTile.setAsStart();
		startPos = gridPos;
		setStart = false;
		startTile = &newTile;
	}
	else if (endPos.x == -1 && endPos.y == -1)
	{
		newTile.setAsEnd();
		endPos = gridPos;
		setStart = true;
		endTile = &newTile;
	}
	else if (setStart == true)
	{
		newTile.setAsStart();
		startPos = gridPos;
		setStart = false;
		startTile = &newTile;
	}
	else if (setStart == false)
	{
		newTile.setAsEnd();
		endPos = gridPos;
		setStart = true;
		endTile = &newTile;
	}
}

void Field::onRightClick(const Vei2 & scrPos)
{
	Vei2 gridPos = ScreenToGrid(scrPos);
	assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
	Tile& tile = atTile(gridPos);
	if(tile.checkWall() == false)
	{
		tile.setAsWall();
	}
	else
	{
		tile.setAsEmpty();
	}
	
	
}

void Field::resetGrid()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			field[y * width + x].setAsEmpty();
		}
	}
	startPos = { -1,-1 };
	endPos = { -1,-1 };
	setStart = true; 
}

void Field::createConnections()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (y > 0)
			{
				field[y * width + x].vecNeighbours.push_back(&field[(y - 1)*width + (x + 0)]); // Up
			}
			if (y < height-1)
			{
				field[y * width + x].vecNeighbours.push_back(&field[(y + 1)*width + (x + 0)]); // Down
			}
			if (x > 0)
			{
				field[y * width + x].vecNeighbours.push_back(&field[(y + 0)*width + (x - 1)]); // Left
			}
			if (x < width -1)
			{
				field[y * width + x].vecNeighbours.push_back(&field[(y + 0)*width + (x + 1)]); // Right
			}
		}
	}
}

void Field::createPath()
{
	if (endTile != nullptr)
	{
		Tile *t = endTile;
		t->setAsEnd();
		t = t->parent;
		if (t != nullptr)
		{
			while (t->parent != nullptr)
			{
				t->setAsPath();
				t = t->parent;
			}
		}
		Tile *s = startTile;
		s->setAsStart();
	}
}

void Field::Astar()
{
	if (startPos.x != -1 && startPos.y != -1 && endPos.x != -1 && endPos.y != -1)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (field[y * width + x].checkWall() != true)
				{
					field[y * width + x].parent = nullptr;
					field[y * width + x].localGoal = INFINITY;
					field[y * width + x].globalGoal = INFINITY;
					field[y * width + x].setAsUnVisited();
					field[y * width + x].setAsEmpty();
				}

			}
		}
		pathNotfound = false;

		auto distance = [](Tile* lhs, Tile* rhs)
		{
			Vei2 vec = lhs->getSrcPos() - rhs->getSrcPos();
			return vec.GetLength();
		};

		auto heuristic = [distance](Tile* lhs, Tile* rhs)
		{
			return distance(lhs, rhs);
		};

		// starting condition
		Tile *currentTile = startTile;
		startTile->localGoal = 0.0f;
		startTile->globalGoal = heuristic(startTile, endTile);

		std::list<Tile*> listNotYetSorted;
		listNotYetSorted.push_back(startTile);

		while (!listNotYetSorted.empty())
		{
			listNotYetSorted.sort([](const Tile* lhs, const Tile* rhs) {return lhs->globalGoal < rhs->globalGoal; });

			while (!listNotYetSorted.empty() && listNotYetSorted.front()->checkVisited() == true)
			{
				listNotYetSorted.pop_front();
			}
			if (listNotYetSorted.empty())
			{
				break;
			}

			currentTile = listNotYetSorted.front();
			currentTile->setAsVisited();

			for (auto neighbourTile : currentTile->vecNeighbours)
			{
				if (neighbourTile->checkVisited() == false && !neighbourTile->checkWall())
				{
					listNotYetSorted.push_back(neighbourTile);
				}


				float fPossLowerGoal = currentTile->localGoal + distance(currentTile, neighbourTile);

				if (fPossLowerGoal < neighbourTile->localGoal)
				{
					neighbourTile->parent = currentTile;
					neighbourTile->localGoal = fPossLowerGoal;


					neighbourTile->globalGoal = neighbourTile->localGoal + heuristic(neighbourTile, endTile);
				}
			}

		}

		createPath();
	}

}

