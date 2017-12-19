#include <stdint.h>	// for int32_t, uint64_t, etc
#include <stdio.h>	// for printf()
#include <Windows.h>	// for QueryPerformance*() functions, LARGE_INTEGER and Sleep()

struct STimer {
	double																	LastFrame							= 0;

	void																	Reset								()																{
		QueryPerformanceFrequency	((LARGE_INTEGER*)&TicksPerSecond);
		SecondsPerTick															= 1.0 / TicksPerSecond;
		QueryPerformanceCounter		((LARGE_INTEGER*)&FrameTimePrevious);
		LastFrame																= 0;

	}

	void																	Frame								()																{
		QueryPerformanceCounter		((LARGE_INTEGER*)&FrameTimeCurrent);
		LastFrame																= (float)(( FrameTimeCurrent - FrameTimePrevious ) * SecondsPerTick);
		FrameTimePrevious														= FrameTimeCurrent;
	}
private:
	uint64_t																TicksPerSecond						= 0;
	double																	SecondsPerTick						= 0;
	uint64_t																FrameTimePrevious					= 0;
	uint64_t																FrameTimeCurrent					= 0;

};

static constexpr const	uint32_t										SCREEN_WIDTH						= 80;
static constexpr const	uint32_t										SCREEN_HEIGHT						= 25;

struct SScreen {
	char																	Cells [SCREEN_HEIGHT][SCREEN_WIDTH];
};

struct SPoint {
	int32_t																	x, y;
};

struct SApplication {
	SScreen																	IntermediateTarget;
	SPoint																	Cursor;
};

int32_t																	setup								(SApplication& app)												{
	app.Cursor.x															= SCREEN_WIDTH	/ 2;
	app.Cursor.y															= SCREEN_HEIGHT	/ 2; 
	memset(app.IntermediateTarget.Cells, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
	return 0; 
}

int32_t																	update								(SApplication& app, double lastFrameSeconds)					{
	if(GetAsyncKeyState(VK_LEFT		)) --app.Cursor.x;
	if(GetAsyncKeyState(VK_RIGHT	)) ++app.Cursor.x;
	if(GetAsyncKeyState(VK_UP		)) --app.Cursor.y;
	if(GetAsyncKeyState(VK_DOWN		)) ++app.Cursor.y;

	if (app.Cursor.y < 0 )						app.Cursor.y					= 0;
	if (app.Cursor.x < 0 )						app.Cursor.x					= 0;
	if (app.Cursor.y >= (SCREEN_HEIGHT	- 1))	app.Cursor.y					= SCREEN_HEIGHT	- 2;
	if (app.Cursor.x >= (SCREEN_WIDTH	- 1))	app.Cursor.x					= SCREEN_WIDTH	- 2;

	for(uint32_t y = 0; y < SCREEN_HEIGHT; ++y)
		for(uint32_t x = 0; x < SCREEN_WIDTH; ++x)
			app.IntermediateTarget.Cells[y][x]										= ' ';

	if (app.Cursor.y >= 0 && app.Cursor.y < SCREEN_HEIGHT
	 && app.Cursor.x >= 0 && app.Cursor.x < SCREEN_WIDTH	
	 )
		app.IntermediateTarget.Cells[app.Cursor.y][app.Cursor.x]				= '@';

	for(uint32_t y = 0; y < SCREEN_HEIGHT; ++y)
		app.IntermediateTarget.Cells[y][SCREEN_WIDTH - 1]						= '\n';

	app.IntermediateTarget.Cells[SCREEN_HEIGHT - 1][SCREEN_WIDTH - 2]		= 0;
	printf("\n%s", app.IntermediateTarget.Cells);
	return 0; 
}

int32_t																	cleanup								(SApplication& app)												{ return 0; }
int32_t																	main								()																{
	SApplication																app;
	setup(app);

	STimer																		timer;
	timer.Reset();
	while(true) {
		timer.Frame();
		update(app, timer.LastFrame);
		Sleep(100);
		if(GetAsyncKeyState(VK_ESCAPE))
			break;
	}

	cleanup(app);
	return 0;
}
