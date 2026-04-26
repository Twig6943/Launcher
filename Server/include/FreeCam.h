#pragma once

// GW1/BFN TBD

#ifdef CYPRESS_GW2

#define OFFSET_CLIENTGAMECONTEXT 0x142B5BB90
#define OFFSET_SETACTIVECAMERA   0x1404DCD80
#define OFFSET_ADDINPUT          0x140412390
#define OFFSET_REMOVEINPUT       0x1404124C0

namespace Cypress
{
	struct FreeCameraInput
	{
		char pad[0x48];
		unsigned int m_moveSpeed;
		unsigned int m_rotateSpeed;
		bool unk1;
		bool m_turboSpeed;
		bool m_enabled;
	};

	struct FreeCamera
	{
		void** vftptr;
		void* unk1;
		char m_cTransform[0x40]; // LinearTransform
		char m_transform[0x40]; // LinearTransform
		char pad[0xC0];
		FreeCameraInput* m_input;
	};

	enum CameraIds : int
	{
		NoCameraId,
		FreeCameraId,
		EntryCameraId,
		CameraIdCount
	};

	struct ClientGameView
	{
		void** vftptr;
		CameraIds m_activeCameraId;
		char pad[0xB8];
		FreeCamera* m_freeCamera;
	};

	struct ClientGameContext
	{
		char pad[120]; // 0x78
		ClientGameView* m_clientGameView;

		static ClientGameContext* GetInstance()
		{
			return *reinterpret_cast<ClientGameContext**>(OFFSET_CLIENTGAMECONTEXT);
		}
	};

	inline bool ToggleFreeCam()
	{
		ClientGameContext* ctx = ClientGameContext::GetInstance();
		if (!ctx || !ctx->m_clientGameView || !ctx->m_clientGameView->m_freeCamera)
			return false;

		ClientGameView* view = ctx->m_clientGameView;
		FreeCamera* cam = view->m_freeCamera;
		if (!cam->m_input) return false;

		bool isActive = (view->m_activeCameraId == FreeCameraId);

		using tSetActiveCamera = void(*)(ClientGameView*, CameraIds);
		auto setActiveCamera = reinterpret_cast<tSetActiveCamera>(OFFSET_SETACTIVECAMERA);

		if (isActive)
		{
			// Deactivate
			setActiveCamera(view, EntryCameraId);

			using tRemoveInput = void(*)(FreeCameraInput*, unsigned int localPlayerId);
			auto removeInput = reinterpret_cast<tRemoveInput>(OFFSET_REMOVEINPUT);
			removeInput(cam->m_input, 0);
		}
		else
		{
			// Activate
			setActiveCamera(view, FreeCameraId);

			using tAddInput = void(*)(FreeCameraInput*, int priority, unsigned int localPlayerId);
			auto addInput = reinterpret_cast<tAddInput>(OFFSET_ADDINPUT);
			addInput(cam->m_input, 32, 0);
		}

		return !isActive; // returns new state: true = now in freecam
	}

	inline bool IsFreeCamActive()
	{
		ClientGameContext* ctx = ClientGameContext::GetInstance();
		if (!ctx || !ctx->m_clientGameView) return false;
		return ctx->m_clientGameView->m_activeCameraId == FreeCameraId;
	}
}

#endif // CYPRESS_GW2
