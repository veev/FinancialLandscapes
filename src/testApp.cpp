#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    
    //screenSyphonServer.setName("Screen Output");
    ofSetFrameRate(60);
    
    currentMode = PLOTTER;
    plotterMode = SINGLE_LINE;
    logoMode = BP;
    screenX = 46;
    screenY = 7;
    screenWidth = 950;
    screenHeight = 992;
    screenXdelta, screenYdelta, screenWidthDelta, screenHeightDelta = 0;
    
    
    if(ofFile::doesFileExist("screenCalibration.txt")){
        ofBuffer calibrationBuffer = ofBufferFromFile("screenCalibration.txt");
        string values = calibrationBuffer.getFirstLine();
        vector<string> valuesVector = ofSplitString(values, ",");
        screenWidth = ofToInt(valuesVector[0]);
        screenHeight =ofToInt(valuesVector[1]);
        screenX = ofToInt(valuesVector[2]);
        screenY = ofToInt(valuesVector[3]);
        ofLogNotice()<<"Calibration values loaded from file."<<endl;
    }
    else{
        ofLogError()<<"CALIBRATION FILE DOES NOT EXIST!"<<endl;
    }
   
    updateRandomNumbers();
    
    
    
    screen.allocate( screenWidth, screenHeight, GL_RGB);
    //screenTex.allocate(screenWidth, screenHeight, GL_RGB);
    imgWidth = screen.getWidth()/2;
    imgHeight = screen.getHeight()/2;
    
    bp_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    chevron_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    exxon_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    shell_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    
    h_chevron_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    h_shell_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    h_exxon_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    h_bp_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    
    bp.loadImage("images/BP_crop.jpg");
    chevron.loadImage("images/chevron_crop.jpg");
    exxon.loadImage("images/exxon.jpg");
    shell.loadImage("images/shell-logo.jpg");
    
    heatMap_chevron.loadImage("images/Chevron_Yahoo.csv.tif");
    heatMap_bp.loadImage("images/BP_Yahoo.csv.tif");
    heatMap_exxon.loadImage("images/XOM_Yahoo.csv.tif");
    heatMap_shell.loadImage("images/RDSA_Yahoo.csv.tif");

    generateHeatMaps();

    
    //numbersFont.loadFont("Menlo.ttc", 18);
    numbersFont.loadFont("Monaco.dfont", 18);
    
    
    timeline.setup();
    timeline.addBangs("bangs");
    timeline.addCurves("scale", ofRange(imgWidth, screenHeight + imgHeight));
    timeline.addCurves("theta", ofRange(0, 720));
    timeline.addCurves("numbersMoveX", ofRange(0, screenWidth));
    timeline.addCurves("linesMoveX", ofRange(-screenWidth, screenWidth));
    timeline.addCurves("linesMoveY", ofRange(-screenHeight*2.0, screenHeight*2.0));
    //    timeline.addCurves("linesMoveY", ofRange(-screenH))
    //timeline.addCurves("linesMoveLeft", ofRange(-0.5, 0));
    timeline.addSwitches("switches");
    timeline.addFlags("flags");
    timeline.addLFO("circleX", ofRange(0, screenWidth) );
    timeline.addLFO("circleY", ofRange(0, screenHeight) );
    timeline.addLFO("circleSize", ofRange(10, 50));
    timeline.addLFO("lineWidth", ofRange(1, screenWidth/2));
    timeline.addColors("colors");
    ofAddListener(timeline.events().bangFired, this, &testApp::bang);
    timeline.setLoopType(OF_LOOP_NORMAL);
    timeline.setDurationInSeconds(2400);
    timeline.enableEvents();
    size = 20;
    theta = 0;
    text = "";

    
    lineDeltaX, lineDeltaY = 0;
    
}

//--------------------------------------------------------------
void testApp::bang(ofxTLBangEventArgs& args){
    
    cout << args.flag << endl;
    
    if(args.flag == "plotter")
    {
        currentMode = PLOTTER;
    }
    else if(args.flag == "singleLine")
    {
        plotterMode = SINGLE_LINE;
    }
    else if(args.flag == "linesHoriz")
    {
        plotterMode = LINES_HORIZ;
    }
    else if(args.flag == "linesVert")
    {
        plotterMode = LINES_VERT;
    }
    else if(args.flag == "grid")
    {
        plotterMode = GRID;
    }
    else if(args.flag == "numbers")
    {
        currentMode = NUMBERS;
    }
    else if(args.flag == "logos")
    {
        currentMode = LOGOS;
    }
    else if(args.flag == "bp")
    {
        logoMode = BP;
    }
    else if(args.flag == "shell")
    {
        logoMode = SHELL;
    }
    else if(args.flag == "exxon")
    {
        logoMode = EXXON;
    }
    else if(args.flag == "chevron")
    {
        logoMode = CHEVRON;
        logoQuadrant = false;
    }
    else if(args.flag == "logoQuad")
    {
        logoMode = QUAD;
    }
    else if(args.flag == "heatmap")
    {
        heatmapMode = H_CHEVRON;
        currentMode = HEATMAP;
    }
    
    else if(args.flag == "spotlight")
    {
        currentMode = SPOTLIGHT;
    }
    else {
        //text = args.flag;
    }
    
}


//--------------------------------------------------------------
void testApp::update(){
    
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    
    ofPoint position;
    
    size = timeline.getValue("scale");
    theta = timeline.getValue("theta");
    
    drawToFbos();
    
    switch (currentMode) {
        case LOGOS:
            runLogoMode(logoMode);
            break;
        
        case HEATMAP:
            runHeatMapMode(heatmapMode);
            break;
        
        case PLOTTER:
            runPlotterMode(plotterMode);
            break;
            
        case NUMBERS:
            runNumbersMode();
            break;
        
        case SPOTLIGHT:
            runSpotlightMode();
            break;

        default:
            break;
    }
    
    if(drawTimeline) {
        ofSetRectMode(OF_RECTMODE_CORNER);
        timeline.draw();
    }
    
    if(calibration) {
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofBackground(255, 0, 0);
        ofSetColor(255);
        
        ofRect(screenX, screenY, screenWidth, screenHeight);
        ofSetColor(0);
        ofDrawBitmapString("screenWidth: " + ofToString(screenWidth), 300, 300);
        ofDrawBitmapString("screenHeight: " + ofToString(screenHeight), 300, 350);
        ofDrawBitmapString("screenX: " + ofToString(screenX), 300, 250);
        ofDrawBitmapString("screenY: " + ofToString(screenY), 300, 200);
        
        
    }
    
    //screenTex = screen.getTextureReference();
    //screenSyphonServer.publishTexture(&screenTex);
    
}

void testApp:: runLogoMode(logo mode){
    
    switch(mode){
            
        case BP:
            screen.begin();
            ofBackground(255);
            ofPushMatrix();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofTranslate(screen.getWidth()/2, screen.getHeight()/2);
            ofRotate(theta);
            bp.draw(0, 0, size, size);
            ofPopMatrix();
            screen.end();
            
            ofPushMatrix();
            ofSetRectMode(OF_RECTMODE_CORNER);
            ofTranslate(screenX, screenY);
            screen.draw(0, 0);
            ofPopMatrix();
            break;
            
        case CHEVRON:
            screen.begin();
            ofBackground(255);
            ofPushMatrix();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofTranslate(screen.getWidth()/2, screen.getHeight()/2);
            ofRotate(theta);
            chevron.draw(0, 0, size, size);
            ofPopMatrix();
            screen.end();
            
            ofPushMatrix();
            ofSetRectMode(OF_RECTMODE_CORNER);
            ofTranslate(screenX, screenY);
            screen.draw(0, 0);
            ofPopMatrix();
            break;
            
        case SHELL:
            screen.begin();
            ofBackground(255);
            ofPushMatrix();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofTranslate(screen.getWidth()/2, screen.getHeight()/2);
            ofRotate(theta);
            shell.draw(0, 0, size, size);
            ofPopMatrix();
            screen.end();
            
            ofPushMatrix();
            ofSetRectMode(OF_RECTMODE_CORNER);
            ofTranslate(screenX, screenY);
            screen.draw(0, 0);
            ofPopMatrix();
            break;
            
        case QUAD:
            screen.begin();
            ofBackground(255);
            ofSetRectMode(OF_RECTMODE_CORNER);
            ofPushMatrix();
            ofTranslate(0, 0);
            exxon_fbo.draw(0, 0);
            ofPopMatrix();
            
            ofPushMatrix();
            ofTranslate(imgWidth, 0);
            bp_fbo.draw(0, 0);
            ofPopMatrix();
            
            ofPushMatrix();
            ofTranslate(0, imgHeight);
            shell_fbo.draw(0, 0);
            ofPopMatrix();
            
            ofPushMatrix();
            ofTranslate(imgWidth , imgHeight);
            chevron_fbo.draw(0, 0);
            ofPopMatrix();
            screen.end();
            
            ofPushMatrix();
            ofTranslate(screenX, screenY);
            screen.draw(0, 0);
            ofPopMatrix();
            break;
            
        case EXXON:
            screen.begin();
            ofBackground(255);
            ofPushMatrix();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofTranslate(screen.getWidth()/2, screen.getHeight()/2);
            ofRotate(theta);
            exxon.draw(0, 0);
            ofPopMatrix();
            screen.end();
            
            ofPushMatrix();
            ofSetRectMode(OF_RECTMODE_CORNER);
            ofTranslate(screenX, screenY);
            screen.draw(0, 0);
            ofPopMatrix();
            break;
            
            
        default:
            
            break;
            
            
    }
    
}

void testApp::generateHeatMaps(){
    
    //generate heatmap Chevron
    for(int i =0; i<heatMap_chevron.getWidth(); i++){
        for(int j = 0; j<heatMap_chevron.getHeight(); j++){
            
            ofColor c = heatMap_chevron.getColor(i, j);
            
            float alpha = c.getBrightness();
            float f = ofMap(alpha, 0, 255, ofFloatColor::red.getHue(),ofFloatColor::green.getHue(),true)  ;
            
            ofFloatColor floatColor = ofFloatColor::fromHsb(f, 200, 200);
            heatMap_chevron.setColor(i, j, floatColor);
        }
    }
    heatMap_chevron.update();
    
    //generate heatmap BP
    for(int i =0; i<heatMap_bp.getWidth(); i++){
        for(int j = 0; j<heatMap_bp.getHeight(); j++){
            
            ofColor c = heatMap_bp.getColor(i, j);
            
            float alpha = c.getBrightness();
            float f = ofMap(alpha, 0, 255, ofFloatColor::red.getHue(),ofFloatColor::green.getHue(),true)  ;
            
            ofFloatColor floatColor = ofFloatColor::fromHsb(f, 200, 200);
            heatMap_bp.setColor(i, j, floatColor);
        }
    }
    heatMap_bp.update();
    
    //generate heatmap Exxon
    for(int i =0; i<heatMap_exxon.getWidth(); i++){
        for(int j = 0; j<heatMap_exxon.getHeight(); j++){
            
            ofColor c = heatMap_exxon.getColor(i, j);
            
            float alpha = c.getBrightness();
            float f = ofMap(alpha, 0, 255, ofFloatColor::red.getHue(),ofFloatColor::green.getHue(),true)  ;
            
            ofFloatColor floatColor = ofFloatColor::fromHsb(f, 200, 200);
            heatMap_exxon.setColor(i, j, floatColor);
        }
    }
    heatMap_exxon.update();
    
    //generate heatmap Shell
    for(int i =0; i<heatMap_shell.getWidth(); i++){
        for(int j = 0; j<heatMap_shell.getHeight(); j++){
            
            ofColor c = heatMap_shell.getColor(i, j);
            
            float alpha = c.getBrightness();
            float f = ofMap(alpha, 0, 255, ofFloatColor::red.getHue(),ofFloatColor::green.getHue(),true)  ;
            
            ofFloatColor floatColor = ofFloatColor::fromHsb(f, 200, 200);
            heatMap_shell.setColor(i, j, floatColor);
        }
    }
    heatMap_shell.update();
}

void testApp::drawToFbos(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    
    bp_fbo.begin();
    ofBackground(255);
    ofPushMatrix();
    ofTranslate(bp_fbo.getWidth()/2, bp_fbo.getHeight()/2);
    ofRotate(theta);
    bp.draw(0, 0, size, size);
    ofPopMatrix();
    bp_fbo.end();
    
    chevron_fbo.begin();
    ofBackground(255);
    ofPushMatrix();
    ofTranslate(chevron_fbo.getWidth()/2, chevron_fbo.getHeight()/2);
    ofRotate(theta);
    chevron.draw(0, 0, size, size);
    ofPopMatrix();
    chevron_fbo.end();
    
    exxon_fbo.begin();
    ofBackground(255);
    ofPushMatrix();
    ofTranslate(exxon_fbo.getWidth()/2, exxon_fbo.getHeight()/2);
    ofRotate(theta);
    exxon.draw(0, 0, size, size);
    ofPopMatrix();
    exxon_fbo.end();
    
    shell_fbo.begin();
    ofBackground(255);
    ofPushMatrix();
    ofTranslate(shell_fbo.getWidth()/2, shell_fbo.getHeight()/2);
    ofRotate(theta);
    shell.draw(0, 0, size, size);
    ofPopMatrix();
    shell_fbo.end();
    
    h_exxon_fbo.begin();
    ofBackground(255,0,0);
    ofPushMatrix();
    ofTranslate(h_exxon_fbo.getWidth()/2, h_exxon_fbo.getHeight()/2);
    ofRotate(theta);
    heatMap_chevron.draw(0, 0, imgWidth, imgHeight);
    ofPopMatrix();
    h_exxon_fbo.end();
    
    h_shell_fbo.begin();
    ofBackground(255,0,0);
    ofPushMatrix();
    ofTranslate(h_shell_fbo.getWidth()/2, h_shell_fbo.getHeight()/2);
    ofRotate(theta);
    heatMap_shell.draw(0, 0, imgWidth, imgHeight);
    ofPopMatrix();
    h_shell_fbo.end();
    
    h_bp_fbo.begin();
    ofBackground(255,0,0);
    ofPushMatrix();
    ofTranslate(h_bp_fbo.getWidth()/2, h_bp_fbo.getHeight()/2);
    ofRotate(theta);
    heatMap_bp.draw(5, 0, imgWidth, imgHeight);
    ofPopMatrix();
    h_bp_fbo.end();
    
    h_chevron_fbo.begin();
    ofBackground(255,0,0);
    ofPushMatrix();
    ofTranslate(h_chevron_fbo.getWidth()/2, h_chevron_fbo.getHeight()/2);
    ofRotate(theta);
    ofRotate(180);
    heatMap_chevron.draw(0, 0, imgWidth, imgHeight);
    ofPopMatrix();
    h_chevron_fbo.end();
    
}

void testApp::runPlotterMode(plotter mode){
    screen.begin();
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofBackground(0);
    ofSetColor(255);
    ofSetLineWidth(0.01);
    
    lineDeltaX = timeline.getValue("linesMoveX");
    lineDeltaY = timeline.getValue("linesMoveY");
    
    switch (mode) {
            
        case GRID:
            for(int i = 0; i < screenWidth; i+=20) {
                ofLine((i + lineDeltaX) % (screenWidth), 0, (i + lineDeltaX) % (screenWidth), screenHeight);
            }
            for(int i = 0; i < screenHeight; i+=20) {
                ofLine(0, (i + lineDeltaY) % screenHeight, screenWidth, (i + lineDeltaY) % screenHeight);
            }
            break;
        case LINES_VERT:
            for(int i = 0; i < screenWidth; i+=timeline.getValue("circleX")) {
                ofLine((i + lineDeltaX) % screenWidth, 0, (i + lineDeltaX) % screenWidth, screenHeight);
                
            }
            break;
            
        case LINES_HORIZ:
            for(int i = screenHeight*2; i >= screenHeight; i-=20) {
                ofLine(0, (i - lineDeltaY), screenWidth, (i - lineDeltaY));
                
            }
            break;
            
        case LINES_CENTER:
            for(int i = screenWidth/2; i < screenWidth; i+=20) {
                ofLine((i + lineDeltaX) % screenWidth/2, 0, (i + lineDeltaX) % screenWidth/2, screenHeight);
            }
            break;
            
        case SINGLE_LINE:
            int lineWidth = timeline.getValue("lineWidth");
            ofPushStyle();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofSetColor(255);
            ofFill();
            ofRect(lineDeltaX,screen.getHeight()/2, lineWidth, screenHeight);
            ofPopStyle();
            break;
            
    }
    screen.end();
    
    ofPushMatrix();
    ofTranslate(screenX, screenY);
    screen.draw(0, 0);
    ofPopMatrix();
}

void testApp::runHeatMapMode(heatmap mode){
    screen.begin();
    ofBackground(0);
    ofSetRectMode(OF_RECTMODE_CORNER);
    
    ofPushMatrix();
    ofTranslate(0, 0);
    h_exxon_fbo.draw(0, 0);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(screen.getWidth()/2, 0);
    h_bp_fbo.draw(0, 0);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(0, screen.getHeight()/2);
    h_shell_fbo.draw(0, 0);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(screen.getWidth()/2, screen.getHeight()/2);
    h_chevron_fbo.draw(0, 0);
    ofPopMatrix();
    
    screen.end();
    
    ofPushMatrix();
    ofTranslate(screenX, screenY);
    screen.draw(0, 0);
    ofPopMatrix();
    
}

void testApp::runNumbersMode(){
    screen.begin();
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofBackground(0);
    ofPushStyle();
    ofSetColor(0,255,0);
    int moveX = timeline.getValue("numbersMoveX");
    unsigned long long currentMillis = ofGetElapsedTimeMillis();
    if( currentMillis - previousMillis > 10) {
        previousMillis = currentMillis;
        for( int i = 0; i < screen.getWidth(); i+= 149) {
            for( int j = 0; j < screen.getHeight(); j += 20) {
                if ((ofGetElapsedTimeMillis() % 10000)) {
                    
                    updateRandomNumbers();
                    
                }
                numbersFont.drawString(randomNumbersMap[make_pair(i, j)], i , j + moveX);
            }
        }
        ofPopStyle();
    }
    
    screen.end();
    
    ofPushMatrix();
    ofTranslate(screenX, screenY);
    screen.draw(0, 0);
    ofPopMatrix();
}

void testApp::runSpotlightMode(){
    ofSetRectMode(OF_RECTMODE_CORNER);
    int circleX = timeline.getValue("circleX");
    int circleY = timeline.getValue("circleY");
    int circleSize = timeline.getValue("circleSize");
    screen.begin();
    ofBackground(0);
    ofSetColor(255);
    ofCircle(circleX, circleY, circleSize);
    screen.end();
    
    ofPushMatrix();
    ofTranslate(screenX, screenY);
    screen.draw(0, 0);
    ofPopMatrix();
}


//--------------------------------------------------------------
void testApp::updateRandomNumbers(){
    
    randomNumbersMap.clear();
    
    for( int i = 0; i < screen.getWidth(); i+= 149) {
        for( int j = 0; j < screen.getHeight(); j += 20) {
            //
            string test = "$" + ofToString(ofRandom(100000, 999999), 2);
            randomNumbersMap[make_pair(i, j)]=test;
        }
    }
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    
    switch (key) {
        case 't':
            drawTimeline = !drawTimeline;
            break;
            
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'c':
            calibration = !calibration;
            break;
            
        case '1':
            screenWidth++;
            break;
            
        case '2':
            screenWidth--;
            break;
            
        case '3':
            screenHeight++;
            break;
            
        case '4':
            screenHeight--;
            break;
            
        case OF_KEY_LEFT:
            screenX--;
            break;
            
        case OF_KEY_RIGHT:
            screenX++;
            break;
            
        case OF_KEY_UP:
            screenY--;
            break;
            
        case OF_KEY_DOWN:
            screenY++;
            break;
            
        case 'y':
            if(calibration){
                updateCalibrationValues();
            }
            break;
            
        case 's':
            timeline.saveTracksToFolder("timeline/" + ofToString(ofGetTimestampString()));
            break;
            
        case'l':
            timeline.loadTracksFromFolder("timeline");
            break;
            
        default:
            break;
    }
    /*
     if( key == 'f') {
     ofToggleFullscreen();
     }
     
     
     if( key == 'c') {
     calibration = !calibration;
     }
     
     if( key == 'd') {
     drawHeatmap = !drawHeatmap;
     }
     
     if( key == 'l') {
     //drawLogos = !drawLogos;
     }
     
     if( key == '1') {
     
     screenWidth++;
     }
     
     else if(key  == '2') {
     
     screenWidth--;
     }
     
     else  if(key == '3') {
     
     screenHeight++;
     }
     
     else if(key  == '4') {
     
     screenHeight--;
     }
     
     else if(key == OF_KEY_RIGHT) {
     
     screenX++;
     
     }
     
     else  if(key == OF_KEY_LEFT) {
     
     screenX--;
     }
     
     else if(key == OF_KEY_DOWN) {
     
     screenY++;
     }
     
     else  if(key == OF_KEY_UP) {
     
     screenY--;
     }
     else if(key =='y' && calibration){
     
     updateCalibrationValues();
     }
     
     
     if(key =='g'){
     timeline.saveTracksToFolder("timeline/" + ofToString(ofGetTimestampString()));
     }
     
     if(key =='h'){
     timeline.loadTracksFromFolder("timeline");
     }
     */
    
}
void testApp::updateCalibrationValues(){
    
    cout<<"New Calibration values: "<<screenWidth<<" , "<<screenHeight<<endl;
    
    stringstream ss;
    
    ss<<screenWidth<<","<<screenHeight<<","<<screenX<<","<<screenY<<endl;
    ofBuffer buffer;
    buffer.set(ss);
    ofBufferToFile("screenCalibration.txt", buffer);
    
    screen.allocate(screenWidth , screenHeight,GL_RGB );
    
    screen.begin();
    ofClear(255);
    screen.end();
    
    timeline.reset();
    
    timeline.setup();
    timeline.addBangs("bangs");
    timeline.addCurves("scale", ofRange(imgWidth, screenHeight + imgHeight));
    timeline.addCurves("theta", ofRange(0, 720));
    timeline.addCurves("numbersMoveX", ofRange(0, screenWidth));
    timeline.addCurves("linesMoveX", ofRange(-screenWidth, screenWidth));
    
    timeline.addSwitches("switches");
    timeline.addFlags("flags");
    timeline.addLFO("circleX", ofRange(0, screenWidth) );
    timeline.addLFO("circleY", ofRange(0, screenHeight) );
    timeline.addLFO("circleSize", ofRange(10, 50));
    timeline.addLFO("lineWidth", ofRange(1, screenWidth/2));
    timeline.addColors("colors");
    
    ofAddListener(timeline.events().bangFired, this, &testApp::bang);
    
    timeline.setLoopType(OF_LOOP_NORMAL);
    timeline.setDurationInSeconds(1800);
    timeline.enableEvents();
}
//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    
}
