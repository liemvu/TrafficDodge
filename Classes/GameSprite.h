//
//  GameSprite.h
//  VNTraffic
//
//  Created by William Vo on 4/23/14.
//
//

#ifndef __VNTraffic__GameSprite__
#define __VNTraffic__GameSprite__

#include "cocos2d.h"
#include "GameConfig.h"
USING_NS_CC;

enum {
    
    kBackground,
    kMiddleground,
    kForeground
};

class GameSprite : public Sprite {
private:
    
    
    
public:
    GameSprite(void);
    ~GameSprite(void);
    
    CC_SYNTHESIZE(Point, _nextPosition, NextPosition);
    CC_SYNTHESIZE(float, _width, Width);
	CC_SYNTHESIZE(float, _height, Height);
    CC_SYNTHESIZE(Point, _vector, Vector);
    
    inline virtual void place () { this->setPosition(_nextPosition); };
    
    inline virtual float radius () {
        return _width * 0.5f;
    }
    
    
    inline void setSize() {
    	_width = this->boundingBox().size.width;
		_height = this->boundingBox().size.height;
    }
    
protected:
	Size _screenSize;
};

#endif /* defined(__VNTraffic__GameSprite__) */
