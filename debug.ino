#include "debug.h"

/*=======================================================
  =================   $DEBUG  ==============================
  =========================================================*/


void debug(String text, bool sameLine) {
  debugLastSameLine = sameLine;
  /*if (!debugNewLine) {
    Serial.print("//");
    }
    Serial.print(text);*/

  text.replace("'", "\\'");
  text.replace("'", "\\\"");

  debugText = debugText + text;

  if (!sameLine) {
    //Serial.println("");
    debugText = debugText + "\n";
  }

  if (debugText.length() > 1000) {
    debugText = debugText.substring(debugText.length() - 1000, debugText.length());
  }
}

void debug(int intInput, bool sameLine) {
  debug(String(intInput), sameLine);
}


