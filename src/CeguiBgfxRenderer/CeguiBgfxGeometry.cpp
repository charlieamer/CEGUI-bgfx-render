#include "CeguiBgfxRenderer/CeguiBgfxGeometry.h"
#include "CeguiBgfxRenderer/CeguiBgfxRenderer.h"

#include <bx/math.h>
#include <iostream>
#include <algorithm>
using namespace std;


namespace CEGUI
{

	void CeguiBgfxGeometry::syncHardwareBuffer()
	{
		size_t sizeToInsert = 0;
		size_t locationOfIsert = 0;
		const size_t vertex_count = d_vertices.size();

		if (d_vertices.size() > 0) {
			// transform each vertex on CPU. Not the best idea, but it is easiest way to
			// embed bgfx shader to library without the need of external shader compiler
			auto mem = bgfx::alloc(sizeof(CeguiBgfxVertex) * d_vertices.size());
			CeguiBgfxVertex* transformedVertices = (CeguiBgfxVertex*)(void*)mem->data;
			float tmpDest[4], tmpOrig[4] = {0, 0, 0, 1};
			transform(d_vertices.begin(), d_vertices.end(), transformedVertices, [&tmpDest, &tmpOrig, this](CeguiBgfxVertex input)-> CeguiBgfxVertex {
				CeguiBgfxVertex ret = input;
				tmpOrig[0] = ret.x;
				tmpOrig[1] = ret.y;
				bx::vec4MulMtx(tmpDest, tmpOrig, matrix);
				ret.x = tmpDest[0];
				ret.y = tmpDest[1];
				return ret;
			});

			bgfx::update(vertexHandle, 0, mem);
		}
		
		if (d_vertices.size() > 0) {
			updateIndexBuffer();
		} else {
			destroyIndexBuffer();
		}

		d_bufferSynched = true;
	}

	void CeguiBgfxGeometry::destroyIndexBuffer() {
		if (bgfx::isValid(indexHandle)) {
			bgfx::destroy(indexHandle);
			indexHandle = BGFX_INVALID_HANDLE;
		}
	}

	void CeguiBgfxGeometry::updateIndexBuffer() {
		const bgfx::Memory* indices = bgfx::alloc(sizeof(uint16_t) * d_vertices.size());
		for (uint16_t i=0; i<d_vertices.size(); i++) {
			((uint16_t*)(void*)indices->data)[i] = i;
		}
		if (bgfx::isValid(indexHandle)) {
			bgfx::update(indexHandle, 0, indices);
		} else {
			indexHandle = bgfx::createDynamicIndexBuffer(indices, BGFX_BUFFER_ALLOW_RESIZE);
		}
	}

	CeguiBgfxGeometry::CeguiBgfxGeometry(CeguiBgfxRenderer & renderer) : owner(renderer)
	{
		d_pivot = Vector3f(0, 0, 0);
		d_translation = Vector3f(0, 0, 0);
		d_effect = NULL;
		d_activeTexture = NULL;
		d_matrixValid = false;

		vertexLayout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true, false)
			.end();

		vertexHandle = bgfx::createDynamicVertexBuffer(0u, vertexLayout, BGFX_BUFFER_ALLOW_RESIZE);
		indexHandle = BGFX_INVALID_HANDLE;
	}

	CeguiBgfxGeometry::~CeguiBgfxGeometry()
	{
		bgfx::destroy(vertexHandle);
		destroyIndexBuffer();
	}

	void CeguiBgfxGeometry::updateMatrix()
	{
		bx::mtxIdentity(matrix);
		Vector3f pivoted = d_pivot + d_translation;
		bx::mtxTranslate(matrix, pivoted.d_x, pivoted.d_y, pivoted.d_z);

		float tmp1[16], tmp2[16];
		bx::mtxQuat(tmp1, bx::Quaternion(d_rotation.d_x, d_rotation.d_y, d_rotation.d_y, d_rotation.d_w));
		bx::mtxMul(tmp2, matrix, tmp1);
		memcpy(matrix, tmp2, sizeof(float) * 16);
		
		bx::mtxIdentity(tmp1);
		bx::mtxTranslate(tmp1, -d_pivot.d_x, -d_pivot.d_y, -d_pivot.d_z);
		bx::mtxMul(tmp2, matrix, tmp1);
		memcpy(matrix, tmp2, sizeof(float) * 16);

		d_matrixValid = true;
		d_bufferSynched = false;
	}

	void CeguiBgfxGeometry::draw() const
	{
		if (!d_matrixValid)
			(const_cast<CeguiBgfxGeometry*>(this))->updateMatrix();

		if (!d_bufferSynched)
			(const_cast<CeguiBgfxGeometry*>(this))->syncHardwareBuffer();

		if (!bgfx::isValid(indexHandle)) {
			return;
		}

		const int pass_count = d_effect ? d_effect->getPassCount() : 1;
		for (int pass = 0; pass < pass_count; ++pass) {

			if (d_effect)
				d_effect->performPreRenderFunctions(pass);
			//Run the batches
			size_t pos = 0;
			for (auto& batch : d_batches)
			{
				bgfx::setIndexBuffer(indexHandle);
				bgfx::setVertexBuffer(0, vertexHandle, pos, batch.vertexCount);

				const CeguiBgfxTexture* currentBatchTexture = batch.texture;
				if (currentBatchTexture) {
					bgfx::setTexture(0, uniform, currentBatchTexture->getHandle());
				}
				else {
					bgfx::setTexture(0, uniform, BGFX_INVALID_HANDLE);
				}
				// bgfx::setTexture(0, uniform, owner.getDebugTexture()->getHandle());
				uint64_t blendState = 0;
				switch (batch.blendMode) {
				case CEGUI::BlendMode::BM_NORMAL:
					blendState = BGFX_STATE_BLEND_FUNC_SEPARATE(
						BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA,
						BGFX_STATE_BLEND_INV_DST_ALPHA, BGFX_STATE_BLEND_ONE
					);
					break;
				case CEGUI::BlendMode::BM_RTT_PREMULTIPLIED:
					blendState = BGFX_STATE_BLEND_FUNC(
						BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA
					);
					break;
				}
				uint64_t screenWritingState = 0;
				if (owner.isViewportTheActiveTarget()) {
					screenWritingState = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;
				}
				bgfx::setState(screenWritingState | blendState);
				bgfx::submit(owner.getActiveViewID(), program);
				pos += batch.vertexCount;
			}
		}
	}

	void CeguiBgfxGeometry::setTranslation(const Vector3f & v)
	{
		d_translation = v;
		d_matrixValid = false;
	}

	void CeguiBgfxGeometry::setRotation(const Quaternion & r)
	{
		d_rotation = r;
		d_matrixValid = false;
	}

	void CeguiBgfxGeometry::setPivot(const Vector3f & p)
	{
		d_pivot = p;
		d_matrixValid = false;
	}

	void CeguiBgfxGeometry::setClippingRegion(const Rectf & region)
	{
		d_clipRect.top(ceguimax(0.0f, region.top()));
		d_clipRect.bottom(ceguimax(0.0f, region.bottom()));
		d_clipRect.left(ceguimax(0.0f, region.left()));
		d_clipRect.right(ceguimax(0.0f, region.right()));
	}

	void CeguiBgfxGeometry::appendVertex(const Vertex & vertex)
	{
		appendGeometry(&vertex, 1);
	}

	void CeguiBgfxGeometry::appendGeometry(const Vertex* const vbuff, uint vertex_count)
	{
		BatchInfo batchFromCurrentState = { d_activeTexture, 0, d_clippingActive, d_blendMode };

		if (d_batches.empty() || d_batches.back() != batchFromCurrentState)
		{
			d_batches.push_back(batchFromCurrentState);
		}

		//Keep track of batch indicies so that one can feed them into 
		uint16_t vc = d_batches.back().vertexCount;
		d_batches.back().vertexCount += vertex_count;

		CeguiBgfxVertex vd;
		for (uint i = 0; i < vertex_count; ++i)
		{
			//Convert from vertex to bgfx format
			vd.x = vbuff[i].position.d_x;
			vd.y = vbuff[i].position.d_y;
			vd.z = vbuff[i].position.d_z;
			vd.u = vbuff[i].tex_coords.d_x;
			vd.v = vbuff[i].tex_coords.d_y;
			vd.a = vbuff[i].colour_val.getAlpha() * 255.0f;
			vd.b = vbuff[i].colour_val.getBlue() * 255.0f;
			vd.g = vbuff[i].colour_val.getGreen() * 255.0f;
			vd.r = vbuff[i].colour_val.getRed() * 255.0f;
			d_vertices.push_back(vd);
		}

		d_bufferSynched = false;
	}


	void CeguiBgfxGeometry::setActiveTexture(Texture * texture)
	{
		this->d_activeTexture = static_cast<CeguiBgfxTexture*>(texture);
	}

	void CeguiBgfxGeometry::reset()
	{
		d_batches.clear();
		d_vertices.clear();
		d_activeTexture = nullptr;
		d_bufferSynched = false;
	}

	Texture * CeguiBgfxGeometry::getActiveTexture() const
	{
		return d_activeTexture;
	}

	uint CeguiBgfxGeometry::getVertexCount() const
	{
		return d_vertices.size();
	}

	uint CeguiBgfxGeometry::getBatchCount() const
	{
		return d_batches.size();
	}

	void CeguiBgfxGeometry::setRenderEffect(RenderEffect * effect)
	{
		this->d_effect = effect;
	}

	RenderEffect * CeguiBgfxGeometry::getRenderEffect()
	{
		return d_effect;
	}

	void CeguiBgfxGeometry::setClippingActive(const bool active)
	{
		d_clippingActive = active;
	}

	bool CeguiBgfxGeometry::isClippingActive() const
	{
		return d_clippingActive;
	}

	void CeguiBgfxGeometry::setProgramHandle(bgfx::ProgramHandle programHandle, bgfx::UniformHandle uniformHandle)
	{
		program = programHandle;
		uniform = uniformHandle;
	}
	const float * CeguiBgfxGeometry::getMatrix() const
	{
		return matrix;
	}
}