#include "AppDelegate.h"
#include "GameLayer.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace CocosDenshion;
AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    srand (time(NULL));
    
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("VNTraffic");
        director->setOpenGLView(glview);
    }

    Size screenSize = glview->getFrameSize();
    Size designSize = Size(320, 480);
    
    glview->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::EXACT_FIT);
    std::vector<std::string> searchPath;
    if (screenSize.width > 768) {
        searchPath.push_back("ipadhd");
        director->setContentScaleFactor(1280/designSize.width);
    } else if (screenSize.width > 320) {
        searchPath.push_back("ipad");
        director->setContentScaleFactor(640/designSize.width);
    } else {
        searchPath.push_back("iphone");
        director->setContentScaleFactor(320/designSize.width);
    }
    
    FileUtils::getInstance()->setSearchPaths(searchPath);
    
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic("background.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("gameover.wav");
    
    
    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = GameLayer::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
