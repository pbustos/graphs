## IMPORTANT

### This component requires 

* the include-only library https://github.com/ryanhaining/cppitertools
Clone it in /usr/local/include
* sudo aptitude install libqglviewer-dev-qt5
* sudo ln -s /usr/include/libxml /usr/include/libxml2/libxml/ 




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


