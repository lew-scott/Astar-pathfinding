#include "Field.h"
#include <assert.h>



void Field::Tile::initTile(const Vei2 & Screen_pos)
{
	assert(initialised == false);
	pos = Screen_pos;
	initialised = true;
}

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

bool Field::Tile::checkwall()
{
	return state == State::Wall;
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
	if (startPos.x == -1 && startPos.y == -1)
	{
		Tile& tile = atTile(gridPos);
		tile.setAsStart();
		startPos = gridPos;
		setStart = false;
	}
	else if (endPos.x == -1 && endPos.y == -1)
	{
 		Tile& tile = atTile(gridPos);
		tile.setAsEnd();
		endPos = gridPos;
		setStart = true;
	}
	else if (setStart == true)
	{
		Tile& oldTile = atTile(startPos);
		oldTile.setAsEmpty();
		Tile& newTile = atTile(gridPos);
		newTile.setAsStart();
		startPos = gridPos;
		setStart = false;
	}
	else if (setStart == false)
	{
		Tile& oldTile = atTile(endPos);
		oldTile.setAsEmpty();
		Tile& newTile = atTile(gridPos);
		newTile.setAsEnd();
		endPos = gridPos;
		setStart = true;
	}
}

void Field::onRightClick(const Vei2 & scrPos)
{
	Vei2 gridPos = ScreenToGrid(scrPos);
	assert(gridPos.x >= 0 && gridPos.x < width && gridPos.y >= 0 && gridPos.y < height);
	Tile& tile = atTile(gridPos);
	if(tile.checkwall() == false)
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
