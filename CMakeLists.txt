# Adafruit Bus IO Library
# https://github.com/adafruit/Adafruit_BusIO
# MIT License

cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "Adafruit_SSD1306.cpp" 
                       INCLUDE_DIRS "."
                       REQUIRES arduino Adafruit-GFX-Library)

project(Adafruit_SSD1306)
