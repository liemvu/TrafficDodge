//
//  Vehicle.cpp
//  VNTraffic
//
//  Created by William Vo on 4/25/14.
//
//

#include "Vehicle.h"

Vehicle::~Vehicle() {
    
}

Vehicle::Vehicle() {
    
}


Vehicle * Vehicle::create() {
    Vehicle * vehicle = new Vehicle();
    if(vehicle && vehicle->initWithFile("blank.png")) {
        vehicle->autorelease();
        vehicle->setSize();
        vehicle->initVehicle();
        return vehicle;
    }
    
    CC_SAFE_DELETE(vehicle);
    return NULL;

}

void Vehicle::initVehicle() {
    _width = 30;
    _height = 30;
    setAnchorPoint(Point(0.5, 1));
    setTextureRect(Rect(0,0,_width, _height));
    setColor(Color3B(255,255,255));
}

void Vehicle::reset() {
    switch(_type) {
        case kVehicleType1:
            _height = 30;
            setColor(Color3B(236, 0, 140));
            break;
        case kVehicleType2:
            _height = 30;
            setColor(Color3B(255, 242, 140));
            break;
        case kVehicleType3:
            _height = 45;
            setColor(Color3B(122, 204, 200));
            break;
        case kVehicleType4:
            _height = 60;
            setColor(Color3B(102, 45, 145));
            break;
        case kVehicleType5:
            _height = 90;
            setColor(Color3B(198, 156, 109));
            break;
    }
    setTextureRect(Rect(0,0,_width, _height));
    _mark = false;
}

void Vehicle::update(float dt) {
    _vector.y = _speed*dt;
    _nextPosition.y = _position.y + _vector.y;
    _nextPosition.x = _position.x;
}

void Vehicle::draw(Renderer *renderer, const kmMat4& transform, bool transformUpdated) {
    GameSprite::draw(renderer, transform, transformUpdated);
    _customCommand.init(_globalZOrder);
    _customCommand.func = CC_CALLBACK_0(Vehicle::onDraw, this, transform, transformUpdated);
    renderer->addCommand(&_customCommand);
}
void Vehicle::onDraw(const kmMat4 &transform, bool transformUpdated) {
    if(_mark) {
        DrawPrimitives::setDrawColor4F(1.0, 0.0, 0.0, 1.0);
        DrawPrimitives::setPointSize(8);
        DrawPrimitives::drawRect(Point(_position.x-_width/2, _position.y-_height), Point(_position.x+_width/2, _position.y));
    }
}