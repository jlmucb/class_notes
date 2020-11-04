#include <unistd.h> 
#include <ctime>
#include <fstream>
#include <iostream>
#include <raspicam/raspicam.h>
using namespace std;

int main(int argc, char **argv) {
  raspicam::RaspiCam Camera;

  // Open camera
  if (!Camera.open()) {
    cerr<<"Error opening camera"<<endl;
    return -1;
  }
  sleep(3);      // Wait a while until camera stabilizes

  Camera.grab(); // Capture
  unsigned char *data = new unsigned char[Camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB)];
  Camera.retrieve(data, raspicam::RASPICAM_FORMAT_RGB); // get camera image

  std::ofstream outFile("raspicam_image.ppm", std::ios::binary);     // Save
  outFile<<"P6\n"<<Camera.getWidth() <<" "<<Camera.getHeight() <<" 255\n";
  outFile.write((char*) data, Camera.getImageTypeSize(raspicam::RASPICAM_FORMAT_RGB));
  cout<<"Image saved at raspicam_image.ppm"<<endl;

  delete data;
  return 0;
}
