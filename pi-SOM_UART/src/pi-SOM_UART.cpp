/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <Base64RK.h>


CloudEvent imageEvent;

//EAF commented out
// CloudEvent inferenceEvent;
// Variant inferenceData;

const int MAX_BUFFER_SIZE = 8192; // Adjust based on your needs
String base64Image;
uint8_t binaryData[MAX_BUFFER_SIZE];

volatile bool transferComplete = false;

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// setup() runs once, when the device is first turned on
void setup() {
  
  Serial1.begin(115200);
  base64Image.reserve(MAX_BUFFER_SIZE); // Pre-allocate memory for image

}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{
  if (Serial1.available() > 0)
  {
    String incomingString = Serial1.readStringUntil('\n');
    incomingString.trim();

    if (incomingString == "START")
    {
      Log.info("Starting base64 image reception.");
      base64Image = ""; // Clear the buffer
    }

    else if (incomingString == "END")
    {
      Log.info("Completed base64 image reception. Length: %d", base64Image.length());
      transferComplete = true; //EAF
    }


  //EAF commented out
    // else if (incomingString.startsWith("INFERENCE"))
    // {
    //   int delimiter = incomingString.indexOf(",");
    //   int prefix = incomingString.indexOf("=");
    //   String inference = incomingString.substring(prefix + 1, delimiter);
    //   String confidence = incomingString.substring(delimiter + 1);

    //   inferenceData.set("inference", inference);
    //   inferenceData.set("confidence", confidence);

    //   Log.info("Inference: %s, Confidence: %s", inference.c_str(), confidence.c_str());
    //   transferComplete = true;
    // }

    else
    {
      base64Image += incomingString;    // increment the incoming data 
    }
  }

  if (transferComplete)
  {
    transferComplete = false;

    size_t binarySize = base64Image.length() * 3 / 4;
    Log.info("Base64 image size: %d bytes", (int)binarySize);

    bool success = Base64::decode(base64Image.c_str(), binaryData, binarySize);
    if (success)
    {
      Log.info("Decoded binary size: %d bytes\n", (int)binarySize);
    }
    else
    {
      Log.warn("Failed to decode Base64.");
    }

    imageEvent.name("image");
    imageEvent.data((const char *)binaryData, binarySize, ContentType::JPEG);
    Particle.publish(imageEvent);

//EAF commented out
    // inferenceEvent.name("inference");
    // inferenceEvent.data(inferenceData);
    // Particle.publish(inferenceEvent);
  }
  if (imageEvent.isSent())
  {
    Log.info("Image publish succeeded");
    imageEvent.clear();
  }
  else if (!imageEvent.isOk())
  {
    Log.info("Image publish failed error=%d", imageEvent.error());
    imageEvent.clear();
  }
  //EAF commented out
  // if (inferenceEvent.isSent())
  // {
  //   Log.info("Inference publish succeeded");
  //   inferenceEvent.clear();
  // }
  // else if (!inferenceEvent.isOk())
  // {
  //   Log.info("Inference publish failed error=%d", inferenceEvent.error());
  //   inferenceEvent.clear();
  // }
}
