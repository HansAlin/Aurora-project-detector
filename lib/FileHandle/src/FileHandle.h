#include "Arduino.h"
#include "LittleFS.h"

#ifndef FileHandle_h
#define FileHandle_h

class FileHandle{
  public:
  void initFS();
  void getParam(float * data);
  void saveParam(float * data);
  void saveAPI(String * API_Data);
  void getAPI(String *API_Data);

  private: 
  void stringToValues(String dataString, float * data);
  void stringToAPI(String dataString, String * data);
  String readFile(String path);
  void writeFile(String message, String path);
  void appendFile(String message, String path);
  void deleteFile(String path);

  
  String data_path;
  String API_path;
  
  // fs::FS &fs, 

  


};
#endif