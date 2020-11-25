import xml.etree.ElementTree as ET

class KML_ColorMode:

    NORMAL=1
    RANDOM=2

    def ToString(mode):
        if mode == KML_ColorMode.NORMAL:
            return 'normal'
        elif mode == KML_ColorMode.RANDOM:
            return 'random'
        else:
            raise Exception('Unknown Color Mode')

    def FromString(mode):

        val = str(mode).lower()
        if val == 'normal':
            return KML_ColorMode.NORMAL

        elif val == 'random':
            return KML_ColorMode.RANDOM

        else:
            raise Exception('Unknown ColorMode (' + str(mode) + ')')


class KML_AltitudeMode:

    CLAMP_TO_GROUND=1
    RELATIVE_TO_GROUND=2
    ABSOLUTE=3

    def ToString(mode):

        if mode == KML_AltitudeMode.CLAMP_TO_GROUND:
            return 'clampToGround'
        elif mode == KML_AltitudeMode.RELATIVE_TO_GROUND:
            return 'relativeToGround'
        elif mode == KML_AltitudeMode.ABSOLUTE:
            return 'absolute'
        else:
            raise Exception('Unknown Mode')

    def FromString(mode):

        val = str(mode).lower()
        if val == 'clamptoground':
            return KML_AltitudeMode.CLAMP_TO_GROUND

        if val == 'relativetoground':
            return KML_AltitudeMode.RELATIVE_TO_GROUND

        if val == 'absolute':
            return KML_AltitudeMode.ABSOLUTE

        raise Exception('Unknown AltitudeMode (' + str(mode) + ')')
        
class KML_Object:

    #  KML Name
    kml_name = None

    #  ID
    id = None

    def __init__(self, id = None, kml_name = 'Object'):

        #  Set the KML Name
        self.kml_name = kml_name

        #  Set the ID
        self.id = id

    def Get_KML_Content(self, offset = 0):
        return ''


    def As_KML(self, offset=0):

        #  Create offset str
        gap = ' ' * offset

        #  Create KML Node
        output = ''
        output += gap + '<' + self.kml_name

        if self.id is not None:
            output += ' id="' + self.id + '"'
        output += '>\n'

        #  Add the content
        output += self.Get_KML_Content(offset + 2)

        #  Close the KML Node
        output += gap + '</' + self.kml_name + '>\n'

        return output

    def __str__(self, offset = 0):

        gap = ' ' * offset

        #  Create output
        output = gap + 'Node: ' + self.kml_name

        return output

    #def __repr__(self):
    #    return self.__str__()

class KML_StyleSelector(KML_Object):

    def __init__(self, id = None, kml_name='StyleSelector'):

        #  Build Parent
        KML_Object.__init__(self, id=id, kml_name=kml_name)


class KML_SubStyle(KML_Object):

    def __init__(self, id=None, kml_name='SubStyle'):

        #  Build Parent
        KML_Object.__init__(self, id=id, kml_name=kml_name)


class KML_ColorStyle(KML_SubStyle):

    def __init__(self, id=None, color=None, color_mode=None, kml_name='ColorStyle'):

        #  Build Parent
        KML_SubStyle.__init__(self, id=None, kml_name=kml_name)

        #  Set the Color
        self.color = color
        self.color_mode = color_mode


    def Get_KML_Content(self, offset = 0):

        #  Create gap string
        gap = ' ' * offset

        #  Create output
        output = ''

        #  Add parent stuff
        output += KML_SubStyle.Get_KML_Content(self, offset=offset)


        #  Set the Color
        if self.color is not None:
            output += gap + '<color>' + self.color + '</color>\n'


        #  Set the color mode
        if self.color_mode is not None:
            output += gap + '<colorMode>' + KML_ColorMode.ToString(self.color_mode) + '</colorMode>\n'


        #  Return output
        return output


class KML_LineStyle(KML_ColorStyle):

    def __init__(self, id=None, color=None, color_mode=None, width=None, kml_name='LineStyle'):

        #  Build Parent
        KML_ColorStyle.__init__(self, id=id, color=color, color_mode=color_mode, kml_name=kml_name)

        #  Set the Width and Color
        self.width = width

    def Get_KML_Content(self, offset=0):

        #  Create gap string
        gap = ' ' * offset

        #  Create Output
        output = ''

        #  Add Parent stuff
        output += KML_ColorStyle.Get_KML_Content(self, offset)


        #  Add the LineStyle Specific Items
        if self.width is not None:
            output += gap + '<width>' + str(self.width) + '</width>\n'


        #  Return result
        return output


class KML_PolyStyle(KML_ColorStyle):

    def __init__(self, id=None, color=None, color_mode = None, fill=None, outline=None, kml_name='PolyStyle'):

        #  Build Parent
        KML_ColorStyle.__init__(self, id=id, color=color, color_mode=color_mode, kml_name=kml_name)

        # Set Polygon Attributes
        self.fill = fill
        self.outline = outline

    def Get_KML_Content(self, offset=0):
        #  Create gap string
        gap = ' ' * offset

        #  Create Output
        output = ''

        #  Add Parent stuff
        output += KML_ColorStyle.Get_KML_Content(self, offset)

        #  Add the fill
        if self.fill:
            output += gap + '<fill>' + str(self.fill) + '</fill>\n'


        #  Return result
        return output

class KML_IconStyle(KML_ColorStyle):

    def __init__(self, id=None, color=None, color_mode = None,
                 scale=None, heading=None, icon=None, kml_name='PolyStyle'):

        #  Build Parent
        KML_ColorStyle.__init__(self, id=id, color=color, color_mode=color_mode, kml_name=kml_name)

        # Set Polygon Attributes
        self.scale = scale
        self.heading = heading
        self.icon = icon


    def Get_KML_Content(self, offset=0):
        #  Create gap string
        gap = ' ' * offset

        #  Create Output
        output = ''

        #  Add Parent stuff
        output += KML_ColorStyle.Get_KML_Content(self, offset)

        #  Add the


        #  Return result
        return output

class KML_LabelStyle(KML_ColorStyle):

    def __init__(self, id=None, color=None, color_mode = None, scale=None, kml_name='PolyStyle'):

        #  Build Parent
        KML_ColorStyle.__init__(self, id=id, color=color, color_mode=color_mode, kml_name=kml_name)

        # Set Polygon Attributes
        self.scale = scale


    def Get_KML_Content(self, offset = 0):

        #  Create gap string
        gap = ' ' * offset

        #  Create Output
        output = ''

        #  Add Parent stuff
        output += KML_ColorStyle.Get_KML_Content(self, offset)

        #  Add the


        #  Return result
        return output

class KML_Style(KML_StyleSelector):

    def __init__(self, id=None,
                 line_style=None,
                 poly_style=None,
                 icon_style=None,
                 label_style=None,
                 kml_name='Style'):

        #  Create Parent
        KML_StyleSelector.__init__(self, id=id, kml_name=kml_name)

        #  Set styles
        self.line_style = line_style
        self.poly_style = poly_style
        self.icon_style = icon_style
        self.label_style = label_style


    def Get_KML_Content(self, offset = 0):

        #  Create gap string
        gap = ' ' * offset

        #  Create output
        output = ''

        #  add parent stuff
        output += KML_StyleSelector.Get_KML_Content(self, offset)

        #  Add Line Style
        if self.line_style is not None:
            output += self.line_style.As_KML( offset=offset)

        #  Add PolyStyle
        if self.poly_style is not None:
            output += self.poly_style.As_KML( offset=offset)

        #  Add Label Style
        if self.label_style is not None:
            output += self.label_style.As_KML(offset=offset)

        #  Add Icon Style
        if self.icon_style is not None:
            output += self.icon_style.As_KML(offset=offset)


        return output


class KML_Feature(KML_Object):

    def __init__(self,
                 id = None,
                 name = None,
                 visibility=None,
                 isOpen=None,
                 description=None,
                 styleUrl=None,
                 kml_name='Feature'):

        #  Construct parent
        KML_Object.__init__(self, id=id, kml_name=kml_name)

        #  Set feature name
        self.name = name
        self.visibility = visibility
        self.isOpen = isOpen
        self.description = description
        self.styleUrl = styleUrl


    def Get_KML_Content(self, offset = 0):

        #  Create gap
        gap = ' ' * offset

        #  Create output
        output = ''

        #  Call parent method
        output += KML_Object.Get_KML_Content(self, offset=offset)

        #  Set name
        if self.name is not None:
            output += gap + '<name>' + self.name + '</name>\n'

        #  Set Style URL
        if self.styleUrl is not None:
            output += gap + '<styleUrl>' + str(self.styleUrl) + '</styleUrl>\n'

        if self.visibility is not None:
            output += gap + '<visibility>'
            if self.visibility == True:
                output += '1'
            else:
                output += '0'
            output += '</visibility>\n'

        #  Process the Description
        if self.description is not None:
            output += gap + '<description>' + self.description + '</description>\n'


        return output

    def __str__(self, offset=0):

        #  Create gap
        gap = ' ' * offset

        #  Create output
        output = KML_Object.__str__(self, offset) + '\n'

        output += gap + 'Name: ' + str(self.name)

        return output

class KML_Geometry(KML_Object):

    def __init__(self, id = None, kml_name='Geometry'):

        #  Call parent
        KML_Object.__init__(self, id=id, kml_name=kml_name)



class KML_Container(KML_Feature):

    def __init__(self, id = None, features = None, name = None, isOpen=None, kml_name='Container'):

        #  Build Parent
        KML_Feature.__init__(self, id=id, name=name, isOpen=isOpen, kml_name=kml_name)


        #  Set the features
        if features is not None:
            self.features = features
        else:
            self.features = []


    def Append_Node(self, new_node):
        self.features.append(new_node)


    def Find(self, name):

        #  Split the path
        parts = name.strip().split('/')

        #  Check if name is empty or junk
        if len(name) <= 0 or len(parts) <= 0:
            return []

        #  Check if we are at the base level
        elif len(parts) == 1:

            # Create output
            output = []

            #  Look over internal features
            for f in self.features:

                #  Check name
                if name == f.name:
                    output.append(f)
            return output


        #  If more than one level, call recursively
        else:


            #  Check if base is in node
            output = []
            for f in self.features:

                #  If the item is a container, call recursively
                if f.name == parts[0] and isinstance(f, KML_Container):

                    #  Run the query
                    res = f.Find('/'.join(map(str,parts[1:])))

                    #  Check if we got a list back
                    if (res != None) and isinstance(res, list):
                        output += res

                #  If the item is not a container, skip

            return output

        return []


    def Get_KML_Content(self, offset = 0):

        #  Create gap
        gap = ' ' * offset

        #  Create output
        output = ''

        #  Add parent material
        output += KML_Feature.Get_KML_Content(self, offset=offset)

        #  Iterate over internal features
        for feature in self.features:
            output += feature.As_KML(offset+2)

        #  Return output
        return output


    def __str__(self, offset = 0):

        #  Create gap
        gap = ' ' * offset

        #  Create output
        output = gap + KML_Feature.__str__(self, offset) + '\n'

        output += gap + 'Feature Nodes: Size (' + str(len(self.features)) + ')\n'
        for feature in self.features:
            output += gap + str(feature) + '\n'

        return output


class KML_Folder(KML_Container):


    def __init__(self, folder_name,
                 id = None,
                 features = None,
                 isOpen=None,
                 kml_name='Folder'):

        #  Construct Parent
        """
        :type kml_name: 'string'
        """
        KML_Container.__init__(self, id=id, features=features, name=folder_name, isOpen=isOpen, kml_name=kml_name)


    def __str__(self, offset=0):

        #  Create gap
        gap = ' ' * offset

        #  Create output
        output = gap + KML_Container.__str__(self, offset)

        return output
    
    @staticmethod
    def From_KML( etree_node ):
        '''
        Create a KML Folder from an ElementTree node
        '''
        id     = etree_node.attrib['id']
        name   = None
        isOpen = 0
        
        #  Create output Folder
        subnodes = etree_node.getchildren()
        features = []
        for subnode in subnodes:
            
            #  Open Placemarks
            if 'Placemark' in subnode.tag:
                features.append( KML_Placemark.From_KML( subnode ) )
                
            elif 'name' in subnode.tag[-5:]:
                name = subnode.text
                
            elif 'open' in subnode.tag[-5:]:
                isOpen = int(subnode.text)
        
        new_folder = KML_Folder( name,
                                 id=id,
                                 features=features,
                                 isOpen=isOpen,
                                 kml_name='Folder' )
        return new_folder


class KML_Document(KML_Container):

    def __init__(self, name=None, id=None, features = None, isOpen=None, kml_name='Document'):

        #  Construct the parent
        KML_Container.__init__(self, id=id, features=features, name=name, isOpen=isOpen, kml_name=kml_name)


class KML_Placemark(KML_Feature):

    def __init__(self,
                 id = None,
                 name = None,
                 visibility=None,
                 isOpen=None,
                 description=None,
                 styleUrl = None,
                 kml_name='Placemark',
                 geometry=None):

        #  Call parent
        KML_Feature.__init__(self,
                             id=id,
                             name=name,
                             visibility=visibility,
                             isOpen=isOpen,
                             description=description,
                             styleUrl=styleUrl,
                             kml_name=kml_name)

        #  Set the geometry
        self.geometry = geometry


    def Get_KML_Content(self, offset = 0):

        #  Create gap
        gap = ' ' * offset

        #  Create output
        output = ''

        #  Add parent material
        output += KML_Feature.Get_KML_Content(self, offset=offset)

        #  Add Geometry
        if self.geometry is not None:
            output += self.geometry.As_KML(offset)

        # Return output
        return output
    
    @staticmethod
    def From_KML( etree_node ):
        '''
        Create a KML Placemark from an ElementTree node
        '''
        id          = etree_node.attrib['id']
        name        = None
        visibility  = None
        isOpen      = 1
        description = None
        styleUrl    = None
        geometry    = None
        
        subnodes = etree_node.getchildren()
        for subnode in subnodes:
            
            #  Open Placemarks
            if 'Placemark' in subnode.tag:
                features.append( KML_Placemark.From_KML( subnode ) )
                
            elif 'name' in subnode.tag[-5:]:
                name = subnode.text
                
            elif 'open' in subnode.tag[-5:]:
                isOpen = int(subnode.text)
                
            elif 'Polygon' in subnode.tag:
                geometry = KML_Polygon.From_KML( subnode )                
            
        new_placemark = KML_Placemark( id, 
                                       name,
                                       visibility,
                                       isOpen,
                                       description,
                                       styleUrl,
                                       'Placemark',
                                       geometry )
        return new_placemark


class KML_Point(KML_Geometry):

    def __init__(self, id=None, lat=None, lon=None, elev=None, alt_mode=None, kml_name='Point'):

        KML_Geometry.__init__(self, id=id, kml_name=kml_name)

        #  Set the coordinates
        self.lat = lat
        self.lon = lon
        self.elev = elev
        self.alt_mode = alt_mode

    def As_KML_Simple(self):

        output = str(self.lon) + ',' + str(self.lat) + ','
        if self.elev is None:
            output += '0'
        else:
            output += str(self.elev)
        return output


    def Get_KML_Content(self, offset = 0):

        #  Create gap
        gap = ' ' * offset

        #  Create output
        output = ''

        #  Add Parent Stuff
        output += KML_Geometry.Get_KML_Content(self, offset=offset)

        #  Add the altitude mode
        if self.alt_mode is not None:
            output += gap + '<altitudeMode>' + str(self.alt_mode) + '</altitudeMode>\n'

        #  Add coordinates
        output += gap + '<coordinates>\n'
        output += gap + ' ' + str(self.lon) + ',' + str(self.lat)
        if self.elev is not None:
            output += ',' + str(self.elev)
        output += '\n' + gap + '</coordinates>\n'


        #  Return output
        return output


class KML_LineString(KML_Geometry):

    def __init__(self, id=None, points=None, kml_name='LineString'):

        #  Build parent
        KML_Geometry.__init__(self, id=id, kml_name=kml_name)

        #  Set points
        if points is None:
            self.points = []
        else:
            self.points = points


class KML_Polygon(KML_Geometry):

    def __init__(self,
                 id=None,
                 innerPoints=None,
                 outerPoints=None,
                 kml_name="Polygon"):

        #  Build Parent
        KML_Geometry.__init__(self, id=id, kml_name=kml_name)

        #  Set inner points
        if innerPoints is None:
            self.innerPoints = []
        else:
            self.innerPoints = innerPoints

        #  Set outer points
        if outerPoints is None:
            self.outerPoints = []
        else:
            self.outerPoints = outerPoints


    def Get_KML_Content(self, offset = 0):

        #  Create gap
        gap = ' ' * offset

        #  Create output
        output = ''

        #  Add Parent Stuff
        output += KML_Geometry.Get_KML_Content(self, offset=offset)

        #  Add the outer loop
        if len(self.outerPoints) > 0:

            #  Add the xml stuff
            output += gap + '<outerBoundaryIs>\n'
            output += gap + '  <LinearRing>\n'
            output += gap + '    <coordinates>\n'
            output += gap + '       '
            for p in self.outerPoints:
                output += p.As_KML_Simple() + ' '
            output += '\n' + gap + '    </coordinates>\n'
            output += gap + '  </LinearRing>\n'
            output += gap + '</outerBoundaryIs>\n'

        #  Add the inner loop
        if len(self.innerPoints) > 0:

            #  Add the xml stuff
            output += gap + '<innerBoundaryIs>\n'
            output += gap + '  <LinearRing>\n'
            output += gap + '    <coordinates>\n'
            output += gap + '       '
            for p in self.innerPoints:
                output += p.As_KML_Simple() + ' '
            output += '\n' + gap + '    </coordinates>\n'
            output += gap + '  </LinearRing>\n'
            output += gap + '</innerBoundaryIs>\n'

        return output
    
    @staticmethod
    def From_KML( etree_node ):
        '''
        Create a KML Polygon from an ElementTree node
        '''
        innerPoints = []
        outerPoints = None
        
        subnodes = etree_node.getchildren()
        for subnode in subnodes:
            
            #  Open Placemarks
            if 'outerBoundaryIs' in subnode.tag:
                coordinates = subnode.getchildren()[0][0].text.strip()
                #  Tuples are split by space
                coord_tuples = coordinates.split(' ')
                
                for tup in coord_tuples:
                    parts = tup.split(',')
                    if len(parts) > 2:
                        innerPoints.append( KML_Point( lat=float(parts[1]),
                                                       lon=float(parts[0]),
                                                       elev=float(parts[2])) )
            
        new_polygon = KML_Polygon( innerPoints = innerPoints,
                                   outerPoints = outerPoints )
        return new_polygon
    
class Bike_Sector_KML_File:
    
    def __init__(self):
        pass
    
    def Parse_KML( self, pathname ):
        
        self.pathname = pathname
        self.tree = ET.ElementTree( file=pathname )
        
        self.root = self.tree.getroot()
        self.root_children = self.root.getchildren()
        
        polygons = []
        
        #  Iterate over each subnode
        for subnode in self.root_children:
            
            #  Check Folder
            if 'Folder' in subnode.tag:
               
                #  Parse Folder
                folder = KML_Folder.From_KML( subnode )
                
                #  For each placemark, grab the polygon
                features = folder.features
                for feature in features:
                    
                    polygon = []
                    for point in feature.geometry.innerPoints:
                        polygon.append([point.lon, point.lat, point.elev])
                    polygons.append( { 'name'      : feature.name,
                                       'polygon'   : polygon } )
                    
        return polygons
                
                
                