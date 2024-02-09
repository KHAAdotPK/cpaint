# cpaint (Character Painter)
cpaint is a WinAPI application designed to facilitate the creation of assets, such as avatars, for a virtual space where users can congregate and participate in various activities.

I'm currently developing an application called cpaint, short for "Character Painter," using the WinAPI framework. This project aims to create assets such as avatars for a virtual space where people can gather and engage in various activities. It's an experimental endeavor focused on generating the necessary resources and images for this virtual environment.

At its current stage, cpaint allows users to draw and save their creations as PNG files. However, this is just the initial iteration of the application, and there's still much work to be done for future improvements.

The envisioned virtual space, which I mentioned earlier, is intended to function as a metaverse—an immersive digital realm where users can interact and explore.

## Dependency

cpaint relies on the zlib library for PNG compression. You can download the zlib library from [zlib.net](http://zlib.net/).

## Included Asset

An example asset created using cpaint is available for demonstration:

![A sample PNG file created with cpaint.](./gigo.png)

## Compiling

To compile cpaint from source, follow these steps:

1. Clone the cpaint repository to your local machine:

    ```bash
    git clone https://github.com/KHAAdotPK/cpaint.git
    ```
2. Copy the source code folder of zlib to the root of the cloned cpaint directory.

3. Navigate to the cloned cpaint directory:

    ```bash
    cd cpaint
    ```

4. Run CMake to generate the necessary build files:

    ```bash
    cmake ./
    ```

5. Compile zlib:

    ```bash
    cd zlib
    cmake ./	
    ```
and ensure that the resulting `zlibd.lib` and `zlibd.dll` files are available un Debug folder.

6. Build the cpaint program:

    ```bash
    cd cpaint
    cmake ./
    msbuild cpaint.sln
    ```

7. Copy `zlibd.lib` and `zlibd.dll` from `./zlib/Debug/` to `./Debug/` directory in cpaint.

8. Once the compilation process is complete, you will have an executable file for cpaint in the `Debug` directory.


### License
This project is governed by a license, the details of which can be located in the accompanying file named 'LICENSE.' Please refer to this file for comprehensive information.
