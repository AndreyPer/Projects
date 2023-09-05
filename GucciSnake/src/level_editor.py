import pygame
import sys
import json
import constants
import os


class Obstacle:
    """
    Represents a single obstacle in the game. Obstacles are rectangular shapes that block the path.
    """
    def __init__(self, position):
        """
        Initialize the obstacle with its position on the game grid.
        """
        self.position = position

    def draw(self, screen):
        """
        Draw the obstacle on the screen at its current position.
        """
        pygame.draw.rect(screen, constants.BLACK, (self.position[0] * constants.GRID_SIZE,
                                                   self.position[1] * constants.GRID_SIZE, constants.GRID_SIZE,
                                                   constants.GRID_SIZE))


class LevelEditor:
    """
    Provides functionality for creating, editing, and saving custom levels.
    """
    def __init__(self):
        """
        Initialize the LevelEditor object with an empty list of obstacles.
        """
        self.obstacles = []

    def add_obstacle(self, position):
        """
        Add an obstacle at the specified position to the list of obstacles.
        """
        self.obstacles.append(position)

    def draw(self, screen):
        """
        Draw the current state of the level editor, including all obstacles, on the screen.
        """
        screen.fill(constants.BACKGROUND)
        for obstacle in self.obstacles:
            Obstacle(obstacle).draw(screen)
        pygame.display.flip()

    def load_level(self):
        """
        Load the level data from a JSON file, or create an empty level if the file is not found.
        """
        try:
            with open("levels/level_6.json", "r") as file:
                level_data = json.load(file)
            self.obstacles = level_data["obstacles"]
        except FileNotFoundError:
            self.obstacles = []

    def run(self):
        """
        Start the level editor, allowing the user to add, edit, and save custom levels.
        """
        screen = pygame.display.set_mode((constants.SCREEN_WIDTH, constants.SCREEN_HEIGHT))
        pygame.display.set_caption("Level Editor - Custom Level")

        self.load_level()

        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.save_level()
                    pygame.quit()
                    sys.exit()
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    if event.button == 1:
                        position = (event.pos[0] // constants.GRID_SIZE, event.pos[1] // constants.GRID_SIZE)
                        self.add_obstacle(position)
                elif event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_RETURN:
                        self.save_level()
                        return
                    elif event.key == pygame.K_c:  # Listen for the 'C' key press
                        self.obstacles.clear()
                    elif event.key == pygame.K_z:  # Listen for the 'Z' key press
                        if self.obstacles:
                            self.obstacles.pop()

            self.draw(screen)
            pygame.time.wait(constants.WAIT_TIME)

    def save_level(self):
        """
        Save the current level data, including all obstacles, to a JSON file.
        """
        level_data = {
            "obstacles": self.obstacles
        }
        level_file = "levels/level_6.json"
        os.makedirs(os.path.dirname(level_file), exist_ok=True)
        with open(level_file, "w") as file:
            json.dump(level_data, file)


if __name__ == "__main__":
    """
    If the script is run directly, start the LevelEditor.
    """
    LevelEditor().run()
