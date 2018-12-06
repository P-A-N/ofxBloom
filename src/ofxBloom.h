#pragma once
#include "ofMain.h"
#include "ofxBlur.h"

class ofxBloom {
public:
	ofxBloom(int width, int height, ofFbo* src_fbo)
		: target_fbo_(src_fbo)
		, scale_(32)
		, shape_(0.2f)
		, passes_(4)
		, downsample_(0.5f)
		, thresh_(0.1f)
		, brightness_(0.2f)
	{	
        ofDisableArbTex();
		result_fbo_.allocate(width, height, GL_RGBA16F);
		result_fbo_.begin(); ofClear(0, 0); result_fbo_.end();
        ofEnableArbTex();
        
		blur_.setup(width, height, 10, .2, 4);
        
        std::string blurthreshSource = generateThreasholdFrag();
        if(ofGetLogLevel() == OF_LOG_VERBOSE) {
            cout << "ofxBlur is loading blur shader:" << endl << blurthreshSource << endl;
        }
        if (ofIsGLProgrammableRenderer()){
            std::string ThresholdPass = generateThreasholdVert();
            if(ofGetLogLevel() == OF_LOG_VERBOSE) {
                cout << "ofxBlur is loading blur vertex shader:" << endl << ThresholdPass << endl;
            }
			threshold_shader_.setupShaderFromSource(GL_VERTEX_SHADER, ThresholdPass);
        }
		threshold_shader_.setupShaderFromSource(GL_FRAGMENT_SHADER, blurthreshSource);
        
        if (ofIsGLProgrammableRenderer()){
			threshold_shader_.bindDefaults();
        }
		threshold_shader_.linkProgram();
        
		quad_.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		quad_.addVertex(ofVec3f(1.0, 1.0, 0.0));
		quad_.addTexCoord(ofVec2f(1.0f, 1.0f));
		quad_.addVertex(ofVec3f(1.0, -1.0, 0.0));
		quad_.addTexCoord(ofVec2f(1.0f, 0.0f));
		quad_.addVertex(ofVec3f(-1.0, -1.0, 0.0));
		quad_.addTexCoord(ofVec2f(0.0f, 0.0f));
		quad_.addVertex(ofVec3f(-1.0, 1.0, 0.0));
		quad_.addTexCoord(ofVec2f(0.0f, 1.0f));
        
    }
    
    void setBrightness(const float brightness) { blur_.setBrightness(brightness); }
    void setScale(const float scale) { blur_.setScale(scale); }
    void setThreshold(const float thresh) { thresh_ = thresh; }

	float& getBrightnessRef() { return brightness_; }
	float& getScaleRef() { return scale_; }
	float& getThreshRef() { return thresh_; }
    
	ofFbo& getResultFbo() { return result_fbo_; }
	void draw() const { result_fbo_.draw(0, 0); }
	void draw(int x, int y) const { result_fbo_.draw(x, y); }
	void draw(int x, int y, int w, int h) const { result_fbo_.draw(x, y, w, h); }
    

	void process() {
		//Blur Threshold
		blur_.begin();
		ofClear(0, 255);
		threshold_shader_.begin();
		threshold_shader_.setUniformTexture("src", target_fbo_->getTexture(), 0);
		threshold_shader_.setUniform1f("thresh", thresh_);
		quad_.draw();
		threshold_shader_.end();
		blur_.end();

		//Composite
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		result_fbo_.begin();
		ofClear(0, 255);
		target_fbo_->draw(0, 0);
		blur_.draw();
		result_fbo_.end();
		ofDisableBlendMode();
	}

private:
	// needed for programmable renderer
	std::string generateThreasholdVert() {
		std::stringstream src;

		src << "#version 410\n";
		src << "uniform mat4 modelViewProjectionMatrix;\n";
		src << "in vec4 position;\n";
		src << "in vec2 texcoord;\n";
		src << "out vec2 v_texcoord;\n";
		src << "void main() {\n";
		src << "\tv_texcoord = texcoord;\n";
		src << "\tgl_Position = position;\n";
		src << "}\n";

		return src.str();
	}

	std::string generateThreasholdFrag() {
		std::stringstream src;

		src << "#version 410\n";
		src << "uniform sampler2D src;\n";
		src << "uniform float thresh;\n";
		src << "in vec2 v_texcoord;\n";
		src << "out vec4 fragcolor;\n";
		src << "void main() {;\n";
		src << "\tvec4 col = texture(src, v_texcoord).rgba;\n";
		src << "\tvec3 threshold = vec3(thresh, thresh, thresh);\n";
		src << "\tvec3 blightness = col.rgb - threshold;\n";
		src << "\tvec3 result = max(blightness, vec3(0.0));\n";
		src << "\tfragcolor = vec4(result, col.a);\n";
		src << "}\n";

		return src.str();
	}


    ofShader threshold_shader_;
	float brightness_;
    float thresh_;
    ofMesh quad_;
    ofxBlur blur_;
	ofFbo *target_fbo_;
    ofFbo result_fbo_;
	float scale_;
	float shape_;
	int passes_;
	float downsample_;
};


