# Volcanic project

> MoSIG M1 - 3D Graphics

## Author Names

- Aleksandr Sergeev
- Pia Dopper
- Farah Maria Majdalani

## How to Run the Code

Run in the terminal:

```bash
 make run
```

This command will make sure all the dependencies required for the project are downloaded.

```bash
 make help
```

Will print detailed description of all the console commands available.

## Controls

See [configurations](#configurations) section for configuration file description.
In runtime, the following actions are recognized:

1. `SPACEBAR` key: restart the animation time (from `initial_time` see [configs](#configurations)).
2. `A` or `LEFT` key: move camera left by one unit.
3. `W` or `UP` key: move camera up by one unit.
4. `D` or `RIGHT` key: move camera right by one unit.
5. `S` or `DOWN` key: move camera down by one unit.
6. Mouse wheel scroll: zoom the simulation.
7. Mouse drag with right button pressed: move camera in the direction of drag.
8. Mouse drag with left button pressed: rotate camera in the direction of drag.

## Features Implemented

Our simulation has different input [parameters](#configurations) to configure, such as size, heat, initialization time, etc.
One of the most important parameters is `size`, as it regulates the size of the simulation.
Depending on it, the volcano, the island and water around the island may appear and disappear.

### Volcano

The volcano was implemented using a terrain functionthat consists of laplacian of gaussian function combined with perlin noise in order to make it seem "bumpy" rather than flat.
We have programmed the Perlin Noise ourselves (using `numpy`) in order not to have to include any additional libraries.
At certain offset, a negative square root function is added to the equation, so that the simulation resambles sea shore.

### Weather

The parameter `heat` schedules the weather on our island.
We have three different climat types 0, 1 and 2.
The parameter heat can also be set to any real number in the interval [0,2].
In order to calculate the exact parameters of color choice and features we use the `lerp` function for interpolation between the defined values.
If heat is set equal to -1 then each time a parameter from [0,2] is chosen randomly.

- For the cold weather:
    The color choice is white and brown.
    The terrain is shiny, representing snow and ice.
    The trees have branches without any leaves.
    There are icebergs in the water.
- For the moderate weather:
    The trees have leaves represented as cubes.
    The color choice is brown and green - the top of the volcano is white and shiny.
    The leaf color changes according to the [season](#leaves).
- For the hot weather:
    The color choice is vivid green and vivid yellow.
    Nothing is shiny.
    The volcano is brown and the trees have leaves that we have rendered with textures.

### Water and Lava

We have semi-transparent water and lava, that is ambient and glows at night.
Both are using the shaders that use both color and normal map files for rendering.
We can adjust the water height or lava height with parameters.

### Trees

The trees are displayed differently depending on the weather/time zone.
In the cold time they have hierachical branches, in the moderate temperature they have leaves represented by cubes and in the summer they have leaves represented by textures.

#### Leaves

The leaves color and transparency in the moderate climate are interpolated according to the season.
In spring the color is vivid green, in summer - just green, in autumn orange and in winter the leaves are transparent.

### Skybox

The skybox was implemented by adding textures to a CubeMap.
The textured CubeMap was implemented similar to the Textured Mesh; however, for the CubeMap, we are looping through the textures that are corresponding to each side of the cube, in addition to disabling then re-enabling the depth test in order to have the skybox be unaffected by the translation.
Moreover, two different skybox textures were created to simulate the day and night cycles.
This is dependent on the time of day as it seamlessly interpolates between day and night.

### Smoke

The smoke particle system follows a sinus function to move up.
When the smoke reaches its maximum height, the particles restart.
Each particle has different speed and trajectory.
The particles' colors are interpolated between black and gray according to the time of day.

### Sun

The sun moves in a cycle around the X axis.
Using that we can make a daily cycle and a yearly cycle on our island.
The annual cycle has 364 days and day 0 is the first day of spring.
The sun has also a bias towards positive X depending on the time of the year.
This bias increases to a positive X value in winter.
So the sun is further away from the island and illumination and shadows change.
In summer the sun is centered over the island.
For the remaining values we interpolate over these two states.
For the moderate climate we have a single bias, for the cold climate we have a double bias.
Thus, we can use our heat parameter to represent the different climates and their changing solar irradiance.
For the hot climate we have bias 0.

### Birds

🚧 The birds are under construction 🚧

## Configurations

The configurations file should have several entries, representing simulation parameters.

1. `general` section
    - `distance`: initial camera distance from the island center
    - `size_limit`: total island size, volcano will be placed in the middle
    - `heat`: island climate property
2. `terrain` section
    - `perlin_seed`: seed for perlin noise
    - `perlin_octaves`: number of octaves for perlin noise, the less octaves there are the more flat terrain looks.
    - `perlin_weight`: weight of the perlin noise in equation, perlin noise values will br multiplied by it
    - `laplacian_sigma`: sigma parameter of laplacian of gaussian, regulates volcano radius and height
    - `sigma_radius`: parameter that should be set so that `laplacian_sigma * sigma_radius` will be equal to desired volcano radius
    - `carrier_weight`: weight of the carrier (laplacian of gaussian) function in equation, carrier function values will br multiplied by it
    - `island_radius`: desired island radius after that the water will begin
3. `trees` section
    - `number`: number of trees to generate
    - `margin`: margin for trees from the simulation edges
    - `height`: tree height
    - `radius`: tree bottom radius
    - `circle`: tree leaves radius
4. `ice` section
    - `number`: number of icebergs to generate
    - `margin`: margin for icebergs from the simulation edges
5. `time` section
    - `sun_bias`: sun bias value for themoderate climate zone
    - `init_day`: initialization time for the animation (in days); the day 0 represents the first day of spring
    - `day_length`: length of a single day in seconds
6. `lava` section
    - `height`: lava height
    - `amplitude`: lava waves amplitude
    - `center_shift`: shift of lava rippling animation center
    - `speed`: lava waves speed
    - `distortion`: distortion that waves cause on lava surface
    - `transparency`: lava transparency
    - `shininess`: lava shininess
7. `water` section
    - `height`: water height
    - `amplitude`: water waves amplitude
    - `center_shift`: shift of water rippling animation center
    - `speed`: water waves speed
    - `distortion`: distortion that waves cause on water surface
    - `transparency`: water transparency
    - `shininess`: water shininess
8. `smoke` section
    - `number`: smoke particle count
    - `size`: smoke particle size
    - `thin`: smoke particle minimal rotation trajectory radius
    - `thick`: smoke particle maximal rotation trajectory radius
    - `slow`: smoke particle minimal speed on its' rotation trajectory
    - `fast`: smoke particle maximal speed on its' rotation trajectory
    - `weak`: smoke particles' minimal rotation trajectory steepness
    - `powerful`: smoke particles' rotation trajectory base radius

## Technical Details

There are a few highlights:

- We have implemented a decorater class to cache function output and not calculate it multiple times.
  It works just like `functools.lru_cache`, but supporting `np.ndarray`s as function arguments.
  We used it especially for the Perlin Noise.
- We have a `MeshedNode` class that we use for the trees, for example.
  This allows to link `Mesh` and `Node` more easily together.
- Under the `sources/wrapper` root you will find all the given classes from the lecture.
- Under the `sources/utils` root you will find some helping classes, mostly for calculation (most of them are implemented by us).
- Under the `sources/objects` root you will find most of the features/objects like terrain, trees, smoke etc.

## Possible Extensions

- Birds with Skinning techniques
- Navigating and controlling a bird flying through the scene
- Volcanic eruption with flowing lava, that is burning the trees
- Controlls

## Difficulties

- Difficult to debug and find the errors
- Sometimes not clear which transform method from what class is used
- Understanding exact functionalities
