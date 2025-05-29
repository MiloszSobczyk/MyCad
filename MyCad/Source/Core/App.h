#pragma once

#include "Algebra.h"
#include "Core/InfiniteGrid.h"
#include "Core/Window.h"
#include "Engine/Camera.h"
#include "Managers/ShaderManager.h"
#include "Objects/Objects.h"
#include "Objects/SelectedShapes.h"
#include "Operations/OperationFactory.h"
#include "Utils/Init.h"
#include <nlohmann/json.hpp>

#include <array>
#include <string>
#include <unordered_set>
#include <vector>

using json = nlohmann::json;

enum class AppMode
{
	Camera,
	AxisCursor,
	Translation,
	Rotation,
	Scaling
};

struct BezierSurfaceParams
{
	bool showPopup = false;
	bool C2 = false;
	bool isCylinder = false;
	float width = 20.f;
	float height = 20.f;
	float radius = 10.f;
	int axis = 1;
	int widthPatches = 1;
	int heightPatches = 1;
};


class App 
{
private:
	bool active;
	bool showGrid = false;
	bool useCursor = true;
	Window window;
	InfiniteGrid grid;

	std::vector<std::shared_ptr<Shape>> shapes;
	std::shared_ptr<SelectedShapes> selectedShapes;
	std::shared_ptr<AxisCursor> axisCursor;
	Point middlePoint;
	std::shared_ptr<Operation> currentOperation;

	OperationFactory operationFactory;

	Algebra::Matrix4 viewMatrix;
	
	Algebra::Vector4 draggingPoint;

	AppMode appMode;
	OperationMode operationMode;
	BezierSurfaceParams bezierParams;

	float interocularDistance = 0.064f;
	float convergenceDistance = 5.f;
	bool useStereoscopy = false;

public:
	static Camera camera;
	static Algebra::Matrix4 projectionMatrix;
	static Algebra::StereoscopicMatrices stereoMatrices;

	App();
	~App();

	void Run();
	void Render();
	void RenderSingleScene();
	void HandleInput();
	void HandleResize();

	void DisplayParameters();
	void DisplayMainMenu();
	void DisplayShapeSelection();
	void DisplayShapeProperties();
	void DisplayAddShapeButtons();
	void DisplayAddPointsButton();
	void DisplayAddSurfacePopup();

	void DisplaySaveToFile();
	void DisplayLoadFromFile();
	void DisplayClearShapes();
	void DisplaySelectAll();

	void DisplayStereoscopyParameters();

	Algebra::Vector4 ScreenToNDC(float x, float y);
	void GetClickedPoint();
};