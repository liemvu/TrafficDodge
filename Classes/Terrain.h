//
//  Terrain.h
//  VNTraffic
//
//  Created by William Vo on 4/23/14.
//
//

#ifndef __VNTraffic__Terrain__
#define __VNTraffic__Terrain__

#include "cocos2d.h"

class Terrain : public cocos2d::Sprite
{
public:
    Terrain(void);
    ~Terrain(void);
    
    static Terrain * create();
    
private:
    void initTerrain(void);
};

#endif /* defined(__VNTraffic__Terrain__) */
