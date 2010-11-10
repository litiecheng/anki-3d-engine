#include <boost/lexical_cast.hpp>
#include "Ssao.h"
#include "Renderer.h"
#include "Camera.h"
#include "RendererInitializer.h"


//======================================================================================================================
// createFbo                                                                                                           =
//======================================================================================================================
void Ssao::createFbo(Fbo& fbo, Texture& fai)
{
	try
	{
		int width = renderingQuality * r.getWidth();
		int height = renderingQuality * r.getHeight();

		// create
		fbo.create();
		fbo.bind();

		// inform in what buffers we draw
		fbo.setNumOfColorAttachements(1);

		// create the texes
		fai.createEmpty2D(width, height, GL_RED, GL_RED, GL_FLOAT);
		fai.setRepeat(false);
		fai.setTexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		fai.setTexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// attach
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fai.getGlId(), 0);

		// test if success
		fbo.checkIfGood();

		// unbind
		fbo.unbind();
	}
	catch(std::exception& e)
	{
		throw EXCEPTION("Cannot create deferred shading post-processing stage SSAO blur FBO");
	}
}


//======================================================================================================================
// init                                                                                                                =
//======================================================================================================================
void Ssao::init(const RendererInitializer& initializer)
{
	enabled = initializer.pps.ssao.enabled;

	if(!enabled)
		return;

	renderingQuality = initializer.pps.ssao.renderingQuality;
	blurringIterations = initializer.pps.ssao.blurringIterations;

	// create FBOs
	createFbo(ssaoFbo, ssaoFai);
	createFbo(hblurFbo, hblurFai);
	createFbo(vblurFbo, fai);

	//
	// Shaders
	//

	// first pass prog
	ssaoSProg.loadRsrc("shaders/PpsSsao.glsl");

	// blurring progs
	const char* SHADER_FILENAME = "shaders/GaussianBlurGeneric.glsl";

	std::string pps = "#define HPASS\n#define COL_R\n";
	std::string prefix = "HorizontalR";
	hblurSProg.loadRsrc(ShaderProg::createSrcCodeToCache(SHADER_FILENAME, pps.c_str(), prefix.c_str()).c_str());

	pps = "#define VPASS\n#define COL_R\n";
	prefix = "VerticalR";
	vblurSProg.loadRsrc(ShaderProg::createSrcCodeToCache(SHADER_FILENAME, pps.c_str(), prefix.c_str()).c_str());

	//
	// noise map
	//

	/// @todo fix this crap
	// load noise map and disable temporally the texture compression and enable mipmapping
	/*bool texCompr = Texture::compressionEnabled;
	bool mipmaping = Texture::mipmappingEnabled;
	Texture::compressionEnabled = false;
	Texture::mipmappingEnabled = true;*/
	noiseMap.loadRsrc("gfx/noise3.tga");
	/*noiseMap->setTexParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	noiseMap->setTexParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	//noise_map->setTexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//noise_map->setTexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	Texture::compressionEnabled = texCompr;
	Texture::mipmappingEnabled = mipmaping;*/
}


//======================================================================================================================
// run                                                                                                                 =
//======================================================================================================================
void Ssao::run()
{
	int width = renderingQuality * r.getWidth();
	int height = renderingQuality * r.getHeight();
	const Camera& cam = r.getCamera();

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);


	Renderer::setViewport(0, 0, width, height);

	// 1st pass
	ssaoFbo.bind();
	ssaoSProg->bind();
	Vec2 camRange(cam.getZNear(), cam.getZFar());
	ssaoSProg->findUniVar("camerarange")->setVec2(&camRange);
	ssaoSProg->findUniVar("msDepthFai")->setTexture(r.getMs().depthFai, 0);
	ssaoSProg->findUniVar("noiseMap")->setTexture(*noiseMap, 1);
	ssaoSProg->findUniVar("msNormalFai")->setTexture(r.getMs().normalFai, 2);
	r.drawQuad();


	// blurring passes
	hblurFai.setRepeat(false);
	fai.setRepeat(false);
	for(uint i=0; i<blurringIterations; i++)
	{
		// hpass
		hblurFbo.bind();
		hblurSProg->bind();
		if(i == 0)
		{
			hblurSProg->findUniVar("img")->setTexture(ssaoFai, 0);
		}
		else
		{
			hblurSProg->findUniVar("img")->setTexture(fai, 0);
		}
		hblurSProg->findUniVar("imgDimension")->setFloat(width);
		r.drawQuad();

		// vpass
		vblurFbo.bind();
		vblurSProg->bind();
		vblurSProg->findUniVar("img")->setTexture(hblurFai, 0);
		vblurSProg->findUniVar("imgDimension")->setFloat(height);
		r.drawQuad();
	}

	// end
	Fbo::unbind();
}

