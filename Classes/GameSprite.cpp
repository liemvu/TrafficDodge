//
//  GameSprite.cpp
//  VNTraffic
//
//  Created by William Vo on 4/23/14.
//
//

#include "GameSprite.h"


GameSprite::~GameSprite(void) {
    
}

GameSprite::GameSprite(void) :
_vector(Point::ZERO),
_screenSize(Director::getInstance()->getWinSize()) {
    
}
