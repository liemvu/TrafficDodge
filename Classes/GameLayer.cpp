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
    CC_SAFE_DELETE(_vehiclesPool);
    CC_SAFE_DELETE(_vehicles);
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
    _vehicles = new Vector<Vehicle*>(50);
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
        for(int i=_vehicles->size()-1; i>=0; i--) {
            Vehicle * v = _vehicles->at(i);
            v->update(dt);
            
            Point nextPos = v->getNextPosition();
            //check out of screen
            if(nextPos.y < 0) {
                v->setVisible(false);
                _vehicles->erase(i);
                continue;
            }
            
            //check collision with player
            if(v->getBoundingBox().intersectsRect(_player->getBoundingBox())) {
                //gameover();
            }
            
            v->place();
            //check collision with other vehicles
            for(int j=i;j>=0;j--) {
                Vehicle * otherV = _vehicles->at(j);
                
                if(v != otherV &&
                   otherV->getLine() == v->getLine() &&
                   otherV->top() + _screenSize.height* 0.25f > v->bottom() &&
                   v->getSpeed() < otherV->getSpeed()) {
                    //update v speed
                    v->setSpeed(otherV->getSpeed());
                }
                
                if(v != otherV &&
                   otherV->getLine() == v->getLine() &&
                   otherV->getBoundingBox().intersectsRect(v->getBoundingBox())) {
                    Point p = v->getPosition();
                    p.y += v->getHeight() + 10;
                    otherV->setPosition(p);
                }
            }
        }
        
        //update score
        _score += _player->getMaxSpeed()*dt*SCORE_RATIO;
        _scoreLabel->setString(StringUtils::format("%0.0f", _score));
    }
    
    
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
    _player->reset();
    _difficultIncreaseInterval = SPEED_INCREASE_INTERVAL;
	_difficultIncreaseTimer = 0;
    
    _spawnVehicleInterval = SPAWN_VEHICLE_INTERVAL;
    _spawnVehicleTimer = 0;
    
    _score = 0;
    _scoreLabel->setString(StringUtils::format("%0.0f", _score));
    
    //hide all vehicles
    _vehiclesPoolIndex = 0;
    for(Vehicle * v : *_vehiclesPool) {
        v->setVisible(false);
    }
    _vehicles->clear();
    
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
    _vehiclesPool = new Vector<Vehicle*>(50);
    Vehicle * _vehicle;
    for(int i=0; i<50; i++) {
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
    Point pos;
    int i;
    //make sure it won't overlap each other
    for(i=0; i<6; i++) {
        line += i;
        line %= 6;
        pos = Point(LINE_POS[line]*_screenSize.width, _screenSize.height + v->getHeight());
        bool overlap = false;
        for(int j=_vehicles->size()-1; j>=0; j--) {
            Vehicle * otherV = _vehicles->at(j);
            if(otherV->getLine() == line && otherV->top() + _screenSize.height*0.1f > pos.y+v->getHeight()) {
                overlap = true;
                break;
            }
            
        }
        
        if(!overlap) {
            //it's ok
            break;
        }
    }
    
    if(i == 6) {
        return; //can't add now
    }
    
    v->setLine(line);
    v->setPosition(pos);
    
    //speed
    float speed;
    
    speed = -_player->getVector().y * VEHICLE_SPEED_RATIO * (CCRANDOM_0_1() + 0.5f);
    v->setSpeed(speed);
    
    //show it
    v->setVisible(true);
    
    //add it to vehicles
    _vehicles->pushBack(v);

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
}