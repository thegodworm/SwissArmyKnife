
#include <functional>
#include <utility>

uint8_t getIdFromOscAddr(char buffer[], String _tkn, int elemLen) {
  uint8_t id = elemLen;
  for (uint8_t i = 0 ; i < elemLen ; i++) {
    String tkn = _tkn + String(i);
    char token[tkn.length() + 1];
    tkn.toCharArray(token, tkn.length() + 1);
    char * p = strstr(buffer, token);
    if (p) {
      return i;
    }
  }
  return elemLen;
}

struct SensorHandler {
  using SensorHandlerFn = std::function<void()>;
  SensorHandler() {};
  SensorHandler(SensorHandlerFn dec)
    : run {std::move(dec)} {};
  SensorHandlerFn run;
};
