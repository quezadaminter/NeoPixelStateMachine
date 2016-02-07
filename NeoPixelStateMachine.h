#ifndef _NeoPixelStateMachine
#define _NeoPixelStateMachine
#include <avr/sfr_defs.h>

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

class NeoPixelStateMachine
{
   public:
      static const uint16_t DT = ((1.0 / 64.0) * 1000000);
      typedef enum { STEADY, FLASH, FADE, FADE_SEQ } STATE_T;
      NeoPixelStateMachine() ://, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, STATE_T state = STEADY):
         //mIndex(index), mR(r), mG(g), mB(0), mState(state),
         mState(STEADY), mPastState(STEADY)
      {}
   void Flash(uint16_t count, uint16_t timeOn, uint16_t timeOff, uint32_t colorOn, uint32_t colorOff);
   void Fade(uint16_t count, uint16_t rate, uint32_t colorFrom, uint32_t colorTo);
   void Fade(uint16_t count, Fader *colorSequence[], uint8_t len);
   void Steady(uint32_t color);

   uint32_t Update();

   static uint8_t limit(uint8_t Min, uint8_t val1, float val2, uint8_t Max);
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
      float mStepR;
      float mStepG;
      float mStepB;
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
