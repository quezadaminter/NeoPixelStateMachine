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

#include "NeoPixelStateMachine.h"

#ifndef round
#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#endif

// Rounds the integer division of n / d the way round(x) does
// except it does not use floating point math.
// Borrowed from here: http://stackoverflow.com/questions/8136974/c-functions-for-integer-division-with-well-defined-rounding-strategy/33790603#33790603
#define div_round(n, d) ((2 * n - d + 2 * (true && (n < 0 ^d > 0)) * d))

void NeoPixelStateMachine::Steady(uint32_t color)
{
   mPastState = mState;
   mState = STEADY;
   mColor1 = color;
   mFrame = 0;
}

void NeoPixelStateMachine::Fade(uint16_t count, uint16_t duration, uint32_t colorFrom, uint32_t colorTo)
{
   mPastState = mState;
   mState = FADE;
   mFrame = 0;
   mDir = true;
   mSteps = duration / (DT / 1000);//31.250;
   mCount = count;
   mColor1 = colorFrom;//rgbToHue(colorFrom);
   mColor2 = colorTo;//rgbToHue(colorTo);
   mTempColor = mColor1;
   uint8_t r1, g1, b1, r2, g2, b2;
   getRGB(colorFrom, r1, g1, b1);
   getRGB(colorTo, r2, g2, b2);
   mStepR = round((1.f * (r2 - r1)) / mSteps);
   mStepG = round((1.f * (g2 - g1)) / mSteps);
   mStepB = round((1.f * (b2 - b1)) / mSteps);
   //mStepR = div_round((r2 - r1), mSteps);
   //mStepG = div_round((g2 - g1), mSteps);
   //mStepB = div_round((b2 - b1), mSteps);
}

void NeoPixelStateMachine::Fade(uint16_t count, Fader *colorSequence[], uint8_t len)
{
   mState = FADE_SEQ;
   mFaderList = colorSequence;
   mFaderListLen = len;
}

void NeoPixelStateMachine::Flash(uint16_t count, uint16_t timeOn, uint16_t timeOff, uint32_t colorOn, uint32_t colorOff)
{
   mCount = count;
   mFramesOn = timeOn / (DT / 1000);//31.250;
   mFramesOff = (timeOff / (DT / 1000)/*31.250*/) + mFramesOn;
   mColor1 = colorOn;
   mColor2 = colorOff;
   mTempColor = mColor2;
   mFrame = 0;
   mPastState = mState;
   mState = count > 0 ? FLASH : STEADY;
}

uint32_t NeoPixelStateMachine::Update()
{
   uint32_t ret(0L);
   switch(mState)
   {
      case(STEADY):
         ret = mColor1;
         break;
      case(FLASH):
         ret = updateFlash();
         break;
      case(FADE):
         ret = updateFade();
         break;
      case(FADE_SEQ):
         ret = updateFadeSeq();
      default:
         break; 
   }
   mPastState = mState;
   return(ret);
}

uint32_t NeoPixelStateMachine::updateFlash()
{
   ++mFrame;
   if(mFrame <= mFramesOn)
   {
      return(mColor1);
   }
   else if(mFrame <= mFramesOff)
   {
      return(mColor2);
   }
   else
   {
      --mCount;
      mFrame = 0;
      if(mCount == 0)
      {
         Steady(0x00000000);
      }
   }
   return(0L);
}

// Limit the increment or decrement of a variable
// from overflowing an unsigned integer.
uint8_t NeoPixelStateMachine::limit(uint8_t Min, uint8_t val1, int val2, uint8_t Max)
{
   if(val2 < 0 && val1 + val2 < Min)
   {
      return(Min);
   }
   else if(val2 > 0 && val1 + val2 > Max)
   {
      return(Max);
   }
   else
   {
     return(val1 + val2);
   }
}

uint32_t NeoPixelStateMachine::updateFade()
{
   uint8_t r, g, b;
   getRGB(mTempColor, r, g, b);
   if(mFrame >= mSteps)
   {
      mDir = !mDir;
      mFrame = 0;
      --mCount;
      if(mCount == 0)
      {
         Steady(0x00000000);
      }
   }

   if(mDir == true)
   {
      r = limit(0, r, mStepR, 255);
      g = limit(0, g, mStepG, 255);
      b = limit(0, b, mStepB, 255);
      mTempColor = Color(r, g, b);
   }
   else
   {
      r = limit(0, r, -mStepR, 255);
      g = limit(0, g, -mStepG, 255);
      b = limit(0, b, -mStepB, 255);
      mTempColor = Color(r, g, b);
   }
   ++mFrame;
   return(mTempColor);
}

uint32_t NeoPixelStateMachine::updateFadeSeq()
{
   // TODO: COMPLETE implementation!!
   return(0x00000000);
}
