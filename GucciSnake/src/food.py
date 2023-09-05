import constants
import random
from level import Levels
import shared


class Apple:
    """
    Represents the Apple object that the snake can collect to grow in size and gain points.
    """
    def __init__(self, snake_body, level):
        """
        Initializes an Apple object with a random position that doesn't collide with the snake, obstacles, or bonuses.
        """
        while True:
            pos = (random.randint(0, constants.GRID_WIDTH - 1), random.randint(0, constants.GRID_HEIGHT - 1))
            if pos not in snake_body and pos not in [obstacle.position for obstacle in Levels().level_obstacles(level)]:
                if shared.ANTI_BONUS is not None:
                    if shared.ANTI_BONUS.position != pos:
                        break
                if shared.BONUS is not None:
                    if shared.BONUS.position != pos:
                        break
                break
        self.position = pos

    def respawn(self, snake_body, level):
        """
        Respawns the Apple object in a new random position that doesn't collide with the snake, obstacles, or bonuses.
        """
        while True:
            pos = (random.randint(0, constants.GRID_WIDTH - 1), random.randint(0, constants.GRID_HEIGHT - 1))
            if pos not in snake_body and pos not in [obstacle.position for obstacle in Levels().level_obstacles(level)]:
                if shared.ANTI_BONUS is not None:
                    if shared.ANTI_BONUS.position != pos:
                        break
                if shared.BONUS is not None:
                    if shared.BONUS.position != pos:
                        break
                break
        self.position = pos
