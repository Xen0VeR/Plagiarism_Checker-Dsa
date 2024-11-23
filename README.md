# CS293-Project-2024
This repository contains the problem statement and all code files for the final course project of CS293: Data Structures and Algorithms Lab course at IIT Bombay held in Autumn 2024

Build the dockerfile

````bash
docker build -t plagiarism-checker .
````

To run the docker image from the dir

````bash
docker run --rm -it -v "$(pwd):/plagiarism-checker" plagiarism-checker
````
