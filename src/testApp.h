#pragma once

#include "ofMain.h"
#include "ofxTimeline.h"

typedef enum{
    BP = 0,
    EXXON,
    CHEVRON,
    SHELL,
    QUAD
    
}logo;

typedef enum{
    SINGLE_LINE,
    LINES_HORIZ,
    LINES_VERT,
    LINES_CENTER,
    GRID
    
}plotter;

typedef enum{
    H_BP,
    H_EXXON,
    H_CHEVRON,
    H_SHELL,
    H_QUAD,
}heatmap;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void updateRandomNumbers();
    
        ofxTimeline timeline;
        void bang(ofxTLBangEventArgs& args);
    int size;
    
    vector<vector<string> > randomNumbers;
    map<pair<int, int>, string> randomNumbersMap;
    logo logoMode;
    plotter plotterMode;
    heatmap heatmapMode;
    string text;
    bool calibration, drawTimeline, drawLogos, drawSpotLight, drawNumbers, drawPlotter;
    bool linesMoveLeft, linesMoveRight, drawLinesVertical, drawLinesHorizontal, drawLinesCenterVertical, drawSingleLineH;
    bool logoQuadrant, drawBP, drawExxon, drawShell, drawChevron;
    bool drawHeatmap, heatmapChevron, heatmapBP, heatmapExxon, heatmapShell;
    ofImage bp;
    ofTexture bp_tex;
    ofImage chevron;
    ofTexture chevron_tex;
    ofImage exxon;
    ofTexture exxon_tex;
    ofImage shell;
    ofTexture shell_tex;
    ofImage heatMap_chevron;
    
    int imgHeight, imgWidth;
    ofFbo screen;
    ofFbo bp_fbo;
    ofFbo chevron_fbo;
    ofFbo exxon_fbo;
    ofFbo shell_fbo;
    
    ofFbo h_bp_fbo;
    ofFbo h_chevron_fbo;
    ofFbo h_exxon_fbo;
    ofFbo h_shell_fbo;
    
    
    int screenHeight;
    int screenWidth;
    int screenX, screenY;
    
    int screenXdelta;
    int screenYdelta;
    int screenHeightDelta, screenWidthDelta;
    
    unsigned long long previousMillis = 0;
    int lineDeltaX, lineDeltaY;
    
    ofTrueTypeFont numbersFont;
};
