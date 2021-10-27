#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <tchar.h>
#include <winioctl.h>
#include <d3d9.h>
#include "d3dx9.h"
#include <dwmapi.h>
#include "font.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx9.h"
#include "Imgui/imgui_impl_win32.h"

#include "utils.hpp"
#include <iostream>

#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <string>
#include<iostream> 
#include<fstream>
#include <Windows.h>
#include <stdio.h>
#include "zStr.h"



IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };

HWND MyWnd = NULL;
HWND GameWnd = NULL;
RECT GameRect = { NULL };
MSG Message = { NULL };

static std::string random_string(size_t length)
{
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	std::string str(length, 0);
	std::generate_n(str.begin(), length, randchar);
	return str;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void CleanD3D();

// Hmu on discord (Chase.#1803) if you need any help :)



Vector3 AimbotCorrection(float bulletVelocity, float bulletGravity, float targetDistance, Vector3 targetPosition, Vector3 targetVelocity) {
	Vector3 recalculated = targetPosition;
	float gravity = fabs(bulletGravity);
	float time = targetDistance / fabs(bulletVelocity);
	float bulletDrop = (gravity / 250) * time * time;
	recalculated.z += bulletDrop * 120;
	recalculated.x += time * (targetVelocity.x);
	recalculated.y += time * (targetVelocity.y);
	recalculated.z += time * (targetVelocity.z);
	return recalculated;
}
HRESULT DirectXInit(HWND hWnd) {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ImFontConfig font_config;
	font_config.OversampleH = 1;
	font_config.OversampleV = 1;
	font_config.PixelSnapH = true;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF,
		0x0400, 0x044F,
		0,
	};

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.BackBufferWidth = Settings::Majors::Width;
	p_Params.BackBufferHeight = Settings::Majors::Height;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.hDeviceWindow = MyWnd;
	p_Params.Windowed = TRUE;

	p_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, MyWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &p_Params, &p_Device);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplWin32_Init(MyWnd);
	ImGui_ImplDX9_Init(p_Device);

	
}

// Hmu on discord (Chase.#1803) if you need any help :)
void SetupWindow()
{
	GameWnd = FindWindowW(NULL, TEXT("Fortnite  "));

	if (GameWnd)
	{
		GetClientRect(GameWnd, &GameRect);

		POINT xy = { 0 };

		ClientToScreen(GameWnd, &xy);

		GameRect.left = xy.x;
		GameRect.top = xy.y;

		Settings::Majors::Width = GameRect.right;
		Settings::Majors::Height = GameRect.bottom;
	}
	else {
		exit(2);
	}

	WNDCLASSEX overlayWindowClass;
	ZeroMemory(&overlayWindowClass, sizeof(WNDCLASSEX));
	overlayWindowClass.cbClsExtra = NULL;
	overlayWindowClass.cbWndExtra = NULL;
	overlayWindowClass.cbSize = sizeof(WNDCLASSEX);
	overlayWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	overlayWindowClass.lpfnWndProc = WinProc;
	overlayWindowClass.hInstance = NULL;
	overlayWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	overlayWindowClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	overlayWindowClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	overlayWindowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	overlayWindowClass.lpszClassName = L"Ass";
	overlayWindowClass.lpszMenuName = L"Ass";
	RegisterClassEx(&overlayWindowClass);

	MyWnd = CreateWindowEx(NULL, L"Ass", L"Ass", WS_POPUP | WS_VISIBLE, GameRect.left, GameRect.top, Settings::Majors::Width, Settings::Majors::Height, NULL, NULL, NULL, NULL);

	MARGINS margin = { GameRect.left, GameRect.top, Settings::Majors::Width, Settings::Majors::Height };
	DwmExtendFrameIntoClientArea(MyWnd, &margin);

	SetWindowLong(MyWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

	ShowWindow(MyWnd, SW_SHOW);
	UpdateWindow(MyWnd);
}
// Hmu on discord (Chase.#1803) if you need any help :)
Vector3 Camera(unsigned __int64 RootComponent)
{
	unsigned __int64 PtrPitch;
	Vector3 Camera;

	auto pitch = Driver::read<uintptr_t>(pid, Settings::Majors::LocalPlayer + 0xb0);
	Camera.x = Driver::read<float>(pid, RootComponent + 0x12C);
	Camera.y = Driver::read<float>(pid, pitch + 0x678);

	float test = asin(Camera.y);
	float degrees = test * (180.0 / M_PI);

	Camera.y = degrees;

	if (Camera.x < 0)
		Camera.x = 360 + Camera.x;

	return Camera;
}
// Hmu on discord (Chase.#1803) if you need any help :)
void actorLoop() {
	std::vector<ActorStruct> actorStructVector;

	uintptr_t uWorld = Driver::read<uintptr_t>(pid, BaseAddr + 0x9cc4570);
	if (!uWorld) {
		return;
	}

	uintptr_t PersistentLevel = Driver::read<uintptr_t>(pid, uWorld + 0x30);
	if (!PersistentLevel) {
		return;
	}

	uintptr_t OwningGameInstance = Driver::read<uintptr_t>(pid, uWorld + 0x188);
	if (!OwningGameInstance) {
		return;
	}

	uintptr_t LocalPlayers = Driver::read<uintptr_t>(pid, OwningGameInstance + 0x38);
	if (!LocalPlayers) {
		return;
	}

	Settings::Majors::LocalPlayer = Driver::read<uintptr_t>(pid, LocalPlayers);
	if (!Settings::Majors::LocalPlayer) {
		return;
	}

	uintptr_t LocalPlayerController = Driver::read<uintptr_t>(pid, Settings::Majors::LocalPlayer + 0x30);
	if (!LocalPlayerController) {
		return;
	}

	Settings::Majors::LocalPawn = Driver::read<uintptr_t>(pid, LocalPlayerController + 0x2A0);
	if (!Settings::Majors::LocalPawn) {
		return;
	}
	else {
		Settings::Majors::LocalPawnRootComponent = Driver::read<uintptr_t>(pid, Settings::Majors::LocalPawn + 0x130);
		Settings::Majors::LocalPlayerRelativeLocation = Driver::read<Vector3>(pid, Settings::Majors::LocalPawnRootComponent + 0x11C);

		Settings::Majors::LocalPlayerID = Driver::read<int>(pid, Settings::Majors::LocalPawn + 0x18);
	}

	uint64_t localplayerstate = Driver::read<uint64_t>(pid, Settings::Majors::LocalPawn + 0x240);
	int LocalTeam = Driver::read<int>(pid, localplayerstate + 0xED8);
	Vector3 Localcam = Camera(Settings::Majors::LocalPawnRootComponent);

	for (int index = 0; index < Driver::read<int>(pid, PersistentLevel + (0x98 + sizeof(uintptr_t))); index++)
	{
		uintptr_t PersistentLevelActors = Driver::read<uintptr_t>(pid, PersistentLevel + 0x98);
		if (!PersistentLevelActors) {
			return;
		}

		uintptr_t CurrentActor = Driver::read<uintptr_t>(pid, PersistentLevelActors + (index * sizeof(uintptr_t)));
		if (!CurrentActor) {
			continue;
		}

		uintptr_t CurrentActorMesh = Driver::read<uintptr_t>(pid, CurrentActor + 0x280);
		if (!CurrentActorMesh) {
			continue;
		}

		int CurrentActorID = Driver::read<int>(pid, CurrentActor + 0x18);
		if (!CurrentActorID) {
			continue;
		}

		bool bSpotted = Driver::read<bool>(pid, CurrentActor + 0x542);
		if (!bSpotted) {
			continue;
		}

		if (CurrentActorID != 0 && ((CurrentActorID == Settings::Majors::LocalPlayerID))) {
			Settings::Majors::CorrectbSpotted = bSpotted;

			ActorStruct Actor{ };
			Actor.pObjPointer = CurrentActor;
			Actor.ID = CurrentActorID;
			Actor.Mesh = CurrentActorMesh;

			actorStructVector.push_back(Actor);
		}
		for (const ActorStruct& ActorStruct : actorStructVector)
		{
			if (ActorStruct.pObjPointer == Settings::Majors::LocalPawn) {
				continue;
			}
			uint64_t playerstate = Driver::read<uint64_t>(pid, ActorStruct.pObjPointer + 0x240);
			int TeamIndex = Driver::read<int>(pid, playerstate + 0xED8);
		}
	}

	if (actorStructVector.empty()) {
		return;
	}

	bool bValidEnemyInArea = false;
	float ClosestActorDistance = FLT_MAX;
	Vector3 ClosestActorMouseAimbotPosition = Vector3(0.0f, 0.0f, 0.0f);
	float distance;


	for (const ActorStruct& ActorStruct : actorStructVector)
	{
		if (ActorStruct.pObjPointer == Settings::Majors::LocalPawn) {
			continue;
		}

		uintptr_t RootComponent = Driver::read<uintptr_t>(pid, ActorStruct.pObjPointer + 0x130);
		if (!RootComponent) {
			continue;
		}

		uint64_t playerstate = Driver::read<uint64_t>(pid, ActorStruct.pObjPointer + 0x240);
		int TeamIndex = Driver::read<int>(pid, playerstate + 0xED8);

		Vector3 vHeadBone = GetBoneWithRotation(ActorStruct.Mesh, 66);
		Vector3 vRootBone = GetBoneWithRotation(ActorStruct.Mesh, 0);

		Vector3 vHeadBoneOut = ProjectWorldToScreen(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 20), Vector3(Localcam.y, Localcam.x, Localcam.z));
		Vector3 vRootBoneOut = ProjectWorldToScreen(vRootBone, Vector3(Localcam.y, Localcam.x, Localcam.z));

		Vector3 RootPos = GetBoneWithRotation(ActorStruct.Mesh, select_hitbox());
		Vector3 selection;

		float BoxHeight = vHeadBoneOut.y - vRootBoneOut.y;

		if (BoxHeight < 0)
			BoxHeight = BoxHeight * (-1.f);
		float BoxWidth = BoxHeight * 0.35;
		float CornerHeight = abs(vHeadBoneOut.y - vRootBoneOut.y);
		float CornerWidth = BoxHeight * 0.80;


		Vector3 RelativeInternalLocation = Driver::read<Vector3>(pid, RootComponent + 0x11C);
		if (!RelativeInternalLocation.x && !RelativeInternalLocation.y) {
			continue;
		}

		Vector3 RelativeScreenLocation = ProjectWorldToScreen(RelativeInternalLocation, Vector3(Localcam.y, Localcam.x, Localcam.z));
		if (!RelativeScreenLocation.x && !RelativeScreenLocation.y) {
			continue;
		}

		distance = Settings::Majors::LocalPlayerRelativeLocation.Distance(RelativeInternalLocation) / 100.f;

		char dist[64];
		sprintf_s(dist, "WinSense softaim - [%.1f Fps]\n", ImGui::GetIO().Framerate);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(8, 2), IM_COL32(0, 255, 0, 255), dist);

		if (TeamIndex != LocalTeam) {

			if (distance <= Settings::EspDistance) {

				if (Settings::Prediction) {

					Vector3 Velocity = Driver::read<Vector3>(pid, ActorStruct.pObjPointer + 0x140);
					auto Result = CalculatePrediction(RootPos, Velocity, distance, 275.0f);
					selection = ProjectWorldToScreen(Result, Vector3(Localcam.y, Localcam.x, Localcam.z));
				}

				float ScreenLocationX = selection.x - Settings::Majors::ScreenCenterX, ScreenLocationY = selection.y - Settings::Majors::ScreenCenterY;
				float ActorDistance = std::sqrtf(ScreenLocationX * ScreenLocationX + ScreenLocationY * ScreenLocationY);


				if (Settings::Box)
			    {

					DrawCornerBox(vRootBoneOut.x - BoxWidth / 2 + 1, vHeadBoneOut.y, BoxWidth, BoxHeight, 1.0f, Settings::Thickness); //no be eac dissabled fuckin shit  YES
						DrawCornerBox(vRootBoneOut.x - BoxWidth / 2 - 1, vHeadBoneOut.y, BoxWidth, BoxHeight, 1.0f, Settings::Thickness);
						DrawCornerBox(vRootBoneOut.x - BoxWidth / 2, vHeadBoneOut.y + 1, BoxWidth, BoxHeight, 1.0f,  Settings::Thickness);
						DrawCornerBox(vRootBoneOut.x - BoxWidth / 2, vHeadBoneOut.y - 1, BoxWidth, BoxHeight, 1.0f, Settings::Thickness);
					
						DrawCornerBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, 1.0f, Settings::Thickness);

					DrawCornerBox(vHeadBoneOut.x - (CornerWidth / 2), vHeadBoneOut.y, CornerWidth, CornerHeight, ImGui::GetColorU32({ Settings::BoxCornerESP[0], Settings::BoxCornerESP[1], Settings::BoxCornerESP[2], 1.0f }), Settings::Thickness);
				}

				
				

				

				if (Settings::Distance_Esp) {
					char dist[64];
					sprintf_s(dist, "[%.f] M", distance);
					ImGui::GetOverlayDrawList()->AddText(ImVec2(vHeadBoneOut.x - 20, vHeadBoneOut.y), ImGui::GetColorU32({ color.Black[0], color.Black[1], color.Black[2], 4.0f }), dist);
				}

				if (Settings::DrawFOV) {
					ImGui::GetOverlayDrawList()->AddCircle(ImVec2(Settings::Majors::ScreenCenterX, Settings::Majors::ScreenCenterY), float(Settings::AimbotFOVValue), ImGui::GetColorU32({ Settings::DrawFOVCircle[0],Settings::DrawFOVCircle[1], Settings::DrawFOVCircle[2], 1.0f }), Settings::Shape, Settings::Thickness);
				}

				if (Settings::CrossHair) {
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(Settings::Majors::ScreenCenterX - 12, Settings::Majors::ScreenCenterY), ImVec2((Settings::Majors::ScreenCenterX - 12) + (12 * 2), Settings::Majors::ScreenCenterY), ImGui::GetColorU32({ Settings::CrossHair[0],Settings::CrossHair[1], Settings::CrossHair[2], 1.0f }), 1.0);
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(Settings::Majors::ScreenCenterX, Settings::Majors::ScreenCenterY - 12), ImVec2(Settings::Majors::ScreenCenterX, (Settings::Majors::ScreenCenterY - 12) + (12 * 2)), ImGui::GetColorU32({ Settings::CrossHair[0],Settings::CrossHair[1], Settings::CrossHair[2], 1.0f }), 1.0);
				}


				if (ActorDistance < ClosestActorDistance && ActorDistance < Settings::AimbotFOVValue) {
					ClosestActorDistance = ActorDistance;
					ClosestActorMouseAimbotPosition = Vector3(ScreenLocationX, ScreenLocationY, 0.0f);
					bValidEnemyInArea = true;
				}
			}
		}
	}


	if (Settings::Aimbot && GetAsyncKeyState(hotkeys::aimkey)) {
		float PlayerLocationX = ClosestActorMouseAimbotPosition.x /= Settings::AimbotSmoothingValue, PlayerLocationY = ClosestActorMouseAimbotPosition.y /= Settings::AimbotSmoothingValue;


		if (!PlayerLocationX || !PlayerLocationY) {
			return;
		}

		mouse_event(MOUSEEVENTF_MOVE, PlayerLocationX, PlayerLocationY, 0, 0);
	}


}







void renderLoopCall() {

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	RECT rect = { 0 };

	if (GetWindowRect(GameWnd, &rect))
	{
		Settings::Majors::Width = rect.right - rect.left;
		Settings::Majors::Height = rect.bottom - rect.top;
	}

	Settings::Majors::ScreenCenterX = (Settings::Majors::Width / 2.0f), Settings::Majors::ScreenCenterY = (Settings::Majors::Height / 2.0f);

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		Settings::Majors::menuIsOpen = !Settings::Majors::menuIsOpen;
	}

	if (GetAsyncKeyState(VK_F1) & 1) {
		Settings::Esp_Line = false;
		Settings::Distance_Esp = false;
		Settings::DrawFOV = false;
		Settings::Box = false;
	}

	actorLoop();

	static int tabb = 1;

	if (Settings::Majors::menuIsOpen)
	{
		if (Settings::Majors::menuIsOpen)
		{
			if (ImGui::Begin(z(" WinSense PRV "), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings))
				ImGui::SetNextWindowSize(ImVec2({ 500, 550 }), ImGuiSetCond_FirstUseEver);
			{
				ImGui::SetWindowSize(ImVec2(590, 315), ImGuiCond_Once);


				ImGuiStyle& style = ImGui::GetStyle();
				style.Alpha = 1.0f;
				style.WindowPadding = ImVec2(0, 0);
				style.WindowMinSize = ImVec2(32, 32);
				style.WindowRounding = 5;
				style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
				//style.ChildWindowRounding = 0.0f;
				style.FramePadding = ImVec2(4, 3);
				style.FrameRounding = 0.0f;
				style.ItemSpacing = ImVec2(8, 8);
				style.ItemInnerSpacing = ImVec2(8, 8);
				style.TouchExtraPadding = ImVec2(0, 0);
				style.IndentSpacing = 21.0f;
				style.ColumnsMinSpacing = 0.0f;
				style.ScrollbarSize = 6.0f;
				style.ScrollbarRounding = 0.0f;
				style.GrabMinSize = 5.0f;
				style.GrabRounding = 0.0f;
				style.ButtonTextAlign = ImVec2(0.0f, 0.5f);
				style.DisplayWindowPadding = ImVec2(22, 22);
				style.DisplaySafeAreaPadding = ImVec2(4, 4);
				style.AntiAliasedLines = true;
				//style.AntiAliasedShapes = false;
				style.CurveTessellationTol = 1.f;

				ImVec4* colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_Text] = ImVec4(.6f, .6f, .6f, 1.00f); // grey
				colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
				colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
				colors[ImGuiCol_WindowBg] = ImVec4(10 / 255.f, 9 / 255.f, 13 / 255.f, 1.00f);
				colors[ImGuiCol_ChildBg] = ImVec4(11 / 255.f, 10 / 255.f, 13 / 255.f, 1.0f);
				colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.90f);
				colors[ImGuiCol_Border] = ImColor(25, 20, 36, 255); // 149, 20, 255
				colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0, 0, 1.00f);
				colors[ImGuiCol_FrameBg] = ImVec4(0.101, 0.101, 0.101, 1.0f);
				colors[ImGuiCol_FrameBgHovered] = ImVec4(.6f, .6f, .6f, 0.40f);
				colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.25f, 0.30f, 1.0f);
				colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.40f, 0.40f, 0.55f, 0.80f);
				colors[ImGuiCol_ScrollbarBg] = ImVec4(32 / 255.f, 58 / 255.f, 67 / 255.f, 1.f);
				colors[ImGuiCol_ScrollbarGrab] = ImVec4(149 / 255.f, 20 / 255.f, 225 / 255.f, 1.f);
				colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(149 / 255.f, 20 / 255.f, 255 / 255.f, 1.f);
				colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(149 / 255.f, 20 / 255.f, 255 / 255.f, 1.f);
				colors[ImGuiCol_Separator] = ImVec4(0.654, 0.094, 0.278, 1.f);
				colors[ImGuiCol_CheckMark] = ImColor(88, 0, 255, 255);
				colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
				colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
				colors[ImGuiCol_Button] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
				colors[ImGuiCol_ButtonHovered] = ImColor(84, 0, 123, 255);
				colors[ImGuiCol_ButtonActive] = ImColor(88, 0, 255, 255);
				colors[ImGuiCol_Header] = ImColor(112, 0, 164, 255);
				colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.f);
				colors[ImGuiCol_HeaderActive] = ImColor(0.2f, 0.2f, 0.2f, 1.f);
				colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
				colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
				colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
				colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
				colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
				colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
				colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
				colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);



				static int tab;




				/* {

					if (ImGui::Button(("! aimbot"), ImVec2(120.0f, 30.0f))) tab = 1;



					ImGui::SameLine();
					if (ImGui::Button(("$ visuals"), ImVec2(120.0f, 30.0f))) tab = 2;


					ImGui::SameLine();
					if (ImGui::Button(("% misc"), ImVec2(120.0f, 30.0f))) tab = 3;

				}*/
				ImGuiStyle* style = &ImGui::GetStyle();
				ImGui::BeginChild(("##Tabs"), ImVec2(-1, 30.0f));
				{
					//style->ItemSpacing = ImVec2(1, 1);
					ImGui::PushFont(Settings::Majors::SkeetFont);
					{
						if (ImGui::Button(((" Aimbot")), ImVec2(190, 25)))
						{
							tab = 1;
						}ImGui::SameLine();
						if (ImGui::Button(((" Visuals")), ImVec2(190, 25)))
						{
							tab = 2;
						}ImGui::SameLine();
						//if (ImGui::Button(((" Misc")), ImVec2(190, 25)))
						//{
						//	tab = 3;
						//}ImGui::SameLine();
					} ImGui::PopFont();

				} ImGui::EndChild();



				if (tab == 1)
				{
					ImGui::BeginChild(("##Aimbot"), ImVec2(0, 0), true); {


						ImGui::Checkbox(z("Enable Aimbot"), &Settings::Aimbot);

						ImGui::Checkbox(z("bBox"), &Settings::Box);

						ImGui::Text(z("Aimbot Key:   ")); ImGui::SameLine(110.f);
						HotkeyButton(hotkeys::aimkey, ChangeKey, keystatus);

						ImGui::Checkbox(z("Draw FOV"), &Settings::DrawFOV);

						ImGui::SliderFloat(z(" "), &Settings::AimbotFOVValue, 10, 275);

						ImGui::Text("FOV Slider^");

						ImGui::SliderFloat(z("   "), &Settings::AimbotDistance, 10, 275);

						ImGui::Text("Aimbot Distance ^");

						ImGui::SliderFloat(z("                                                      x"), &Settings::AimbotSmoothingValue, 1, 30);

						ImGui::Text("Smoothness ^");
					}
					ImGui::EndChild();


				}
				if (tab == 2)
				{

					ImGui::BeginChild(("##Visuals"), ImVec2(0, 0), true); {

						ImGui::Checkbox(z("Enable Box Esp"), &Settings::Box);

						ImGui::Checkbox(z("Distance "), &Settings::Distance_Esp);

						ImGui::Checkbox(z("Lines "), &Settings::Esp_Line);

						ImGui::Text(z("Aimbot Bone: "));

						ImGui::Combo(z("##BONES"), &Settings::hitbox, Hitbox, IM_ARRAYSIZE(Hitbox));


						ImGui::Text(z("Aimbot Bone: "));

						ImGui::Combo(z("##NIGGA"), &Settings::bBoxMode, boxStyle, IM_ARRAYSIZE(boxStyle));

						ImGui::SliderFloat(z("Esp Distance"), &Settings::EspDistance, 10, 275);


					}
					ImGui::EndChild();
				}
				/*if (tab == 3)
				{

					ImGui::BeginChild(("##Misc"), ImVec2(0, 0), true); {
						ImGui::Checkbox(z("Crosshair"), &Settings::Cross_Hair);
						ImGui::Checkbox(z("Lockline"), &Settings::Lock_Line);
						//ImGui::Checkbox(z("Auto Lock-Pos Switch"), &Settings::Auto_Bone_Switch);
						ImGui::Checkbox(z("Trigger-Bot"), &Settings::Auto_Fire);



					}
					ImGui::EndChild();
				}*/

			}
			ImGui::End();
		}
	}
	else {
	}

	ImGui::EndFrame();

		p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (p_Device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
	}

	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&p_Params);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}
// Hmu on discord (Chase.#1803) if you need any help :)
WPARAM MainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();

		if (hwnd_active == GameWnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(MyWnd, &rc);
		ClientToScreen(MyWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = MyWnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;

			Settings::Majors::Width = rc.right;
			Settings::Majors::Height = rc.bottom;

			p_Params.BackBufferWidth = Settings::Majors::Width;
			p_Params.BackBufferHeight = Settings::Majors::Height;
			SetWindowPos(GameWnd, (HWND)0, xy.x, xy.y, Settings::Majors::Width, Settings::Majors::Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		renderLoopCall();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	
		DestroyWindow(GameWnd);
}
// Hmu on discord (Chase.#1803) if you need any help :)
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}
// Hmu on discord (Chase.#1803) if you need any help :)
void CleanD3D() {
	if (p_Device != NULL)
	{
		p_Device->EndScene();
		p_Device->Release();
	}

	if (p_Object != NULL)
	{
		p_Object->Release();
	}
}
// Hmu on discord (Chase.#1803) if you need any help :)
bool CheckDriverStatus() {
	int icheck = 29;
	NTSTATUS status = 0;
	Unprotect(Driver::GetBaseAddress);
	uintptr_t BaseAddr = Driver::GetBaseAddress(Driver::currentProcessId);
	if (BaseAddr == 0) {
		return false;
	}
	Protect(Driver::GetBaseAddress);

	int checked = Driver::read<int>(Driver::currentProcessId, (uintptr_t)&icheck, &status);
	if (checked != icheck) {
		return false;
	}

	return true;
}
// Hmu on discord (Chase.#1803) if you need any help :)
DWORD GetProcessIdByName(wchar_t* name) {
	Protect(_ReturnAddress());

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (_wcsicmp(entry.szExeFile, name) == 0) {
				Unprotect(_ReturnAddress());
				return entry.th32ProcessID;
			}
		}
	}

	CloseHandle(snapshot);
	Unprotect(_ReturnAddress());
	return 0;
}
// Hmu on discord (Chase.#1803) if you need any help :)
uintptr_t milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}
// Hmu on discord (Chase.#1803) if you need any help :)
void LoadProtectedFunctions() {

	uintptr_t t = milliseconds_now();
	BYTE xorkey = 0x0;
	for (DWORD i = 0; i < 8; i++) {
		xorkey = ((BYTE*)&t)[i];
		if (xorkey > 0x3 && xorkey < 0xf0) {
			break;
		}
	}
	if (xorkey <= 0x3 || xorkey >= 0xf0) {
		xorkey = 0x56;
	}

	addFunc({ LoadProtectedFunctions, (uintptr_t)CheckDriverStatus - (uintptr_t)LoadProtectedFunctions - 0x3, xorkey, false });
	addFunc({ CheckDriverStatus, (uintptr_t)GetProcessIdByName - (uintptr_t)CheckDriverStatus - 0x3, xorkey, false });
	addFunc({ GetProcessIdByName, (uintptr_t)milliseconds_now - (uintptr_t)GetProcessIdByName - 0x3, xorkey, false });

	addFunc({ Driver::SendCommand, (uintptr_t)Driver::GetBaseAddress - (uintptr_t)Driver::SendCommand - 0x3, xorkey, false });
	addFunc({ Driver::GetBaseAddress, (uintptr_t)Driver::copy_memory - (uintptr_t)Driver::GetBaseAddress - 0x3, xorkey, false });
	addFunc({ Driver::copy_memory, (uintptr_t)GetKernelModuleExport - (uintptr_t)Driver::copy_memory - 0x3, xorkey, false });
	addFunc({ GetKernelModuleExport, (uintptr_t)GetKernelModuleAddress - (uintptr_t)GetKernelModuleExport - 0x3, xorkey, false });
	addFunc({ GetKernelModuleAddress, (uintptr_t)Driver::initialize - (uintptr_t)GetKernelModuleAddress - 0x3, xorkey, false });
	addFunc({ Driver::initialize, (uintptr_t)Driver::read_memory - (uintptr_t)Driver::initialize - 0x3, xorkey, false });
	//addFunc({ Driver::read_memory, (uintptr_t)Driver::write_memory - (uintptr_t)Driver::read_memory - 0x3, xorkey, false });
	//addFunc({ Driver::write_memory, (uintptr_t)getEntity - (uintptr_t)Driver::write_memory - 0x3, xorkey, false });

	for (size_t i = 0; i < funcCount; i++) {
		if (functions[i].address != LoadProtectedFunctions)
			Protect(functions[i].address);
	}
	Unprotect(_ReturnAddress());
}
// Hmu on discord (Chase.#1803) if you need any help :)
bool verify_game() {

	wchar_t name[] = { 'F', 'o', 'r', 't', 'n', 'i', 't', 'e', 'C', 'l', 'i', 'e', 'n', 't', '-', 'W', 'i' , 'n' , '6', '4', '-' , 'S' , 'h', 'i', 'p', 'p', 'i', 'n', 'g', '.','e', 'x', 'e' , 0 };
	//FortniteClient-Win64-Shipping
	Unprotect(GetProcessIdByName);
	pid = GetProcessIdByName(name);
	Protect(GetProcessIdByName);
	memset(name, 0, sizeof(name));

	Unprotect(Driver::GetBaseAddress);
	BaseAddr = Driver::GetBaseAddress(pid);
	Protect(Driver::GetBaseAddress);

	if (BaseAddr != 0) {

		GamePid = pid;
		GameBaseAddress = BaseAddr;


		GamePid = 0;
		GameBaseAddress = 0;

	}
	//ProtectedSleep(2000);

	return true;
}
// Hmu on discord (Chase.#1803) if you need any help :)
int main()
{
	
	LoadProtectedFunctions();
	Protect(LoadProtectedFunctions);
	Unprotect(Driver::initialize);
	Unprotect(CheckDriverStatus);

	if (!Driver::initialize() || !CheckDriverStatus()) {
		wchar_t VarName[] = { 'F','a','s','t','B','o','o','t','O','p','t','i','o','n','\0' };
		UNICODE_STRING FVariableName = UNICODE_STRING();
		FVariableName.Buffer = VarName;
		FVariableName.Length = 28;
		FVariableName.MaximumLength = 30;
		myNtSetSystemEnvironmentValueEx(
			&FVariableName,
			&DummyGuid,
			0,
			0,
			ATTRIBUTES);
		memset(VarName, 0, sizeof(VarName));
		Beep(600, 1000);
		printf(z("Driver not Loaded"));
		ProtectedSleep(3000);
		exit(1);
		return 1;
	}
	Protect(Driver::initialize);
	Protect(CheckDriverStatus);
	Sleep(1000);
	SetupWindow();
	DirectXInit(MyWnd);
	verify_game();
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	std::cout << "" << std::endl;

	Beep(500, 500);
	while (TRUE)
	{
		
		MainLoop();
	}

	Sleep(10000);
	return 0;
}