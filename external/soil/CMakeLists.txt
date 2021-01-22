cmake_minimum_required(VERSION 3.1)

include_directories(inc)

ADD_LIBRARY( soil STATIC
  src/image_helper.c
  src/stb_image_aug.c
  src/image_DXT.c
  src/SOIL.c
  )
