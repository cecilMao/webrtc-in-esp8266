# webrtc-in-esp8266

A demo shows how to deploy Webrtc in a wireless network composed of two clients and a WiFi chip of ESP8266, while ESP8266 also plays the role of a signaling server.

## Supported Platforms and Browsers

Safari and Chrome do not allow to use `MediaDevices.getUserMedia()` without using https for some security reasons. Temporarily, I can‘t deploy https in ESP8266. 

So, **Firefox** is the only browser that could be used in PC, Mac OS and Android. iOS could not work. The rest platforms and browsers I didnot test yet.

You can check the [Browser compatibility](https://developer.mozilla.org/en-US/docs/Web/API/MediaDevices/getUserMedia#Browser_compatibility) of `MediaDevices.getUserMedia()`.

## Usage

* esp8266 
      
     The whole project is installed via the Arduino IDE which can be [downloaded here](https://www.arduino.cc/en/main/software).
      
    The web app, e.g. the **data** folder, needs to be uploaded to the ESP8266's SPIFFS. With ESP8266FS installed upload the web app using `ESP8266 Sketch Data Upload` command in the Arduino Tools menu.

   Then enter your WiFi network SSID and password in line 18&19 of file **webrtcVideoPrototype.ino**, and upload the sketch using the Upload button.
   
* Device A
 1. Connect your WiFi network SSID with password
 2. Open firefox and enter the URL "esp/create.html" or "192.168.2.2/create.html"
 3. Grant the premission for camera and mircophone
 4. Click “create” button
 
* Device B
 1. Connect your WiFi network SSID with password
 2. Open firefox and enter the URL "esp/join.html" or "192.168.2.2/join.html"
 3. Grant the premission for mircophone
 4. Click “join” button

You could see a video in the screen of device B, which is taken by device A. Device A could hear a voice from device B.

You could also control the LEDs which are connected with ESP8266 by clicking the buttons below the video frame from both devices. The states of LEDs which are shown in both devices are synchronized.

## Limitation

According to CNLohr’s [test](https://www.youtube.com/watch?v=7BYdZ_24yg0&t=357s), WiFi Range of ESP8266 could be above 100m. However, based on my test, if you need a smooth video streaming, the range between device and ESP8266 should not be over 10m. Of course the bandwidth in these two situation is different. But within the range of 10m, the SNR should support the need of video streaming. I didn‘t know the reason.

## Credits

The realization of using ESP8266 as a signaling server is inspired by [svarunan](https://github.com/svarunan)’s work [serverless-webrtc](https://github.com/svarunan/serverless-webrtc). 
