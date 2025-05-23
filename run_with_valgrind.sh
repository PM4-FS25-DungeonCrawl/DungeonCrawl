#!/usr/bin/env bash

if ! command -v valgrind &> /dev/null; then
    echo "Error: Valgrind is not installed. Please install it first, to run this script. [sudo apt install valgrind]"
    exit 1
fi

# ensure the log directory exists
mkdir -p log

valgrind --leak-check=full --log-file=log/valgrind_log%p.txt ./buildDir/DungeonCrawl