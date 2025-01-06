from PIL import Image

# Define the pixel art data as a string
pixel_art = """
  XXXXXX
 XXXX    XXXXX
X          X 
XXXX      XXXX
X   XX  X   
X  X X  X   
X  X X  X   
X  X X  X   
 X  X X    
  XXXXXX

 XXXX     XXXX
X         X  
XXXX    XXXX 
X    XX   X  
X   X X X X  
X  X X X X X
 X  X X X X 

XXXXXXX
XXXXXX  XXXXX
X        X   
X       X    
X      X     
X     X      
X    X       
X   X        
X  X         
 X            
"""

# Define the dimensions of the image
width = 32
height = 32

# Create a new image with the specified dimensions and color mode
img = Image.new('RGB', (width, height), (255, 255, 255)) # White background

# Split the pixel art data into lines
lines = pixel_art.split('\n')

# Iterate over each line in the pixel art data
for y, line in enumerate(lines):
    for x, char in enumerate(line):
        if char == 'X':
            # Set the corresponding pixel to green
            img.putpixel((x, y), (0, 128, 0)) # Green

# Save the image as a PNG file
img.save('robin_hood.png')