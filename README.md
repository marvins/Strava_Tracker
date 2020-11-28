# Strava Tracker

## Setup

Distribution Packages

    sudo apt install npm

PIP Packages

    pip3 install ipywidgets shapely sqlalchemy ipyleaflet gpxpy pandas matplotlib geopy
    jupyter labextension install @jupyter-widgets/jupyterlab-manager jupyter-leaflet
    jupyter nbextension enable --py widgetsnbextension
    
For Plotly Packages
    pip3 install plotly
    jupyter labextension install jupyterlab-plotly
    jupyter labextension install @jupyter-widgets/jupyterlab-manager plotlywidget
    
## Recommended Tools

    sudo apt install sqlitebrowser
    
## Starting Project

    jupyter-lab .

or 
    
    jupyter-notebook .
    
## Building Datasets

### Step 1:  Copy datasets 

    Make sure to download all desired datasets from Strava and copy to `./datasets/` sub-directory.
    
### Step 2:  Parse GPX data and construct database

    Run the `gpx_parsing.ipynb` notebook.  
    
### Step 3:  Build route/dataset database tables

