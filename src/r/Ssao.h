#ifndef SSAO_H
#define SSAO_H

#include "RenderingPass.h"
#include "gl/Fbo.h"
#include "rsrc/Texture.h"
#include "rsrc/ShaderProgram.h"
#include "rsrc/RsrcPtr.h"
#include "gl/Vbo.h"
#include "gl/Vao.h"


/// Screen space ambient occlusion pass
///
/// Three passes:
/// 1) Calc ssao factor
/// 2) Blur vertically
/// 3) Blur horizontally repeat 2, 3
class Ssao: public SwitchableRenderingPass
{
	public:
		Ssao(Renderer& r_)
		:	SwitchableRenderingPass(r_)
		{}

		void init(const RendererInitializer& initializer);
		void run();

		/// @name Accessors
		/// @{
		float getRenderingQuality() const
		{
			return renderingQuality;
		}

		const Texture& getFai() const
		{
			return fai;
		}
		/// @}

	private:
		Texture ssaoFai; ///< It contains the unblurred SSAO factor
		Texture hblurFai;
		Texture fai;  ///< AKA vblurFai The final FAI
		float renderingQuality;
		float blurringIterationsNum;
		Fbo ssaoFbo;
		Fbo hblurFbo;
		Fbo vblurFbo;
		RsrcPtr<Texture> noiseMap;
		RsrcPtr<ShaderProgram> ssaoSProg;
		RsrcPtr<ShaderProgram> hblurSProg;
		RsrcPtr<ShaderProgram> vblurSProg;

		void createFbo(Fbo& fbo, Texture& fai);
};


#endif