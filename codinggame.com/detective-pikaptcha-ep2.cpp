# link: https://www.codingame.com/training/easy/detective-pikaptcha-ep2



#include <iostream>

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#define enable_tests


using lines_t = std::vector<std::string>;

struct Cell
{
	size_t y;
	size_t x;
};

class Map
{
public:
	Map(size_t w, size_t h, lines_t& lines)
		: width_(w)
		, height_(h)
		, lines_(lines)
	{
	}

	size_t Width() { return width_; }
	size_t Height() { return height_; }

	bool IsWall(Cell cell)
	{
		return cell.x >= width_ || cell.y >= height_ || lines_[cell.y][cell.x] == '#';
	}

	void IncrementCounter(Cell cell)
	{
		if (IsWall(cell))
			return;

		// here we make an assumption, that steps count of the same cell will less than 10
		++lines_[cell.y][cell.x];
	}

	size_t Value(Cell cell)
	{
		return lines_[cell.y][cell.x] - '0';
	}

	char& Char(Cell cell)
	{
		return lines_[cell.y][cell.x];
	}

private:
	size_t width_;
	size_t height_;
	lines_t& lines_;
};

class LinesComparator
{
public:
	LinesComparator(const lines_t& left, const lines_t& right)
		: left_(left),
		right_(right)
	{
	}

	void Compare()
	{
		_ASSERT(left_.size() == right_.size());

		for (int i = 0; i < left_.size(); ++i)
		{
			_ASSERT(left_[i].size() == right_[i].size());

			for (int j = 0; j < right_[i].length(); ++j)
			{
				_ASSERT(left_[i][j] == right_[i][j]);
			}
		}
	}

private:
	lines_t left_;
	lines_t right_;
};

class Player
{
public:
	Player(Cell start_pos, char preferred_side, char direction, Map& map)
		: start_position_(start_pos)
		, position_(start_pos)
		, preferred_side_(preferred_side)
		, direction_(direction)
		, map_(map)
	{
		// replace direction symbol with a '0'
		map_.Char(start_position_) = '0';
	}

	bool IsWallForward()
	{
		switch (direction_)
		{
		case '>':
			return map_.IsWall({ position_.y, position_.x + 1 });
		case '^':
			return map_.IsWall({ position_.y - 1, position_.x});
		case '<':
			return map_.IsWall({ position_.y, position_.x - 1});
		case 'v':
			return map_.IsWall({ position_.y + 1, position_.x});
		}
	}

	virtual bool IsWallOnSide() = 0;

	bool IsStartPosition()
	{
		return start_position_.x == position_.x && start_position_.y == position_.y;
	}
	
	// allowed values 90 and -90
	void TurnAround(int degree)
	{
		if (degree == 90)
		{
			switch (direction_)
			{
			case '<':
				direction_ = 'v';
				break;
			case 'v':
				direction_ = '>';
				break;
			case '>':
				direction_ = '^';
				break;
			case '^':
				direction_ = '<';
			}
		}
		else if (degree == -90)
		{
			switch (direction_)
			{
			case '<':
				direction_ = '^';
				break;
			case 'v':
				direction_ = '<';
				break;
			case '>':
				direction_ = 'v';
				break;
			case '^':
				direction_ = '>';
			}
		}
		else
		{
			// do nothing
		}
	}

	void Move()
	{
		if (!IsWallOnSide())
		{
			MoveSide();
		}
		else if (!IsWallForward())
		{
			MoveForward();
		}
		else
		{
			// if we reachec here, it means
			// we tried to move on side - it was blocked
			// we tried to move forward - it was also blocked
			// and now we try to move on opposite side
			TurnOpposite();
			if (!IsWallForward())
				return Move();

			// if opposite side was also blocked, than move back (if all four sides not blocked of course)
			TurnOpposite();
			if (!IsWallForward())
				return Move();
		}
		
		// special case, when no movement is possible, anyway we should set '1' at start position
		//if (map_.Value(start_position_) == 0)
		//	map_.IncrementCounter(start_position_);
	}

protected:
	// should increment current position and move on appropriate position
	virtual void MoveSide() = 0;

	void MoveForward()
	{
		if (IsWallForward())
			return;

		map_.IncrementCounter(position_);

		switch (direction_)
		{
		case '>':
			++position_.x;
			break;
		case '^':
			--position_.y;
			break;
		case '<':
			--position_.x;
			break;
		case 'v':
			++position_.y;
			break;
		}

	}
	
	virtual void TurnOpposite() = 0;

//protected:
public:
	const Cell start_position_;
	Cell position_;
	char preferred_side_;
	char direction_;
	Map& map_;
};

class LeftSidePrefferedPlayer : public Player
{
public:
	LeftSidePrefferedPlayer(Cell start_pos, char direction, Map& map)
		: Player(start_pos, Side_, direction, map)
	{
	}

	bool IsWallOnSide() override
	{
		if (direction_ == '>')
			return map_.IsWall({ position_.y - 1, position_.x });

		if (direction_ == '<')
			return map_.IsWall({ position_.y + 1, position_.x });

		if (direction_ == '^')
			return map_.IsWall({ position_.y, position_.x - 1 });
		
		if (direction_ == 'v')
			return map_.IsWall({ position_.y, position_.x + 1 });

		return false;
	}

	void MoveSide() override
	{
		if (IsWallOnSide())
			return;

		// A left cell is free, move on there
		TurnAround(90);
		MoveForward();
	}
	
	void TurnOpposite() override
	{
		TurnAround(-90);
	}

private:
	const char Side_ = 'L';
};

class RightSidePrefferedPlayer : public Player
{
public:
	RightSidePrefferedPlayer(Cell start_pos, char direction, Map& map)
		: Player(start_pos, Side_, direction, map)
	{
	}

private:
	const char Side_ = 'R';

	// Inherited via Player
	virtual bool IsWallOnSide() override
	{
		if (direction_ == '>')
			return map_.IsWall({ position_.y + 1, position_.x });

		if (direction_ == '<')
			return map_.IsWall({ position_.y - 1, position_.x });

		if (direction_ == '^')
			return map_.IsWall({ position_.y, position_.x + 1 });
		
		if (direction_ == 'v')
			return map_.IsWall({ position_.y, position_.x - 1 });

		return false;
	}

	virtual void MoveSide() override
	{
		if (IsWallOnSide())
			return;

		// A Right cell is free, move on there
		TurnAround(-90);
		MoveForward();
	}

	virtual void TurnOpposite() override
	{
		TurnAround(90);
	}
};


#ifdef  enable_tests

struct MapTests0
{
	MapTests0()
	{
		{
			lines_t lines =
			{
				{"#0"},
				{"##"}
			};
			Map map(2, 2, lines);

			_ASSERT(map.IsWall({ 0, 0 }) == true);
			_ASSERT(map.IsWall({ 1, 0 }) == true);
			_ASSERT(map.IsWall({ 1, 1 }) == true);
			_ASSERT(map.IsWall({ 0, 2 }) == true);
			_ASSERT(map.IsWall({ 2, 0 }) == true);

			_ASSERT(map.IsWall({ 0, 1 }) == false);
		}

		{
			lines_t lines =
			{
				{"00"}
			};

			Map m(2, 1, lines);
			_ASSERT(m.Value({ 0, 0 }) == 0);

			m.IncrementCounter({ 0, 0 });
			_ASSERT(m.Value({ 0, 0 }) == 1);
			
			m.IncrementCounter({ 0, 0 });
			_ASSERT(m.Value({ 0, 0 }) == 2);

			m.IncrementCounter({ 0, 0 });
			_ASSERT(m.Value({ 0, 0 }) == 3);
		}
	}

}map_tests_instance0;

struct PlayerLeftTests
{
	PlayerLeftTests()
	{
		{
			lines_t lines =
			{
				{">0"},
				{"##"}
			};
			Map map(2, 2, lines);

			Player* player = new LeftSidePrefferedPlayer({ 0,0 }, '>', map);

			_ASSERT(player->IsWallOnSide() == true);
			delete player;
		}
		
		{
			lines_t lines =
			{
				{"<0"},
				{"##"}
			};
			Map map(2, 2, lines);

			Player* player = new LeftSidePrefferedPlayer({ 0,0 }, '<', map);

			_ASSERT(player->IsWallOnSide() == true);
			delete player;
		}
		
		{
			lines_t lines =
			{
				{"<0"},
				{"0#"}
			};
			Map map(2, 2, lines);

			Player* player = new LeftSidePrefferedPlayer({ 0,0 }, '<', map);

			_ASSERT(player->IsWallOnSide() == false);
			delete player;
		}
		
		{
			lines_t lines =
			{
				{"^0"},
				{"0#"}
			};
			Map map(2, 2, lines);

			Player* player = new LeftSidePrefferedPlayer({ 0,0 }, '^', map);

			_ASSERT(player->IsWallOnSide() == true);

			delete player;
		}
		
		{
			lines_t lines =
			{
				{"v0"},
				{"0#"}
			};
			Map map(2, 2, lines);

			Player* player = new LeftSidePrefferedPlayer({ 0,0 }, 'v', map);

			_ASSERT(player->IsWallOnSide() == false);

			delete player;
		}

		// Complex test0
		{
			lines_t lines =
			{
				{"v0"},
				{"0#"}
			};
			Map map(2, 2, lines);

			Player* player = new LeftSidePrefferedPlayer({ 0,0 }, 'v', map);

			do
			{
				player->Move();
			} while (!player->IsStartPosition());

			lines_t expected_lines =
			{
				{"11"},
				{"0#"}
			};

			LinesComparator(lines, expected_lines).Compare();
		}

		// Complex test1
		{
			lines_t lines =
			{
				{">000#"},
				{"#0#00"},
				{"00#0#"}
			};
			Map map(5, 3, lines);

			Player* player = new LeftSidePrefferedPlayer({ 0,0 }, '>', map);

			do
			{
				player->Move();
			} while (!player->IsStartPosition());

			_ASSERT(player->position_.y == 0 && player->position_.x == 0);
			
			lines_t result_lines =
			{
				{"1322#"},
				{"#2#31"},
				{"12#1#"}
			};

			LinesComparator(lines, result_lines).Compare();
		}
		
		// Complex test2
		{
			lines_t lines =
			{
				{"0#0"},
				{"#>#"},
				{"0#0"}
			};
			Map map(3, 3, lines);

			Player* player = new LeftSidePrefferedPlayer({ 1,1 }, '>', map);

			do
			{
				player->Move();
			} while (!player->IsStartPosition());

			lines_t expected_lines =
			{
				{"0#0"},
				{"#0#"},
				{"0#0"}
			};

			LinesComparator(lines, expected_lines).Compare();
		}
		
		// Complex test3
		{
			lines_t lines =
			{
				{"###"},
				{"#0#"},
				{"<0#"}
			};
			Map map(3, 3, lines);

			Player* player = new LeftSidePrefferedPlayer({ 2,0 }, '<', map);

			do
			{
				player->Move();
			} while (!player->IsStartPosition());

			lines_t expected_lines =
			{
				{"###"},
				{"#1#"},
				{"12#"}
			};

			LinesComparator(lines, expected_lines).Compare();
		}
	}
}PlayerLeftTestsInstance;

struct PlayerRightTests
{
	PlayerRightTests()
	{
		// Complex test0
		{
			lines_t lines =
			{
				{">000#"},
				{"#0#00"},
				{"00#0#"}
			};
			Map map(5, 3, lines);

			Player* player = new RightSidePrefferedPlayer({ 0,0 }, '>', map);

			do
			{
				player->Move();
			} while (!player->IsStartPosition());

			_ASSERT(player->position_.y == 0 && player->position_.x == 0);
			
			lines_t result_lines =
			{
				{"1322#"},
				{"#2#31"},
				{"12#1#"}
			};

			LinesComparator(lines, result_lines).Compare();
		}

	}
}PlayerRightTestsInstance;


#endif //  enable_testso


int main()
{
	cout << "Start" << endl;

	
	cout << "End" << endl;

	return 0;
}
