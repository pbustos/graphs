```
```
#
``` graphdraw
```
Intro to component here


## Configuration parameters
As any other component,
``` *graphdraw* ```
needs a configuration file to start. In

    etc/config

you can find an example of a configuration file. We can find there the following lines:

    EXAMPLE HERE


## Starting the component
To avoid changing the *config* file in the repository, we can copy it to the component's home directory, so changes will remain untouched by future git pulls:

    cd

``` <graphdraw 's path> ```

    cp etc/config config

After editing the new config file we can run the component:

    bin/

```graphdraw ```

    --Ice.Config=config


## IMPORTANT

The component uses the library libnabo. You have to instal it from https://github.com/ethz-asl/libnabo and edit the CMakeLists.txt with cmake-gui to select the MAKE_SHARED option. Then, sudo make install and the mandatory ldconfig.

libnabo uses Eigen. You need to install it from the repo and create a symbolic link: 
ln -s /usr/include/eigen3/Eigen /usr/include/Eigen so it can find the included files.
