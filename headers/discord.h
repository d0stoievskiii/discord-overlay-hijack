#pragma once

#include "memmanager.h"
#include <iostream>
#include <string>

namespace discord {

    typedef struct _DHEADER
	{
		uint32_t Size; // size of the mapped buffer
		uint64_t Sequence; // frame sequence number, increase after present
		uint32_t Width;
		uint32_t Height;
		uint8_t Data[1];

	} DHEADER, *PDHEADER;

    typedef struct _DISCORDCONTROLLER {
        MemoryManager MM;
        PDHEADER framebuffer;
    } DiscordController, *PDC;

    inline bool connect(DiscordController& DC) {
        std::string filename = "DiscordOverlay_Framebuffer_Memory_" + std::to_string(DC.MM.pID);
        DC.MM.hProc = OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, filename.c_str());
        
        if (!DC.MM.hProc || DC.MM.hProc == INVALID_HANDLE_VALUE) return false;

        DC.framebuffer  = static_cast<PDHEADER>(MapViewOfFile(DC.MM.hProc, FILE_MAP_ALL_ACCESS, 0, 0, 0));

        return DC.framebuffer != NULL;
    }

    inline void disconnect(DiscordController& DC) {
        UnmapViewOfFile(DC.framebuffer);
        DC.framebuffer = nullptr;

        CloseHandle(DC.MM.hProc);
        DC.MM.hProc = nullptr;
    }

    inline void sendFrame(DiscordController& DC, uint32_t width, uint32_t height, void* frame, size_t size) {

        DC.framebuffer->Width = width;
        DC.framebuffer->Height = height;


        memcpy(DC.framebuffer->Data, frame, size);

        DC.framebuffer->Sequence++;
    }


    namespace draw {

        typedef struct _FRAME {
            uint32_t width;
            uint32_t height;
            size_t size;
            void* buffer;
        } Frame, *PFrame;

        typedef struct _PIXEL {
            BYTE B, G, R, A;
        } Pixel, *PPixel;

        typedef struct _POINT {
            uint32_t x, y;
        } Point;

        inline Frame createFrame(uint32_t width, uint32_t height) {
            Frame ret;

            ret.width = width;
            ret.height = height;
            ret.size = width * height * 4;

            ret.buffer = malloc(ret.size);
            memset(ret.buffer, 0, ret.size);

            return ret;
        }

        inline void cleanFrame(Frame& frame) {
            memset(frame.buffer, 0, frame.size);
        }

        inline void _setPixel(Frame& frame, Point p1, Pixel pixel) {
            if (frame.width > p1.x && frame.height > p1.y) {

                PPixel buffer = static_cast<PPixel>(frame.buffer);
                buffer[p1.y*frame.width + p1.x] = pixel;

            }
        }

        inline void hLine(Frame& frame, Point p1, Point p2, Pixel color) {
            int dx = p1.x > p2.x ? -1 : 1;

            while (p1.x != p2.x) {
                _setPixel(frame, p1, color);
                p1.x += dx;
            }
        }

        inline void vLine(Frame& frame, Point p1, Point p2, Pixel color) {
            int dy = p1.y > p2.y ? -1 : 1;

            while (p1.y != p2.y) {
                _setPixel(frame, p1, color);
                p1.y += dy;
            }
        }

        inline void Rect(Frame& frame, Point max, Point min, Pixel color) {
            Point p4 = {min.x, max.y};
            Point p2 = {max.x, min.y};
            hLine(frame, min, p2, color);
            vLine(frame, p2, max, color);
            hLine(frame, max, p4, color);
            vLine(frame, p4, min, color);
        }
    }

}




