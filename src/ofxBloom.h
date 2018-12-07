#pragma once
#include "ofMain.h"
#include "ofxBlur.h"

class ofxBloom {
public:
	ofxBloom(int width, int height, ofFbo* src_fbo)
		: width_(width)
		, height_(height)
		, target_fbo_(src_fbo)
		, scale_(32)
		, shape_(0.2f)
		, passes_(4)
		, downsample_(0.5f)
		, thresh_(0.1f)
		, brightness_(0.2f)
	{	
        ofDisableArbTex();
		result_fbo_.allocate(width, height, GL_RGBA32F);
		result_fbo_.begin(); ofClear(0, 0); result_fbo_.end();
        ofEnableArbTex();
        
		blur_.setup(width, height, 10, .2, 4);
        
        if (ofIsGLProgrammableRenderer()){
			std::string threshold_fragment_source = generateThreasholdFrag();
            std::string threshold_vertex_source = generateThreasholdVert();
            
			threshold_shader_.setupShaderFromSource(GL_VERTEX_SHADER, threshold_vertex_source);
			threshold_shader_.setupShaderFromSource(GL_FRAGMENT_SHADER, threshold_fragment_source);
			threshold_shader_.bindDefaults();
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
		else {
			std::stringstream src;

			src << "#version 120\n";
			src << "uniform sampler2D src;\n";
			src << "uniform float thresh;\n";
			src << "void main() {;\n";
			src << "\tvec2 st = gl_TexCoord[0].st;\n";
			src << "\tgl_FragColor = max(texture2D(src, st) - vec4(vec3(thresh), 0.0), vec4(0.0));\n";
			src << "}\n";

			std::string threshold_fragment_source = src.str();
			threshold_shader_.setupShaderFromSource(GL_FRAGMENT_SHADER, threshold_fragment_source);
			threshold_shader_.linkProgram();

			quad_.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
			quad_.addVertex(ofVec3f(width_, 0.f, 0.f));
			quad_.addTexCoord(ofVec2f(1.f, 1.f));
			quad_.addVertex(ofVec3f(width_, height_, 0.f));
			quad_.addTexCoord(ofVec2f(1.f, 0.f));
			quad_.addVertex(ofVec3f(0, height_, 0.0));
			quad_.addTexCoord(ofVec2f(0.f, 0.f));
			quad_.addVertex(ofVec3f(0.f, 0.f, 0.f));
			quad_.addTexCoord(ofVec2f(0.f, 1.f));
		}
        

        
    }
    
    void setBrightness(const float brightness) { blur_.setBrightness(brightness); }
    void setScale(const float scale) { blur_.setScale(scale); }
    void setThreshold(const float thresh) { thresh_ = thresh; }

	ofFbo& getResultFbo() { return result_fbo_; }
	void draw() const { result_fbo_.draw(0, 0); }
	void draw(int x, int y) const { result_fbo_.draw(x, y); }
	void draw(int x, int y, int w, int h) const { result_fbo_.draw(x, y, w, h); }

	void process() {
		ofPushStyle();
		ofDisableDepthTest();

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
		ofPopStyle();
	}

private:
	std::string generateThreasholdVert() {
		std::stringstream src;

		src << "#version 410\n";
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
	int width_;
	int height_;
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


