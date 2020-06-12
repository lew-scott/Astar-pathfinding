#pragma once
#include "Graphics.h"
#include "Vei2.h"


class Field
{
	class Tile
	{
		enum class State
		{
			Normal,
			Path,
			Wall,
			Start,
			End
		};
	public:
		void initTile(const Vei2& Screen_pos);
		void drawTile(Graphics& gfx, const Vei2 ScreenPos);
		void setAsWall();
		void setAsPath();
		void setAsEmpty();
		void setAsStart();
		void setAsEnd();
		bool checkwall();

		
	private:
		Vei2 pos;
		static constexpr int width = 20;
		static constexpr int height = 20;
		State state = State::Normal;
		bool initialised = false;
	};

public:
	void drawScene(Graphics& gfx);
	Tile& atTile(const Vei2& gridPos);
	const Tile& atTile(const Vei2& gridPos) const;
	Vei2 ScreenToGrid(const Vei2& srcPos);
	void onLeftClick(const Vei2& scrPos);
	void onRightClick(const Vei2& scrPos);
	void resetGrid();

private:
	static constexpr int width = 20;
    static constexpr int height = 20;
	static constexpr int xOffset = 200;
	static constexpr int yOffset = 100;
	static constexpr int tileSize = 20;
	Vei2 startPos = { -1,-1 };
	Vei2 endPos = { -1,-1 };
	bool setStart = true;
	Tile field[width * height];

};