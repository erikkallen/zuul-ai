from PIL import Image
import numpy as np
import os

# Load tileset image
tileset_path = os.path.join(os.getcwd(), "../assets/map_tiles.png")  # Ensure correct path
if not os.path.exists(tileset_path):
    raise FileNotFoundError(f"Tileset file not found at {tileset_path}")

tileset_image = Image.open(tileset_path)

# Define parameters
tile_size = 32
map_width = 50
map_height = 50

# Calculate tile dimensions
tileset_width = tileset_image.width // tile_size
tileset_height = tileset_image.height // tile_size

# Categorize tiles (manually define based on tileset)
tile_categories = {
    'grass': [0],
    'waterLB': [4],
    'waterMB': [5],
    'waterRB': [6],
    'waterLM': [12],
    'waterMM': [13],
    'waterRM': [14],
    'waterLO': [20],
    'waterMO': [21],
    'waterRO': [22],
    'road': [46, 47, 39, 38],
    # 'building': [40, 41],
    # 'decoration': [3, 26,27,28,29,30,31,32]
}

# Directional adjacency rules for WFC
adjacency_rules = {
    'grass': {
        'north': ['grass', 'road', 'waterLO', 'waterMO', 'waterRO'], 
        'south': ['grass', 'road', 'waterLB', 'waterMB', 'waterRB'], 
        'east': ['grass', 'road', 'waterLM', 'waterLO', 'waterLB'], 
        'west': ['grass', 'road', 'waterRM', 'waterRO', 'waterRB']
    },
    
    'waterLB': {
        'north': ['grass'], 
        'south': ['waterLM', 'waterLO'], 
        'east': ['waterMM'], 
        'west': ['grass']
    },
    'waterMB': {
        'north': ['grass'], 
        'south': ['waterMM'], 
        'east': ['waterMB', 'waterRB'], 
        'west': ['waterMB', 'waterLB']
    },
    'waterRB': {
        'north': ['grass'], 
        'south': ['waterRM', 'waterRO'], 
        'east': ['grass'], 
        'west': ['waterMB', 'waterLB']
    },
    'waterLO': {
        'north': ['waterLM'], 
        'south': ['grass'], 
        'east': ['waterMO', 'waterRO'], 
        'west': ['grass']
    },
    'waterMO': {
        'north': ['waterMM'], 
        'south': ['grass'], 
        'east': ['waterMO', 'waterRO'], 
        'west': ['waterMO', 'waterLO']
    },
    'waterRO': {
        'north': ['waterRM', 'waterRB'], 
        'south': ['grass'], 
        'east': ['grass'], 
        'west': ['waterMO', 'waterLO']
    },
    'waterRM': {
        'north': ['waterRM', 'waterRB'], 
        'south': ['waterRM', 'waterRO'], 
        'east': ['grass'], 
        'west': ['waterMM']
    },
    'waterMM': {
        'north': ['waterMB'], 
        'south': ['waterMO'], 
        'east': ['waterRM'], 
        'west': ['waterLM']
    },
    'waterLM': {
        'north': ['waterLM', 'waterLB'], 
        'south': ['waterLM', 'waterLO'], 
        'east': ['waterMM'], 
        'west': ['grass']
    },

    'road': {'north': ['road', 'grass'], 'south': ['road', 'grass'], 'east': ['road', 'grass'], 'west': ['road', 'grass']},
}

# Pre-seed water regions to guide growth
def seed_initial_state(map_layout):
    map_layout[25][25]['options'] = ['waterMM']

# Wave Function Collapse Algorithm for procedural generation
def wave_function_collapse(map_width, map_height, categories, rules, max_iterations=100000):
    map_layout = [[{'options': list(categories.keys())} for _ in range(map_width)] for _ in range(map_height)]
    seed_initial_state(map_layout)

    def get_neighbors(x, y):
        neighbors = []
        if x > 0: neighbors.append((x-1, y, 'west'))
        if x < map_width - 1: neighbors.append((x+1, y, 'east'))
        if y > 0: neighbors.append((x, y-1, 'north'))
        if y < map_height - 1: neighbors.append((x, y+1, 'south'))
        return neighbors

    def collapse_cell(x, y):
        cell = map_layout[y][x]
        if len(cell['options']) == 1:
            return
        choice = np.random.choice(cell['options'])
        cell['options'] = [choice]

        # Propagate constraints
        stack = [(x, y)]
        while stack:
            cx, cy = stack.pop()
            current_choice = map_layout[cy][cx]['options'][0]
            for nx, ny, direction in get_neighbors(cx, cy):
                neighbor = map_layout[ny][nx]
                valid_options = []
                for option in neighbor['options']:
                    if option in rules[current_choice][direction]:
                        valid_options.append(option)
                if len(valid_options) < len(neighbor['options']):
                    neighbor['options'] = valid_options
                    if len(neighbor['options']) == 0:
                        raise ValueError("Contradiction detected: No valid options for a cell." + str(current_choice))
                    stack.append((nx, ny))

    iteration = 0
    while any(len(cell['options']) > 1 for row in map_layout for cell in row):
        iteration += 1
        if iteration > max_iterations:
            print("Reached iteration limit. Restarting process.")
            return wave_function_collapse(map_width, map_height, categories, rules)

        min_entropy_cells = [(x, y) for y in range(map_height) for x in range(map_width) if len(map_layout[y][x]['options']) > 1]
        x, y = min_entropy_cells[np.random.randint(0, len(min_entropy_cells))]

        try:
            collapse_cell(x, y)
        except ValueError as e:
            print(f"Error: {e}. Restarting process.")
            return wave_function_collapse(map_width, map_height, categories, rules)

    return [[np.random.choice(categories[map_layout[y][x]['options'][0]]) for x in range(map_width)] for y in range(map_height)]

# Generate map using wave function collapse
map_data = wave_function_collapse(map_width, map_height, tile_categories, adjacency_rules)

if map_data is not None:
    # Create map canvas
    map_image = Image.new('RGBA', (map_width * tile_size, map_height * tile_size))

    # Draw the map
    for y in range(map_height):
        for x in range(map_width):
            tile_index = map_data[y][x]
            tile_x = (tile_index % tileset_width) * tile_size
            tile_y = (tile_index // tileset_width) * tile_size
            tile = tileset_image.crop((tile_x, tile_y, tile_x + tile_size, tile_y + tile_size))
            map_image.paste(tile, (x * tile_size, y * tile_size))

    # Save map
    output_path = os.path.join(os.getcwd(), 'generated_map.png')
    map_image.save(output_path)
    print(f"Map saved at: {output_path}")
