#ifndef __debug__
#define __debug__

String debugText;
bool debugLastSameLine = true;

static void debug(String text, bool sameLine = false);
static void debug(int intInput, bool sameLine = false);


#endif
