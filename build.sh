#!/bin/bash

gcc -I'/ogitor-parent-directory/Ogitor/include/' -I'/usr/local/include/OGRE/' -I'./src/' -fPIC -shared -o ./Plugins/Fysics.so ./src/Fysics.cpp


