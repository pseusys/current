# Visual Computing

## What is visual computing?

1. Computer-user interface
2. Image processing
3. Sketching -> encoding -> 3D printing
4. Object identification, location, tracking (harmful plant species): efficiency, knowledge
5. Games, computer graphics
6. Object designing, render-camera models
7. Salience: quantify what do peoplelook first for in an image
8. Image generation: stitch, overlay, mask
9. Augmented reality

**Domain**: Visual computing is the computer science discipline that deals with images and 3D objects.

**This course goal**: Introduce basic principles + techniques for computer vision, graphics and medical imaging.
In particular:

1. What is an image?
2. How is it formed?
3. ...

## What is image?

Array of pixels.
Pixel is a set of RGBA values (bytes).
These 3 (or 4) values -> different pictures, channels.
Mathematical operations are available (gradients, derivatives, intensity profiles).

Vector format, arcs, filling.

Information.
Observation of the light of a scene.

## Lab 1

### Exercise 1

1. It converts ASCII-encoded PGM files (P2) to binary-encoded ones (P5) and vice versa.
2. It is stored inside of a pixel array (matrix) called `graymap`.
3. The first one reads and parses ASCII-encoded pixel value (ignoring whitespaces) and the second one reads binary pixel value.
4. In both cases integer is used for intensity storing.
5. The white color.
6. ASCII-encoded file has almost 4 times bigger size (1064037 butes) than binary-encoded one (262160 bytes).
