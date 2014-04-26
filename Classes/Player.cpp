//
//  Player.cpp
//  VNTraffic
//
//  Created by William Vo on 4/23/14.
//
//

#include "Player.h"

Player::~Player(void) {
    
}

Player::Player(void) {
}


Player * Player::create() {
    Player * player = new Player();
    if(player && player->initWithFile("blank.png")) {
        player->autorelease();
        player->setSize();
        player->initPlayer();
        return player;
    }
    
    CC_SAFE_DELETE(player);
    return NULL;
}

void Player::update(float dt) {
    if (_speed + ENV_ACCELERATION*dt <= _maxSpeed) {
        _speed += ENV_ACCELERATION*dt;
    } else {
        _speed = _maxSpeed;
    }
    _vector.y = _speed*dt;
    
    if(_vector.x != 0) {
        //moving x
        int targetX = LINE_POS[_line-1]*_screenSize.width;
        int currentX = _position.x;
        int nextX = currentX + _vector.x*dt;
        if((targetX >= currentX && nextX >= targetX) ||
           (targetX <= currentX && nextX <= targetX)) {
            //move over
            nextX = targetX;
            _vector.x = 0;
        }

        _nextPosition.x = nextX;
    }
    
    
}

void Player::initPlayer() {
    _height = 30;
    _width = 30;
    setAnchorPoint(Point(0.5, 1));
    setTextureRect(Rect(0,0,_width, _height));
    setColor(Color3B(255,0,0));
}

void Player::reset (void) {
    _speed = PLAYER_INITIAL_SPEED;
    _maxSpeed = PLAYER_INITIAL_SPEED;
    _moveSpeed = PLAYER_INITIAL_MOVE_SPEED;
    _vector = Point::ZERO;
    _line = 3;
    _nextPosition = Point((int) _screenSize.width*LINE_POS[_line-1], 0);
    _position = _nextPosition;
    _position.y -= _height;
}

void Player::moveLeft() {
    _line++;
    if(_line > NUMBER_OF_LINES) {
        _line = NUMBER_OF_LINES;
    }
    
    _vector.x = -_moveSpeed;
}

void Player::moveRight() {
    _line--;
    if(_line < 1) {
        _line = 1;
    }
    
    _vector.x = _moveSpeed;
}