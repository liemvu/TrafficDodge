//
//  GameLayer.h
//  VNTraffic
//
//  Created by William Vo on 4/23/14.
//
//

#ifndef __VNTraffic__GameLayer__
#define __VNTraffic__GameLayer__

#include "cocos2d.h"
#include "Player.h"
#include "Vehicle.h"
USING_NS_CC;

enum {
    kStateIntro,
    kStatePlaying,
    kStateGameOver
};

class GameLayer : public cocos2d::Layer
{
public:
    
    GameLayer();
    ~GameLayer();
    
    static cocos2d::Scene* createScene();
    virtual bool init();
    
    void update (float dt);
    
    virtual void onEnter();
    virtual void onExit();
    virtual void onTouchesBegan(const std::vector<Touch*>& touches, Event* event);
    virtual void onTouchesEnded(const std::vector<Touch*>& touches, Event* event);
private:
    Size _screenSize;
    
    int _difficultIncreaseInterval;
	float _difficultIncreaseTimer;
    
    int _spawnVehicleInterval;
    float _spawnVehicleTimer;
    
    Sprite * _background;
    Sprite * _intro;
    Sprite * _gameover;
    Player * _player;
    ParticleSystemQuad * _fireParticle;
    
    Vector<Vehicle*> * _vehiclesPool;
    int _vehiclesPoolIndex;
    Vector<Vehicle*> * _vehicles;
    
    double _score;
    LabelBMFont * _scoreLabel;
    
    int _state;
    
    
    EventListenerTouchAllAtOnce * _touchListener;
    
    void createGameScreen(void);
    void resetGame(void);
    void increaseDifficulty();
    void spawnVehicle();
    void gameover();
    CREATE_FUNC(GameLayer);
};

#endif /* defined(__VNTraffic__GameLayer__) */
