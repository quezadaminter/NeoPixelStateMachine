
/*--------------------------------------------------------------------
  NeoPixelStateMachine is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.
  NeoPixelStateMachine is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with NeoPixelStateMachine.  If not, see
  <http://www.gnu.org/licenses/>.
  --------------------------------------------------------------------*/
#ifndef _NeoPixelStateMachine
#define _NeoPixelStateMachine
#include <avr/sfr_defs.h>


/**
* Controls the transition properties of one color to another.
*/
class Fader
{
   public:
      Fader() : color1(0x00000000), color2(0x00000000), rate(0)
      {}
      uint32_t color1;
      uint32_t color2;
      uint16_t rate;
      float stepR, stepG, stepB;
      uint16_t steps;
};

/**
* The state machine tracks the status of the LED.
* It also provides an interface to update the state.
*/
class NeoPixelStateMachine
{
   public:
      static const uint16_t DT = ((1.0 / 64.0) * 1000000);
      typedef enum { STEADY, FLASH, FADE, FADE_SEQ } STATE_T;
      NeoPixelStateMachine() :
         mState(STEADY), mPastState(STEADY), mFaderList(NULL)
      {}
   /**
     * Changes the color of the pixel by alternating between two
     * colors.
     * @param count Number of times to repeat the pattern.
     * @param timeOn The time in ms that the flash should remain on.
     * @param timeOff The time in ms that the flash should remain off.
     * @param colorOn The color to use in the On condition.
     * @param colorOff The color to use in the OFF condition.
     */    
   void Flash(uint16_t count, uint16_t timeOn, uint16_t timeOff, uint32_t colorOn, uint32_t colorOff);
   void Fade(uint16_t count, uint16_t rate, uint32_t colorFrom, uint32_t colorTo);
   void Fade(uint16_t count, Fader *colorSequence[], uint8_t len);
   void Steady(uint32_t color);

   uint32_t Update();

   static uint8_t limit(uint8_t Min, uint8_t val1, int val2, uint8_t Max);
   static uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
   {
      return (((uint32_t)r << 16) | ((uint32_t)g <<  8) | b);
   }

   static uint32_t rgbToHue(uint32_t color)//uint8_t r, uint8_t g, uint8_t b)
   {
      uint8_t r; uint8_t g; uint8_t b;
      r = (uint8_t)(color >> 16);
      g = (uint8_t)(color >> 8);
      b = (uint8_t)color;
      uint8_t Min(r > g ? (g > b ? b : g) : (r > b ? b : r));
      uint8_t Max(r < g ? (g < b ? b : g) : (r < b ? b : r));

      long hue(0);
      if(Max == r)
      {
         hue = ((g - b) * 60) / (Max - Min);
      }
      else if(Max == g)
      {
         hue = (120 + ((b - r) * 60)) / (Max - Min);
      }
      else
      {
         hue = (240 + ((r - g) * 60)) / (Max - Min);
      }
      
      if(hue < 0)
      {
         hue += 360;
      }
      return((uint32_t)hue);
   }

   static void getRGB(uint32_t color, uint8_t &r, uint8_t &g, uint8_t &b)
   {
      r = (uint8_t)(color >> 16);
      g = (uint8_t)(color >> 8);
      b = (uint8_t)color;
   }
   
   private:
      uint16_t mIndex;
      uint32_t mColor1;
      uint32_t mColor2;
      uint32_t mTempColor;
      uint16_t mFramesOn;
      uint16_t mFramesOff;
      uint16_t mCount;
      uint16_t mSteps;
      int      mStepR;
      int      mStepG;
      int      mStepB;
      STATE_T mState;
      STATE_T mPastState;
      uint32_t mFrame;
      bool mDir;
      Fader **mFaderList;
      uint8_t mFaderListLen;

      uint32_t updateFlash();
      uint32_t updateFade();
      uint32_t updateFadeSeq();
};

#endif
