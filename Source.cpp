#include "pxcsession.h"
#include "pxcsensemanager.h"
#include "pxchandconfiguration.h"

#include <stdio.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <string>

int main(char* args,char* argsc) {

	pxcStatus status = PXC_STATUS_ALLOC_FAILED;

	PXCSenseManager *session =  PXCSenseManager::CreateInstance();
	if(!session) {
		printf("Instance create failed\n");
		return 10;
	}

	status = session->EnableHand(nullptr);

	if(status = PXC_STATUS_NO_ERROR) {
		printf("Hand data unavailable\n");
		return 20;
	}

	session->Init();

	if(status = PXC_STATUS_NO_ERROR) {
		printf("init failed\n");
		return 30;
	}

	PXCHandModule* handTracker = session->QueryHand();
	

	if(status = PXC_STATUS_NO_ERROR) {
		printf("no hand tracking support\n");
		return 40;
	}

	PXCHandConfiguration* handConfig = handTracker->CreateActiveConfiguration();
	handConfig->EnableAllGestures();
	handConfig->ApplyChanges();

	PXCHandData* handData = handTracker->CreateOutput();
	bool running = true;

	status = session->EnableStream(PXCCapture::StreamType::STREAM_TYPE_DEPTH,1920,1080,30.0);

	if (status = PXC_STATUS_NO_ERROR) {
		printf("Unknown error when enabling stream.");
		return 50;
	}

	while(running) {
		//printf("Acquire frame. ");
		status = session->AcquireFrame(true);
		//printf("Got frame.\n");
		if(status >= PXC_STATUS_NO_ERROR) {
			printf("He's dead Jim.\n");
			return 50;
		}

		handData->Update();
		//printf("Got %i gestures for %i hands.\n", handData->QueryFiredGesturesNumber(),handData->QueryNumberOfHands());
		for(int i=0; i<handData->QueryFiredGesturesNumber(); i++) {
			PXCHandData::GestureData gestureData;
			handData->QueryFiredGestureData(i,gestureData);
			
			
			wchar_t* name = (wchar_t*)gestureData.name;
			std::wstring nameStr(name);
			std::wcout << nameStr << std::endl;
			
			//printf("%s - len %d\n",nameStr.c_str(), nameStr.size());

			if(nameStr == L"v_sign") {
				running = false;
			}
		}

		PXCCapture::Sample* capture = session->QuerySample();
		PXCImage* depthImage = capture->depth;

		//std::this_thread::sleep_for(std::chrono::milliseconds(10));

		session->ReleaseFrame();
	}

	session->Release();

	return 0;
}