# Aurora-project-detector
Aurora is a light phenomenon in the night sky that fascinates many people. This project has aimed to develop a detector that can send alerts to users and tell whether or not there is an aurora event. Many want to see the aurora, but not as many want to stay up all night to have the chance to see them. 
By using a multi-layer interference filter in front of a photodiode on a single Complementary metal-oxide-semiconductor, CMOS, available on the market, I have managed to produce a detector that gives promising results regarding the ability to at least distinguish intense aurora events. 
# Electrical components
# Microcontroller
WEMOS D1 MINI
![WEMOS_D1_MINI](https://user-images.githubusercontent.com/90710177/174956911-271bcadf-524d-4759-b445-734195778e47.JPG | width=100)
# Light sensors TSL2591
The TSL2591 is a sensor with a photodiode on an integrated circuit from AMS. Available on a development break-out board from Adafruit.
Two sensors were appplied. One with a interferense filter in front of and one without.![TSL2591 (2)](https://user-images.githubusercontent.com/90710177/174957303-da9ee40a-dd40-4470-b88a-274c8a069c3d.JPG)
There is an green led on the board that I coverd with black melting glue.
# Multiplexer TCA9548A
The light sensors have same I2C address which means that a mutiplexer is needed in order to distribute the signals to the light sensors.![TCA9548A](https://user-images.githubusercontent.com/90710177/174957905-a6a4f0b3-57c1-46bc-92d4-34bb1072ddfc.JPG)
# Cloud detector
In order to get rid of artificial light, reflected in the clouds, a cloud detector was implemented. 
Consists of a MLX90614, infrared thermometer and a DHT11, humidity sensor, was used.
![DHT11](https://user-images.githubusercontent.com/90710177/174958409-90c4fb9c-f473-434d-bf51-a733861208a3.JPG)
![MLX90614](https://user-images.githubusercontent.com/90710177/174958382-e6448e8e-b7b3-4bc3-ab16-64d781564ec4.JPG)

# Optical components
# Multy-layer interference filter
To filter out the green, 557 nm light, a multi-layer interference filter with a 10 nm bandwidth centered at 557 nm was used. 
![bild](https://user-images.githubusercontent.com/90710177/174958681-f6d2a6cd-c02c-49a4-aca7-f3eb6f4c374b.png)
