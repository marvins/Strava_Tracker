# Strava Tracker

## Setup

Distribution Packages

    sudo apt install npm

PIP Packages

    pip3 install ipywidgets shapely sqlalchemy ipyleaflet gpxpy pandas matplotlib pyproj geopy
    jupyter labextension install @jupyter-widgets/jupyterlab-manager jupyter-leaflet
    jupyter nbextension enable --py widgetsnbextension
    
For Plotly Packages
    pip3 install plotly
    jupyter labextension install jupyterlab-plotly
    jupyter labextension install @jupyter-widgets/jupyterlab-manager plotlywidget
    
## Recommended Tools

    sudo apt install sqlitebrowser

##  C++ Module
    
    git submodule add git@github.com:google/googletest.git
    git submodule init
    git submodule update
    
## Starting Project

    jupyter-lab .

or 
    
    jupyter-notebook .
    
## Building Datasets

### Step 0:  Copy datasets 

    Make sure to download all desired datasets from Strava and copy to `./datasets/` sub-directory.
    
### Step 1:  Parse GPX data and construct database

    Run the `gpx_parsing.ipynb` notebook.  
    
### Step 2:  Build average route



