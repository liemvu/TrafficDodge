//
//  Shake.h
//  VNTraffic
//
//  Created by William Vo on 4/26/14.
//
//

#ifndef __VNTraffic__Shake__
#define __VNTraffic__Shake__

#include "cocos2d.h"
USING_NS_CC;

class Shake : public ActionInterval {
public:
    
    
    static Shake* create(float d, float strength );
    static Shake* create(float d, float strength_x, float strength_y );
    
    
    virtual Shake* clone() const override;
	virtual Shake* reverse(void) const  override;
    virtual void startWithTarget(Node *pTarget);
    virtual void update(float time);
    virtual void stop(void);
    
protected:
    Shake();
    virtual ~Shake();
    bool initWithDuration(float d, float strength_x, float strength_y );
    
    // Initial position of the shaked node
    float _initial_x, _initial_y;
    // Strength of the action
    float _strength_x, _strength_y;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(Shake);
};

#endif /* defined(__VNTraffic__Shake__) */
