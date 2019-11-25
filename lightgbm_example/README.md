# Setup and Running

* Run `brew install lightgbm`.
* On OS X locate lib_lightgbm.dylib. Usually located at `/usr/local/lib/lib_lightgbm.dylib`.
* Run `g++ -std=c++11 -o lg lightgbm.cpp /usr/local/lib/lib_lightgbm.dylib`

# Next Steps

* Include LightGBM library without need to install or messing around with dylib files.
* Update the Prediction code to use https://github.com/microsoft/LightGBM/blob/master/include/LightGBM/c_api.h#L741.
* Explore other ways to load the dataset when integrating with rest of the project.
* Explore lightGBM parameters.
