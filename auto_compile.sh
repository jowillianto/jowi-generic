#!/bin/bash
fswatch -0 -o ./src ./tests CMakeLists.txt | ./compile.sh