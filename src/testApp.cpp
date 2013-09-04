#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);
    
    
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
    
    
    
    
    screen.allocate( screenWidth, screenHeight, GL_RGB);
    imgWidth = screen.getWidth()/2;
    imgHeight = screen.getHeight()/2;
    
    bp_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    chevron_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    exxon_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    shell_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    
    bp.loadImage("images/BP_crop.jpg");
    chevron.loadImage("images/chevron_crop.jpg");
    exxon.loadImage("images/exxon.jpg");
    shell.loadImage("images/shell-logo.jpg");
    
    heatMap_chevron.loadImage("images/Chevron_Yahoo.csv.tif");
    
    h_chevron_fbo.allocate(imgWidth, imgHeight, GL_RGB);
    
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
    timeline.setDurationInSeconds(1800);
    timeline.enableEvents();
    size = 20;
    text = "";
    
    //    drawTimeline = false;
    //    drawLogos = false;
    //    drawPlotter = false;
    //    drawNumbers = false;
    //    drawSpotLight = false;
    //    drawLinesHorizontal = false;
    //    drawLinesVertical = false;
    //    drawLinesCenterVertical = false;
    //    logoQuadrant, drawBP, drawExxon, drawShell, drawChevron = false;
    
    lineDeltaX, lineDeltaY = 0;
    
    //generate heatmap Chevron
    for(int i =0; i<heatMap_chevron.getWidth(); i++){
        for(int j = 0; j<heatMap_chevron.getHeight(); j++){
            ofColor c= heatMap_chevron.getColor(i, j);
            float alpha = c.getBrightness();
            
            
            float f = ofMap(alpha, 0, 255, ofFloatColor::red.getHue(),ofFloatColor::green.getHue(),true)  ;
            
            ofFloatColor floatColor = ofFloatColor::fromHsb(f, 200, 200);
            heatMap_chevron.setColor(i, j, floatColor);
            
        }
    }
    heatMap_chevron.update();
    
}

//--------------------------------------------------------------
void testApp::bang(ofxTLBangEventArgs& args){
    
    cout << args.flag << endl;
    
    if(args.flag == "plotter")
    {
        drawLogos = false;
        drawNumbers = false;
        drawPlotter = true;
        drawSpotLight = false;
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
        drawLogos = false;
        drawNumbers = true;
        drawPlotter = false;
        drawSpotLight = false;
    }
    else if(args.flag == "logos")
    {
        drawLogos = true;
        drawNumbers = false;
        drawPlotter = false;
        drawSpotLight = false;
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
        drawHeatmap = true;
    }
    
    else if(args.flag == "spotlight")
    {
        drawLogos = false;
        drawNumbers = false;
        drawPlotter = false;
        drawSpotLight = true;
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
    int size;
    float theta;
    size = timeline.getValue("scale");
    theta = timeline.getValue("theta");
    
    //    if(timeline.isSwitchOn("switches")) {
    //        //ofRect(position, size, size);
    //    } else {
    //        //ofCircle(position, size);
    //    }
    
    ofSetRectMode(OF_RECTMODE_CENTER);
    
    bp_fbo.begin();
    ofBackground(0);
    ofPushMatrix();
    ofTranslate(bp_fbo.getWidth()/2, bp_fbo.getHeight()/2);
    ofRotate(theta);
    //bp_tex.draw(0, 0, size, size);
    bp.draw(0, 0, size, size);
    ofPopMatrix();
    bp_fbo.end();
    
    chevron_fbo.begin();
    ofBackground(0);
    ofPushMatrix();
    ofTranslate(chevron_fbo.getWidth()/2, chevron_fbo.getHeight()/2);
    ofRotate(theta);
    //chevron_tex.draw(0, 0, size, size);
    chevron.draw(0, 0, size, size);
    ofPopMatrix();
    chevron_fbo.end();
    
    exxon_fbo.begin();
    ofBackground(0);
    ofPushMatrix();
    ofTranslate(exxon_fbo.getWidth()/2, exxon_fbo.getHeight()/2);
    ofRotate(theta);
    //chevron_tex.draw(0, 0, size, size);
    exxon.draw(0, 0, size, size);
    ofPopMatrix();
    exxon_fbo.end();
    
    shell_fbo.begin();
    ofBackground(0);
    ofPushMatrix();
    ofTranslate(shell_fbo.getWidth()/2, shell_fbo.getHeight()/2);
    ofRotate(theta);
    //chevron_tex.draw(0, 0, size, size);
    shell.draw(0, 0, size, size);
    ofPopMatrix();
    shell_fbo.end();
    
    h_chevron_fbo.begin();
    ofBackground(0);
    ofPushMatrix();
    ofTranslate(h_chevron_fbo.getWidth()/2, h_chevron_fbo.getHeight()/2);
    ofRotate(theta);
    heatMap_chevron.draw(0, 0, size, size);
    ofPopMatrix();
    h_chevron_fbo.end();
    
    
    if(drawLogos) {
        
        if(logoMode == QUAD){
            cout<< "IM A QUAD!!!"<<endl;
        }
        switch(logoMode){
                
            case BP:
                screen.begin();
                ofBackground(0);
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
                ofBackground(0);
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
                ofBackground(0);
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
                ofBackground(0);
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
                ofBackground(0);
                ofPushMatrix();
                ofSetRectMode(OF_RECTMODE_CENTER);
                ofTranslate(screen.getWidth()/2, screen.getHeight()/2);
                ofRotate(theta);
                exxon.draw(0, 0, size, size);
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
    
    if(drawHeatmap) {
        
        cout << "drawing Heatmap!!" << endl;
        
        screen.begin();
        ofBackground(0);
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofPushMatrix();
        ofTranslate(0, 0);
        h_chevron_fbo.draw(0, 0);
        ofPopMatrix();
        screen.end();
        
        ofPushMatrix();
        ofTranslate(screenX, screenY);
        screen.draw(0, 0);
        ofPopMatrix();
        
    }
    
    if (drawNumbers) {
        screen.begin();
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofBackground(0);
        ofSetColor(255);
        int moveX = timeline.getValue("numbersMoveX");
        unsigned long long currentMillis = ofGetElapsedTimeMillis();
        if( currentMillis - previousMillis > 10) {
            previousMillis = currentMillis;
            for( int i = 0; i < screen.getWidth(); i+= 147) {
                for( int j = 0; j < screen.getHeight(); j += 20) {
                    if ((ofGetElapsedTimeMillis() % 1000)) {
                        //                     ofDrawBitmapString("$" + ofToString((int)ofRandom(10000, 99999) + "." + ofToString((int)ofRandom(10, 99))), 0, 0);
                        numbersFont.drawString("$" + ofToString(ofRandom(10000, 999999), 2), i , j + moveX);
                    }
                }
            }
        }
        
        screen.end();
        
        ofPushMatrix();
        ofTranslate(screenX, screenY);
        screen.draw(0, 0);
        ofPopMatrix();
    }
    
    
    if(drawSpotLight) {
        ofSetRectMode(OF_RECTMODE_CORNER);
        int circleX = timeline.getValue("circleX");
        int circleY = timeline.getValue("circleY");
        int circleSize = timeline.getValue("circleSize");
        screen.begin();
        ofBackground(0);
        ofSetColor(255);
        //ofCircle(mouseX, mouseY, 20, 20);
        ofCircle(circleX, circleY, circleSize);
        screen.end();
        
        ofPushMatrix();
        ofTranslate(screenX, screenY);
        screen.draw(0, 0);
        ofPopMatrix();
    }
    
    if(drawPlotter) {
        screen.begin();
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofBackground(0);
        ofSetColor(255);
        ofSetLineWidth(0.01);
        lineDeltaX = timeline.getValue("linesMoveX");
        lineDeltaY = timeline.getValue("linesMoveY");
        
        switch (plotterMode) {
                
            case GRID:
                for(int i = 0; i < screenWidth; i+=20) {
                    ofLine((i + lineDeltaX) % screenWidth, 0, (i + lineDeltaX) % screenWidth, screenHeight);
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
                
                
                //            default:
                //                break;
        }
        //        if(drawLinesVertical) {
        //            for(int i = 0; i < screenWidth; i+=20) {
        //                ofLine((i + lineDelta) % screenWidth, 0, (i + lineDelta) % screenWidth, screenHeight);
        //
        //            }
        //        }
        //
        //        if(drawLinesHorizontal) {
        //            for(int i = 0; i < screenHeight; i+=20) {
        //                ofLine(0, (i + lineDelta) % screenHeight, screenWidth, (i + lineDelta) % screenHeight);
        //
        //            }
        //        }
        //
        //        if(drawLinesCenterVertical) {
        //            for(int i = screenWidth/2; i < screenWidth; i+=20) {
        //                ofLine((i + lineDelta) % screenWidth/2, 0, (i + lineDelta) % screenWidth/2, screenHeight);
        //            }
        //
        //        }
        //
        //        if(drawSingleLineH) {
        //            int lineWidth = timeline.getValue("lineWidth");
        //            ofPushStyle();
        //            ofSetRectMode(OF_RECTMODE_CENTER);
        //            ofSetColor(255);
        //            ofFill();
        //            ofRect(lineDelta,screen.getHeight()/2, lineWidth, screenHeight);
        //            ofPopStyle();
        //        }
        screen.end();
        
        
        ofPushMatrix();
        ofTranslate(screenX, screenY);
        screen.draw(0, 0);
        ofPopMatrix();
        
        
        
    }
    
    
    
    if(drawTimeline) {
        ofSetRectMode(OF_RECTMODE_CORNER);
        timeline.draw();
    }
    
    if(calibration) {
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofBackground(255, 0, 0);
        ofSetColor(255);
        
        // ofRect(screenX + screenXdelta, screenY + screenYdelta, screenWidth + screenWidthDelta, screenHeight + screenHeightDelta);
        ofRect(screenX, screenY, screenWidth, screenHeight);
        ofSetColor(0);
        ofDrawBitmapString("screenWidth: " + ofToString(screenWidth), 300, 300);
        ofDrawBitmapString("screenHeight: " + ofToString(screenHeight), 300, 350);
        ofDrawBitmapString("screenX: " + ofToString(screenX), 300, 250);
        ofDrawBitmapString("screenY: " + ofToString(screenY), 300, 200);
        
        //        cout << screenX
        
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if( key == 't') {
        drawTimeline = !drawTimeline;
    }
    
    if( key == 'f') {
        ofToggleFullscreen();
    }
    
    
    if( key == 'c') {
        calibration = !calibration;
    }
    
    if( key == 'd') {
        drawHeatmap = !drawHeatmap;
    }
    /*
     if( key == 'l') {
     drawLogos = !drawLogos;
     }
     
     if( key == 'q') {
     logoQuadrant = !logoQuadrant;
     }
     
     if( key == 'w') {
     drawBP = !drawBP;
     }
     
     if( key == 'e') {
     drawExxon = !drawExxon;
     }
     
     if( key == 'r') {
     drawShell = !drawShell;
     }
     
     if( key == 'a') {
     drawChevron = !drawChevron;
     }
     
     if( key == 's') {
     drawSpotLight = !drawSpotLight;
     }
     
     if( key == 'n') {
     drawNumbers = !drawNumbers;
     }
     
     if( key == 'p') {
     drawPlotter = !drawPlotter;
     }
     
     if( key == 'h') {
     drawLinesHorizontal = !drawLinesHorizontal;
     }
     
     if( key == 'v') {
     drawLinesVertical = !drawLinesVertical;
     }
     
     if( key == 'b') {
     drawLinesCenterVertical = !drawLinesCenterVertical;
     }
     
     if( key == 'j') {
     
     }
     
     if( key == 'i') {
     drawSingleLineH = !drawSingleLineH;
     }
     
     if(key == '[') {
     linesMoveLeft = !linesMoveLeft;
     linesMoveRight = false;
     }
     
     if(key == ']') {
     linesMoveRight = !linesMoveRight;
     linesMoveLeft = false;
     }
     */
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
        //timeline.addCurves("linesMoveY", ofRange(-screenH))
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
        timeline.setDurationInSeconds(1800);
        timeline.enableEvents();
        
    }
    
    
    if(key =='g'){
        timeline.saveTracksToFolder("timeline/" + ofToString(ofGetTimestampString()));
    }
    
    if(key =='h'){
        timeline.loadTracksFromFolder("timeline");
    }
    
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
