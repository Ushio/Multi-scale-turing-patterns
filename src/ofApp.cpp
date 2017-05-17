#include "ofApp.h"


static Xor random;
//--------------------------------------------------------------
void ofApp::setup() {
	float s = 5.0f;
	_layerParams[0].activatorRadius = 100.0f;
	_layerParams[0].inhibitorRadius = 200.0f;
	_layerParams[0].smallAmount = 0.05f * s;

	_layerParams[1].activatorRadius = 20.0f;
	_layerParams[1].inhibitorRadius = 40.0f;
	_layerParams[1].smallAmount = 0.04f * s;

	_layerParams[2].activatorRadius = 10.0f;
	_layerParams[2].inhibitorRadius = 20.0f;
	_layerParams[2].smallAmount = 0.03f * s;

	_layerParams[3].activatorRadius = 5.0f;
	_layerParams[3].inhibitorRadius = 10.0f;
	_layerParams[3].smallAmount = 0.02f * s;

	_layerParams[4].activatorRadius = 1.0f;
	_layerParams[4].inhibitorRadius = 2.0f;
	_layerParams[4].smallAmount = 0.01f * s;

	for (int y = 0; y < kHeight; ++y) {
		for (int x = 0; x < kWidth; ++x) {
			_values[x][y] = random.uniform(-1.0, 1.0);
		}
	}

	_image.allocate(kWidth, kHeight, OF_IMAGE_GRAYSCALE);

	for (int i = 0; i < kLayerCount; ++i) {
		_activatorMeans[i].allocate(kWidth, kHeight);
		_inhibitorMeans[i].allocate(kWidth, kHeight);
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	#pragma omp parallel for
	for (int layer = 0; layer < kLayerCount; ++layer) {
		float *p_activator = _activatorMeans[layer].getFloatPixelsRef().getPixels();
		float *p_inhibitor = _inhibitorMeans[layer].getFloatPixelsRef().getPixels();
		for (int y = 0; y < kHeight; ++y) {
			for (int x = 0; x < kWidth; ++x) {
				p_activator[y * kWidth + x] = _values[x][y];
				p_inhibitor[y * kWidth + x] = _values[x][y];
			}
		}
		int activatorBlurRadius = _layerParams[layer].activatorRadius;
		int inhibitorBlurRadius = _layerParams[layer].inhibitorRadius;
		_activatorMeans[layer].blurGaussian(activatorBlurRadius & 0x1 ? activatorBlurRadius : activatorBlurRadius + 1);
		_inhibitorMeans[layer].blurGaussian(inhibitorBlurRadius & 0x1 ? inhibitorBlurRadius : inhibitorBlurRadius + 1);
	}

	for (int layer = 0; layer < kLayerCount; ++layer) {
		float *p_activator = _activatorMeans[layer].getFloatPixelsRef().getPixels();
		float *p_inhibitor = _inhibitorMeans[layer].getFloatPixelsRef().getPixels();

		for (int y = 0; y < kHeight; ++y) {
			for (int x = 0; x < kWidth; ++x) {
				_cells[x][y][layer].activator = p_activator[y * kWidth + x];
				_cells[x][y][layer].inhibitor = p_inhibitor[y * kWidth + x];
				_cells[x][y][layer].variation = abs(_cells[x][y][layer].activator - _cells[x][y][layer].inhibitor);
			}
		}
	}

	//for (int layer = 0; layer < kLayerCount; ++layer) {
	//	#pragma omp parallel for schedule (dynamic, 10)
	//	for (int y = 0; y < kHeight; ++y) {
	//		for (int x = 0; x < kWidth; ++x) {
	//			// 

	//			float radius_activator = _layerParams[layer].activatorRadius;
	//			float radiusSq_activator = radius_activator * radius_activator;
	//			int move_activator = (int)ceil(radiusSq_activator);

	//			float radius_inhibitor = _layerParams[layer].inhibitorRadius;
	//			float radiusSq_inhibitor = radius_inhibitor * radius_inhibitor;
	//			int move_inhibitor = (int)ceil(radiusSq_inhibitor);

	//			float sum_of_activator = 0.0f;
	//			int count_of_activator = 0;
	//			float sum_of_inhibitor = 0.0f;
	//			int count_of_inhibitor = 0;

	//			for (int xx = x - move_activator; xx <= x + move_activator; ++xx) {
	//				if (xx < 0 || kWidth <= xx) {
	//					continue;
	//				}
	//				for (int yy = y - move_activator; yy <= y + move_activator; ++yy) {
	//					if (yy < 0 || kHeight <= yy) {
	//						continue;
	//					}

	//					int dx = xx - x;
	//					int dy = yy - y;
	//					int distanceSq = dx * dx + dy * dy;
	//					if (radiusSq_activator < distanceSq) {
	//						continue;
	//					}

	//					count_of_activator++;
	//					sum_of_activator += _values[xx][yy];
	//				}
	//			}
	//			for (int xx = x - move_inhibitor; xx <= x + move_inhibitor; ++xx) {
	//				if (xx < 0 || kWidth <= xx) {
	//					continue;
	//				}
	//				for (int yy = y - move_inhibitor; yy <= y + move_inhibitor; ++yy) {
	//					if (yy < 0 || kHeight <= yy) {
	//						continue;
	//					}

	//					int dx = xx - x;
	//					int dy = yy - y;
	//					int distanceSq = dx * dx + dy * dy;
	//					if (radiusSq_inhibitor < distanceSq) {
	//						continue;
	//					}

	//					count_of_inhibitor++;
	//					sum_of_inhibitor += _values[xx][yy];
	//				}
	//			}

	//			_cells[x][y][layer].activator = sum_of_activator / count_of_activator;
	//			_cells[x][y][layer].inhibitor = sum_of_inhibitor / count_of_inhibitor;
	//			_cells[x][y][layer].variation = abs(_cells[x][y][layer].activator - _cells[x][y][layer].inhibitor);
	//		}
	//	}
	//}

	for (int y = 0; y < kHeight; ++y) {
		for (int x = 0; x < kWidth; ++x) {
			int min_variation_index = -1;
			float min_variation = std::numeric_limits<float>::max();
			for (int layer = 0; layer < kLayerCount; ++layer) {
				if (_cells[x][y][layer].variation < min_variation) {
					min_variation_index = layer;
					min_variation = _cells[x][y][layer].variation;
				}
			}
			if (_cells[x][y][min_variation_index].activator > _cells[x][y][min_variation_index].inhibitor) {
				_values[x][y] += _layerParams[min_variation_index].smallAmount;
			}
			else {
				_values[x][y] -= _layerParams[min_variation_index].smallAmount;
			}
		}
	}

	float min_value = std::numeric_limits<float>::max();
	float max_value = -std::numeric_limits<float>::max();
	for (int y = 0; y < kHeight; ++y) {
		for (int x = 0; x < kWidth; ++x) {
			min_value = std::min(min_value, _values[x][y]);
			max_value = std::max(max_value, _values[x][y]);
		}
	}
	float range = max_value - min_value;
	float inverse_range = 1.0f / range;
	for (int y = 0; y < kHeight; ++y) {
		for (int x = 0; x < kWidth; ++x) {
			_values[x][y] = (_values[x][y] - min_value) * inverse_range * 2.0f - 1.0f;
		}
	}
	// 
	uint8_t *dst = _image.getPixels().getPixels();
	for (int y = 0; y < kHeight; ++y) {
		for (int x = 0; x < kWidth; ++x) {
			float value = (_values[x][y] + 0.5f) * 255.9f;
			int value_i = (int)value;
			value_i = max(value_i, 0);
			value_i = min(value_i, 255);
			dst[y * kWidth + x] = value_i;
		}
	}
	_image.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
	_image.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'r') {
		
		for (int y = 0; y < kHeight; ++y) {
			for (int x = 0; x < kWidth; ++x) {
				_values[x][y] = random.uniform(-1.0, 1.0);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
