//
//  Vehicle.h
//  VNTraffic
//
//  Created by William Vo on 4/25/14.
//
//

#ifndef __VNTraffic__Vehicle__
#define __VNTraffic__Vehicle__

#include "GameSprite.h"

enum {
    kVehicleType1,
    kVehicleType2,
    kVehicleType3,
    kVehicleType4,
    kVehicleType5,
};

class Vehicle : public GameSprite {
public:
    Vehicle();
    ~Vehicle();
    
    CC_SYNTHESIZE(int, _type, Type);
    CC_SYNTHESIZE(int, _line, Line);
    CC_SYNTHESIZE(float, _speed, Speed);
    CC_SYNTHESIZE(float, _mark, Mark);
    
    static Vehicle * create();
    void reset();
    virtual void update(float dt);
    
    inline float bottom() {
        return _position.y - _height;
    }
    
    inline float top() {
        return _position.y;
    }
    
    virtual void draw(Renderer *renderer, const kmMat4& transform, bool transformUpdated);
protected:
    void onDraw(const kmMat4 &transform, bool transformUpdated);
    CustomCommand _customCommand;
    
private:
    void initVehicle();
};

#endif /* defined(__VNTraffic__Vehicle__) */
