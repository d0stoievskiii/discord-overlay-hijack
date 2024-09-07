#include "headers/discord.h"


int main(void) {
	discord::DiscordController DC{};
	DC.MM.getProcess(L"cs2.exe");
	

	if (DC.MM.pID == 0) {
		printf("process not found\n");
		exit(-1);
	}

	
	if (!discord::connect(DC)) {
		printf("handle to framebuffer not established\n");
		exit(-1);
	}
	
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);
	discord::draw::Frame frame = discord::draw::createFrame(w, h);

	while (true) {

		discord::draw::cleanFrame(frame);
		discord::draw::Point max = { w / 2, h / 2 };
		discord::draw::Point min = { w / 2 -100, h / 2 -200 };
		discord::draw::Pixel color = { 0, 0, 255, 200 };
		discord::draw::Rect(frame, max, min, color);

		discord::sendFrame(DC, (uint32_t)w, (uint32_t)h, frame.buffer, frame.size);
		
		//press insert to quit
		if ((GetKeyState(VK_INSERT) & 0x8000)) {
			break;
		}
	}

	return 0;

}