import constants
import pygame
import json
import os


class Obstacle:
    """
    Represents a single obstacle in the game. Obstacles are rectangular shapes that block the path.
    """
    def __init__(self, x, y):
        """
        Initialize the obstacle with its position (x, y) on the game grid.
        """
        self.position = (x, y)

    def draw(self, screen):
        """
        Draw the obstacle on the screen at its current position.
        """
        pygame.draw.rect(screen, constants.BLACK, (self.position[0] * constants.GRID_SIZE,
                                                   self.position[1] * constants.GRID_SIZE, constants.GRID_SIZE,
                                                   constants.GRID_SIZE))


class Levels:
    """
    Manages the loading, creation, and retrieval of game levels.
    """
    def __init__(self, level_file_path=None):
        """
        Initialize the Levels object and load the level data from files.
        If level_file_path is provided, a custom level will be loaded from the specified file.
        """
        self.levels = []

        for i, level_file in enumerate(constants.LEVELS_FILES, start=1):
            if i == constants.CUSTOM_NUM and level_file_path:
                level_file = level_file_path
            if not os.path.exists(level_file):
                self.create_level_file(i)
            with open(level_file, "r") as file:
                level_data = json.load(file)
            level_obstacles = [Obstacle(*position) for position in level_data["obstacles"]]
            self.levels.append(level_obstacles)

    def get_level_file_path(self, level_number):
        """
        Return the file path for the JSON file containing the level_number's data.
        """
        return os.path.join(os.path.dirname(os.path.abspath(__file__)), "levels", f"level_{level_number}.json")

    def create_level_file(self, level_number):
        """
        Create a JSON file for the given level_number with the corresponding level's obstacle data.
        """
        level_methods = {
            1: self.one,
            2: self.two,
            3: self.three,
            4: self.four,
            5: self.five,
        }

        level_data = {
            "obstacles": [obstacle.position for obstacle in level_methods[level_number]()]
        }
        level_file = self.get_level_file_path(level_number)
        os.makedirs(os.path.dirname(level_file), exist_ok=True)
        with open(level_file, "w") as file:
            json.dump(level_data, file)

    def one(self):
        """
        Create the first level with no obstacles.
        """
        obstacles = []
        return obstacles

    def two(self):
        """
        Create the second level with obstacles around the border.
        """
        length = 40
        width = 30
        obstacles = []
        for i in range(width):
            obstacles.append(Obstacle(0, i))
            obstacles.append(Obstacle(length - 1, i))
        for i in range(1, length - 1):
            obstacles.append(Obstacle(i, 0))
            obstacles.append(Obstacle(i, width - 1))
        return obstacles

    def three(self):
        """
        Create the third level with obstacles around the border and additional obstacles forming small clusters.
        """
        length = 40
        width = 30
        x1 = 11
        x2 = 25
        x3 = 35
        x4 = 6
        y1 = 11
        y2 = 20
        y3 = 7
        y4 = 25
        obstacles = []
        for i in range(width):
            obstacles.append(Obstacle(0, i))
            obstacles.append(Obstacle(length - 1, i))
        for i in range(1, length - 1):
            obstacles.append(Obstacle(i, 0))
            obstacles.append(Obstacle(i, width - 1))
        for i in range(-1, 2):
            for j in range(-1, 2):
                obstacles.append(Obstacle(x1 + i, y1 + j))
                obstacles.append(Obstacle(x2 + i, y2 + j))
                obstacles.append(Obstacle(x3 + i, y3 + j))
                obstacles.append(Obstacle(x4 + i, y4 + j))
        return obstacles

    def four(self):
        """
        Create the fourth level with obstacles around the border and a grid of obstacles.
        """
        length = 40
        width = 30
        ran = 5
        obstacles = [Obstacle(x, y) for x in range(2, constants.GRID_WIDTH, ran)
                     for y in range(2, constants.GRID_HEIGHT, ran)]
        for i in range(width):
            obstacles.append(Obstacle(0, i))
            obstacles.append(Obstacle(length - 1, i))
        for i in range(1, length - 1):
            obstacles.append(Obstacle(i, 0))
            obstacles.append(Obstacle(i, width - 1))
        return obstacles

    def five(self):
        """
        Create the fifth level with obstacles around the border and a pattern of obstacles.
        """
        length = 40
        width = 30
        ran = 7
        obstacles = []
        for i in range(width):
            obstacles.append(Obstacle(0, i))
            obstacles.append(Obstacle(length - 1, i))
        for i in range(1, length - 1):
            obstacles.append(Obstacle(i, 0))
            obstacles.append(Obstacle(i, width - 1))
        for i in range(-1, 2):
            for j in range(-1, 2):
                for e in range(ran):
                    for t in range(ran):
                        dist1 = 10
                        dist2 = 4
                        obstacles.append(Obstacle(3 + dist1 * e + (t % 2) * 3 + i, 3 + dist2 * t + j))
        return obstacles

    def level_obstacles(self, num):
        """
        Return the list of Obstacle objects for the given level number (num).
        """
        if 1 <= num <= constants.CUSTOM_NUM:
            return self.levels[num - 1]
        return []
