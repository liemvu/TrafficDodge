//
//  GameLayer.cpp
//  VNTraffic
//
//  Created by William Vo on 4/23/14.
//
//

#include "GameLayer.h"
#include "Player.h"
#include "SimpleAudioEngine.h"


using namespace CocosDenshion;
USING_NS_CC;

GameLayer::~GameLayer() {
    CC_SAFE_RELEASE(_shake);
    CC_SAFE_RELEASE(_lostLife);
    
    
    for(int i=NUMBER_OF_LINES-1; i>=0; i--) {
        auto vv = _vehicles[i];
        CC_SAFE_DELETE(vv);
        _vehicles[i] = NULL;
    }
    CC_SAFE_DELETE(_vehiclesPool);
    CC_SAFE_DELETE_ARRAY(_vehicles);
    CC_SAFE_DELETE_ARRAY(_hearts);
}

GameLayer::GameLayer() {
}

Scene* GameLayer::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameLayer::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    _screenSize = Director::getInstance()->getWinSize();
    _vehicles = new Vector<Vehicle*>*[NUMBER_OF_LINES];
    for(int i=0; i<NUMBER_OF_LINES;i++) {
        Vector<Vehicle*> * vv = new Vector<Vehicle*>(10);
        _vehicles[i] = vv;
    }
    
    _vehiclesPoolIndex = 0;
    _state = kStateIntro;
    createGameScreen();
    resetGame();
    
    scheduleUpdate();
    
    return true;
}

void GameLayer::onEnter() {
    Layer::onEnter();
    
    //register touch event
    _touchListener = EventListenerTouchAllAtOnce::create();
    
    _touchListener->onTouchesBegan = CC_CALLBACK_2(GameLayer::onTouchesBegan, this);
    _touchListener->onTouchesEnded = CC_CALLBACK_2(GameLayer::onTouchesEnded, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
}

void GameLayer::onExit() {
    Layer::onExit();
    _eventDispatcher->removeEventListener(_touchListener);
}

void GameLayer::update(float dt) {
    
    if(_state == kStatePlaying) {
        //increase speed
        _difficultIncreaseTimer += dt;
        if (_difficultIncreaseTimer > _difficultIncreaseInterval) {
            _difficultIncreaseTimer = 0;
            increaseDifficulty();
        }
        
        //spawn vehicle
        _spawnVehicleTimer += dt;
        if(_spawnVehicleTimer > _spawnVehicleInterval) {
            _spawnVehicleTimer = 0;
            spawnVehicle();
        }
        
        //update player;
        _player->update(dt);
        _player->place();

        //update vehicles
        for(int i=NUMBER_OF_LINES-1; i>=0; i--) {
            Vector<Vehicle*> *vv = _vehicles[i];
            for(int j=vv->size()-1; j>=0; j--) {
                Vehicle * v = vv->at(j);
                v->update(dt);

                Point nextPos = v->getNextPosition();
                //check out of screen
                if(nextPos.y < 0) {
                    v->setVisible(false);
                    vv->erase(j);
                    continue;
                }
                
                //check collision with player
                if(!_player->getInvincible() && v->getBoundingBox().intersectsRect(_player->getBoundingBox())) {
                    _life--;
                    updateLife();
                    if(_life <= 0) {
                        gameover();
                    }
                    else {
                        _player->runAction(_lostLife->clone());
                        _player->setInvincible(true);
                    }
                }
                
                v->place();
                
                //check collision with other vehicles
                if(j == 0) {
                    //we no need to check the first in line
                    //but may be should rand speed it up
                    continue;
                }
                
                //the ahead run to slow, adjust v speed to the ahead
                //may be it switch left or right
                Vehicle * aheadV = vv->at(j-1);
                if(aheadV->top() + 20 > v->bottom() &&
                   v->getSpeed() < aheadV->getSpeed()) {
                    v->setSpeed(aheadV->getSpeed());
                    v->setMark(true);
                }
            }
            
            
            
        }
        
        //update score
        _score += _player->getMaxSpeed()*dt*SCORE_RATIO;
        _scoreLabel->setString(StringUtils::format("%0.0f", _score));
        
        //update background parallax
        if (_player->getVector().y > 0) {
            _background->setPositionY(_background->getPosition().y - _player->getVector().y);
            float diffx;
            
            if (_background->getPositionY() < -_background->getContentSize().height) {
                diffx = fabs(_background->getPositionY()) - _background->getContentSize().height;
                _background->setPositionY(-diffx);
            }   
        }
    }
}

void GameLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event* event) {
    if(touches.size() == 0) return;
    
    if(_state == kStateIntro) {
        _state = kStatePlaying;
        _intro->setVisible(false);
        return;
    }
    else if(_state == kStateGameOver) {
        resetGame();
        _state = kStatePlaying;
        _gameover->setVisible(false);
        _fireParticle->stopSystem();
        _fireParticle->setVisible(false);
    }
    
    
    Touch * touch = touches.at(0);
    Point tap = touch->getLocation();
    
    if(tap.x < _player->getPosition().x) {
        //move left
        _player->moveLeft();
    }
    else {
        //move right
        _player->moveRight();
    }
}

void GameLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event* event) {

}


void GameLayer::resetGame(void) {
    stopAllActions();
    _player->reset();
    _difficultIncreaseInterval = SPEED_INCREASE_INTERVAL;
	_difficultIncreaseTimer = 0;
    
    _spawnVehicleInterval = SPAWN_VEHICLE_INTERVAL;
    _spawnVehicleTimer = 0;
    
    _life = 3;
    updateLife();
    
    _score = 0;
    _scoreLabel->setString(StringUtils::format("%0.0f", _score));
    
    //hide all vehicles
    _vehiclesPoolIndex = 0;
    for(Vehicle * v : *_vehiclesPool) {
        v->setVisible(false);
    }
    
    for(int i=0; i<NUMBER_OF_LINES; i++) {
        _vehicles[i]->clear();
    }
    
    SimpleAudioEngine::getInstance()->stopAllEffects();
    SimpleAudioEngine::getInstance()->playBackgroundMusic("background.mp3");
}

void GameLayer::createGameScreen(void) {
    Sprite * repeat;
    
    //bg
    _background = Sprite::create("bg.png");
    _background->setAnchorPoint(Point::ZERO);
    addChild(_background, kBackground);
    
    repeat = Sprite::create("bg.png");
    repeat->setAnchorPoint(Point::ZERO);
    repeat->setPosition(Point(0, _background->getContentSize().height-1));
    _background->addChild(repeat, kBackground);
    
    repeat = Sprite::create("bg.png");
    repeat->setAnchorPoint(Point::ZERO);
    repeat->setPosition(Point(0, 2*(_background->getContentSize().height-1)));
    _background->addChild(repeat, kBackground);
    
    //player
    _player = Player::create();
    addChild(_player, kForeground);
    
    //vehicle pool
    _vehiclesPool = new Vector<Vehicle*>(60);
    Vehicle * _vehicle;
    for(int i=0; i<60; i++) {
        _vehicle = Vehicle::create();
        _vehicle->setType(rand()%5);
        _vehicle->setVisible(false);
        _vehicle->setTag(i);
        addChild(_vehicle, kForeground);
        _vehiclesPool->pushBack(_vehicle);
    }
    
    //create particle
    _fireParticle = ParticleSystemQuad::create("fire_particle.plist");
    _fireParticle->setVisible(true);
    _fireParticle->stopSystem();
    addChild(_fireParticle, kForeground);
    
    
    
    
    //intro
    _intro = Sprite::create("intro.png");
    _intro->setPosition(Point(_screenSize.width*0.5, _screenSize.height*0.5));
    addChild(_intro, kForeground);
    
    _gameover = Sprite::create("gameover.png");
    _gameover->setPosition(Point(_screenSize.width*0.5, _screenSize.height*0.5));
    _gameover->setVisible(false);
    addChild(_gameover, kForeground);
    
    //create score label
    _scoreLabel = LabelBMFont::create("0", "font_levels.fnt", _screenSize.width * 0.1f);
    _scoreLabel->setPosition(Point(_screenSize.width*0.5f, _screenSize.height*0.95f));
    addChild(_scoreLabel, kForeground);
    
    
    //hearts
    _hearts = new Sprite*[MAX_LIFE];
    for(int i=0; i<MAX_LIFE; i++) {
        Sprite * heart = Sprite::create("heart.png");
        heart->setVisible(false);
        heart->setPosition(Point(_screenSize.width * 0.15f + i*(heart->getContentSize().width + 5), _screenSize.height * 0.95f));
        addChild(heart, kForeground);
        _hearts[i] = heart;
    }
    
    
    //action
    _shake = Shake::create(2.0f, 5);
    _shake->retain();
    
    _lostLife = Sequence::create(Blink::create(3, 10),
                                 CallFuncN::create(CC_CALLBACK_1(GameLayer::lostLifeAnimationCompleted, this)), NULL);
    _lostLife->retain();
}

void GameLayer::increaseDifficulty() {
    //increase player speed
    _player->setMaxSpeed (_player->getMaxSpeed() + SPEED_INCREASE_AMOUNT);
    if(_player->getMaxSpeed() > SPEED_INCREASE_MAX) {
        _player->setMaxSpeed(SPEED_INCREASE_MAX);
    }
    
    //increase spawn vehicle rate
    _spawnVehicleInterval += SPAWN_VEHICLE_DECREASE_AMOUNT;
    if(_spawnVehicleInterval < SPAWN_VEHICLE_MAX) {
        _spawnVehicleInterval = SPAWN_VEHICLE_MAX;
    }
}

void GameLayer::spawnVehicle() {
    Vehicle * v = _vehiclesPool->at(_vehiclesPoolIndex);
    
    //type
    v->setType(rand()%5);
    v->reset();
    
    //position
    int line = rand()%6;
    Vector<Vehicle*> * vv;
    Vehicle * aheadV;
    Point pos;
    float speed;
    int i;
    
    //make sure it won't overlap each other
    for(i=0; i<NUMBER_OF_LINES; i++) {
        line += i;
        line %= 6;

        vv = _vehicles[line];
        
        //position
        pos = Point(LINE_POS[line]*_screenSize.width, _screenSize.height + v->getHeight());
        if(vv->size() > 0) {
            //make sure no overlap with ahead vehicle
            aheadV = vv->at(vv->size()-1);
            if(aheadV->top() + 20 > _screenSize.height) {
                continue;
            }
        }
        
        v->setLine(line);
        v->setPosition(pos);
        
        //speed
        speed = -_player->getVector().y * VEHICLE_SPEED_RATIO * (CCRANDOM_0_1() + 1);
        v->setSpeed(speed);
        break;
    }
    
    if(i == NUMBER_OF_LINES) {
        return; //can't add now
    }
    
    
    
    //show it
    v->setVisible(true);
    
    //add it to vehicles
    vv->pushBack(v);

    //pool
    _vehiclesPoolIndex++;
    if(_vehiclesPoolIndex >= _vehiclesPool->size()) {
        _vehiclesPoolIndex = 0;
    }

}

void GameLayer::gameover() {
    _state = kStateGameOver;
    _gameover->setVisible(true);
    _fireParticle->setPosition(_player->getPosition());
    _fireParticle->setVisible(true);
    _fireParticle->resetSystem();
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->playEffect("gameover.wav");
    runAction(_shake->clone());
}

void GameLayer::debugVehicles() {
    printf("== debug start ==\n");
    for(int i=0; i<NUMBER_OF_LINES; i++) {
        printf("line %d\n", i);
        auto vv = _vehicles[i];
        
        for(int j=vv->size()-1; j>=0; j--) {
            auto v = vv->at(j);
            printf("vehicle [%d_%d] pos:%f speed:%f \n", i, v->getTag(), v->getPosition().y, v->getSpeed());
        }
    }
    
    printf("== debug end ==\n");
}

void GameLayer::updateLife() {
    if(_life < 0) {
        _life = 0;
    }
    else if(_life > MAX_LIFE) {
        _life = MAX_LIFE;
    }

    for(int i=0; i<MAX_LIFE; i++) {
        _hearts[i]->setVisible(i<=_life-1);
    }
}

void GameLayer::lostLifeAnimationCompleted(Node * node) {
    _player->setInvincible(false);
}