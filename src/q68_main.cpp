/*
 * Copyright (c) 2022 Graeme Gregory
 *
 * SPDX: GPL-2.0-only
 */

#include <SDL.h>

#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "q68_emulator.hpp"
#include "q68_events.hpp"
#include "q68_memory.hpp"
#include "q68_screen.hpp"

extern "C" {

    void renderTick() {
        if (emulator::q68RenderScreenFlag) {
            emulator::q68UpdatePixelBuffer(emulator::q68MemorySpace + 0x20000, 0x8000, 4);
            emulator::q68RenderScreen();
            emulator::q68RenderScreenFlag = false;
        }
        emulator::q68ProcessEvents();
    }
}


int main(int argc, char **argv)
{
#if __EMSCRIPTEN__
    int ret = SDL_Init(SDL_INIT_EVERYTHING & ~(SDL_INIT_TIMER | SDL_INIT_HAPTIC)); 
#else
    int ret = SDL_Init(SDL_INIT_EVERYTHING); 
#endif
    if ( ret < 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return ret;
	}

    emulator::q68AllocateMemory();
    emulator::q68ScreenInit();

    SDL_Thread *thread = SDL_CreateThread(emulator::q68MainLoop, "sQ68ux Emu Thread", NULL);
#if __EMSCRIPTEN__
    emscripten_set_main_loop(renderTick, -1, 1);
    return 0;
#else
    while(1) {
        renderTick();
        if (emulator::exitLoop) {
            SDL_WaitThread(thread, NULL);
            return 0;
        }
    }
#endif
}