//
//  Player.h
//  VNTraffic
//
//  Created by William Vo on 4/23/14.
//
//

#ifndef __VNTraffic__Player__
#define __VNTraffic__Player__

#include "GameSprite.h"
USING_NS_CC;



class Player : public GameSprite {
    
public:
    Player(void);
    ~Player(void);
    
    static Player * create();
    
    CC_SYNTHESIZE(float, _maxSpeed, MaxSpeed);
    
    inline virtual void place () {
		this->setPositionX( _nextPosition.x );
        if (_vector.y > 0 && this->getPositionY() < _screenSize.height * 0.1f) {
            this->setPositionY(this->getPositionY() + _vector.y * 0.25f);
            if (this->getPositionY() > _screenSize.height * 0.1f) {
                this->setPositionY(_screenSize.height * 0.1f);
            }
        }
	};
    
    void reset (void);
    virtual void update (float dt);
    void moveLeft();
    void moveRight();
private:
    
    float _speed;
    float _moveSpeed;
    int _line;
    void initPlayer(void);

};

#endif /* defined(__VNTraffic__Player__) */
