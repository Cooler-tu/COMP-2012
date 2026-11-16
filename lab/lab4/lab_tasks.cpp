#include "GameObject.hpp"
#include "GameBoard.hpp"
#include "objects.hpp"

// Complete your code down below!
Player::Player(int x, int y)
    : GameObject(x, y, " P ", 6, true, true){}

Wall::Wall(int x, int y)
    : GameObject(x, y, "###", 5, true, false){}

Box::Box(int x, int y)
    : GameObject(x, y, " X ", 4, true, true){}

Door::Door(int x, int y)
    : GameObject(x, y, "[#]", 3, true, false){}

Button::Button(int x, int y, Door* door)
    : GameObject(x, y, "---", 2, false, false), door(door){}

Flag::Flag(int x, int y)
    : GameObject(x, y, "!!!", 1, false, false){}


GameObject* GameBoard::find_collidable_object_at(int x, int y) const
{
    for(int i = 0; i < object_count; i++)
    {
        if(objects[i]->get_x() == x && objects[i]->get_y() == y)
            if(objects[i]->get_is_collidable()) return objects[i];
    }
    return nullptr;
}

bool GameObject::try_move(GameBoard& board, Direction direction)
{
    if(!this->is_movable) return false;

    int dx = 0, dy = 0;
    if(direction == EAST) dx = 1;
    else if(direction == WEST) dx = -1;
    else if(direction == SOUTH) dy = 1;
    else if(direction == NORTH) dy = -1;
    int x = this->get_x() + dx, y = this->get_y() + dy;
    GameObject* nxt = board.find_collidable_object_at(x, y);
    if(nxt  == nullptr || nxt->try_move(board, direction))
    {
        this->set_x(x); this->set_y(y);
        return true;
    }
    return false;
}

void Door::open_door()
{
    this->set_is_collidable(false);
    this->set_sprite("[ ]");
}

void Door::close_door(const GameBoard& board)
{
    GameObject* in = board.find_collidable_object_at(this->get_x(), this->get_y());
    if(in == nullptr)
    {
        this->set_is_collidable(true);
        this->set_sprite("[#]");
    }
}

void Button::update_button(const GameBoard& board)
{
    GameObject* in = board.find_collidable_object_at(this->get_x(), this->get_y());
    if(in && in->get_is_collidable())
    {
        this->door->open_door();
        this->set_sprite("___");
    }
    else
    {
        this->door->close_door(board);
        this->set_sprite("---");
    }
}