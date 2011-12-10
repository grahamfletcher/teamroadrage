TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += ./src \
               ./src/control \
               ./src/devices \
               ./src/gui \
               ./src/sensors \
               ./src/util \
               ./src/vision
DESTDIR = bin
OBJECTS_DIR = obj
MOC_DIR = moc

# Input
HEADERS  = src/globals.h \
           src/control/Controller.h \
           src/control/HeadwayKalmanFilter.h \
           src/control/ReactionTimeTracker.h \
           src/devices/AndroidDevice.h \
           src/devices/ArduinoDevice.h \
           src/devices/CameraDevice.h \
           src/devices/OBDDevice.h \
           src/gui/GUIController.h \
           src/gui/HeadwayTab.h \
           src/gui/InspectorWindow.h \
           src/gui/SensorsTab.h \
           src/gui/VideoWindow.h \
           src/sensors/DistanceSensor.h \
           src/sensors/HumiditySensor.h \
           src/sensors/RainSensor.h \
           src/sensors/SpeedSensor.h \
           src/sensors/TemperatureSensor.h \
           src/util/MatToQImage.h \
#           src/vision/Capture.h \
           src/vision/LaneDetector.h \
           src/vision/VehicleDetector.h
SOURCES += src/main.cpp \
           src/control/Controller.cpp \
           src/control/HeadwayKalmanFilter.cpp \
           src/control/ReactionTimeTracker.cpp \
           src/devices/AndroidDevice.cpp \
           src/devices/ArduinoDevice.cpp \
           src/devices/CameraDevice.cpp \
           src/devices/OBDDevice.cpp \
           src/gui/GUIController.cpp \
           src/gui/HeadwayTab.cpp \
           src/gui/InspectorWindow.cpp \
           src/gui/SensorsTab.cpp \
           src/gui/VideoWindow.cpp \
           src/sensors/DistanceSensor.cpp \
           src/sensors/HumiditySensor.cpp \
           src/sensors/RainSensor.cpp \
           src/sensors/SpeedSensor.cpp \
           src/sensors/TemperatureSensor.cpp \
           src/util/MatToQImage.cpp \
#           src/vision/Capture.cpp \
           src/vision/LaneDetector.cpp \
           src/vision/VehicleDetector.cpp


LIBS += -lftdi -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lopencv_gpu
