/*  This file is part of UKNCBTL.
    UKNCBTL is free software: you can redistribute it and/or modify it under the terms
of the GNU Lesser General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.
    UKNCBTL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License along with
UKNCBTL. If not, see <http://www.gnu.org/licenses/>. */

// Emulator.cpp

#include "stdafx.h"
#include "emubase\Emubase.h"
#include <emscripten/emscripten.h>



void Emulator_PrepareScreenRGB32(void* pBits, const uint32_t* colors);


const uint32_t Emulator_CanvasABGRColors[16 * 8] =
{
    0xFF000000, 0xFF800000, 0xFF000080, 0xFF800080, 0xFF008000, 0xFF808000, 0xFF008080, 0xFF808080,
    0xFF000000, 0xFFFF0000, 0xFF0000FF, 0xFFFF00FF, 0xFF00FF00, 0xFFFFFF00, 0xFF00FFFF, 0xFFFFFFFF,
    0xFF000000, 0xFF600000, 0xFF000080, 0xFF600080, 0xFF008000, 0xFF608000, 0xFF008080, 0xFF608080,
    0xFF000000, 0xFFDF0000, 0xFF0000FF, 0xFFDF00FF, 0xFF00FF00, 0xFFDFFF00, 0xFF00FFFF, 0xFFDFFFFF,
    0xFF000000, 0xFF800000, 0xFF000060, 0xFF800060, 0xFF008000, 0xFF808000, 0xFF008060, 0xFF808060,
    0xFF000000, 0xFFFF0000, 0xFF0000DF, 0xFFFF00DF, 0xFF00FF00, 0xFFFFFF00, 0xFF00FFDF, 0xFFFFFFDF,
    0xFF000000, 0xFF600000, 0xFF000060, 0xFF600060, 0xFF008000, 0xFF608000, 0xFF008060, 0xFF608060,
    0xFF000000, 0xFFDF0000, 0xFF0000DF, 0xFFDF00DF, 0xFF00FF00, 0xFFDFFF00, 0xFF00FFDF, 0xFFDFFFDF,
    0xFF000000, 0xFF800000, 0xFF000080, 0xFF800080, 0xFF006000, 0xFF806000, 0xFF006080, 0xFF806080,
    0xFF000000, 0xFFFF0000, 0xFF0000FF, 0xFFFF00FF, 0xFF00DF00, 0xFFFFDF00, 0xFF00DFFF, 0xFFFFDFFF,
    0xFF000000, 0xFF600000, 0xFF000080, 0xFF600080, 0xFF006000, 0xFF606000, 0xFF006080, 0xFF606080,
    0xFF000000, 0xFFDF0000, 0xFF0000FF, 0xFFDF00FF, 0xFF00DF00, 0xFFDFDF00, 0xFF00DFFF, 0xFFDFDFFF,
    0xFF000000, 0xFF800000, 0xFF000060, 0xFF800060, 0xFF006000, 0xFF806000, 0xFF006060, 0xFF806060,
    0xFF000000, 0xFFFF0000, 0xFF0000DF, 0xFFFF00DF, 0xFF00DF00, 0xFFFFDF00, 0xFF00DFDF, 0xFFFFDFDF,
    0xFF000000, 0xFF600000, 0xFF000060, 0xFF600060, 0xFF006000, 0xFF606000, 0xFF006060, 0xFF606060,
    0xFF000000, 0xFFDF0000, 0xFF0000DF, 0xFFDF00DF, 0xFF00DF00, 0xFFDFDF00, 0xFF00DFDF, 0xFFDFDFDF,
};


//////////////////////////////////////////////////////////////////////


CMotherboard* g_pBoard = NULL;

bool g_okEmulatorInitialized = false;
bool g_okEmulatorRunning = false;

uint16_t m_wEmulatorCPUBreakpoint = 0177777;
uint16_t m_wEmulatorPPUBreakpoint = 0177777;

bool m_okEmulatorSound = false;

void* g_pFrameBuffer = 0;

long m_nFrameCount = 0;
uint32_t m_dwTickCount = 0;
uint32_t m_dwEmulatorUptime = 0;  // UKNC uptime, seconds, from turn on or reset, increments every 25 frames
long m_nUptimeFrameCount = 0;

#ifdef __cplusplus
extern "C" {
#endif

void EMSCRIPTEN_KEEPALIVE Emulator_Init()
{
    printf("Emulator_Init()\n");

    g_pFrameBuffer = malloc(640 * 288 * 4);

    ASSERT(g_pBoard == NULL);

    CProcessor::Init();

    g_pBoard = new CMotherboard();

    g_pBoard->Reset();

    g_okEmulatorInitialized = true;

    //return g_pFrameBuffer;
}

void EMSCRIPTEN_KEEPALIVE Emulator_LoadROM(const uint8_t* pROM)
{
    printf("Emulator_LoadROM()\n");
    //printf("%02x %02x\n", pROM[0], pROM[32767]);

    g_pBoard->LoadROM(pROM);

    //g_pBoard->Reset();
}

uint32_t EMSCRIPTEN_KEEPALIVE Emulator_GetUptime()
{
    return m_dwEmulatorUptime;
}

uint16_t EMSCRIPTEN_KEEPALIVE Emulator_GetReg()
{
    return g_pBoard->GetPPU()->GetPC();
}

void EMSCRIPTEN_KEEPALIVE Emulator_Start()
{
    printf("Emulator_Start()\n");

    g_okEmulatorRunning = true;
}
void EMSCRIPTEN_KEEPALIVE Emulator_Stop()
{
    printf("Emulator_Stop()\n");

    g_okEmulatorRunning = false;
}

void EMSCRIPTEN_KEEPALIVE Emulator_Reset()
{
    printf("Emulator_Reset()\n");

    ASSERT(g_pBoard != NULL);

    g_pBoard->Reset();
}

void EMSCRIPTEN_KEEPALIVE Emulator_SystemFrame()
{
    //printf("Emulator_SystemFrame()\n");

    //SoundGen_SetVolume(Settings_GetSoundVolume());

    g_pBoard->SetCPUBreakpoint(m_wEmulatorCPUBreakpoint);
    g_pBoard->SetPPUBreakpoint(m_wEmulatorPPUBreakpoint);

    //ScreenView_ScanKeyboard();

    if (!g_pBoard->SystemFrame())
        return;

    // Calculate emulator uptime (25 frames per second)
    m_nUptimeFrameCount++;
    if (m_nUptimeFrameCount >= 25)
    {
        m_dwEmulatorUptime++;
        m_nUptimeFrameCount = 0;
    }
}

void* EMSCRIPTEN_KEEPALIVE Emulator_PrepareScreen()
{
    //printf("Emulator_PrepareScreen()\n");
    if (g_pFrameBuffer == 0)
        printf("Emulator_PrepareScreen() null framebuffer\n");

    Emulator_PrepareScreenRGB32(g_pFrameBuffer, Emulator_CanvasABGRColors);

    return g_pFrameBuffer;
}

#ifdef __cplusplus
}
#endif

int main()
{
    Emulator_Init();
}

void Emulator_PrepareScreenRGB32(void* pImageBits, const uint32_t* colors)
{
    if (pImageBits == NULL) return;
    if (!g_okEmulatorInitialized) return;

    // Tag parsing loop
    uint8_t cursorYRGB = 0;
    bool okCursorType = false;
    uint8_t cursorPos = 128;
    bool cursorOn = false;
    uint8_t cursorAddress = 0;  // Address of graphical cursor
    uint16_t address = 0000270;  // Tag sequence start address
    bool okTagSize = false;  // Tag size: TRUE - 4-uint16_t, false - 2-uint16_t (first tag is always 2-uint16_t)
    bool okTagType = false;  // Type of 4-uint16_t tag: TRUE - set palette, false - set params
    int scale = 1;           // Horizontal scale: 1, 2, 4, or 8
    uint32_t palette = 0;       // Palette
    uint32_t palettecurrent[8];  memset(palettecurrent, 0, sizeof(palettecurrent)); // Current palette; update each time we change the "palette" variable
    uint8_t pbpgpr = 0;         // 3-bit Y-value modifier
    for (int yy = 0; yy < 307; yy++)
    {
        if (okTagSize)  // 4-uint16_t tag
        {
            uint16_t tag1 = g_pBoard->GetRAMWord(0, address);
            address += 2;
            uint16_t tag2 = g_pBoard->GetRAMWord(0, address);
            address += 2;

            if (okTagType)  // 4-uint16_t palette tag
            {
                palette = MAKELONG(tag1, tag2);
            }
            else  // 4-uint16_t params tag
            {
                scale = (tag2 >> 4) & 3;  // Bits 4-5 - new scale value
                pbpgpr = (uint8_t)((7 - (tag2 & 7)) << 4);  // Y-value modifier
                cursorYRGB = (uint8_t)(tag1 & 15);  // Cursor color
                okCursorType = ((tag1 & 16) != 0);  // true - graphical cursor, false - symbolic cursor
                //ASSERT(okCursorType == 0);  //DEBUG
                cursorPos = (uint8_t)(((tag1 >> 8) >> scale) & 0x7f);  // Cursor position in the line
                cursorAddress = (uint8_t)((tag1 >> 5) & 7);
                scale = 1 << scale;
            }
            for (uint8_t c = 0; c < 8; c++)  // Update palettecurrent
            {
                uint8_t valueYRGB = (uint8_t) (palette >> (c << 2)) & 15;
                palettecurrent[c] = colors[pbpgpr | valueYRGB];
                //if (pbpgpr != 0) DebugLogFormat(_T("pbpgpr %02x\r\n"), pbpgpr | valueYRGB);
            }
        }

        uint16_t addressBits = g_pBoard->GetRAMWord(0, address);  // The uint16_t before the last uint16_t - is address of bits from all three memory planes
        address += 2;

        // Calculate size, type and address of the next tag
        uint16_t tagB = g_pBoard->GetRAMWord(0, address);  // Last uint16_t of the tag - is address and type of the next tag
        okTagSize = (tagB & 2) != 0;  // Bit 1 shows size of the next tag
        if (okTagSize)
        {
            address = tagB & ~7;
            okTagType = (tagB & 4) != 0;  // Bit 2 shows type of the next tag
        }
        else
            address = tagB & ~3;
        if ((tagB & 1) != 0)
            cursorOn = !cursorOn;

        // Draw bits into the bitmap, from line 20 to line 307
        if (yy < 19 /*|| yy > 306*/)
            continue;

        // Loop thru bits from addressBits, planes 0,1,2
        // For each pixel:
        //   Get bit from planes 0,1,2 and make value
        //   Map value to palette; result is 4-bit value YRGB
        //   Translate value to 24-bit RGB
        //   Put value to m_bits; repeat using scale value

        int xr = 640;
        int y = yy - 19;
        uint32_t* pBits = ((uint32_t*)pImageBits) + y * 640;
        int pos = 0;
        for (;;)
        {
            // Get bit from planes 0,1,2
            uint8_t src0 = g_pBoard->GetRAMByte(0, addressBits);
            uint8_t src1 = g_pBoard->GetRAMByte(1, addressBits);
            uint8_t src2 = g_pBoard->GetRAMByte(2, addressBits);
            // Loop through the bits of the byte
            int bit = 0;
            for (;;)
            {
                uint32_t valueRGB;
                if (cursorOn && (pos == cursorPos) && (!okCursorType || (okCursorType && bit == cursorAddress)))
                    valueRGB = colors[cursorYRGB];  // 4-bit to 32-bit color
                else
                {
                    // Make 3-bit value from the bits
                    uint8_t value012 = (src0 & 1) | ((src1 & 1) << 1) | ((src2 & 1) << 2);
                    valueRGB = palettecurrent[value012];  // 3-bit to 32-bit color
                }

                // Put value to m_bits; repeat using scale value
                //WAS: for (int s = 0; s < scale; s++) *pBits++ = valueRGB;
                switch (scale)
                {
                case 8:
                    *pBits++ = valueRGB;
                    *pBits++ = valueRGB;
                    *pBits++ = valueRGB;
                    *pBits++ = valueRGB;
                case 4:
                    *pBits++ = valueRGB;
                    *pBits++ = valueRGB;
                case 2:
                    *pBits++ = valueRGB;
                case 1:
                    *pBits++ = valueRGB;
                default:
                    break;
                }

                xr -= scale;

                if (bit == 7)
                    break;
                bit++;

                // Shift to the next bit
                src0 >>= 1;
                src1 >>= 1;
                src2 >>= 1;
            }
            if (xr <= 0)
                break;  // End of line
            addressBits++;  // Go to the next uint8_t
            pos++;
        }
    }
}


//////////////////////////////////////////////////////////////////////