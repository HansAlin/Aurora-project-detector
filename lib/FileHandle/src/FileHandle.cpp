#include "Arduino.h"
#include "FileHandle.h"
#include "LittleFS.h"



void FileHandle::initFS() {
  data_path = "/data.txt";
  API_path = "/API.txt";

  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else{
    Serial.println("LittleFS mounted successfully");
  }
}

void FileHandle::appendFile(String message, String path) {
  Serial.printf("Appending to file: %s\n", path);

  File file = LittleFS.open(path, "a");
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void FileHandle::deleteFile(String path) {
  Serial.printf("Deleting file: %s\n", path);
  if (LittleFS.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}
void FileHandle::writeFile(String message, String path){
  Serial.printf("Writing file: %s\r\n", path);

  File file = LittleFS.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    // Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
  file.close();
}

String FileHandle::readFile(String path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = LittleFS.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    // Serial.println("From file: " + fileContent);
    break;
  }
  file.close();
  return fileContent;
}

void FileHandle::getParam(float * data) {
  String dataString = readFile(data_path);
  stringToValues(dataString, data);
}

void FileHandle::saveParam(float * data) {
  String save_string = String(data[0]) + " ";   // Longitude
  save_string += String(data[1]) + " ";         // Latitude
  save_string += String(data[2]) + " ";         // Zenit
  save_string += String(data[3]) + " ";         // UTC off
  save_string += String(data[4]) + " ";         // Cloud scale value

  save_string += "\n";
  writeFile(save_string, data_path);
  
}

void FileHandle::stringToValues(String dataString, float * data) {
 
  int index = dataString.indexOf(" ");
  String value = dataString.substring(0,index);
  data[0] = value.toFloat();

  String subString = dataString.substring(index + 1);
  index = subString.indexOf(" ");
  value = subString.substring(0,index);
  data[1] = value.toFloat();

  subString = subString.substring(index + 1);
  index = subString.indexOf(" ");
  value = subString.substring(0,index);
  data[2] = value.toFloat();

  subString = subString.substring(index + 1);
  index = subString.indexOf(" ");
  value = subString.substring(0,index);
  data[3] = value.toFloat();

  subString = subString.substring(index + 1);
  index = subString.indexOf(" ");
  value = subString.substring(0,index);
  data[4] = value.toFloat();

}

void FileHandle::stringToAPI(String dataString, String * data) {
  int index = dataString.indexOf(" ");
  String value = dataString.substring(0,index);
  // Serial.println("1 Channel ID: "+ value);
  data[0] = value;

  String subString = dataString.substring(index + 1);
  // Serial.println("Substring: " + subString);
  index = subString.indexOf(" ");
  value = subString.substring(0,index);
  data[1] = value;

  subString = subString.substring(index + 1);
  // Serial.println("Substring: " + subString);
  index = subString.indexOf(" ");
  value = subString.substring(0,index);
  data[2] = value;
}

void FileHandle::saveAPI(String * API_Data) {
  String save_string = API_Data[0] + " ";     // Channel ID
  save_string += API_Data[1] + " ";           // Write API
  save_string += API_Data[2] + " ";           // Read API
  writeFile(save_string, API_path);
}

void FileHandle::getAPI(String *API_Data) {
  String dataString = readFile(API_path);
  stringToAPI(dataString, API_Data);
  
}

