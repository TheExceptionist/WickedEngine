#pragma once
#include "CommonInclude.h"
#include "wiGraphicsAPI.h"
#include "wiIntersectables.h"
#include "ShaderInterop_EmittedParticle.h"
#include "wiImageEffects.h"

struct Object;
struct Material;

class wiArchive;

class wiEmittedParticle
{
public:
	enum PARTICLESHADERTYPE
	{
		SOFT,
		SOFT_DISTORTION,
		SIMPLEST,
		PARTICLESHADERTYPE_COUNT,
	};

private:
	ParticleCounters debugData = {};
	wiGraphicsTypes::GPUBuffer* debugDataReadbackBuffer;

	wiGraphicsTypes::GPUBuffer* particleBuffer;
	wiGraphicsTypes::GPUBuffer* aliveList[2];
	wiGraphicsTypes::GPUBuffer* deadList;
	wiGraphicsTypes::GPUBuffer* distanceBuffer; // for sorting
	wiGraphicsTypes::GPUBuffer* counterBuffer;
	wiGraphicsTypes::GPUBuffer* indirectBuffers; // kickoffUpdate, simulation, draw, kickoffSort
	wiGraphicsTypes::GPUBuffer* constantBuffer;
	void CreateSelfBuffers();

	static wiGraphicsTypes::ComputeShader		*kickoffUpdateCS, *emitCS, *simulateCS, *simulateCS_SORTING, *simulateCS_DEPTHCOLLISIONS, *simulateCS_SORTING_DEPTHCOLLISIONS;
	static wiGraphicsTypes::ComputeShader		*kickoffSortCS, *sortCS, *sortInnerCS, *sortStepCS;
	static wiGraphicsTypes::GPUBuffer			*sortCB;
	static wiGraphicsTypes::VertexShader		*vertexShader;
	static wiGraphicsTypes::PixelShader			*pixelShader[PARTICLESHADERTYPE_COUNT];
	static wiGraphicsTypes::BlendState			blendStates[BLENDMODE_COUNT];
	static wiGraphicsTypes::RasterizerState		rasterizerState,wireFrameRS;
	static wiGraphicsTypes::DepthStencilState	depthStencilState;

	static wiGraphicsTypes::GraphicsPSO			PSO[BLENDMODE_COUNT][PARTICLESHADERTYPE_COUNT];
	static wiGraphicsTypes::GraphicsPSO			PSO_wire;
	static wiGraphicsTypes::ComputePSO			CPSO_kickoffUpdate, CPSO_emit, CPSO_simulate, CPSO_simulate_SORTING, CPSO_simulate_DEPTHCOLLISIONS, CPSO_simulate_SORTING_DEPTHCOLLISIONS;
	static wiGraphicsTypes::ComputePSO			CPSO_kickoffSort, CPSO_sort, CPSO_sortInner, CPSO_sortStep;

public:
	static void LoadShaders();
private:
	static void LoadBuffers();
	static void SetUpStates();

	float emit;

	bool buffersUpToDate = false;
	uint32_t MAX_PARTICLES;

public:
	wiEmittedParticle();
	wiEmittedParticle(const std::string& newName, const std::string& newMat, Object* newObject, float newSize, float newRandomFac, float newNormalFac
		,float newCount, float newLife, float newRandLife, float newScaleX, float newScaleY, float newRot);
	wiEmittedParticle(const wiEmittedParticle& other);
	static void SetUpStatic();
	static void CleanUpStatic();

	void Update(float dt);
	void Burst(float num);

	void UpdateRenderData(GRAPHICSTHREAD threadID);

	void Draw(GRAPHICSTHREAD threadID);
	void CleanUp();

	bool DEBUG = false;
	ParticleCounters GetDebugData() { return debugData; }

	bool SORTING = false;
	bool DEPTHCOLLISIONS = false;

	PARTICLESHADERTYPE shaderType = SOFT;

	std::string name;
	Object* object;
	std::string materialName;
	Material* material;

	float size,random_factor,normal_factor;
	float count,life,random_life;
	float scaleX,scaleY,rotation;
	float motionBlurAmount;

	void SetMaxParticleCount(uint32_t value);
	uint32_t GetMaxParticleCount() const { return MAX_PARTICLES; }
	uint32_t GetMemorySizeInBytes() const;

	XMFLOAT3 GetPosition() const;

	void Serialize(wiArchive& archive);
};

