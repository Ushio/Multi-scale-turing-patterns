#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "peseudo_random.hpp"

struct GridCell {
	float activator = 0.0f;
	float inhibitor = 0.0f;
	float variation = 0.0f;
};
static const int kWidth = 512;
static const int kHeight = 512;
static const int kLayerCount = 5;

struct LayerParams {
	float activatorRadius = 0.0f;
	float inhibitorRadius = 0.0f;
	float smallAmount = 0.0f;
};

class ofApp : public ofBaseApp {
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
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	LayerParams _layerParams[kLayerCount];
	float _values[kWidth][kHeight];
	GridCell _cells[kWidth][kHeight][kLayerCount];
	ofxCvFloatImage _activatorMeans[kLayerCount];
	ofxCvFloatImage _inhibitorMeans[kLayerCount];
	ofImage _image;
};
