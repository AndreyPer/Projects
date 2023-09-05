import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
LEVELS_DIR = os.path.join(BASE_DIR, 'levels')

LEVELS_FILES = [
    os.path.join(LEVELS_DIR, 'level_1.json'),
    os.path.join(LEVELS_DIR, 'level_2.json'),
    os.path.join(LEVELS_DIR, 'level_3.json'),
    os.path.join(LEVELS_DIR, 'level_4.json'),
    os.path.join(LEVELS_DIR, 'level_5.json'),
    os.path.join(LEVELS_DIR, 'level_6.json'),
]
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600
GRID_SIZE = 20
GRID_WIDTH = SCREEN_WIDTH // GRID_SIZE
GRID_HEIGHT = SCREEN_HEIGHT // GRID_SIZE

FONT_SZ = 36
TITLE_SZ = 72
BACKGROUND = (240, 224, 188)
WHITE = (255, 255, 255)
GREEN = (6, 54, 38)
PURPLE = (139, 0, 255)
RED = (153, 0, 18)
BLUE = (66, 170, 255)
BLACK = (50, 37, 31)
SKIN = (255, 245, 245)
YELLOW = (255, 185, 1)
ORANGE = (255, 165, 0)
TURQUOISE = (48, 213, 200)
LIGHT_BLUE = (166, 202, 240)
LIGHT_GREEN = (224, 179, 182)
GOLD = (255, 215, 0)

X1 = 20
X2 = 660
Y = SCREEN_HEIGHT - 42
TITLE_Y = 50

SEC5 = 5000
SEC10 = 10000
SEC15 = 15000
SEC20 = 20000

WAIT_TIME = 100

OPTIONS_NUM = 10
CUSTOM_NUM = 6

RAND_IND = 0.02
APPLES_NUM = 5

RECORDS_FILE = "records.txt"
