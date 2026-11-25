#include "Window.h"
#include "Core.h"
#include "Math.h"
#include <fstream >
#include <sstream >

extern "C" {
	_declspec (dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

struct PRIM_VERTEX
{
	Vec3 position;
	Color colour;
};

class ScreenSpaceTriangle
{
public:
	ID3D12Resource* vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView;
	D3D12_INPUT_ELEMENT_DESC inputLayout[2];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;


	void init(Core* core)
	{
		PRIM_VERTEX vertices[3];
		vertices[0].position = Vec3(0, 1.0f, 0);
		vertices[0].colour = Color(0, 1.0f, 0);
		vertices[1].position = Vec3(-1.0f, -1.0f, 0);
		vertices[1].colour = Color(1.0f, 0, 0);
		vertices[2].position = Vec3(1.0f, -1.0f, 0);
		vertices[2].colour = Color(0, 0, 1.0f);

		init(core, &vertices[0], sizeof(PRIM_VERTEX), 3);

		inputLayout[0] = { "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT , 0, D3D12_APPEND_ALIGNED_ELEMENT ,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 };
		inputLayout[1] = { "COLOUR" , 0, DXGI_FORMAT_R32G32B32_FLOAT , 0, D3D12_APPEND_ALIGNED_ELEMENT ,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 };
		inputLayoutDesc.NumElements = 2;
		inputLayoutDesc.pInputElementDescs = inputLayout;
	}
	void init(Core* core, void* vertices, int vertexSizeInBytes, int numVertices)
	{
		D3D12_HEAP_PROPERTIES heapprops = {};
		heapprops.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapprops.CreationNodeMask = 1;
		heapprops.VisibleNodeMask = 1;


		D3D12_RESOURCE_DESC vbDesc = {};
		vbDesc.Width = numVertices * vertexSizeInBytes;
		vbDesc.Height = 1;
		vbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		vbDesc.DepthOrArraySize = 1;
		vbDesc.MipLevels = 1;
		vbDesc.SampleDesc.Count = 1;
		vbDesc.SampleDesc.Quality = 0;
		vbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		core->device->CreateCommittedResource(&heapprops, D3D12_HEAP_FLAG_NONE, &vbDesc,
			D3D12_RESOURCE_STATE_COMMON, NULL, IID_PPV_ARGS(&vertexBuffer));

		core->uploadResource(vertexBuffer, vertices, numVertices * vertexSizeInBytes,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		vbView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbView.StrideInBytes = vertexSizeInBytes;
		vbView.SizeInBytes = numVertices * vertexSizeInBytes;


	}



	void draw(Core* core)
	{
		core->getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		core->getCommandList()->IASetVertexBuffers(0, 1, &vbView);
		core->getCommandList()->DrawInstanced(3, 1, 0, 0);
	}
};

class ScreenPrim
{
public:
	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;
	PSOManager psos;
	ScreenSpaceTriangle prim;
	std::string file(std::string filename)
	{
		std::ifstream file(filename);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
	void init(Core *core)
	{
		std::string vertexShaderStr = file("vertex_shader.txt");
		std::string pixelShaderStr = file("pixel_shader.txt");
		ID3DBlob* status;
		HRESULT hr = D3DCompile(vertexShaderStr.c_str(), strlen(vertexShaderStr.c_str()), NULL,
			NULL, NULL, "VS", "vs_5_0", 0, 0, &vertexShader, &status);

		hr = D3DCompile(pixelShaderStr.c_str(), strlen(pixelShaderStr.c_str()), NULL, NULL,
			NULL, "PS", "ps_5_0", 0, 0, &pixelShader, &status);

		if (FAILED(hr))
		{
			MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Shader Compile Error", 0);
			exit(0);
		}

		prim.init(core);
		psos.createPSO(core, "Triangle", vertexShader, pixelShader, prim.inputLayoutDesc);

	}
	void draw(Core* core) {
		core->beginRenderPass();
		psos.bind(core, "Triangle");
		prim.draw(core);
	}
};


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	Window window;
	window.create(1024, 1024, "My Window");

	Core core;
	core.init(window.hwnd, 1024, 1024);

	ScreenPrim p;
	p.init(&core);

	while (true)
	{

		window.processMessages();

		core.beginFrame();

		if (window.keys[VK_ESCAPE] == 1)
		{
			break;
		}
		core.beginRenderPass();
		p.draw(&core);
		core.finishFrame();

		core.flushGraphicsQueue();
	}

}



